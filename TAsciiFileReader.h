/*
 * TAsciiFileReader.h
 *
 *  Created on: Mar 22, 2016
 *      Author: mss
 */

#ifndef TASCIIFILEREADER_H_
#define TASCIIFILEREADER_H_

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>

namespace std {

template <typename TStorage> class TAsciiFileReader {
private:
	int verbose;
	bool fileOpened;			// has the file been opened successfully?
	std::string filePath;		// full path
	std::string fileContents;	// whole-file-buffer
	std::string comment;		// usually the first lines of file

protected:
	vector<TStorage> vStorage;	// structured buffer for file data
	std::stringstream log;		// push all warnings here

	int16_t ReadString();					// invoked at the end of ReadFile(),
											//   parses the file data from fileContents
											//   and saves it to vector<TStorage>
	virtual int16_t ReadLine(const std::string& sLine, const int32_t iLine)=0;
											//   invoked in the center of ReadString()
	void ResetDataLow();					// Resets all data fields of the base parent class only,
	virtual void ResetData()=0;				// Resets all data fields,
	virtual void InitLutVectors()=0;
	virtual void FillLutVectors()=0;


	bool CheckVectorIndex(const std::vector<int16_t>& v, const uint32_t index);
	bool CheckVectorLutValue(const std::vector<int16_t>& v, const uint32_t index);

public:
	static bool HasOnlyWhitespaces(const string& str);

	TAsciiFileReader();
	virtual ~TAsciiFileReader();

	int16_t ReadFile(const string& path);	// opens file with all error checks and saves it to string
	streampos GetFileSize() const;			// opens the file again and checks the size
	virtual int16_t Print(ostream&)=0;		// prints contents of the vector<TStorage>, not the fileContents string

	const std::stringstream& GetLog() const;
	const std::string& GetComment() const;
	const std::string& GetFileContents() const;
	bool IsFileOpened() const;
	const std::string& GetFilePath() const;
	std::vector<TStorage>& GetStorage();
	void SetVerbose(int v);

};

} /* namespace std */

#endif /* TASCIIFILEREADER_H_ */
