//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 
#include <math.h>

#include <inet/mobility/contract/IMobility.h>
#include <inet/linklayer/common/MACAddress.h>
#include <inet/linklayer/common/Ieee802Ctrl.h>

#include "PositionPacket_m.h"
#include "PositionReporter.h"

Define_Module(PositionReporter);

int PositionReporter::nextOffsetMs = 0;
std::default_random_engine* PositionReporter::random = new std::default_random_engine();

void PositionReporter::initialize() {
    // Get parameters
    periodMs = par("periodMs");
    printReports = par("printReports");
    report = par("report");

    // Get out gate identifier
    lowerLayerOut = findGate("lowerLayerOut");

    // Get data dumper modules
    delaysLogger = check_and_cast<DataLogger *>(getModuleByPath("delaysLogger"));

    // Initialize period distribution
    runOffsetDist = new std::uniform_int_distribution<int>(periodMs * 1000 / 2, periodMs * 1000);

    // Schedule first reporting event
    std::uniform_int_distribution<int> initOffsetDist(0, periodMs * 1000);
    int offsetUs = (initOffsetDist)(*random);
    std::cout << "Offset: " << offsetUs << " us" << std::endl;
    this->scheduleAt(SimTime(offsetUs, SIMTIME_US), &event);
}

void PositionReporter::handleMessage(cMessage *msg) {
    if(msg == &event) {
        handleTimerEvent(msg);
    } else {
        if(dynamic_cast<PositionPacket *>(msg) != nullptr) {
            PositionPacket *positionPacket = check_and_cast<PositionPacket*>(msg);
            handlePositionUpdate(positionPacket);
        }

        delete msg;
    }
}

void PositionReporter::handlePositionUpdate(PositionPacket *packet) {
    //std::cout << getParentModule()->getFullName() << ": Received at: " << packet->getArrivalTime() << std::endl;

    Info other = {packet->getId(), packet->getX(), packet->getY(), packet->getTime()};
    others[packet->getId()] = other;
    //std::cout << getParentModule()->getFullName() << ": Size: " << others.size() << std::endl;
}

void PositionReporter::handleTimerEvent(cMessage *msg) {
    //std::cout << getParentModule()->getFullName() << ": Incoming timer at: " << simTime() << std::endl;

    if(report) {
        sendPositionUpdate();
    }

    collectDelays();

    int delayUs = (*runOffsetDist)(*random);

    std::cout << "Delay: " << delayUs << " us" << std::endl;
    this->scheduleAt(simTime() + SimTime(delayUs, SIMTIME_US), msg);
}

inet::Coord PositionReporter::getPosition(int moduleId) {
    cModule *module = getSimulation()->getModule(moduleId);
    inet::IMobility *mobility = check_and_cast<inet::IMobility *>(module->getSubmodule("mobility"));
    return mobility->getCurrentPosition();
}

double PositionReporter::getMaxSpeed(int moduleId) {
    cModule *module = getSimulation()->getModule(moduleId);
    inet::IMobility *mobility = check_and_cast<inet::IMobility *>(module->getSubmodule("mobility"));
    return mobility->getMaxSpeed();
}

void PositionReporter::sendPositionUpdate() {
    // Get position and time
    inet::Coord position = getPosition(getParentModule()->getId());
    double time = simTime().dbl();

    // Construct packet
    PositionPacket *packet = new PositionPacket();
    packet->setId(getParentModule()->getId());
    packet->setX(position.x);
    packet->setY(position.y);
    packet->setTime(time);
    packet->setMaxSpeed(getMaxSpeed(getParentModule()->getId()));
    packet->setByteLength(sizeof(int) + 4 * sizeof(double));

    inet::Ieee802Ctrl *ctrl = new inet::Ieee802Ctrl();
    ctrl->setDest(inet::MACAddress::BROADCAST_ADDRESS);
    packet->setControlInfo(ctrl);

    // Send packet
    send(packet, lowerLayerOut);
    std::cout << getParentModule()->getFullName() << ": Send at: " << simTime() << std::endl;
}

void PositionReporter::collectDelays() {
    inet::Coord position = getPosition(getParentModule()->getId());
    double time = simTime().dbl();

    if(printReports) {
        std::cout << ">>> Report from node id: " << getId() << std::endl;
    }

    //std::cout << getParentModule()->getFullName() << ": ReadSize: " << others.size() << std::endl;
    for (auto& kv : others) {
        Info other = kv.second;

        double lattency = time - other.time;

        // Distance from data packet
        double dx = other.x - position.x;
        double dy = other.y - position.y;
        double distance = sqrt(dx * dx + dy * dy);

        // Ground truth distance
        inet::Coord gtPos = getPosition(other.id);
        double gtdx = position.x - gtPos.x;
        double gtdy = position.y - gtPos.y;
        double gtDistance = sqrt(gtdx * gtdx + gtdy * gtdy);

        delaysLogger->getStream() << lattency << "\t" << distance << "\t" << gtDistance << std::endl;

        if(printReports) {
            std::cout << "id:" << other.id << " lattency: " << lattency << " gtDistance: " << gtDistance << " distance: " << distance << std::endl;
        }
    }
    if(printReports) {
        std::cout << std::endl;
    }
}

void PositionReporter::deleteModule() {
    // Ensure we are not deleting scheduled event
    cancelEvent(&event);

    cSimpleModule::deleteModule();
}

