/*
 * TSingleLeCroyWaveformLine.cpp
 *
 *  Created on: Apr 24, 2019
 *      Author: mss
 */

#include "TSingleLeCroyWaveformLine.h"

namespace std {

TSingleLeCroyWaveformLine::TSingleLeCroyWaveformLine() {
	time = 0;
	amplitude = 0;
}

TSingleLeCroyWaveformLine::~TSingleLeCroyWaveformLine() {
	// Nothing needed here
}

void TSingleLeCroyWaveformLine::Print(ostream &out){
	out << left << setw(20) << GetTime();
	out << left << setw(20) << GetAmplitude();
	out << endl;
}

double std::TSingleLeCroyWaveformLine::GetAmplitude() const {
	return amplitude;
}

void std::TSingleLeCroyWaveformLine::SetAmplitude(double amplitude) {
	this->amplitude = amplitude;
}

double std::TSingleLeCroyWaveformLine::GetTime() const {
	return time;
}

void std::TSingleLeCroyWaveformLine::SetTime(double time) {
	this->time = time;
}

} /* namespace std */
