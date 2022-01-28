/*
 * TSingleLeCroyMeasureLine.cpp
 *
 *  Created on: Apr 24, 2019
 *      Author: mss
 */

#include "../inc/TSingleLeCroyMeasureLine.h"

namespace std {

TSingleLeCroyMeasureLine::TSingleLeCroyMeasureLine() {
	// TODO Auto-generated constructor stub
}

const vector<string>& TSingleLeCroyMeasureLine::GetItem() const {
	return vItem;
}

void TSingleLeCroyMeasureLine::Print(ostream &out){
	for(uint16_t i=0; i<vItem.size(); i++){
		out << left << setw(12) << vItem.at(i);
	}
	out << endl;
}

void TSingleLeCroyMeasureLine::SetItem(const vector<string>& item) {
	this->vItem = item;
}

TSingleLeCroyMeasureLine::~TSingleLeCroyMeasureLine() {
	// TODO Auto-generated destructor stub
}

} /* namespace std */
