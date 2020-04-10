/*
 * TLeCroyWaveformFileReader.cpp
 *
 *  Created on: Apr 24, 2019
 *      Author: mss
 */

#include "TLeCroyWaveformFileReader.h"


namespace std {

TLeCroyWaveformFileReader::TLeCroyWaveformFileReader() {
	// Nothing here
}

TLeCroyWaveformFileReader::~TLeCroyWaveformFileReader() {
	// Nothing here
}

int16_t TLeCroyWaveformFileReader::ReadLine(const string& sLine, const int32_t iLine){
	stringstream ssLine(sLine);
	ssLine.imbue(locale(ssLine.getloc(), new comma_is_space)); 	// Set comma as delimiter

	int16_t iCol=0;
	if(!isHeader){
		string sWord;
		while(!ssLine.eof()){ // first parsing to count the number of columns
			ssLine >> sWord;
			iCol++;
		}
		if(iCol != nCols){
			cerr << "<E> TLeCroyWaveformFileReader::ReadLine(): Wrong number of columns when parsing line " << iLine << ": " << iCol << " != " << nCols << endl;
			cerr << "                                -> " << ssLine.str() << endl;
			return -1;
		}
		else{
			ssLine.clear();
			ssLine.seekg(ios_base::beg);	// go back to the beginning of the line
			TSingleLeCroyWaveformLine singleLine;	// temp object to store read line in
			double t=0, a=0;
			ssLine >> t;
			if(!(ssLine.good() || ssLine.eof())){	// check if there were stringstream conversion errors
				vStorage.clear();
				cerr << "<E> TLeCroyWaveformFileReader::::ReadLine(): Time conversion problems at line " << iLine;
				cerr << " (eof,fail,bad)=(" << ssLine.eof() << "," << ssLine.fail() << "," << ssLine.bad() << ")" << endl;
				return -4;
			}
			ssLine >> a;
			if(!(ssLine.good() || ssLine.eof())){	// check if there were stringstream conversion errors
				vStorage.clear();
				cerr << "<E> TLeCroyWaveformFileReader::::ReadLine(): Amplitude conversion problems at line " << iLine;
				cerr << " (eof,fail,bad)=(" << ssLine.eof() << "," << ssLine.fail() << "," << ssLine.bad() << ")" << endl;
				return -4;
			}
			singleLine.SetTime(t);
			singleLine.SetAmplitude(a);
			vStorage.push_back(singleLine);
		}
	}

	if(sLine.find_first_of("Time,Ampl") == 0){ // The last comment line
		isHeader = false;
		return 0;
	}
	return 0;
}

int16_t TLeCroyWaveformFileReader::Print(ostream &out){
	cout << "Printing file contents: " << GetFilePath() << endl;
	cout << vStorage.size() << endl;
	for(uint32_t iline=0; iline<vStorage.size(); iline++){
		vStorage.at(iline).Print(out);
	}

	return 0;
}

void TLeCroyWaveformFileReader::ResetData(){
	// Nothing to delete additionally to what base class handles
}
void TLeCroyWaveformFileReader::InitLutVectors(){
	// Nothing to init
}
void TLeCroyWaveformFileReader::FillLutVectors(){
	// Nothing to fill
}

} /* namespace std */
