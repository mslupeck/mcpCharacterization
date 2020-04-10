/*
 * TGainLogFileReader.cpp
 *
 *  Created on: Apr 30, 2019
 *      Author: mss
 */

#include "TGainLogFileReader.h"

namespace std {

TGainLogFileReader::TGainLogFileReader() {
	comment = "";
}

TGainLogFileReader::~TGainLogFileReader() {
	// TODO Auto-generated destructor stub
}

int16_t TGainLogFileReader::ReadLine(const string& sLine, const int32_t iLine){
	stringstream ssLine(sLine);
	if(sLine.find_first_of("#") == 0){ // The comment line
		comment += sLine + "\n";
		return 0;
	}

	int16_t iCol=0;
	string sWord;
	// Ignore empty lines
	if(HasOnlyWhitespaces(sLine)){
		return 0;
	}
	while(!ssLine.eof()){ // first parsing to count the number of columns
		ssLine >> sWord;
		iCol++;
	}
	if(iCol != nCols){
		cerr << "<E> TGainLogFileReader::ReadLine(): Wrong number of columns when parsing line " << iLine << ": " << iCol << " != " << nCols << endl;
		cerr << "                                -> " << ssLine.str() << endl;
		return -1;
	}
	else{
		ssLine.clear();
		ssLine.seekg(ios_base::beg);	// go back to the beginning of the line
		TSingleGainLogLine singleLine;	// temp object to store read line in
		double u=0, i=0, a=0;
		int file = -1;
		ssLine >> u;
		if(!(ssLine.good() || ssLine.eof())){	// check if there were stringstream conversion errors
			vStorage.clear();
			cerr << "<E> TGainLogFileReader::::ReadLine(): Voltage conversion problems at line " << iLine;
			cerr << " (eof,fail,bad)=(" << ssLine.eof() << "," << ssLine.fail() << "," << ssLine.bad() << ")" << endl;
			return -2;
		}
		ssLine >> i;
		if(!(ssLine.good() || ssLine.eof())){	// check if there were stringstream conversion errors
			vStorage.clear();
			cerr << "<E> TGainLogFileReader::::ReadLine(): Current conversion problems at line " << iLine;
			cerr << " (eof,fail,bad)=(" << ssLine.eof() << "," << ssLine.fail() << "," << ssLine.bad() << ")" << endl;
			return -3;
		}
		ssLine >> a;
		if(!(ssLine.good() || ssLine.eof())){	// check if there were stringstream conversion errors
			vStorage.clear();
			cerr << "<E> TGainLogFileReader::::ReadLine(): Reference amplitude conversion problems at line " << iLine;
			cerr << " (eof,fail,bad)=(" << ssLine.eof() << "," << ssLine.fail() << "," << ssLine.bad() << ")" << endl;
			return -4;
		}
		ssLine >> file;
		if(!(ssLine.good() || ssLine.eof())){	// check if there were stringstream conversion errors
			vStorage.clear();
			cerr << "<E> TGainLogFileReader::::ReadLine(): File number conversion problems at line " << iLine;
			cerr << " (eof,fail,bad)=(" << ssLine.eof() << "," << ssLine.fail() << "," << ssLine.bad() << ")" << endl;
			return -5;
		}
		singleLine.SetU(u);
		singleLine.SetI(i);
		singleLine.SetRef(a);
		singleLine.SetFileNumber(file);
		vStorage.push_back(singleLine);
	}

	return 0;
}

int16_t TGainLogFileReader::Print(ostream &out){
	cout << "Printing file contents: " << GetFilePath() << endl;
	cout << vStorage.size() << endl;
	for(uint32_t iline=0; iline<vStorage.size(); iline++){
		vStorage.at(iline).Print(out);
	}

	return 0;
}

void TGainLogFileReader::ResetData(){
	// Nothing to delete additionally to what base class handles
}
void TGainLogFileReader::InitLutVectors(){
	// Nothing to init
}
void TGainLogFileReader::FillLutVectors(){
	// Nothing to fill
}

} /* namespace std */
