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
#include <inet/linklayer/common/SimpleLinkLayerControlInfo.h>

#include "PositionPacket_m.h"
#include "PositionReporter.h"

Define_Module(PositionReporter);

double PositionReporter::nextOffset = 0;

void PositionReporter::initialize() {
    // Get parameters
    periodMs = par("periodMs");
    printReports = par("printReports");
    report = par("report");

    // Get out gate identifier
    lower802154LayerOut = findGate("lower802154LayerOut");

    // Get data dumper module
    dumper = check_and_cast<Dumper *>(getModuleByPath("dumper"));

    // Schedule first reporting event
    double offset = ((double)(std::rand() % periodMs)) / 1000.0;//  nextOffset;
    //double offset = nextOffset;
    //nextOffset += 0.002;
    std::cout << "Offset: " << offset << std::endl;
    this->scheduleAt(offset, &event);
}

void PositionReporter::handleMessage(cMessage *msg) {
    if(msg == &event) {
        handleTimerEvent(msg);
    } else {
        PositionPacket *positionPacket = check_and_cast<PositionPacket*>(msg);
        handlePositionUpdate(positionPacket);
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

    // Schedule next position reporting event
    this->scheduleAt(simTime() + SimTime(periodMs, SIMTIME_MS), msg);
}

inet::Coord PositionReporter::getPosition(int moduleId) {
    cModule *module = getSimulation()->getModule(moduleId);
    inet::IMobility *mobility = check_and_cast<inet::IMobility *>(module->getSubmodule("mobility"));
    return mobility->getCurrentPosition();
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

    // Attach destination address
    inet::SimpleLinkLayerControlInfo* ctrl = new inet::SimpleLinkLayerControlInfo();
    ctrl->setDest(inet::MACAddress::BROADCAST_ADDRESS);
    packet->setControlInfo(ctrl);

    // Send packet
    send(packet, lower802154LayerOut);
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

        dumper->dump(lattency, distance, gtDistance);

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

