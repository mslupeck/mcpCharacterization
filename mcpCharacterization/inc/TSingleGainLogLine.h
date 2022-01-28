/*
 * TSingleGainLogLine.h
 *
 *  Created on: Apr 30, 2019
 *      Author: mss
 */

#ifndef TSINGLEGAINLOGLINE_H_
#define TSINGLEGAINLOGLINE_H_

#include <string>
#include <iostream>
#include <iomanip>

namespace std {

class TSingleGainLogLine {
private:
	double u;        // Voltage of the MCP-PMT
	double i;        // Current of the MCP-PMT
	double aRef;     // Amplitude of the reference PMT
	int fileNumber;  // Number of corresponding file

public:
	TSingleGainLogLine();
	virtual ~TSingleGainLogLine();

	void Print(ostream &out);

	double GetRef() const;
	void SetRef(double ref);
	int GetFileNumber() const;
	void SetFileNumber(int fileNumber);
	double GetI() const;
	void SetI(double i);
	double GetU() const;
	void SetU(double u);
};

} /* namespace std */

#endif /* TSINGLEGAINLOGLINE_H_ */
