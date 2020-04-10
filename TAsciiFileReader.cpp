/*
 * TAsciiFileReader.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: mss
 *  Adjusted for HV monitoring CAEN Gecko logs on 28.02.2019
 */

#include "TAsciiFileReader.h"

#ifndef TASCIIFILEREADER_CPP_
#define TASCIIFILEREADER_CPP_

namespace std {

template <typename TStorage>
bool TAsciiFileReader<TStorage>::HasOnlyWhitespaces(const string& str) {
	return str.find_first_not_of(" \n\t") == str.npos;
}

template <typename TStorage>
TAsciiFileReader<TStorage>::TAsciiFileReader() {
	ResetDataLow();
}

template <typename TStorage>
TAsciiFileReader<TStorage>::~TAsciiFileReader() {
}

template <typename TStorage>
void TAsciiFileReader<TStorage>::ResetDataLow(){
	fileOpened = false;
	filePath = "x";
	fileContents = "x";
	vStorage.clear();
}

template <typename TStorage>
bool TAsciiFileReader<TStorage>::CheckVectorIndex(const vector<int16_t>& v, const uint32_t index){
	if(index<v.size()){
		return true;
	}
	else{
		log << " <W> TAsciiFileReader<TStorage>::CheckVectorIndex: Vector index out of bounds: " << index << ">=" << v.size() << endl;
		return false;
	}
}

template <typename TStorage>
bool TAsciiFileReader<TStorage>::CheckVectorLutValue(const vector<int16_t>& v, const uint32_t index){
	if(CheckVectorIndex(v,index)){
		if(v.at(index)>=0){
			return true;
		}
		else{
			log << " <W> TAsciiFileReader<TStorage>::CheckVectorLutValue: Vector value at: " << index << " is <0: " << v.at(index) << endl;
			return false;
		}
	}
	else{
		return false;
	}
}

template <typename TStorage>
int16_t TAsciiFileReader<TStorage>::ReadFile(const string& path){
	if(IsFileOpened()){
		if(verbose>0){
			cout << " <W> TAsciiFileReader::ReadFile(): Opening another map file on top of existing one. Overwriting input form old file." << endl;
		}
		ResetDataLow();
		ResetData();
	}
	filePath = path;

	ifstream infile;
	infile.open(filePath.c_str(), ios_base::in);
	if(infile.fail()==true){
		if(verbose>0){
			cerr << " <W> TAsciiReader: Error opening ascii file: " << filePath << endl;
		}
		return -1;
	}

	if(!getline(infile,comment)){
		if(verbose>0){
			cerr << "<E> TAsciiReader: Error opening ascii file (it is probably a folder!): " << filePath << endl;
		}
		return -2;
	}

	stringstream ssFileData;
	infile.seekg(infile.beg);
	ssFileData << infile.rdbuf();
	infile.close();

	fileContents = ssFileData.str();
	int16_t err = ReadString();
	if(err == 0){
		FillLutVectors();
		fileOpened = true;
	}
	else{
		fileOpened = false;
	}
	return err;

}

template <typename TStorage>
int16_t TAsciiFileReader<TStorage>::ReadString() {
	stringstream ssFileContents(GetFileContents());
	int32_t iLine=0;
	string sLine;

	getline(ssFileContents,sLine);
	while(!ssFileContents.eof()){
		int16_t err = ReadLine(sLine, iLine);
		if(err < 0){
			return err;
		}
		if(err == 1){
			break;
		}
		iLine++;
		getline(ssFileContents,sLine);
	}
	return 0;
}
/*
template <typename TStorage>
streampos TAsciiFileReader<TStorage>::GetFileSize() const{
    streampos filesize = 0;
    ifstream f(filePath.c_str(), ios::binary);

    filesize = f.tellg();
    f.seekg( 0, ios::end );
    filesize = f.tellg() - filesize;
    f.close();

    return filesize;
}
*/
// -------------------------------
// --- Getter / setter methods ---
// -------------------------------
template <typename TStorage>
const stringstream& TAsciiFileReader<TStorage>::GetLog() const {
	return log;
}

template <typename TStorage>
const string& TAsciiFileReader<TStorage>::GetComment() const {
	return comment;
}

template <typename TStorage>
const string& TAsciiFileReader<TStorage>::GetFileContents() const {
	return fileContents;
}

template <typename TStorage>
bool TAsciiFileReader<TStorage>::IsFileOpened() const {
	return fileOpened;
}

template <typename TStorage>
const string& TAsciiFileReader<TStorage>::GetFilePath() const {
	return filePath;
}

template <typename TStorage>
vector<TStorage>& TAsciiFileReader<TStorage>::GetStorage(){
	return vStorage;
}

template <typename TStorage>
void TAsciiFileReader<TStorage>::SetVerbose(int v){
	verbose = v;
}

} /* namespace std */

#endif /* TASCIIFILEREADER_CPP_ */
