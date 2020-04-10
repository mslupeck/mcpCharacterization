/*
 * globalParams.cpp
 *
 *  Created on: Aug 31, 2017
 *      Author: mss
 */

#include "cliParams.h"

cliParams::cliParams() {}

bool cliParams::SetPar(const std::string& parName, const std::string& parContent) {
	if(parName.size() < 1){
		std::cout << "<E> globalParams::SetParameter(): Parameter name is empty" << std::endl;
		return false;
	}
	if(parContent.size() < 1){
		std::cout << "<E> globalParams::SetParameter(): Parameter content is empty" << std::endl;
		return false;
	}
	for(uint16_t i=0; i<vName.size(); i++){
		if(parName.compare(vName.at(i)) == 0){
			vContent.at(i) = parContent;
			return true;
		}
	}
	std::cout << "<E> globalParams::SetParameter(): No parameter with given name found: " << parName << std::endl;
	return false;
}

bool cliParams::AddPar(const std::string& parName, const std::string& parDescription, const bool isRequired) {
	if(parName.size() < 1){
		std::cout << "<E> globalParams::AddParameter(): Parameter name is empty" << std::endl;
		return false;
	}
	for(uint16_t i=0; i<vName.size(); i++){
		if(parName.compare(vName.at(i)) == 0){
			std::cout << "<E> globalParams::AddParameter(): Parameter already exists: " << parName << std::endl;
			return false;
		}
	}
	vName.push_back(parName);
	vDescription.push_back(parDescription);
	vIsRequired.push_back(isRequired);
	vContent.push_back("");
	return true;
}

std::string cliParams::GetParString(const std::string& parName) const {
	bool entryExists = false;
	for(uint16_t i=0; i<vName.size(); i++){
		if(parName.compare(vName.at(i)) == 0){
			entryExists = true;
			return vContent.at(i);
		}
	}
	if(!entryExists){
		std::cout << "<E> globalParams::GetParameter(): Unknown parameter name: " << parName << std::endl;
		std::cout << "  <I> Available parameter names:" << parName << std::endl;
		for(uint16_t i=0; i<vName.size(); i++){
			std::cout << "  <I> " << vName.at(i) << std::endl;
		}
		return "";
	}
	return "";
}

int cliParams::GetParInt(const std::string& parName) const {
	int result;
	std::stringstream ss;
	ss << GetParString(parName);
	ss >> result;
	if(ss.fail()){
		std::cout << "<E> cliParams::GetParameterInt(): Conversion of parameter " << parName << " (" << ss.str() << ") to int failed." << std::endl;
		return -999999;
	}
	return result;
}

float cliParams::GetParFloat(const std::string& parName) const {
	float result;
	std::stringstream ss;
	ss << GetParString(parName);
	ss >> result;
	if(ss.fail()){
		std::cout << "<E> cliParams::GetParameterFloat(): Conversion of parameter " << parName << " (" << ss.str() << ") to float failed." << std::endl;
		return -999999;
	}
	return result;
}

void cliParams::PrintPars(std::ostream& out, std::string parPrefix) const {
	uint16_t maxParNameLength=0;
	for(uint16_t i=0; i<vName.size(); i++){
		if(maxParNameLength < vName.at(i).size()){
			maxParNameLength = vName.at(i).size();
		}
	}
	maxParNameLength += 4;
	for(uint16_t i=0; i<vName.size(); i++){
		out << parPrefix;
		out << std::setw(maxParNameLength) << std::setfill('.') << std::left << (vName.at(i)+" ") << std::setfill(' ') << " ";
		if(vIsRequired.at(i)){
			out << "*";
		}
		else{
			out << " ";
		}
		out << vDescription.at(i) << std::endl;
	}
}

void cliParams::PrintContents(std::ostream& out) const {
	for(uint16_t i=0; i<vName.size(); i++){
		out << "  <I> cliParams::PrintContents(): ";
		out << std::setw(4) << std::left << (vName.at(i)+" ");
		out << (vContent.at(i)+" ") << std::endl;
	}
}

std::string cliParams::GetParName(uint16_t i) const {
	if(i<vName.size()){
		return vName.at(i);
	}
	return "";
}

std::string cliParams::GetParDescription(uint16_t i) const {
	if(i<vDescription.size()){
		return vDescription.at(i);
	}
	return "";
}

uint16_t cliParams::CalculateNumberOfRequiredPars() const {
	uint16_t n=0;
	for(uint16_t i=0; i<vIsRequired.size(); i++){
		if(vIsRequired.at(i)){
			n++;
		}
	}
	return n;
}

int16_t cliParams::CheckRequiredCliPars(){
	uint16_t nFailed=0;
	for(uint16_t i=0; i<vIsRequired.size(); i++){
		if(vIsRequired.at(i)){
			if(vContent.at(i).size()<1){
				std::cout << "<E> cliParams::CheckRequiredCliParams(): Required parameter -" << vName.at(i) << " is not assigned a value" << std::endl;
				nFailed++;
			}
		}
	}
	if(nFailed>0){
		PrintContents(std::cout);
		return -2;
	}
	return 0;
}

