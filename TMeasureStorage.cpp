/*
 * TMeasureStorage.cpp
 *
 *  Created on: Apr 24, 2019
 *      Author: mss
 */

#include "TMeasureStorage.h"

namespace std {

TMeasureStorage::TMeasureStorage() {
	sMeasureType = "x";
	sUnit = "x";
	value = 0;
}

TMeasureStorage::~TMeasureStorage() {
	// TODO Auto-generated destructor stub
}

void TMeasureStorage::Print(ostream &out){
	out << left << setw(15) << sMeasureType;
	out << left << setw(5) << sUnit;
	out << left << setw(14) << value;
	out << endl;
}

const string& TMeasureStorage::GetMeasureType() const {
	return sMeasureType;
}

void TMeasureStorage::SetMeasureType(const string& measureType) {
	sMeasureType = measureType;
}

const string& TMeasureStorage::GetUnit() const {
	return sUnit;
}

void TMeasureStorage::SetUnit(const string& unit) {
	sUnit = unit;
}

double TMeasureStorage::GetValue() const {
	return value;
}

void TMeasureStorage::SetValue(double value) {
	this->value = value;
}


} /* namespace std */
