/*
 * TSingleLeCroyMeasureLine.h
 *
 *  Created on: Apr 24, 2019
 *      Author: mss
 */

#ifndef TSINGLELECROYMEASURELINE_H_
#define TSINGLELECROYMEASURELINE_H_

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

namespace std {

class TSingleLeCroyMeasureLine {
private:
	vector<string> vItem;

public:
	TSingleLeCroyMeasureLine();
	virtual ~TSingleLeCroyMeasureLine();

	void Print(ostream &out);

	const vector<string>& GetItem() const;
	void SetItem(const vector<string>& item);
};

} /* namespace std */

#endif /* TSINGLELECROYMEASURELINE_H_ */
