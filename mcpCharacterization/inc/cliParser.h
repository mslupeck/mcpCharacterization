/*
 * cliParser.h
 *
 *  Created on: Nov 10, 2017
 *      Author: mss
 */

#ifndef CLIPARSER_H_
#define CLIPARSER_H_

#include "cliParams.h"
#include <vector>

class cliParser : public cliParams{
private:
	// The inherited members of cliParams are used to store actual parameters read from command line
	cliParams modeList; // same structure type can be used to store the list of available modes of operation
public:
	cliParser();
	cliParams* GetModeList();

	int16_t Parse(int argc, char* argv[]);
	void PrintUsage(std::ostream& out, std::string execName);
};

#endif /* CLIPARSER_H_ */
