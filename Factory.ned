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

import inet.environment.common.PhysicalEnvironment;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;

network Factory
{
    parameters:
        int numRobots;
        int numHumans;
        int numJammers;

        @display("bgb=50,50");
    submodules:
        environment: PhysicalEnvironment {
            parameters:
                @display("p=0,0");
        }

        radioMedium: Ieee80211ScalarRadioMedium {
            parameters:
                @display("p=15,15");
        }

        robots[numRobots]: Robot;

        humans[numHumans]: Human {
            @display("p=291,60");
        }
        
        delaysLogger: DataLogger {
            name = "delays";
        }
        
        jammers[numJammers]: Jammer;

	connections allowunconnected:
}
