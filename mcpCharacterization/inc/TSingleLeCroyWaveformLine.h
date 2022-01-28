/*
 * TSingleLeCroyWaveformLine.h
 *
 *  Created on: Apr 24, 2019
 *      Author: mss
 */

#ifndef TSINGLELECROYWAVEFORMLINE_H_
#define TSINGLELECROYWAVEFORMLINE_H_

#include <string>
#include <iostream>
#include <iomanip>

namespace std {

class TSingleLeCroyWaveformLine {
private:
	double time;
	double amplitude;

public:
	TSingleLeCroyWaveformLine();
	virtual ~TSingleLeCroyWaveformLine();

	void Print(ostream &out);

	// Setters and getters
	double GetAmplitude() const;
	void SetAmplitude(double amplitude);
	double GetTime() const;
	void SetTime(double time);
};

} /* namespace std */

#endif /* TSINGLELECROYWAVEFORMLINE_H_ */
