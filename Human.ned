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

import inet.mobility.single.GaussMarkovMobility;
import inet.linklayer.ieee80211.Ieee80211Nic;

//
// Human
//
module Human {
	parameters:
		@node;
		@networkNode();
        
	gates:
		input radio80211In @directIn;
		inout direct[];
		
	submodules:
		mobility: GaussMarkovMobility {
			parameters:
			    alpha = 0.5;
			    variance = 1.5;
		    	margin = 10m;
		    	speed = 1mps;
		}
		
		nic80211: Ieee80211Nic;
		
		positionReporter: PositionReporter;
	
	connections allowunconnected:
	    nic80211.upperLayerOut --> positionReporter.lowerLayerIn;
		nic80211.upperLayerIn <-- positionReporter.lowerLayerOut;
}
