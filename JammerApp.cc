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
#include <inet/linklayer/common/Ieee802Ctrl.h>

#include "JammerPacket_m.h"
#include "JammerApp.h"

Define_Module(JammerApp);

void JammerApp::initialize() {
    periodMs = par("periodMs");
    packetSize = par("packetSize");

    lowerLayerOut = findGate("lowerLayerOut");

    this->scheduleAt(getNextJammingTime(), &event);

    std::cout << "Jammer initialized" << std::endl;
}

SimTime JammerApp::getNextJammingTime() {
    return simTime() + SimTime(std::rand() % periodMs, SIMTIME_MS);
}

void JammerApp::handleMessage(cMessage *msg) {
    if(msg == &event) {
        JammerPacket *packet = new JammerPacket();
        inet::Ieee802Ctrl *ctrl = new inet::Ieee802Ctrl();
        ctrl->setDest(inet::MACAddress::BROADCAST_ADDRESS);
        packet->setControlInfo(ctrl);
        packet->setByteLength(packetSize);
        send(packet, lowerLayerOut);
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
