/*
 * TLeCroySpeReader.cpp
 *
 *  Created on: Apr 24, 2019
 *      Author: mss
 */

#include "../inc/TGainSpeReader.h"

namespace std {

const string TGainSpeReader::sGainDirectory = "gain";
const string TGainSpeReader::sGainPrefix    = "Measure--";
const string TGainSpeReader::sGainCurve     = "-gain_curve--";
const string TGainSpeReader::sExt           = ".txt";
const string TGainSpeReader::sSpePrefix[nSpeFiles] = {"C1","C2","C3","C4","F1","F2","F3","F4","F5","F6","F7","F8"};
const string TGainSpeReader::sSpe           = "SPE";
const string TGainSpeReader::sLogDirectory  = "log";
const string TGainSpeReader::sLogSuffix     = "-gainScan";
const string TGainSpeReader::sLogExt        = ".log";

TGainSpeReader::TGainSpeReader(string &inPath) {
	nMcp = 0;
	hvSpe = 0;
	inputPath = inPath;
	// Make sure input path has slash at the end
	if(inputPath.at(inputPath.size()-1) != '/'){
		inputPath += '/';
	}
}

TGainSpeReader::~TGainSpeReader() {
	for(uint16_t i=0; i<vGainReader.size(); i++){
		delete vGainReader.at(i);
	}
	for(uint16_t i=0; i<vSpeReader.size(); i++){
		delete vSpeReader.at(i);
	}
}

int TGainSpeReader::ReadLog(uint32_t nMcp){
	cout << "  <I> TGainSpe::GenerateLogPaths(): " << nMcp << endl;
	this->nMcp = nMcp;

	// Log file - generate path and open to check the range of the gain and SPE indices
	stringstream ssLogFilePath;
	ssLogFilePath << inputPath << sLogDirectory << "/" << nMcp << sLogSuffix << sLogExt;
	string logFilePath = ssLogFilePath.str();
	int err = logReader.ReadFile(logFilePath);
	if(err < 0){
		cout << "<E> TGainSpe::GeneratePaths(): Error parsing log file " << logFilePath << endl;
		return err;
	}
	if(logReader.GetStorage().size() < 1){
		cout << "<E> TGainSpe::GeneratePaths(): Error reading log file: no data read from:  " << logFilePath << endl;
		return -1;
	}
	return 0;
}

int TGainSpeReader::GenerateScopePaths(uint32_t nMcp, uint32_t lastSpeNumber=0){
	vector<TSingleGainLogLine> vLog = logReader.GetStorage();
	// Gain curve file paths
	for(uint16_t iLog = 0; iLog<vLog.size(); iLog++){
		stringstream ssGainCurveFilePath;
		ssGainCurveFilePath << inputPath << sGainDirectory << "/";
		ssGainCurveFilePath << nMcp << "/" << sGainPrefix << nMcp << sGainCurve;
		ssGainCurveFilePath.fill('0');
		ssGainCurveFilePath << setw(5) << vLog.at(iLog).GetFileNumber() << sExt;
		vGainCurveFilePath.push_back(ssGainCurveFilePath.str());
	}

	// Find HV used for SPE (last HV value in the gain scan log)
	if(logReader.IsFileOpened()){
		hvSpe = logReader.GetStorage().at(logReader.GetStorage().size()-1).GetU();
	}
	else{
		cout << "<E> TGainSpeReader::GenerateScopePaths(): Log file was not read: " << endl;
		return -1;
	}

	// SPE file paths
	for(uint16_t ifile=0; ifile<nSpeFiles; ifile++){
		stringstream ssSpeFilePath;
		ssSpeFilePath << inputPath << sGainDirectory << "/";
		ssSpeFilePath << nMcp << "/" << sSpePrefix[ifile] << "--";
		ssSpeFilePath << nMcp << "-" << sSpe << "-" << hvSpe << "V--";
		ssSpeFilePath.fill('0');
		ssSpeFilePath << setw(5) << lastSpeNumber << sExt;
		vSpeFilePath.push_back(ssSpeFilePath.str());
	}

	// Try to open files
	uint16_t nGainFiles = 0;
	for(uint16_t ifile=0; ifile<vGainCurveFilePath.size(); ifile++){
		if(fileExists(vGainCurveFilePath.at(ifile))){
			nGainFiles++;
		}
		else{
			cout << "  <I> TLeCroySpeReader::GeneratePaths(): The file does not exist: " << vGainCurveFilePath.at(ifile) << endl;
		}
	}

	if(nGainFiles != vGainCurveFilePath.size()){
		cout << " <W> TLeCroySpeReader::GeneratePaths(): Only " << nGainFiles;
		cout << " out of " << vGainCurveFilePath.size() << " were opened. Aborting." << endl;
		return -1;
	}
	else{
		cout << "  <I> TLeCroySpeReader::GeneratePaths(): All gain files exist." << endl;
	}

	bool speOk = true;
	for(uint16_t ifile=0; ifile<vSpeFilePath.size(); ifile++){
		if(!fileExists(vSpeFilePath.at(ifile))){
			cout << " <W> TLeCroySpeReader::GeneratePaths(): The file does not exist: " << vSpeFilePath.at(ifile) << endl;
			speOk = false;
		}
	}
	if(speOk){
		cout << "  <I> TLeCroySpeReader::GeneratePaths(): All SPE files exist." << endl;
	}
	return 0;
}

int TGainSpeReader::ReadScopeFiles(){
	int err = GenerateScopePaths(nMcp);
	if(err<0){
		cout << "<E> TGainSpe::ReadAll(): Failed to generate paths." << endl;
		return err;
	}

	// Read gain curves
	for(uint16_t ifile=0; ifile<vGainCurveFilePath.size(); ifile++){
		TLeCroyMeasureFileReader* gainReader = new TLeCroyMeasureFileReader();
		vGainReader.push_back(gainReader);
		gainReader->SetVerbose(1);
		int err = gainReader->ReadFile(vGainCurveFilePath.at(ifile));
		if(err<0){
			cout << "<E> TGainSpe::ReadAll(): speReader failed at: " << ifile << ": " << vSpeFilePath.at(ifile) << endl;
			return err;
		}
		gainReader->ReorganizeStorage();
		//gainReader->Print(cout);
	}

	for(uint16_t ifile=0; ifile<vSpeFilePath.size(); ifile++){
		TLeCroyWaveformFileReader* speReader = new TLeCroyWaveformFileReader();
		vSpeReader.push_back(speReader);
		speReader->SetVerbose(1);
		int err = speReader->ReadFile(vSpeFilePath.at(ifile));
		if(err<0){
			cout << "<E> TGainSpe::ReadAll(): speReader failed at: " << ifile << ": " << vSpeFilePath.at(ifile) << endl;
			return err;
		}
		//speReader->Print(cout);
	}
	return 0;
}

int TGainSpeReader::ReadAll(uint32_t nMcp, vector<TSpeGainContainer*>& vGain){
	this->nMcp = nMcp;
	ReadLog(nMcp);
	ReadScopeFiles();

	// Used later to link the channel number with the record name in the gain file
	vector<string> vMeasChannelName;
	vMeasChannelName.push_back("C1");
	vMeasChannelName.push_back("C2");
	vMeasChannelName.push_back("C3");
	vMeasChannelName.push_back("C4");

	// Find file indices with amplitude and charge info (could be parametrized with vMeasChannelName)
	vector<string> vAmplitudeChannelName;
	vAmplitudeChannelName.push_back("F1");
	vAmplitudeChannelName.push_back("F3");
	vAmplitudeChannelName.push_back("F5");
	vAmplitudeChannelName.push_back("F7");
	vector<string> vChargeChannelName;
	vChargeChannelName.push_back("F2");
	vChargeChannelName.push_back("F4");
	vChargeChannelName.push_back("F6");
	vChargeChannelName.push_back("F8");
	if(vChargeChannelName.size() != vAmplitudeChannelName.size()){
		cout << "<E> TGainSpeReader::ReadAll(): The number of amplitude and charge channels should be equal: ";
		cout << vChargeChannelName.size() << " != " << vAmplitudeChannelName.size() << endl;
		return -1;
	}

	vector<uint16_t> vAmplitudeChannelNumber;
	vector<uint16_t> vChargeChannelNumber;
	uint16_t nCh = vChargeChannelName.size();
	for(uint16_t iCh=0; iCh<nCh; iCh++){
		string* chNameAmp = &(vAmplitudeChannelName.at(iCh));
		for(uint16_t iSpeFile=0; iSpeFile<vSpeReader.size(); iSpeFile++){
			if(vSpeReader.at(iSpeFile)->GetFilePath().find(*chNameAmp) != string::npos){
				vAmplitudeChannelNumber.push_back(iSpeFile);
				break;
			}
		}
		string* chNameCharge = &(vChargeChannelName.at(iCh));
		for(uint16_t iSpeFile=0; iSpeFile<vSpeReader.size(); iSpeFile++){
			if(vSpeReader.at(iSpeFile)->GetFilePath().find(*chNameCharge) != string::npos){
				vChargeChannelNumber.push_back(iSpeFile);
				break;
			}
		}
	}

	// Create output structure of vGain
	for(uint16_t iq=0; iq<nCh; iq++){
		TSpeGainContainer *sgc = new TSpeGainContainer();
		stringstream ssChName;
		ssChName << "#" << nMcp << "-" << iq+1;
		sgc->channelName = ssChName.str();
		sgc->nMcp = nMcp;
		sgc->hvSpe = hvSpe;
		sgc->nQuadrant = iq+1;
		for(uint16_t iLogLine=0; iLogLine<logReader.GetStorage().size(); iLogLine++){
			sgc->vRawGain.push_back(new TSingleLineGainContainer());
		}
		vGain.push_back(sgc);
	}

	// Fill SPE graphs and store them in vGain
	vector<TGraph*> vGr;
	for(uint16_t iq=0; iq<nCh; iq++){ // loop over 4 MCP quadrants
		TLeCroyWaveformFileReader* speAmpReader    = vSpeReader.at(vAmplitudeChannelNumber.at(iq));
		TLeCroyWaveformFileReader* speChargeReader = vSpeReader.at(vChargeChannelNumber.at(iq));
		TGraph* grAmp    = new TGraph(speAmpReader   ->GetStorage().size());
		TGraph* grCharge = new TGraph(speChargeReader->GetStorage().size());
		for(uint32_t ibin=0; ibin<speAmpReader->GetStorage().size(); ibin++){
			TSingleLeCroyWaveformLine* point = &(speAmpReader->GetStorage().at(ibin));
			grAmp->SetPoint(ibin, -point->GetTime(), point->GetAmplitude());
		}
		for(uint32_t ibin=0; ibin<speChargeReader->GetStorage().size(); ibin++){
			TSingleLeCroyWaveformLine* point = &(speChargeReader->GetStorage().at(ibin));
			grCharge->SetPoint(ibin, -point->GetTime(), point->GetAmplitude());
		}
		vGain.at(iq)->grSpeAmp = grAmp;
		vGain.at(iq)->grSpeCharge = grCharge;
	}

	// Fill raw gain curve data and store it in vGain
	for(uint16_t iLogLine=0; iLogLine<logReader.GetStorage().size(); iLogLine++){
		TSingleGainLogLine* logLine = &(logReader.GetStorage().at(iLogLine));
		for(uint16_t iq=0; iq<nCh; iq++){ // loop over 4 MCP quadrants
			TSingleLineGainContainer* slgc = vGain.at(iq)->vRawGain.at(iLogLine);
			slgc->uMcp = logLine->GetU();
			slgc->iMcp = logLine->GetI();
			slgc->aRef = logLine->GetRef();
			slgc->amplitude = -1;
			slgc->charge = -1;

			// Access proper amplitude-charge measurement file
			vector<TMeasureStorage>* vAmpChargeStorage = &(vGainReader.at(iLogLine)->GetStorage());
			for(uint16_t ich=0; ich<vMeasChannelName.size(); ich++){
				for(uint16_t irecord=0; irecord<vAmpChargeStorage->size(); irecord++){
					string measName = vAmpChargeStorage->at(irecord).GetMeasureType();
					string measAmp = "min(" + vMeasChannelName.at(ich) + ")";
					if(measName.find(measAmp) != string::npos){
						slgc->amplitude = vAmpChargeStorage->at(irecord).GetValue();
					}
					string measCharge = "area(" + vMeasChannelName.at(ich) + ")";
					if(measName.find(measCharge) != string::npos){
						slgc->charge = vAmpChargeStorage->at(irecord).GetValue();
					}
				}
			}
			if((slgc->amplitude == -1) || (slgc->charge == -1)){
				cout << "<E> TGainSpeReader::ReadAll(): Area or min not found at quadrant: " << iq << ", log line: " << iLogLine << endl;
			}
		}
	}

	return 0;
}

bool TGainSpeReader::fileExists(string &path){
	ifstream infile;
	infile.open(path.c_str(), ios_base::in);
	if(infile.fail()==true){
		return false;
	}
	string comment;
	if(!getline(infile,comment)){
		return false;
	}
	return true;
}
} /* namespace std */
