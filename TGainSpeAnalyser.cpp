/*
 * TGainSpeAnalyser.cpp
 *
 *  Created on: May 16, 2019
 *      Author: mss
 */

#include "TGainSpeAnalyser.h"

namespace std {

TGainSpeAnalyser::TGainSpeAnalyser(vector<TSpeGainContainer*> *vGain, float meanSpe, int v) {
	this->vGain = vGain;
	fout = NULL;
	verbosity = v;
	if(verbosity < 0){
		fitOption = "Q";
	}
	else if(verbosity > 1){
		fitOption = "V";
	}
	else{
		fitOption = "";
	}

	// Fit parameters (initial values) for SPE
	scalePedestal = 1e4;
	meanPedestal = 0;
	sigmaPedestal = 1e-12;
	scaleSpe = 150;
	this->meanSpe = meanSpe;
	sigmaSpe = 1.5e-11;
	scaleExp = 300;
	shiftExp = -2.2e11;

	outMeasHvGain = new TGraph(4);
	outMeasHvGain->SetName("grMeasuredAvgGain");
	outSpecHvGain = new TGraph(4);
	outMeasHvGain->SetName("grSpecsAvgGain");
	outPixelGainCorrFactorAt1M = new TGraph(4);
	outPixelGainCorrFactorAt1M->SetName("grPixelGainCorrFactorAt1M");
	outPixelGainCorrFactorAt15k = new TGraph(4);
	outPixelGainCorrFactorAt15k->SetName("grPixelGainCorrFactorAt15k");
}

TGainSpeAnalyser::~TGainSpeAnalyser() {
	delete outMeasHvGain;
	delete outSpecHvGain;
	delete outPixelGainCorrFactorAt1M;
	delete outPixelGainCorrFactorAt15k;
}

int TGainSpeAnalyser::OpenOutputFile(string &outFilePath){
	fout = new TFile(outFilePath.c_str(), "RECREATE");
	if(!fout->IsOpen()){
		cout << "<E>TGainSpeAnalyser::OpenOutputFile(): Out file is not opened: " << outFilePath << endl;
		return -1;
	}
	gDirectory->mkdir(sAscDir.c_str());
	gDirectory->mkdir(sPgcDir.c_str());

	return 0;
}

string TGainSpeAnalyser::sAscDir = "AnalyseSpeCharge";
string TGainSpeAnalyser::sPgcDir = "ProduceGainCurves";

int TGainSpeAnalyser::Fit(TGraph *gr, TF1* f, string &fitOption, TFile* fout=NULL, string dir="", double xmin=0, double xmax=0){
	int fitStatus=0;
	if(fabs(xmin-xmax)<1e-15){
		fitStatus = gr->Fit(f, fitOption.c_str(), "");
	}
	else{
		fitStatus = gr->Fit(f, fitOption.c_str(), "", xmin, xmax);
	}
	if(fitStatus != 0){
		cout << "<E> TGainSpeAnalyser::CheckLatestFitStatus(): Problems with latest fit status: " << fitStatus << ", " << gr->GetName() << " " << f->GetName() << endl;
		//return fitStatus;
	}
	if(fout!=NULL){
		TGraph* grsave = (TGraph*)(gr->Clone());
		string grname("gr");
		string fname = f->GetName();
		grname += fname.substr(1,fname.size()-1);
		grsave->SetName(grname.c_str());
		fout->cd();
		gDirectory->cd(dir.c_str());
		grsave->Write();
	}
	return 0;
}

int TGainSpeAnalyser::AnalyseSpeCharge(string outFilePath){
	int err = OpenOutputFile(outFilePath);
	if(err<0){
		return err;
	}

	// Define fit function
	string speFormula  = "  [0]*TMath::Exp(-0.5*((x-[1])/[2])*((x-[1])/[2]))";
	speFormula        += "+ [3]*TMath::Exp(-0.5*((x-[4])/[5])*((x-[4])/[5]))";
	speFormula        += "+ [6]*TMath::Exp(x*[7])*(x>([1]+2*[2]))";
	// TODO: It's possible to add one more gaussian component, corresponding to two-photoelectron peak, visible in some cases

	TF1* fSpe = new TF1("fSpe", speFormula.c_str());

	fSpe->SetParName(0, "scalePedestal");
	fSpe->SetParName(1, "meanPedestal");
	fSpe->SetParName(2, "sigmaPedestal");
	fSpe->SetParName(3, "scaleSpe");
	fSpe->SetParName(4, "meanSpe");
	fSpe->SetParName(5, "sigmaSpe");
	fSpe->SetParName(6, "scaleExp");
	fSpe->SetParName(7, "shiftExp");
	fSpe->SetParLimits(0, 1, 1e30);
	fSpe->SetParLimits(1, -10e-12, 20e-12);
	fSpe->SetParLimits(2, 0.5e-12, 3e-12);
	fSpe->SetParLimits(3, 0, 1e30);
	fSpe->SetParLimits(4, 12e-12, 80e-12);
	fSpe->SetParLimits(5, 0.5e-11, 4e-11);
	fSpe->SetParLimits(6, 0, 1e30);
	fSpe->SetParLimits(7, -1e30, 0);

	for(uint16_t iq=0; iq<vGain->size(); iq++){
		//if(iq!=0) continue;
		TGraph *gr = vGain->at(iq)->grSpeCharge;
		stringstream grname;
		grname << "grSpeCharge-" << vGain->at(iq)->nMcp << "-" << iq+1;
		gr->SetName(grname.str().c_str());
		gr->SetTitle(grname.str().c_str());
		stringstream fSpeName;
		fSpeName << "fSpe-" << vGain->at(iq)->nMcp << "-" << vGain->at(iq)->nQuadrant;
		TF1* f = (TF1*)fSpe->Clone(fSpeName.str().c_str());
		// 1st pedestal fitting (no Spe, no Exp)
		f->FixParameter(0, scalePedestal);
		f->SetParameter(1, meanPedestal);
		f->SetParameter(2, sigmaPedestal);
		f->FixParameter(3, 0);
		f->FixParameter(4, meanSpe);
		f->FixParameter(5, sigmaSpe);
		f->FixParameter(6, 0);
		f->FixParameter(7, shiftExp);
		if(Fit(gr, f, fitOption, fout, sAscDir) != 0) return -1;

		// 2nd pedestal fitting (no Spe, no Exp)
		f->ReleaseParameter(0);
		if(Fit(gr, f, fitOption, fout, sAscDir) != 0) return -1;

		// 1st Spe fit, pedestal fixed (no Exp)
		f->FixParameter(0, f->GetParameter(0));
		f->FixParameter(1, f->GetParameter(1));
		f->FixParameter(2, f->GetParameter(2));
		f->FixParameter(3, scaleSpe);
		f->ReleaseParameter(4);
		f->ReleaseParameter(5);
		if(Fit(gr, f, fitOption, fout, sAscDir) != 0) return -1;

		// Fit of SPE together with pedestal (no Exp)
		f->ReleaseParameter(0);
		f->ReleaseParameter(1);
		f->ReleaseParameter(2);
		f->ReleaseParameter(3);
		if(Fit(gr, f, fitOption, fout, sAscDir) != 0) return -1;

		// Fix pedestal and Spe and 1st fit of Exp
		f->FixParameter(0, f->GetParameter(0));
		f->FixParameter(1, f->GetParameter(1));
		f->FixParameter(2, f->GetParameter(2));
		f->FixParameter(3, f->GetParameter(3));
		f->FixParameter(4, f->GetParameter(4));
		f->FixParameter(5, f->GetParameter(5));
		f->SetParameter(6, scaleExp); // the scale param of Exp is also fixed at first
		f->ReleaseParameter(6);
		f->ReleaseParameter(7);
		if(Fit(gr, f, fitOption, fout, sAscDir) != 0) return -1;

		// Final fit - all params are freed
		f->ReleaseParameter(0);
		f->ReleaseParameter(1);
		f->ReleaseParameter(2);
		f->ReleaseParameter(3);
		f->ReleaseParameter(4);
		f->ReleaseParameter(5);
		f->ReleaseParameter(6);
		f->ReleaseParameter(7);
		if(Fit(gr, f, fitOption, fout, sAscDir) != 0) return -1;
	}

	DrawSpeCharge();
	fout->cd();
	return 0;
}

int TGainSpeAnalyser::ProduceGainCurves(){
	for(uint16_t iq=0; iq<vGain->size(); iq++){
		TSpeGainContainer* quadrantSpeGain = vGain->at(iq);

		// Find number of photoelectrons during the final gain scan step, using SPE measurement
		bool speVoltageFound = false;
		for(uint16_t ihv=quadrantSpeGain->vRawGain.size()-1; ihv>=0; ihv--){
			TSingleLineGainContainer* rawGainData = quadrantSpeGain->vRawGain.at(ihv);
			// Find gain scan record corresponding to voltage used during SPE measurement
			if(fabs(rawGainData->uMcp - quadrantSpeGain->hvSpe) < 1e-3){
				TList* funcList = quadrantSpeGain->grSpeCharge->GetListOfFunctions();
				if(funcList->GetSize() <= 0){
					cout << "<E> TGainSpeAnalyser::ProduceGainCurves(): No SPE fit funtion found for: " << quadrantSpeGain->channelName << ", at HV=" << quadrantSpeGain->hvSpe << endl;
					return -1;
				}
				TF1* fSpeFit = (TF1*)funcList->At(0);
				float speCharge = fSpeFit->GetParameter(4) - fSpeFit->GetParameter(1);
				quadrantSpeGain->nPhotoelectronsAtSpeHv = -rawGainData->charge / speCharge;
				quadrantSpeGain->aRefAtSpeHv = -rawGainData->aRef;
				cout << "  <I> " << quadrantSpeGain->channelName << " nPE = " << quadrantSpeGain->nPhotoelectronsAtSpeHv << endl;
				speVoltageFound = true;
				break;
			}
		}
		if(!speVoltageFound){
			cout << "<E> TGainSpeAnalyser::ProduceGainCurves(): Spe voltage of " << quadrantSpeGain->hvSpe << " not found in gain log" << endl;
			return -1;
		}

		// Generate graph: gain vs. hv
		quadrantSpeGain->grGain = new TGraphErrors(quadrantSpeGain->vRawGain.size());
		stringstream grname;
		grname << "grGain-" << quadrantSpeGain->nMcp << "-" << quadrantSpeGain->nQuadrant;
		quadrantSpeGain->grGain->SetName(grname.str().c_str());
		quadrantSpeGain->grGain->SetTitle(grname.str().c_str());
		TGraphErrors* gr = quadrantSpeGain->grGain;
		string grName = (string)"grGain-" + quadrantSpeGain->channelName.substr(1).c_str();
		gr->SetName(grName.c_str());
		for(uint16_t ihv=0; ihv<quadrantSpeGain->vRawGain.size(); ihv++){
			TSingleLineGainContainer* rawGainData = quadrantSpeGain->vRawGain.at(ihv);
			float gain = rawGainData->charge / (50 * TMath::Qe() * quadrantSpeGain->nPhotoelectronsAtSpeHv * rawGainData->aRef / quadrantSpeGain->aRefAtSpeHv);
			gr->SetPoint(ihv, rawGainData->uMcp, gain);

			// Base weights are set to arbitrary 1% resolution with respect to gain
			//  -> Constant errors lead to overweighting of high-gain points over low-gain points during fitting
			//  -> Almost linear errors help to avoid it and make all points matter equally
			gr->SetPointError(ihv, 0, 0.05*gain);
		}

		// Find same hv values
		for(int16_t ihv=0; ihv<quadrantSpeGain->vRawGain.size()-1; ihv++){
			double x1, y1, x2, y2;
			gr->GetPoint(ihv  , x1, y1);
			gr->GetPoint(ihv+1, x2, y2);
			if(fabs(x1-x2) < 0.001){
				// Set the errors of double points at the same HV to two times of normal
				//  to effectively half their weight during fitting (assumes there are no triple-points)
				// if same hv value -> set both y-weights to their double
//				gr->SetPointError(ihv  , gr->GetErrorX(ihv)  , 2.0*gr->GetErrorY(ihv));
//				gr->SetPointError(ihv+1, gr->GetErrorX(ihv+1), 2.0*gr->GetErrorY(ihv+1));
				gr->RemovePoint(ihv+1);
			}
		}

		string fName = (string)"fGain-" + quadrantSpeGain->channelName.substr(1).c_str();
/*		// Fit with the function glued from Exp (for lower part ~<1.4 kV) and from pol2, valid only above ~1.4 kV (par [0])
		// Parameters 4 and 5 (offset) are dependent
		//  -> from the condition that the glued function is continuous and smooth at the gluing point [0]
		//  -> i.e. fExp([0]) = fPol2([0])   and   d/dx(fExp([0])) = d/dx(fPol2([0]))
		string sp4 = "([2]*[1]*TMath::Exp([2]*[0]) - 2*[3]*[0])";
		string sp5 =     "([1]*TMath::Exp([2]*[0]) - [3]*[0]*[0] - [0]*";
		sp5 += sp4 + ")";
		stringstream ssformula;
		ssformula <<   "(x<[0])  * [1]*TMath::Exp([2]*x) ";
		ssformula << "+ (x>=[0]) * ([3]*x*x + " << sp4 << "*x + " << sp5 << ")";

		TF1* fGain = new TF1(fName.c_str(), ssformula.str().c_str());

		// Fit only Exp part
		fGain->SetParameter(1, 1e-3);
		fGain->SetParameter(2, 0.013);
		fGain->FixParameter(0, 1400);
		fGain->FixParameter(3, 0);
		if(Fit(gr, fGain, fitOption, fout, sPgcDir, 1000, 1400) != 0) return -1;

		// Fit only Pol2 part
		fGain->FixParameter(1, fGain->GetParameter(1));
		fGain->FixParameter(2, fGain->GetParameter(2));
		fGain->ReleaseParameter(3);
		if(Fit(gr, fGain, fitOption, fout, sPgcDir, 1600, 2000) != 0) return -1;

		// Fit all parameters apart from the gluing point
		fGain->ReleaseParameter(1);
		fGain->ReleaseParameter(2);
		fGain->ReleaseParameter(3);
		if(Fit(gr, fGain, fitOption, fout, sPgcDir, 1000, 2000) != 0) return -1;

		// Refine the fit with all parameters freed
		fGain->ReleaseParameter(0);
		if(Fit(gr, fGain, fitOption, fout, sPgcDir, 1000, 2000) != 0) return -1;
*/

		// Fit with the function glued from Exp (for lower part ~<1.4 kV) and from pol3, valid only above ~1.4 kV (par [0])
		// Parameters 5 and 6 (offset) are dependent
		//  -> from the condition that the glued function is continuous and smooth at the gluing point [0]
		//  -> i.e. fExp([0]) = fPol3([0])   and   d/dx(fExp([0])) = d/dx(fPol3([0]))
		string sp5 = "([2]*[1]*TMath::Exp([2]*[0]) - 3*[3]*[0]*[0]     - 2*[4]*[0])";
		string sp6 = "    ([1]*TMath::Exp([2]*[0]) -   [3]*[0]*[0]*[0] -   [4]*[0]*[0] - [0]*";
		sp6 += sp5 + ")";
		stringstream ssformula;
		ssformula <<   "(x<[0])  * [1]*TMath::Exp([2]*x) ";
		ssformula << "+ (x>=[0]) * ([3]*x*x*x + [4]*x*x + " << sp5 << "*x + " << sp6 << ")";
		TF1* fGain = new TF1(fName.c_str(), ssformula.str().c_str());

		// Fit only Exp part
		fGain->SetParameter(1, 1e-3);
		fGain->SetParameter(2, 0.013);
		fGain->FixParameter(0, 1400);
		fGain->FixParameter(3, 0);
		fGain->FixParameter(4, 0);
		if(Fit(gr, fGain, fitOption, fout, sPgcDir, 1000, 1400) != 0) return -1;

		// Fit only Pol3 part
		fGain->FixParameter(1, fGain->GetParameter(1));
		fGain->FixParameter(2, fGain->GetParameter(2));
		fGain->ReleaseParameter(3);
		fGain->ReleaseParameter(4);
		if(Fit(gr, fGain, fitOption, fout, sPgcDir, 1600, 2000) != 0) return -1;

		// Fit all parameters apart from the gluing point
		fGain->ReleaseParameter(1);
		fGain->ReleaseParameter(2);
		fGain->ReleaseParameter(3);
		fGain->ReleaseParameter(4);
		if(Fit(gr, fGain, fitOption, fout, sPgcDir, 1000, 2000) != 0) return -1;

		// Refine the fit with all parameters freed
		fGain->ReleaseParameter(0);
		if(Fit(gr, fGain, fitOption, fout, sPgcDir, 1000, 2000) != 0) return -1;

		if(iq != 0) continue;

		fout->cd();
		gr->Write();
	}
	DrawGainCharge();
	PrintGainCharge();
	return 0;
}

void TGainSpeAnalyser::DrawSpeCharge(){
	float lmargin = 0.08;
	float rmargin = 0.07;
	float tmargin = 0.0;
	float bmargin = 0.135;
	float fontsize = 0.065;
	TCanvas *cSpe = new TCanvas("cSpe","cSpe",1600,800);
	cSpe->Divide(2, vGain->size()/2);
	for(uint16_t iq=0; iq<vGain->size(); iq++){
		cSpe->cd(iq+1);
		gPad->SetLogy();
		TGraph *gr = (TGraph*)(vGain->at(iq)->grSpeCharge->Clone());
		gr->GetXaxis()->SetTitle("Charge [Wb]");
		gr->GetYaxis()->SetTitle("Counts");
		gPad->SetMargin(lmargin, rmargin, bmargin, tmargin);
		gr->GetXaxis()->SetLabelSize(fontsize);
		gr->GetXaxis()->SetTitleSize(fontsize*1.15);
		gr->GetXaxis()->SetTitleOffset(0.85);
		gr->GetXaxis()->SetNdivisions(4);
		gr->GetYaxis()->SetLabelSize(fontsize);
		gr->GetYaxis()->SetTitleSize(fontsize*1.15);
		gr->GetYaxis()->SetTitleOffset(0.55);
		gr->Draw("AL* same");
		string padName = gr->GetTitle();
		gr->SetTitle("");
		padName = padName.substr(padName.find_first_of('-')+1);
		DrawTextNdc(padName, 0.78, 0.88, 0.08, kBlack, 0);

		// Extract selected fit parameters
		TF1* f = (TF1*)(gr->GetListOfFunctions()->At(gr->GetListOfFunctions()->GetSize()-1));
		if(f==NULL){
			continue;
		}
		uint16_t icnt = 0;
		for(int ipar=0; ipar<8; ipar++){
			if((ipar == 1) || (ipar == 2) || (ipar == 4) || (ipar == 5)){
				stringstream ssFitPar;
				ssFitPar << "#font[8]{";
				ssFitPar << left << setw(14) << f->GetParName(ipar);
				ssFitPar.precision(3);
				ssFitPar << setw(10) << right << f->GetParameter(ipar);
				ssFitPar << " +/- ";
				ssFitPar << setw(10) << right << f->GetParError(ipar);
				ssFitPar << "}";
				DrawTextNdc(ssFitPar.str(), 0.35, 0.8-icnt*0.06, 0.055, kRed-2, 0);
				icnt++;
			}
		}
	}
}

void TGainSpeAnalyser::FindCommonHvForTargetGain(double targetGain, double &commonHv, double &commonGain){
	// Find starting point for a minimizer as average hv (x)
	commonHv=0;
	// Get average x for four quadrants
	for(uint16_t iq=0; iq<vGain->size(); iq++){
		TGraph *gr = vGain->at(iq)->grGain;
		if(gr->GetListOfFunctions()->GetSize()>0){
			TF1* f = (TF1*)gr->GetListOfFunctions()->At(0);
			commonHv += f->GetX(targetGain);
		}
	}
	commonHv /= vGain->size();

	// Run minimizer using commonHv as starting point
	double dCommonHv = 2;
	commonGain=0;
	int cnt = 0;
	while(fabs(dCommonHv) > 0.01){
		if(cnt == 0) dCommonHv = 0;
		if(cnt++ > 20) break;
		// Get average y for average x
		commonGain = 0;
		commonHv -= dCommonHv/2;
		for(uint16_t iq=0; iq<vGain->size(); iq++){
			TGraph *gr = vGain->at(iq)->grGain;
			if(gr->GetListOfFunctions()->GetSize()>0){
				TF1* f = (TF1*)gr->GetListOfFunctions()->At(0);
				commonGain += f->Eval(commonHv);
			}
		}
		commonGain /= vGain->size();

		// Get average difference in x, given difference in y
		dCommonHv = 0;
		for(uint16_t iq=0; iq<vGain->size(); iq++){
			TGraph *gr = vGain->at(iq)->grGain;
			if(gr->GetListOfFunctions()->GetSize()>0){
				TF1* f = (TF1*)gr->GetListOfFunctions()->At(0);
				dCommonHv += f->GetX(commonGain) - f->GetX(targetGain);
			}
		}
		dCommonHv /= vGain->size();
	}

	// Save best common HV into the separate output graph
	static int ipoint = 0;
	outMeasHvGain->SetPoint(ipoint++, commonHv, commonGain);

	// Get and save the correction factors for all quadrants, when using the common HV
	for(uint16_t iq=0; iq<vGain->size(); iq++){
		TGraph *gr = vGain->at(iq)->grGain;
		if(gr->GetListOfFunctions()->GetSize()>0){
			TF1* fit = (TF1*)gr->GetListOfFunctions()->At(0);
			float qGain = fit->Eval(commonHv);
			if(fabs(targetGain-1e6) < 1e-3){
				outPixelGainCorrFactorAt1M->SetPoint(iq, iq, qGain/targetGain);
			}
			else if(fabs(targetGain-15e3) < 1e-3){
				outPixelGainCorrFactorAt15k->SetPoint(iq, iq, qGain/targetGain);
			}
		}
	}
}

void TGainSpeAnalyser::DrawZoomedGain(double targetGain, double commonHv, double commonGain){
	// Find range suitable for all curves
	double dy = targetGain*0.4;
	double x0min=1000, x1max=2000;
	for(uint16_t iq=0; iq<vGain->size(); iq++){
		TGraph *gr = vGain->at(iq)->grGain;
		if(gr->GetListOfFunctions()->GetSize()>0){
			TF1* f = (TF1*)gr->GetListOfFunctions()->At(0);
			if(iq==0){
				x0min = f->GetX(targetGain-dy);
				x1max = f->GetX(targetGain+dy);
			}
			else{
				double x0 = f->GetX(targetGain-dy);
				double x1 = f->GetX(targetGain+dy);
				if(x0min > x0) x0min = x0;
				if(x1max < x1) x1max = x1;
			}
		}
	}

	// Draw within specified range
	TGraph *grScale = new TGraph(2);
	grScale->SetPoint(0, x0min, targetGain-dy);
	grScale->SetPoint(1, x1max, targetGain+dy);
	grScale->GetXaxis()->SetRangeUser(x0min, x1max);
	grScale->GetYaxis()->SetRangeUser(targetGain-dy, targetGain+dy);
	grScale->GetXaxis()->SetTitle("Voltage [V]");
	grScale->GetYaxis()->SetTitle("Gain");
	SetupPadAndSizes(grScale);
	grScale->Draw("AP");
	for(uint16_t iq=0; iq<vGain->size(); iq++){
		TGraph *gr = vGain->at(iq)->grGain;
		gr->SetLineWidth(2);
		gr->Draw("PL same");
		if(gr->GetListOfFunctions()->GetSize()>0){
			TF1* fit = (TF1*)gr->GetListOfFunctions()->At(0);
			fit->SetLineStyle(3);
			fit->SetLineColor(kWhite);
			fit->DrawCopy("same");
		}
	}

	// Draw lines corresponding to target gain and to the 'best fit' for HV
	TLine *lhorizontal = new TLine(x0min, commonGain, commonHv, commonGain);
	lhorizontal->SetLineColor(kGray+1);
	lhorizontal->SetLineStyle(2);
	lhorizontal->SetLineWidth(4);
	lhorizontal->Draw("same");
	TLine *lvertical = new TLine(commonHv, targetGain-dy, commonHv, commonGain);
	lvertical->SetLineColor(kGray+1);
	lvertical->SetLineStyle(2);
	lvertical->SetLineWidth(4);
	lvertical->Draw("same");

	// Draw labels with optimally adjusted values
	stringstream ssLabel;
	ssLabel.precision(1);
	ssLabel << scientific << "#splitline{Avg. gain = " << commonGain;
	ssLabel.precision(0);
	ssLabel << fixed      << "}{Avg. voltage = "       << commonHv << "}";
	DrawTextNdc(ssLabel.str(), 0.5, 0.18, 0.05, kBlack, 0);
}

void TGainSpeAnalyser::DrawGainCharge(){
	// Setup styles and colors
	Color_t col[4] = {kRed, kMagenta+2, kBlue+1, kCyan+2};
	for(uint16_t iq=0; iq<vGain->size(); iq++){
		gPad->SetLogy();
		TGraphErrors *gr = vGain->at(iq)->grGain;
		gr->SetLineColor(col[iq]);
		gr->SetLineWidth(2);
		gr->SetMarkerStyle(8);
		gr->SetMarkerSize(1);
		gr->SetMarkerColor(col[iq]);
		TF1* fit = (TF1*)gr->GetListOfFunctions()->At(0);
		if(fit!=NULL){
			fit->SetLineColor(col[iq]);
			fit->SetLineWidth(2);
		}
	}

	TCanvas *cGain = new TCanvas("cGain","cGain",1700,600);
	gPad->SetCrosshair();
	cGain->Divide(4);
	// Draw full scale
	cGain->cd(1);
	gPad->SetLogy();
	double x0 = 1100, x1=2080, y0=1e3, y1=1e7;
	TGraph *grScale = new TGraph(2);
	grScale->SetPoint(0, x0, y0);
	grScale->SetPoint(1, x1, y1);
	grScale->GetXaxis()->SetRangeUser(x0, x1);
	grScale->GetYaxis()->SetRangeUser(y0, y1);
	grScale->GetXaxis()->SetTitle("Voltage [V]");
	grScale->GetYaxis()->SetTitle("Gain");
	SetupPadAndSizes(grScale);
	grScale->Draw("AP");
	for(uint16_t iq=0; iq<vGain->size(); iq++){
		TGraphErrors *gr = (TGraphErrors*)(vGain->at(iq)->grGain->Clone());
		gr->SetName("");
		gr->SetTitle("");
		gr->DrawClone("P same");
		if(gr->GetListOfFunctions()->GetSize()>0){
			TF1* fit = (TF1*)gr->GetListOfFunctions()->At(0);
			fit->DrawCopy("same");
		}
		else{
			cout << " <W> TGainSpeAnalyser::DrawGainCharge(): No fit function found for: " << gr->GetName() << endl;
		}

		string legEntryName = vGain->at(iq)->grGain->GetTitle();
		legEntryName = legEntryName.substr(legEntryName.find_first_of('-')+1);
		DrawTextNdc(legEntryName, 0.22, 0.87-0.05*iq, 0.07, gr->GetLineColor(), 0);
	}

	double commonHv=0, commonGain=0;
	FindCommonHvForTargetGain(10e3, commonHv, commonGain); // Save, but don't draw

	cGain->cd(2);
	// Draw zoom-in to 15k Gain
	FindCommonHvForTargetGain(15e3, commonHv, commonGain);
	DrawZoomedGain(15e3, commonHv, commonGain);

	cGain->cd(3);
	// Draw zoom-in to 100k Gain
	FindCommonHvForTargetGain(1e5, commonHv, commonGain);
	DrawZoomedGain(1e5, commonHv, commonGain);

	cGain->cd(4);
	// Draw zoom-in to 1M Gain
	FindCommonHvForTargetGain(1e6, commonHv, commonGain);
	DrawZoomedGain(1e6, commonHv, commonGain);
}

void TGainSpeAnalyser::PrintGainCharge(){
	for(uint16_t iq=0; iq<vGain->size(); iq++){
		TGraphErrors *gr = (TGraphErrors*)(vGain->at(iq)->grGain->Clone());
		string mcpName = gr->GetName();
		mcpName = mcpName.substr(mcpName.find_first_of('-')+1);
		double hv=0, gain=0;
		cout << mcpName << endl;
		for(uint16_t ip=0; ip<gr->GetN(); ip++){
			gr->GetPoint(ip, hv, gain);
			cout << "  " << hv << " " << gain << endl;
		}
	}
}

int TGainSpeAnalyser::SaveAndCloseOutputFiles(){
	fout->cd();
	outMeasHvGain->Write();
	outSpecHvGain->Write();
	outPixelGainCorrFactorAt15k->Write();
	outPixelGainCorrFactorAt1M->Write();
	fout->Close();
	return 0;
}

void TGainSpeAnalyser::DrawTextNdc(string s, double x, double y, double size, Color_t col, Float_t tangle){
	TLatex *t = new TLatex(0,0,s.c_str());
	t->SetTextAngle(tangle);
	t->SetNDC();
	t->SetX(x);
	t->SetY(y);
	t->SetTextSize(size);
	t->SetTextColor(col);
	t->Draw();
}

void TGainSpeAnalyser::SetupPadAndSizes(TGraph *gr){
	TGaxis::SetMaxDigits(4);
	float lmargin = 0.17;
	float rmargin = 0.02;
	float tmargin = 0.07;
	float bmargin = 0.12;
	float fontsize = 0.065;
	gPad->SetMargin(lmargin, rmargin, bmargin, tmargin);
	gr->GetXaxis()->SetLabelSize(fontsize);
	gr->GetXaxis()->SetTitleSize(fontsize*1.15);
	gr->GetXaxis()->SetTitleOffset(0.8);
	gr->GetXaxis()->SetNdivisions(4);
	gr->GetYaxis()->SetLabelSize(fontsize);
	gr->GetYaxis()->SetTitleSize(fontsize*1.15);
	gr->GetYaxis()->SetTitleOffset(1.2);
	gr->SetTitle("");
}

} /* namespace std */
