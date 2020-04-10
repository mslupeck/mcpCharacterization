/*
 * TGainLogFileReader.h
 *
 *  Created on: Apr 30, 2019
 *      Author: mss
 */

#ifndef TGAINLOGFILEREADER_H_
#define TGAINLOGFILEREADER_H_

#include "TSingleGainLogLine.h"
#include "TAsciiFileReader.cpp" // cpp needed because TAsciiFileReader is a template
#include "helpers.h"

namespace std {

class TGainLogFileReader : public TAsciiFileReader<TSingleGainLogLine>  {
private:
	string comment;
	int16_t nCols = 4;
	void InitLutVectors();										// VIIH = virtual inherited, implemented here
	void FillLutVectors();										// VIIH

public:
	TGainLogFileReader();
	virtual ~TGainLogFileReader();

	// Declarations of virtual methods declared in parent class, which are defined here
	int16_t ReadLine(const string& sLine, const int32_t iLine);	// VIIH
	int16_t Print(ostream&);									// VIIH
	void ResetData();											// VIIH
};

} /* namespace std */

#endif /* TGAINLOGFILEREADER_H_ */
