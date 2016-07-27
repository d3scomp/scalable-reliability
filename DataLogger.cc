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

#include <ctime>
#include <string>

#include "DataLogger.h"

Define_Module(DataLogger);

void DataLogger::initialize() {
    std::string name = par("name");

    // Open data file
    dataFile.open("logs/" + name + "." + std::to_string(std::time(0)) + ".txt");
}

std::ostream& DataLogger::getStream() {
    return dataFile;
}

void DataLogger::deleteModule() {
    dataFile.close();
    cSimpleModule::deleteModule();
}
