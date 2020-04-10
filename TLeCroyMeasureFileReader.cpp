/*
 * TLeCroyMeasureFileReader.cpp
 *
 *  Created on: Apr 24, 2019
 *      Author: mss
 */

#include "TLeCroyMeasureFileReader.h"

namespace std {

TLeCroyMeasureFileReader::TLeCroyMeasureFileReader() {
	// TODO Auto-generated constructor stub

}

TLeCroyMeasureFileReader::~TLeCroyMeasureFileReader() {
	// TODO Auto-generated destructor stub
}

int16_t TLeCroyMeasureFileReader::ReadLine(const string& sLine, const int32_t iLine){
	stringstream ssLine(sLine);
	ssLine.imbue(locale(ssLine.getloc(), new comma_is_space)); 	// Set comma as delimiter

	int16_t iCol=0;
	ssLine.clear();
	ssLine.seekg(ios_base::beg);	// go back to the beginning of the line
	TSingleLeCroyMeasureLine singleLine;	// temp object to store read line in
	vector<string> vItem;
	while(true){
		string sItem;
		ssLine >> sItem;
		if(!ssLine.good()){
			break;
		}
		if(iCol==0 && iLine==0){ // get rid of some non-printable characters in front
			sItem = sItem.substr(3);
		}
		vItem.push_back(sItem);
		iCol++;
	}
	singleLine.SetItem(vItem);
	vStorage.push_back(singleLine);
	return 0;
}

int16_t TLeCroyMeasureFileReader::PrintRaw(ostream &out){
	cout << "Printing raw file contents: " << GetFilePath() << endl;
	for(uint32_t ielement=0; ielement<vStorage.size(); ielement++){
		vStorage.at(ielement).Print(out);
	}
	return 0;
}

int16_t TLeCroyMeasureFileReader::Print(ostream &out){
	cout << "Printing reorganized file contents: " << GetFilePath() << endl;
	for(uint32_t ielement=0; ielement<vMeasureStorage.size(); ielement++){
		vMeasureStorage.at(ielement).Print(out);
	}
	return 0;
}

void TLeCroyMeasureFileReader::ResetData(){
	// Nothing to delete additionally to what base class handles
}
void TLeCroyMeasureFileReader::InitLutVectors(){
	// Nothing to init
}
void TLeCroyMeasureFileReader::FillLutVectors(){
	// Nothing to fill
}

int TLeCroyMeasureFileReader::ReorganizeStorage(){
	if(vStorage.size() != 4){
		cout << "<E> TLeCroyMeasureFileReader::ReorganizeStorage(): Wrong number of lines (" << vStorage.size() << " != 4) in this file: " << GetFilePath() << endl;
		return -1;
	}

	// Check that all vectors stored in vStorage have the same number of elements
	uint32_t nElements1=0, nElements0=0;
	for(uint32_t iline=1; iline<vStorage.size(); iline++){
		nElements1 = vStorage.at(iline).GetItem().size();
		nElements0 = vStorage.at(iline-1).GetItem().size();
		if(nElements1 != nElements0){
			cout << "<E> TLeCroyMeasureFileReader::ReorganizeStorage(): Wrong number of items (";
			cout << iline << ":" << nElements1 << " != " << iline-1 << ":" << nElements0 << endl;
			cout << "      in this file: " << GetFilePath() << endl;
			return -2;
		}
	}

	// Reorganization assumes the file structure does not change
	//   ie. first line corresponds to name, second line - to unit, and third line stores value
	// The first element is omitted, because it is the name of the line
	for(uint32_t ielement=1; ielement<nElements0; ielement++){
		TMeasureStorage ms;
		ms.SetMeasureType(vStorage.at(0).GetItem().at(ielement));
		ms.SetUnit(vStorage.at(1).GetItem().at(ielement));

		// Convert the value from string to double
		double value = 0;
		stringstream ssValue(vStorage.at(2).GetItem().at(ielement));
		ssValue >> value;
		if(!(ssValue.good() || ssValue.eof())){	// check if there were stringstream conversion errors
			cerr << "<E> TLeCroyMeasureFileReader::ReorganizeStorage(): Value conversion problems at element: " << ielement << ":" << ssValue.str() << " -> " << value;
			cerr << " (eof,fail,bad)=(" << ssValue.eof() << "," << ssValue.fail() << "," << ssValue.bad() << ")" << endl;
			return -4;
		}
		ms.SetValue(value);
		vMeasureStorage.push_back(ms);
	}
	return 0;
}

vector<TMeasureStorage>& TLeCroyMeasureFileReader::GetStorage(){
	return vMeasureStorage;
}

} /* namespace std */
