/*
 * cliParser.cpp
 *
 *  Created on: Nov 10, 2017
 *      Author: mss
 */

#include "cliParser.h"

cliParser::cliParser() {}

cliParams* cliParser::GetModeList(){
	return &modeList;
}


void cliParser::PrintUsage(std::ostream& out, std::string execName) {
	std::stringstream ssFirstLine;
	ssFirstLine << "  Usage of " << execName << "  ";
	std::string sFirstLine = ssFirstLine.str();
	out << "\n";
	for(uint16_t i=0; i<sFirstLine.size(); i++) { out << "-"; }
	out << "\n";
	out << sFirstLine;
	out << "\n";
	for(uint16_t i=0; i<sFirstLine.size(); i++) { out << "-"; }
	out << "\n";
	out << "    List of possible parameters (* means the parameter is required):\n";
	PrintPars(out, "      -");
	out << "    List of possible modes:\n";
	GetModeList()->PrintPars(out, "      ");
	out << "  Example:\n";
	out << "    ./" << execName << " -" << GetParName(0) << " parValue\n" << std::endl;
}

int16_t cliParser::Parse(int argc, char* argv[]){
	uint16_t nReqPar = 2*CalculateNumberOfRequiredPars();
	if(argc > nReqPar){
		for(int iarg=1; iarg<argc-1; iarg++){
			std::stringstream ssParName(argv[iarg]);		// [1] safe way to read any user input in c++ (prevents overflows)
			std::string sParName;							// [2] safe way to read any user input in c++ (prevents overflows)
			ssParName >> sParName;							// [3] safe way to read any user input in c++ (prevents overflows)
			if(sParName.at(0)=='-'){
				sParName.erase(0,1);
			}
			else{
				std::cout << "<E> cliParser::Parse(): Parameter name expected (starting with '-'). Found: " << sParName << std::endl;
				return -1;
			}
			std::stringstream ssParContent(argv[++iarg]);	// [1] safe way to read any user input in c++ (prevents overflows)
			std::string sParContent;						// [2] safe way to read any user input in c++ (prevents overflows)
			ssParContent >> sParContent;					// [3] safe way to read any user input in c++ (prevents overflows)
			SetPar(sParName, sParContent);
		}
	}
	else{
		std::cout << "<E> cliParser::Parse(): Too few cli parameters to be parsed. Found: " << argc-1 << ", required: " << nReqPar << std::endl;
		return -2;
	}
	return CheckRequiredCliPars();
}
