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
import inet.physicallayer.ieee802154.packetlevel.Ieee802154NarrowbandScalarRadioMedium;

network Factory {
	parameters:
		int numRobots;
		int numHumans;
		
//		double playgroundSizeX @unit(m) = default(50m);
//      double playgroundSizeY @unit(m) = default(50m);
//      double playgroundSizeZ @unit(m) = default(10m);

	submodules:
		environment: PhysicalEnvironment {
			parameters:
				@display("p=0,0");
		}
		
		radioMedium: Ieee802154NarrowbandScalarRadioMedium {
			parameters:
				@display("p=50,50");
        }
        
		robots[numRobots]: Robot {
//			parameters:
//				@display("r=,,#707070");
        }

		humans[numHumans]: Human {
//			parameters:
//				@display("r=,,#707070");
        }

//        channelControl: ChannelControl
//        {
//            parameters:
//                @display("p=60,50");
//        }

//        configurator: IPv4NetworkConfigurator
//        {
//            @display("p=140,50");
//        }
}