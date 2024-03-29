/*
 * TGainSpeAnalyser.h
 *
 *  Created on: May 16, 2019
 *      Author: mss
 */

#ifndef TGAINSPEANALYSER_H_
#define TGAINSPEANALYSER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <stdint.h>

#include <TFile.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TF1.h>
#include <TH1D.h>
#include <TMath.h>
#include <TFormula.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TAxis.h>
#include <TDirectory.h>
#include <TROOT.h>
#include <TLine.h>
#include <TLatex.h>
#include <TGaxis.h>
#include <TStyle.h>
#include <TVector.h>

namespace std {

struct TSingleLineGainContainer{
	float uMcp;
	float iMcp;
	float aRef;
	float amplitude;
	float charge;
};

struct TSpeGainContainer{
	// Inputs
	string channelName;
	int hvSpe;
	int nMcp;
	int nQuadrant;
	TGraph* grSpeCharge;
	TGraph* grSpeChargeWithoutExp; // additional, derived plot
	TGraph* grSpeAmp;
	vector<TSingleLineGainContainer*> vRawGain;

	// Transient variables
	float nPhotoelectronsAtSpeHv;
	float aRefAtSpeHv;

	// Results
	TGraphErrors* grGain;
};

class TCalibAnalyser {
private:
	// Fit parameters
	float scalePedestal;
	float meanPedestal;
	float sigmaPedestal;
	float scaleSpe;
	float meanSpe;
	float sigmaSpe;
	float scaleExp;
	float shiftExp;

	vector<TSpeGainContainer*> *vGain;
	int verbosity;
	string fitOption;

	TFile* fout;

	int OpenOutputFile(string &outputFilePath);
	void DrawSpeCharge(ofstream *outtxt = nullptr, string *outPdfName = nullptr);
	void FindCommonHvForTargetGain(double targetGain, double &commonHv, double &commonGain);
	void DrawZoomedGain(double targetGain, double x0, double x1);
	void DrawGainCharge();
	void PrintGainCharge();

	static string sAscDir;
	static string sPgcDir;
	int Fit(TGraph *gr, TF1* f, string &fitOption, TFile *fout, string dir, double xmin, double xmax);

	void DrawTextNdc(std::string s, double x, double y, double size, Color_t col, Float_t tangle);
	void SetupPadAndSizes(TGraph* gr);

	TGraph *outMeasHvGain;
	TGraph *outSpecHvGain;
	TGraph *outPixelGainCorrFactorAt1M;
	TGraph *outPixelGainCorrFactorAt15k;

public:

	TCalibAnalyser(vector<TSpeGainContainer*> *vGain, int v, float meanSpe = 20e-12, float scaleSpe = 80, float scaleExp = 300);
	virtual ~TCalibAnalyser();

	int AnalyseSpeCharge(string outputFilePath, ofstream *outtxt = nullptr, string *outPdfName = nullptr);
	int ProduceGainCurves();
	int SaveAndCloseOutputFiles();

};

} /* namespace std */

#endif /* TGAINSPEANALYSER_H_ */
