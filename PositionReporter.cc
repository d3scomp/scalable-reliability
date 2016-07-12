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

#include <inet/mobility/contract/IMobility.h>
#include <inet/linklayer/common/MACAddress.h>
#include <inet/linklayer/common/SimpleLinkLayerControlInfo.h>

#include "PositionPacket_m.h"

#include "PositionReporter.h"

Define_Module(PositionReporter);

void PositionReporter::initialize() {
    std::cout << "Position reporter - initialize" << std::endl;
    // Get parameters
    periodMs = par("periodMs");

    // Schedule first reporting event
    this->scheduleAt(0, &event);
}

void PositionReporter::handleMessage(cMessage *msg) {
    // React to custom event
    if(msg == &event) {
        // Get position and time
        inet::IMobility *mobility = check_and_cast<inet::IMobility *>(getParentModule()->getSubmodule("mobility"));
        inet::Coord position = mobility->getCurrentPosition();
        SimTime time = simTime();

        // Construct packet
        PositionPacket *packet = new PositionPacket();
        packet->setX(position.x);
        packet->setY(position.y);
        packet->setTime(time.dbl());

        // Attach destination address
        inet::SimpleLinkLayerControlInfo* ctrl = new inet::SimpleLinkLayerControlInfo();
        ctrl->setDest(inet::MACAddress::BROADCAST_ADDRESS);
        packet->setControlInfo(ctrl);

        // Send packet
        send(packet, findGate("lower802154LayerOut"));

        // Schedule next position reporting event
        this->scheduleAt(simTime() + SimTime(periodMs, SIMTIME_MS), &event);
    }
}

void PositionReporter::deleteModule() {
    // Ensure we are not deleting scheduled event
    cancelEvent(&event);
    cSimpleModule::deleteModule();
}

