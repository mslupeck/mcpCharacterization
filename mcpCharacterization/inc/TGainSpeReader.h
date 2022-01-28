/*
 * TLeCroySpeReader.h
 *
 *  Created on: Apr 24, 2019
 *      Author: mss
 */

#ifndef TGAINSPEREADER_H_
#define TGAINSPEREADER_H_

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <stdint.h>

#include "TLeCroyWaveformFileReader.h"
#include "TLeCroyMeasureFileReader.h"
#include "TGainLogFileReader.h"

#include "TGainSpeAnalyser.h"

namespace std {

class TGainSpeReader {
	static constexpr uint16_t nPixels = 4;

	static const string sGainDirectory;
	static const string sGainPrefix;
	static const string sGainCurve;
	static const string sExt;
	static constexpr uint16_t nSpeFiles = 12;
	static const string sSpePrefix[nSpeFiles];
	static const string sSpe;
	static const string sLogDirectory;
	static const string sLogSuffix;
	static const string sLogExt;

	uint32_t nMcp;
	uint32_t hvSpe;

	string inputPath;
	vector<string> vGainCurveFilePath;
	vector<string> vSpeFilePath;

	TGainLogFileReader logReader;
	vector<TLeCroyMeasureFileReader*> vGainReader;
	vector<TLeCroyWaveformFileReader*> vSpeReader;

	int ReadLog(uint32_t nMcp);
	int GenerateScopePaths(uint32_t nMcp, uint32_t lastSpeNumber);
	bool fileExists(string &path);
	int ReadScopeFiles();

public:
	TGainSpeReader(string &inputPath);
	virtual ~TGainSpeReader();

	int ReadAll(uint32_t nMcp, vector<TSpeGainContainer*>& vGain);
};

} /* namespace std */

#endif /* TGAINSPEREADER_H_ */
