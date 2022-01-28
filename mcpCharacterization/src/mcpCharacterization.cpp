//============================================================================
// Name        : mcpCharacterization.cpp
// Author      : mss
// Version     :
// Copyright   : Your copyright notice
// Description : Analysis package for MCP-PMT characterization
//============================================================================

#include <iostream>
#include <string>

#include "../inc/cliParser.h"

#include <TApplication.h>
#include <TFile.h>
#include <TH1D.h>

#include "../inc/TGainSpeReader.h"
#include "../inc/TGainSpeAnalyser.h"

using namespace std;

int main(int argc, char* argv[]){
	cliParser cli;
	cli.AddPar("m", "Mode of operation", true);
	cli.GetModeList()->AddPar("spe", "Default mode");
	cli.GetModeList()->AddPar("all", "Plot all SPE spectra");
	cli.AddPar("r", "File with list of MCPs to be plotted", false);
	cli.AddPar("i", "Input directory or file", true);
	cli.AddPar("mcp", "MCP number to be analysed", false);
	cli.AddPar("nameSuffix", "Name suffix to be added to the outputs", false);
	cli.AddPar("speMean", "Expected mean value of the single photoelectron peak", false);
	cli.AddPar("speScale", "Expected scale (height_ of single photoelectron peak", false);
	cli.AddPar("expScale", "Expected scale (height) of the exponential part in SPE spectrum", false);
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
		speMean = 20e-12;
	}
	float speScale = cli.GetParFloat("speScale");
	if(fabs(speScale + 999999) < 1e-3){
		speScale = 80;
	}
	float expScale = cli.GetParFloat("expScale");
	if(fabs(expScale + 999999) < 1e-3){
		expScale = 3;
	}

	int v = cli.GetParInt("v");
	if(v==-999999){
		v=0;
	}
	string nameSuffix = cli.GetParString("nameSuffix");

	if(mode.compare("spe")==0){
		TApplication *theApp = new TApplication("App", &argc, argv);	// needed to print interactive canvases and gui
		theApp->SetIdleTimer(100000,"exit()"); 							// exit automatically after 100000s of being idle
		theApp->SetReturnFromRun(true);									// when the program exits there will be no error message
			TGainSpeReader *gsr = new TGainSpeReader(inputPath);
			vector<TSpeGainContainer*> vGain;
			gsr->ReadAll(imcp, vGain);
			TCalibAnalyser *gsa = new TCalibAnalyser(&vGain, v, speMean, speScale, expScale);
			stringstream ssFileOutPath;
			if(outputPath.at(outputPath.size()-1) == '/'){
				outputPath = outputPath.substr(0, outputPath.size()-1);
			}
			outputPath += "/";
			ssFileOutPath << outputPath << "gain-" << imcp << nameSuffix << ".root";
			gsa->AnalyseSpeCharge(ssFileOutPath.str());
//			gsa->ProduceGainCurves();
			//gsa->SaveAndCloseOutputFiles();
		cout << "Finished..." << endl;
		theApp->Run();
	}

	if(mode.compare("all")==0){
		const float defaultSpeMean = 20e-12;
		const float defaultSpeScale = 80;
		const float defaultExpScale = 300;
		string inFile = cli.GetParString("r");
		ifstream ifile(inFile);
		TApplication *theApp = new TApplication("App", &argc, argv);	// needed to print interactive canvases and gui
		theApp->SetIdleTimer(100000,"exit()"); 							// exit automatically after 100000s of being idle
		theApp->SetReturnFromRun(true);									// when the program exits there will be no error message
		string line;
		bool first = true;
		string outPdfFileName = "out.pdf";
		ofstream outTxtFile("out.txt");
		while (getline (ifile,line)) {
			stringstream ss(line);
			ss >> imcp;
			cout << " -> " << imcp << '\n';
			float speScale = defaultSpeScale;
			float expScale = defaultExpScale;
			float speMean = defaultSpeMean;

			TGainSpeReader *gsr = new TGainSpeReader(inputPath);
			vector<TSpeGainContainer*> vGain;
			gsr->ReadAll(imcp, vGain);
			TCalibAnalyser *gsa = new TCalibAnalyser(&vGain, v, speMean, speScale, expScale);
			stringstream ssFileOutPath;
			if(outputPath.at(outputPath.size()-1) == '/'){
				outputPath = outputPath.substr(0, outputPath.size()-1);
			}
			outputPath += "/";
			ssFileOutPath << outputPath << "gain-" << imcp << nameSuffix << ".root";

			string outPdfName = outPdfFileName;
			if(first){
				outPdfName = outPdfFileName + (first?"(":"");
				first = false;
			}

			gsa->AnalyseSpeCharge(ssFileOutPath.str(), &outTxtFile, &outPdfName);

		}
		ifile.close();
		outTxtFile.close();
		// Close the pdf file
		TCanvas *c = new TCanvas("dummy", "dummy", 1600,800);
		c->Print((outPdfFileName + ")").c_str());

		cout << "Finished..." << endl;
		theApp->Run();
	}
	return 0;
}
