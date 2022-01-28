/*
 * TLeCroyWaveformFileReader.h
 *
 *  Created on: Apr 24, 2019
 *      Author: mss
 */

#ifndef TLECROYWAVEFORMFILEREADER_H_
#define TLECROYWAVEFORMFILEREADER_H_

#include <locale>
#include <iostream>

#include "TSingleLeCroyWaveformLine.h"
#include "../src/TAsciiFileReader.cpp" // cpp file needed because TAsciiFileReader is a template
#include "helpers.h"

namespace std {

class TLeCroyWaveformFileReader : public TAsciiFileReader<TSingleLeCroyWaveformLine> {
private:
	int16_t nCols = 2;
	bool isHeader = true;
	void InitLutVectors();										// VIIH = virtual inherited, implemented here
	void FillLutVectors();										// VIIH

public:
	TLeCroyWaveformFileReader();
	virtual ~TLeCroyWaveformFileReader();

	// Declarations of virtual methods declared in parent class, which are defined here
	int16_t ReadLine(const string& sLine, const int32_t iLine);	// VIIH
	int16_t Print(ostream&);									// VIIH
	void ResetData();											// VIIH
};

} /* namespace std */

#endif /* TLECROYWAVEFORMFILEREADER_H_ */
