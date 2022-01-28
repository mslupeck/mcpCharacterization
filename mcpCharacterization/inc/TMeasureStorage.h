/*
 * TMeasureStorage.h
 *
 *  Created on: Apr 24, 2019
 *      Author: mss
 */

#ifndef TMEASURESTORAGE_H_
#define TMEASURESTORAGE_H_

#include <string>
#include <iostream>
#include <iomanip>

namespace std {

class TMeasureStorage {
private:
	string sMeasureType;
	string sUnit;
	double value;

public:
	TMeasureStorage();
	virtual ~TMeasureStorage();

	void Print(ostream &out);

	const string& GetMeasureType() const;
	void SetMeasureType(const string& measureType);
	const string& GetUnit() const;
	void SetUnit(const string& unit);
	double GetValue() const;
	void SetValue(double value);
};

} /* namespace std */

#endif /* TMEASURESTORAGE_H_ */
