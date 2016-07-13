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

#include <cstdlib>

#include <inet/linklayer/common/MACAddress.h>
#include <inet/linklayer/common/SimpleLinkLayerControlInfo.h>

#include "JammerPacket_m.h"
#include "JammerApp.h"

Define_Module(JammerApp);

void JammerApp::initialize() {
    periodMs = par("periodMs");

    lower802154LayerOut = findGate("lower802154LayerOut");

    this->scheduleAt(getNextJammingTime(), &event);

    std::cout << "Jammer initialized" << std::endl;
}

SimTime JammerApp::getNextJammingTime() {
    double next = ((double)(std::rand() % periodMs)) / 1000.0;
    return simTime() + next;
}

void JammerApp::handleMessage(cMessage *msg) {
    if(msg == &event) {
        JammerPacket *packet = new JammerPacket();
        inet::SimpleLinkLayerControlInfo* ctrl = new inet::SimpleLinkLayerControlInfo();
        ctrl->setDest(inet::MACAddress::BROADCAST_ADDRESS);
        packet->setControlInfo(ctrl);
        send(packet, lower802154LayerOut);
        this->scheduleAt(getNextJammingTime(), msg);
    } else {
        delete msg;
    }
}

void JammerApp::deleteModule() {
    // Ensure we are not deleting scheduled event
    cancelEvent(&event);

    cSimpleModule::deleteModule();
}
