/*
 * TSingleGainLogLine.cpp
 *
 *  Created on: Apr 30, 2019
 *      Author: mss
 */

#include "../inc/TSingleGainLogLine.h"

namespace std {

TSingleGainLogLine::TSingleGainLogLine() {
	u = 0;
	i = 0;
	aRef = 0;
	fileNumber = -1;
}

TSingleGainLogLine::~TSingleGainLogLine() {
	// TODO Auto-generated destructor stub
}

void TSingleGainLogLine::Print(ostream &out){
	out << left << setw(7) << u;
	out << left << setw(9) << i;
	out << left << setw(9) << aRef;
	out << right << setw(3) << fileNumber;
	out << endl;
}

double TSingleGainLogLine::GetRef() const {
	return aRef;
}

void TSingleGainLogLine::SetRef(double ref) {
	aRef = ref;
}

int TSingleGainLogLine::GetFileNumber() const {
	return fileNumber;
}

void TSingleGainLogLine::SetFileNumber(int fileNumber) {
	this->fileNumber = fileNumber;
}

double TSingleGainLogLine::GetI() const {
	return i;
}

void TSingleGainLogLine::SetI(double i) {
	this->i = i;
}

double TSingleGainLogLine::GetU() const {
	return u;
}

void TSingleGainLogLine::SetU(double u) {
	this->u = u;
}

} /* namespace std */
