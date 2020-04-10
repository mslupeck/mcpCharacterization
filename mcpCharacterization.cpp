//============================================================================
// Name        : mcpCharacterization.cpp
// Author      : mss
// Version     :
// Copyright   : Your copyright notice
// Description : Analysis package for MCP-PMT characterization
//============================================================================

#include <iostream>
#include <string>

#include "cliParser.h"

#include <TApplication.h>
#include <TFile.h>
#include <TH1D.h>

#include "TGainSpeReader.h"
#include "TGainSpeAnalyser.h"

using namespace std;

int main(int argc, char* argv[]){
	cliParser cli;
	cli.AddPar("m", "Mode of operation", true);
	cli.GetModeList()->AddPar("spe", "Default mode");
	cli.AddPar("i", "Input directory or file", true);
	cli.AddPar("mcp", "MCP number to be analysed", true);
	cli.AddPar("nameSuffix", "Name suffix to be added to the outputs", false);
	cli.AddPar("speMean", "Expected mean value of the single photoelectron peak", false);
	cli.AddPar("o", "Output directory or file", false);
	cli.AddPar("v", "Verbosity [0:default, 1:less output, -1:debug]", false);
	if(cli.Parse(argc, argv)!=0){
		cli.PrintUsage(cout, "mcpCharacterization");
		return -1;
	}

	string inputPath = cli.GetParString("i");
	string outputPath = cli.GetParString("o");
	string mode = cli.GetParString("m");
	int imcp = cli.GetParInt("mcp");
	float speMean = cli.GetParFloat("speMean");
	if(fabs(speMean + 999999) < 1e-3){
		speMean = 35e-12;
	}

	int v = cli.GetParInt("v");
	if(v==-999999){
		v=0;
	}
	string nameSuffix = cli.GetParString("nameSuffix");

	//int verbose = cli.GetParInt("v");
	if(mode.compare("spe")==0){
		TApplication *theApp = new TApplication("App", &argc, argv);	// needed to print interactive canvases and gui
		theApp->SetIdleTimer(100000,"exit()"); 							// exit automatically after 100000s of being idle
		theApp->SetReturnFromRun(true);									// when the program exits there will be no error message
			TGainSpeReader *gsr = new TGainSpeReader(inputPath);
			vector<TSpeGainContainer*> vGain;
			gsr->ReadAll(imcp, vGain);
			TGainSpeAnalyser *gsa = new TGainSpeAnalyser(&vGain, speMean, v);
			stringstream ssFileOutPath;
			if(outputPath.at(outputPath.size()-1) == '/'){
				outputPath = outputPath.substr(0, outputPath.size()-1);
			}
			outputPath += "/";
			ssFileOutPath << outputPath << "gain-" << imcp << nameSuffix << ".root";
			gsa->AnalyseSpeCharge(ssFileOutPath.str());
			gsa->ProduceGainCurves();
			gsa->SaveAndCloseOutputFiles();
		cout << "Finished..." << endl;
		theApp->Run();
	}

	return 0;
}
