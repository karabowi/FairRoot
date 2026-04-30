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

//----------------------------------------------------------------------
// File and Version Information:
//      $Id: //
// Description:
//      Class PndEmcWaveformToCalibratedDigi. Module to take the ADC waveforms and produces digi.
// 
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//		
// Author List:
//      Phil Strother                  Original Author
// Dima Melnichuk - adaption for PANDA		
// Copyright Information:
//      Copyright (C) 1996             Imperial College
//
//----------------------------------------------------------------------

#include "PndEmcWaveformToCalibratedDigi.h"

#include "PndEmcWaveform.h"
#include "PndEmcDigi.h"
#include "PndEmcDigiPar.h"		
#include "PndEmcRecoPar.h"				
#include "PndEmcAsicPulseshape.h"
#include "PndEmcPSAParabolic.h"
#include "PndEmcPSAParabolicBaseline.h"
#include "PndEmcPSAMatchedDigiFilter.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
		
#include <iostream>
#include <fstream>
//#include <map>

using std::cout;
using std::endl;
using std::fstream;

PndEmcWaveformToCalibratedDigi::PndEmcWaveformToCalibratedDigi(Int_t verbose, Bool_t storedigis)
{
	fVerbose=verbose;
	fDigiPosMethod="depth";// "surface" or "depth"
	fEmcDigiRescaleFactor=1.08;
	SetPersistency(storedigis);
	fCalibrationFileName = "";
	//fPndEmcDigiPositionDepth=6.2;
}

//--------------
// Destructor --
//--------------

PndEmcWaveformToCalibratedDigi::~PndEmcWaveformToCalibratedDigi()
{
}


InitStatus PndEmcWaveformToCalibratedDigi::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcWaveformToCalibratedDigi::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}
	
	// Get input array
	fWaveformArray = (TClonesArray*) ioman->GetObject("EmcWaveform");
	if ( ! fWaveformArray ) {
		cout << "-W- PndEmcWaveformToCalibratedDigi::Init: "
		<< "No PndEmcWaveform array!" << endl;
		return kERROR;
	}
	
	// Create and register output array
	fDigiArray = new TClonesArray("PndEmcDigi");

	ioman->Register("EmcDigi","Emc",fDigiArray, GetPersistency());
	fSampleRate=fDigiPar->GetSampleRate();
	fSampleRate_PMT=fDigiPar->GetSampleRate_PMT();
	fASIC_Shaping_int_time=fDigiPar->GetASIC_Shaping_int_time();      //s
	fPMT_Shaping_int_time=fDigiPar->GetPMT_Shaping_int_time();      //s
	fPMT_Shaping_diff_time=fDigiPar->GetPMT_Shaping_diff_time();      //s
	fCrystal_time_constant=fDigiPar->GetCrystal_time_constant();  //s
	fShashlyk_time_constant=fDigiPar->GetShashlyk_time_constant();  //s
	fNumber_of_samples_in_waveform=fDigiPar->GetNumber_of_samples_in_waveform();
	fNumber_of_samples_in_waveform_pmt=fDigiPar->GetNumber_of_samples_in_waveform_pmt();
	fEnergyDigiThreshold=fDigiPar->GetEnergyDigiThreshold();
	fEmcDigiPositionDepthPWO=fRecoPar->GetEmcDigiPositionDepthPWO();
	fEmcDigiPositionDepthShashlyk=fRecoPar->GetEmcDigiPositionDepthShashlyk();
	
	cout<<"fEmcDigiPositionDepthPWO: "<<fEmcDigiPositionDepthPWO<<endl;
	cout<<"fEmcDigiPositionDepthShashlyk: "<<fEmcDigiPositionDepthShashlyk<<endl;
				
	if (!fDigiPosMethod.CompareTo("surface"))
	{
		PndEmcDigi::selectDigiPositionMethod( PndEmcDigi::surface, 0., 0., 1.0 );
	}
	else if (!fDigiPosMethod.CompareTo("depth"))
	{
	   PndEmcDigi::selectDigiPositionMethod( PndEmcDigi::depth, 
			 fEmcDigiPositionDepthPWO, fEmcDigiPositionDepthShashlyk, fEmcDigiRescaleFactor);
	}
	else 
	{
		cout << "-W- PndEmcWaveformToCalibratedDigi::Init: "
		<< "Unknown digi position method!" << endl;
		return kERROR;
	}
	
	fPulseshape= new PndEmcAsicPulseshape(fASIC_Shaping_int_time,fCrystal_time_constant);
	fPulseshape_pmt= new PndEmcCRRCPulseshape(fPMT_Shaping_int_time,fPMT_Shaping_diff_time,fShashlyk_time_constant);


	// Pulse shape analysis algorithm.
	// Simple parabolic fit.
	//psaAlgorithm = new PndEmcPSAParabolic();

	// Matched digital filter
	// Parameters of the filter are hardcoded at the moment
	// For different pulseshape in barrel and endcaps different filters should be implemented
 	std::vector<Double_t> params;
 	params.push_back(30); // width
	params.push_back(fSampleRate); // Sample rate
	psaAlgorithm = new PndEmcPSAMatchedDigiFilter(params,fPulseshape);

//	std::vector<Double_t> params2;
// 	params2.push_back(30); // width
//	params2.push_back(fSampleRate_PMT); // Sample rate
	psaAlgorithm_pmt = new PndEmcPSAParabolic();
	
	// Determine normalisation constant for PndEmcWaveform
	PndEmcWaveform *tmpwaveform=new PndEmcWaveform(0,101010001, fNumber_of_samples_in_waveform);
	PndEmcWaveform *tmpwaveform2=new PndEmcWaveform(0,101010001, fNumber_of_samples_in_waveform_pmt);
	
	PndEmcHit *gevHit=new PndEmcHit();
	gevHit->SetEnergy(1.0);
	gevHit->SetTime(0.);
	tmpwaveform->UpdateWaveform(gevHit, 0, false, 1., 0., fSampleRate, fPulseshape);
	tmpwaveform2->UpdateWaveform(gevHit, 0, false, 1., 0., fSampleRate_PMT, fPulseshape_pmt);
	Double_t tmpPeakPosition;
	Double_t tmpPeakPosition2;
	psaAlgorithm->Process(tmpwaveform,fWfNormalisation,tmpPeakPosition);
	psaAlgorithm_pmt->Process(tmpwaveform2,fWfNormalisation_pmt,tmpPeakPosition2);

	psaAlgorithm_proto192 = new PndEmcPSAParabolicBaseline(20);
	fWfNormalisation_proto192 = 1;

	if(fCalibrationFileName !="")
		ReadCalibrationFile();

  LOG(info) << " PndEmcWaveformToCalibratedDigi: Read " << fCalibrationMap.size() << " Calibration Entries";

  LOG(info) << " PndEmcWaveformToCalibratedDigi: Intialization successfull";

  return kSUCCESS;
}

void PndEmcWaveformToCalibratedDigi::Exec(Option_t*)
{
	TStopwatch timer;
	if (fVerbose>2){
		timer.Start();
	}
// Reset output array
	if ( ! fDigiArray ) Fatal("Exec", "No Digi Array");
  	fDigiArray->Delete();
	Double_t peakPosition;
	Double_t energy;
	Double_t digi_time;
	Int_t i_digi=0; //index of digi in TClonesArray
	Int_t hitIndex;
	Int_t nHits;
	Int_t detId;
	Int_t trackId;
	//Int_t module; //[R.K. 03/2017] unused variable?
	Int_t nWaveforms = fWaveformArray->GetEntriesFast();
	//cout<<"PndEmcWaveformToCalibratedDigi: "<<nWaveforms<<" waveforms to convert"<<endl;
	for (Int_t iWaveform=0; iWaveform<nWaveforms; iWaveform++) {
		PndEmcWaveform* theWaveform = (PndEmcWaveform*) fWaveformArray->At(iWaveform);
		hitIndex=theWaveform->GetHitIndex();
		detId=theWaveform->GetDetectorId();
		trackId=theWaveform->GetTrackId();
		//module=theWaveform->GetModule(); //[R.K. 03/2017] unused variable?
		// Determine waveform maximum and its position
/*		if(module==5){
			psaAlgorithm_pmt->Process(theWaveform,energy,peakPosition);
			energy/=fWfNormalisation_pmt;
			digi_time=peakPosition/fSampleRate_PMT;

		}
		else{
			psaAlgorithm->Process(theWaveform,energy,peakPosition);
			energy/=fWfNormalisation;
			digi_time=peakPosition/fSampleRate;
		}
		*/
		nHits = psaAlgorithm_proto192->Process(theWaveform);
		for(Int_t i = 0; i< nHits; i++){
			psaAlgorithm_proto192->GetHit(i,energy,peakPosition);
			energy/=fWfNormalisation_proto192;
			digi_time = peakPosition/fSampleRate;
			std::map<Int_t,Double_t>::iterator it;
			it = fCalibrationMap.find(detId);
			if(it!=fCalibrationMap.end()){
				//            std::cout << "found calibration value for hitindex " << hitIndex << std::endl;
				energy/=it->second;
			}
			//        std::cout << "energy: " << energy << " threshold: "<< fEnergyDigiThreshold << endl;
			//            std::cout << "creating digi for detid: " << detId << "hitindex: " << hitIndex <<std::endl;
			if (energy>fEnergyDigiThreshold)
			{
				//            std::cout << "energy: " << energy << endl;
				PndEmcDigi* myDigi = new((*fDigiArray)[i_digi]) PndEmcDigi(trackId,detId, energy, digi_time, hitIndex);
				myDigi->AddLink(FairLink("EmcWaveform", iWaveform));
				i_digi++;

			}
		}
	}
	if (fVerbose>2){
		timer.Stop();
		Double_t rtime = timer.RealTime();
		Double_t ctime = timer.CpuTime();
		cout << "PndEmcWaveformToCalibratedDigi, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}

}

void PndEmcWaveformToCalibratedDigi::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) LOG(fatal) << "SetParContainers: No runtime database";

  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar*) db->getContainer("PndEmcDigiPar");
  
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");
 
}

void PndEmcWaveformToCalibratedDigi::SetStorageOfData(Bool_t val)
{
  SetPersistency(val);
  return;
}

void PndEmcWaveformToCalibratedDigi::ReadCalibrationFile(){
	std::ifstream in(fCalibrationFileName,std::ifstream::in);
//    in.open(fCalibrationFileName);
	if (!in.good()) {
		std::cerr << "Cannot open calibration file!" << endl;
		return;
	}

	char buf[255];
	while (in.getline(buf, 255)) {
		TString tmp = buf;
		TObjArray *tokens = tmp.Tokenize(" \t;");
		if(tokens->GetEntries()<2){
			continue;
		}
		tmp=tokens->UncheckedAt(0)->GetName();
		TString tmp2 = tokens->UncheckedAt(1)->GetName();
		if(tmp.IsDigit() && tmp2.IsFloat()){
			fCalibrationMap.insert(std::pair<Int_t,Double_t>(tmp.Atoi(),tmp2.Atof()));

		}
	}
}


ClassImp(PndEmcWaveformToCalibratedDigi)
