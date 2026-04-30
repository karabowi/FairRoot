//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************

#include "PndEmcHighLowPSA.h"
#include "PndEmcWaveform.h"
#include "PndEmcMultiWaveform.h"
#include "PndEmcPSAFPGASampleAnalyser.h"
#include "FairLogger.h"
//#include "TH1.h"
#include <iostream>
//TH1F *Noise[2];
//TFile *f;
PndEmcHighLowPSA::PndEmcHighLowPSA(Int_t verbose) : fHighgainPSA(nullptr), fLowgainPSA(nullptr), fVerbose(verbose){};

void PndEmcHighLowPSA::Init(PndEmcPSAFPGASampleAnalyser* highgainPSA, PndEmcPSAFPGASampleAnalyser* lowgainPSA, Double_t overflowThreshold, Int_t highgainWfIndex, Int_t lowgainWfIndex) {
	fHighgainPSA = highgainPSA;
	fLowgainPSA = lowgainPSA;
	fIdx_high = highgainWfIndex;
	fIdx_low = lowgainWfIndex;
	fOverflowThreshold = overflowThreshold;
	if (fVerbose >= 7)
	{
    LOG(info) << " HighLowPSA::Init";
  }
	//f = new TFile("hsimple.root","UPDATE");
	//Noise[0]= new TH1F("High","",200,-100,100);
	//Noise[1]= new TH1F("Low","",200,-100,100);
}

void PndEmcHighLowPSA::Reset() {
	//fActivePSA=0;
	fHitsInFE.clear();
	fHighgainPSA->Reset();
	fLowgainPSA->Reset();
	if (fVerbose >= 7)
	{
    LOG(info) << " HighLowPSA::Reset";
  }
}


Int_t PndEmcHighLowPSA::Process(const PndEmcWaveform* waveform) {


	Reset();
	if (fVerbose >= 7)
	{
    LOG(info) << " PndEmcHighLowPSA::Process";
  }

	PndEmcMultiWaveform* multiWf = const_cast<PndEmcMultiWaveform*>(dynamic_cast<const PndEmcMultiWaveform*>(waveform));

	if(!multiWf) {
		std::cerr << "passed waveform is not of type PndEmcMultiwaveform" << std::endl;
		return -1;
	}

	//active method

	Int_t activeWf = multiWf->GetActiveWaveform();

	multiWf->SetActiveWaveform(fIdx_low);
	std::vector<double> signal_low = waveform->GetSignal();
	std::vector<double>::iterator it_low=signal_low.begin();

	multiWf->SetActiveWaveform(fIdx_high);
	std::vector<double> signal_high = waveform->GetSignal();
	std::vector<double>::iterator it_high=signal_high.begin();

	multiWf->SetActiveWaveform(activeWf);

	bool active_high = false;
	bool active_low = false;
	int NofHithigh=0;
	int NofHitlow=0;
	Int_t counter_low=0;
	Int_t counter_high=0;
	PndEmcPSAFPGASampleAnalyser::status_t status_low;
	PndEmcPSAFPGASampleAnalyser::status_t status_high;

	std::set<Int_t> hits_lowgain;
	bool overflow = false;
	// Uncomment if you want to print out waveforms
	int n=0;
	int highsize=signal_high.size();
	int lowsize=signal_low.size();
	double* higharray = new double[highsize];
	double* lowarray = new double[lowsize];
	if (fVerbose >= 7)
	{
		std::cout << "highsize and lowsize :  "<< highsize<<"  and  "<< lowsize<< std::endl;
	}
	while(it_low!=signal_low.end() && it_high!=signal_high.end()) {
		//std::cout << "Hi: ";
		const char *M[5];
		M[0]="kBaseline";M[1]="kInPulse";M[2]="kPulseDetected";M[3]="kPulseFinished";M[4]="kPileupFinished";
		if (fVerbose >= 7)
		{     
			std::cout << " I- PndEmcHighLowPSA: highgain :  " << std::endl;
		}
		fHighgainPSA->put(*it_high);
		status_high = fHighgainPSA->GetStatus();
		NofHithigh = fHighgainPSA->nHits();
		if (fVerbose >= 7)
		{
			std::cout << " I- PndEmcHighLowPSA: highgain :NofHithigh  " <<NofHithigh<< std::endl<<std::endl;
		}
		higharray[n]=*it_high;
		lowarray[n]=*it_low;
		n++;
		//std::cout << "Lo: ";
		if (fVerbose >= 7)
		{
			std::cout << " I- PndEmcHighLowPSA: lowgain :  " << std::endl;
		}
		fLowgainPSA->put(*it_low);
		status_low = fLowgainPSA->GetStatus();
		NofHitlow  = fLowgainPSA->nHits();
		if (fVerbose >= 7)
		{
			std::cout << " I- PndEmcHighLowPSA: highgain :NofHitlow  " <<NofHitlow<< std::endl;
			std::cout << " I- status_high =  "<< M[status_high] << " overflow "<< overflow<< std::endl;
			std::cout << " I- status_low =  "<< M[status_low] << " overflow "<< overflow<< std::endl<< std::endl;
			// std::cout << " I- hits_lowgain.size()  "<< hits_lowgain.size()<< std::endl;
		}
		if(status_high==PndEmcPSAFPGASampleAnalyser::kPulseFinished) {
			active_high = false;
			if(!overflow) {
				for(int i=counter_high;i<NofHithigh;i++)
				{
					fHitsInFE.push_back(std::make_pair(fHighgainPSA, i));
					if(fVerbose>=2) {
						std::cout << "-------------------I- PndEmcHighLowPSA: adding highgain hit: #:"<< i << "------"<<std::endl;
					}
				}

			}
			counter_high=NofHithigh;
		} else if(status_high==PndEmcPSAFPGASampleAnalyser::kPileupFinished) {
			active_high = false;
			if(!overflow) {
				fHitsInFE.push_back(std::make_pair(fHighgainPSA, counter_high));
				fHitsInFE.push_back(std::make_pair(fHighgainPSA, counter_high+1));
				if(fVerbose>=2) { 
					std::cout << "-------------------I- PndEmcHighLowPSA: adding highgain hits #:" << counter_high << "," << counter_high++ <<"--------"<< std::endl; //FIXME Implicit operation is bad style!
				}
			}
			counter_high+=2;
		} else if(status_high==PndEmcPSAFPGASampleAnalyser::kPulseDetected) {
			active_high = true;
		}

		if(active_high && ((*it_high) > fOverflowThreshold) && !overflow) {
			if(fVerbose>=2) { 
				std::cout << "----------I- PndEmcHighLowPSA: overflow detected" << std::endl;
			}
			overflow = true;
		}

		if(status_low==PndEmcPSAFPGASampleAnalyser::kPulseFinished) {
			active_low = false;
			if((!active_low)){
				if(overflow) {
					for(int i=counter_low;i<NofHitlow;i++)
					{
						fHitsInFE.push_back(std::make_pair(fLowgainPSA, i));
						if(fVerbose>=2) {
							std::cout << "-------------------I- PndEmcHighLowPSA: adding lowgain hit: #:"<< i << "------"<<std::endl;
						}
					}
				}
			}
			counter_low=NofHitlow;
		} else if(status_low==PndEmcPSAFPGASampleAnalyser::kPileupFinished) {
			if(fVerbose>=3) {
				std::cout << "-------------I- PndEmcHighLowPSA: pileup in lowgain candidate: #" << counter_low << ", #" << counter_low+1 << std::endl;
			}
			hits_lowgain.insert(counter_low);
			hits_lowgain.insert(counter_low+1);
			counter_low+=2;
			active_low = false;
		} else if(status_low==PndEmcPSAFPGASampleAnalyser::kPulseDetected) {
			active_low = true;
		}
		if((!active_low)&&(!active_high)) overflow = false;

		/*if((!active_high) && (!active_low)) {
			if(overflow) {
				if(fVerbose>=2) { 
					std::cout << "-----------------I- PndEmcHighLowPSA: adding lowgain hit(s) #:";
					for(std::set<Int_t>::iterator it=hits_lowgain.begin(); it!=hits_lowgain.end(); it++) {
						std::cout << *it << ",";
					}
					std::cout << "\b\n" << std::flush;
				}
				for(std::set<Int_t>::iterator it=hits_lowgain.begin(); it!=hits_lowgain.end(); it++) {
					fHitsInFE.push_back(std::make_pair(fLowgainPSA, *it));
				}
			}
			hits_lowgain.clear();
			overflow = false;
		}*/
		it_low++;
		it_high++;
	} //fHighgainPSA->put(9999);
	//    fLowgainPSA->put(9999);
	//for (int l=0; l<lowsize; l++) if(l<180) Noise[1]->Fill(lowarray[l]);//std::cout << lowarray[l] << " ";
	//std::cout<<std::endl;
	//for (int h=0; h<highsize; h++)  if(h<180) Noise[0]->Fill(higharray[h]);//std::cout << higharray[h] << " ";
	//std::cout<<std::endl;
	//std::cout<<fHitsInFE.size()<<std::endl;*/
	// f->Write();
	// Noise[1]->Write("Low",TObject::kOverwrite);
	//   Noise[0]->Write("High",TObject::kOverwrite);
	if(fVerbose>=3) std::cout<<" fHitsInFE.size() "<<fHitsInFE.size()<<std::endl;
	return fHitsInFE.size();
}


void PndEmcHighLowPSA::GetHit(Int_t i, Double_t& energy, Double_t& time) {

	PndEmcPSAFPGASampleAnalyser* psa = fHitsInFE[i].first;
	//std::cout<<"GetHit fHitsInFE[i].first "<<fHitsInFE[i].first<<std::endl;
  if (psa != nullptr) {
    psa->GetHit(fHitsInFE[i].second, energy, time);
		if(fVerbose>=3)        std::cout<<"GetHit fHitsInFE[i].second "<<fHitsInFE[i].second<<std::endl;
  } else {
    //std::cerr << "-E PndEmcHighLowPSA::GetHit: No suitable PSA available" << std::cout;
		energy = 0;
		time = 0;
  }
}
void PndEmcHighLowPSA::GetHit2(Int_t i, Double_t& energy, Double_t& time, Int_t& PileupType) {

	PndEmcPSAFPGASampleAnalyser* psa = fHitsInFE[i].first;
	//std::cout<<"GetHit fHitsInFE[i].first "<<fHitsInFE[i].first<<std::endl;
  if (psa != nullptr) {
    psa->GetHit2(fHitsInFE[i].second, energy, time, PileupType);
		if(fVerbose>=3)        std::cout<<"GetHit fHitsInFE[i].second "<<fHitsInFE[i].second<<std::endl;
  } else {
    //std::cerr << "-E PndEmcHighLowPSA::GetHit: No suitable PSA available" << std::cout;
		energy = 0;
		time = 0;
  }
}
Int_t PndEmcHighLowPSA::GetWaveformIdx(Int_t i) {

	if(i<(int)fHitsInFE.size() && i>=0) {
		PndEmcPSAFPGASampleAnalyser* psa = fHitsInFE[i].first;
		if(psa==fHighgainPSA) return fIdx_high;
		else if(psa==fLowgainPSA) return fIdx_low;
	}

	return -1;		//error
}


