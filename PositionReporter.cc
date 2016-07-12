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

void PositionReporter::initialize() {
    // Get parameters
    periodMs = par("periodMs");
    printReports = par("printReports");

    // Get out gate identifier
    lower802154LayerOut = findGate("lower802154LayerOut");

    // Get data dumper module
    dumper = check_and_cast<Dumper *>(getModuleByPath("dumper"));

    // Schedule first reporting event
    this->scheduleAt(0, &event);
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
    others[packet->getId()] = {packet->getId(), packet->getX(), packet->getY(), packet->getTime()};
}

void PositionReporter::handleTimerEvent(cMessage *msg) {
    sendPositionUpdate();

    collectDelays();

    // Schedule next position reporting event
    this->scheduleAt(simTime() + SimTime(periodMs, SIMTIME_MS), msg);
}

inet::Coord PositionReporter::getPosition() {
    inet::IMobility *mobility = check_and_cast<inet::IMobility *>(getParentModule()->getSubmodule("mobility"));
    return mobility->getCurrentPosition();
}

void PositionReporter::sendPositionUpdate() {
    // Get position and time
    inet::Coord position = getPosition();
    double time = simTime().dbl();

    // Construct packet
    PositionPacket *packet = new PositionPacket();
    packet->setId(getId());
    packet->setX(position.x);
    packet->setY(position.y);
    packet->setTime(time);

    // Attach destination address
    inet::SimpleLinkLayerControlInfo* ctrl = new inet::SimpleLinkLayerControlInfo();
    ctrl->setDest(inet::MACAddress::BROADCAST_ADDRESS);
    packet->setControlInfo(ctrl);

    // Send packet
    send(packet, lower802154LayerOut);
}

void PositionReporter::collectDelays() {
    inet::Coord position = getPosition();
    double time = simTime().dbl();

    if(printReports) {
        std::cout << ">>> Report from node id: " << getId() << std::endl;
    }
    for (auto& kv : others) {
        double lattency = time - kv.second.time;
        double distance = sqrt((kv.second.x - position.x) * (kv.second.x - position.x) + (kv.second.y - position.y) * (kv.second.y - position.y));

        dumper->dump(lattency, distance);

        if(printReports) {
            std::cout << "id:" << kv.first << " lattency: " << lattency << " distance: " << distance << std::endl;
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

