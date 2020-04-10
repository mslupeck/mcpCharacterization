/*
 * cliParams.h
 *
 *  Created on: Aug 31, 2017
 *      Author: mss
 */

#ifndef GLOBALPARAMS_H_
#define GLOBALPARAMS_H_

#include "stdint.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

class cliParams {
private:
	std::vector<std::string> vName;
	std::vector<std::string> vDescription;
	std::vector<bool> vIsRequired;
	std::vector<std::string> vContent;

protected:
	std::string GetParName(uint16_t i) const;
	std::string GetParDescription(uint16_t i) const;
	uint16_t CalculateNumberOfRequiredPars() const;
	int16_t CheckRequiredCliPars();

public:
	cliParams();
	bool AddPar(const std::string& parName, const std::string& parDescription, const bool isRequired=false);
	bool SetPar(const std::string& parName, const std::string& parContent);
	void PrintPars(std::ostream& out, std::string parPrefix) const;
	void PrintContents(std::ostream& out) const;

	std::string GetParString	(const std::string& parName) const;
	int 		GetParInt		(const std::string& parName) const;
	float 		GetParFloat		(const std::string& parName) const;
};

#endif /* GLOBALPARAMS_H_ */
