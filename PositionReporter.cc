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

#include "PositionReporter.h"

Define_Module(PositionReporter);

void PositionReporter::initialize() {
    std::cout << "Position reporter - initialize" << std::endl;
    // Schedule first reporting event
    this->scheduleAt(0, &event);
}

void PositionReporter::handleMessage(cMessage *msg) {
    // React to custom event
    if(msg == &event) {
        // TODO: Broadcast position
        std::cout << "Position reporter - event" << std::endl;

        // Schedule next position reporting event
        this->scheduleAt(simTime() + SimTime(20, SIMTIME_MS), &event);
    }
}
