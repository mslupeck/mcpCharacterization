/*
 * TLeCroyMeasureFileReader.h
 *
 *  Created on: Apr 24, 2019
 *      Author: mss
 */

#ifndef TLECROYMEASUREFILEREADER_H_
#define TLECROYMEASUREFILEREADER_H_

#include <locale>
#include <iostream>

#include "TSingleLeCroyMeasureLine.h"
#include "TMeasureStorage.h"
#include "../src/TAsciiFileReader.cpp" // cpp file needed because TAsciiFileReader is a template
#include "helpers.h"

namespace std {

class TLeCroyMeasureFileReader : public TAsciiFileReader<TSingleLeCroyMeasureLine> {
private:
	void InitLutVectors();										// VIIH = virtual inherited, implemented here
	void FillLutVectors();										// VIIH
	vector<TMeasureStorage> vMeasureStorage;	                // additional storage, filled by ReorganizeStorage() function

public:
	TLeCroyMeasureFileReader();
	virtual ~TLeCroyMeasureFileReader();

	// Declarations of virtual methods declared in parent class, which are defined here
	int16_t ReadLine(const string& sLine, const int32_t iLine);	// VIIH
	int16_t Print(ostream&);									// VIIH
	int16_t PrintRaw(ostream &out);
	void ResetData();											// VIIH

	int ReorganizeStorage();
	std::vector<TMeasureStorage>& GetStorage();
};

} /* namespace std */

#endif /* TLECROYMEASUREFILEREADER_H_ */
