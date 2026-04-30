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
//      Class PndEmcWaveformToDigi. Module to take the ADC waveforms and produces digi.
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

#include "PndEmcWaveformToDigi.h"
#include "PndEmcMapper.h"
#include "PndEmcWaveform.h"
#include "PndEmcDigi.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcFpgaPar.h"
#include "PndEmcAsicPulseshape.h"
#include "PndEmcPSAParabolic.h"
#include "PndEmcPSAMatchedDigiFilter.h"
#include "FairEventHeader.h"
#include "FairRootManager.h"
#include "FairFileSource.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "PndEmcPSAFPGADigitalFilterAnalyser.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include "PndEmcStructure.h"
#include "PndEmcXtal.h"
#include "PndMCTrack.h"
#include <iostream>
#include "PndEmcPSAMatchedDigiFilter.h"
#include "PndEmcSimCrystalCalibrator.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TRandom.h"

using std::cout;
using std::endl;
using std::fstream;

PndEmcWaveformToDigi::PndEmcWaveformToDigi(Int_t verbose, Bool_t storedigis)
  : fWaveformArray(nullptr), fDigiArray(nullptr), fSampleRate(0), fSampleRate_PMT(0), fEnergyDigiThreshold(0), fASIC_Shaping_int_time(0), fPMT_Shaping_int_time(0),
    fPMT_Shaping_diff_time(0), fCrystal_time_constant(0), fShashlyk_time_constant(0), fNumber_of_samples_in_waveform(0), fNumber_of_samples_in_waveform_pmt(0), fDigiPosMethod(0),
    fEmcDigiRescaleFactor(0), fEmcDigiPositionDepthPWO(0), fEmcDigiPositionDepthShashlyk(0), fPulseshape(nullptr), fPulseshape_pmt(nullptr), fpsaAlgorithm(nullptr),
    fpsaAlgorithm_pmt(nullptr), fpsaAlgorithm_fwd(nullptr), fCalibrator(), fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()), fFpgaPar(new PndEmcFpgaPar()),
    fVerbose(verbose), fStoreDigis(storedigis), fTimeOrderedDigi(kFALSE), fWfNormalisation(0), fWfNormalisation_fwd(0), fWfNormalisation_pmt(0), fDigitizationVersion2(kFALSE),
    fFakeOnline(false)
{
	fDigiPosMethod="depth";// "surface" or "depth"
	fEmcDigiRescaleFactor=1.08;
	SetPersistency(storedigis);
	//fPndEmcDigiPositionDepth=6.2;
}

//--------------
// Destructor --
//--------------
PndEmcWaveformToDigi::~PndEmcWaveformToDigi()
{
	delete fPulseshape;
	delete fPulseshape_pmt;
	delete fPulseshape_fwd;

}


/**
 * @brief Init Task
 * 
 * Prepares the TClonesArrays of PndEmcWaveform for reading 
 * and of PndEmcDigi for writing. Also reads the EMC parameters and prepares the pulseshapes
 * (PndEmcAbsPulseshape) and pulse shape analyser (PndEmcAbsPSA).
 * 
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcWaveformToDigi::Init()
{
	PndEmcDigi::InitDigiArrayTBD();
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcWaveformToDigi::Init: "
			<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	// Get input array
	if(fTimeOrderedDigi){
		fWaveformArray = dynamic_cast<TClonesArray *>( ioman->GetObject("EmcSortedWaveform"));
		if (!FairRunAna::Instance()->IsTimeStamp()) {
			cout << "-W- PndEmcWaveformToDigi::Init: "
				  << "Task running timebased, but run not running timebased" << endl;
		}
	}else{
		fWaveformArray =dynamic_cast<TClonesArray *> (ioman->GetObject("EmcWaveform"));
	}
//		fWaveformArrayOrg = (TClonesArray*) ioman->GetObject("EmcWaveform");
	//fEvtHeaderArray = (TClonesArray*) ioman->GetObject("EventHeader.");

	if ( ! fWaveformArray ) {
		cout << "-W- PndEmcWaveformToDigi::Init: "
			<< "No PndEmcWaveform array!" << endl;
		return kERROR;
	}

	// Create and register output array
	fDigiArray = new TClonesArray("PndEmcDigi");
	ioman->Register("EmcDigi","Emc",fDigiArray,GetPersistency());


	fSampleRate=fDigiPar->GetSampleRate();
	fSampleRate_PMT=fDigiPar->GetSampleRate_PMT();
	fSampleRate_FWD = fDigiPar->GetSampleRate_FWD();
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
	fNumber_of_samples_in_waveform_fwd=fDigiPar->GetNumber_of_samples_in_waveform_fwd();
	fFWD_Shaping_int_time=fDigiPar->GetFWD_Shaping_int_time();
	fFWD_time_constant=fDigiPar->GetFWD_time_constant(); 

	if(fVerbose>2){
		cout<<"fEnergyDigiThreshold: "<<fEnergyDigiThreshold<<endl;
		cout<<"fEmcDigiPositionDepthPWO: "<<fEmcDigiPositionDepthPWO<<endl;
		cout<<"fEmcDigiPositionDepthShashlyk: "<<fEmcDigiPositionDepthShashlyk<<endl;
	}

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
		cout << "-W- PndEmcWaveformToDigi::Init: "
			<< "Unknown digi position method!" << endl;
		return kERROR;
	}

	fPulseshape= new PndEmcAsicPulseshape(fASIC_Shaping_int_time, fCrystal_time_constant);
	fPulseshape_pmt= new PndEmcCRRCPulseshape(fPMT_Shaping_int_time,fPMT_Shaping_diff_time,fShashlyk_time_constant);
	fPulseshape_fwd =new PndEmcAsicPulseshape(fFWD_Shaping_int_time, fFWD_time_constant);//LNP raw signal, decay time constant 25 microseconds

	// Determine normalisation constant for PndEmcWaveform
	PndEmcWaveform *tmpwaveform1=new PndEmcWaveform(0,101010001, fSampleRate, fNumber_of_samples_in_waveform);
	PndEmcWaveform *tmpwaveform2=new PndEmcWaveform(0,101010001, fSampleRate_PMT, fNumber_of_samples_in_waveform_pmt);
	PndEmcWaveform *tmpwaveform3=new PndEmcWaveform(0,101010001, fSampleRate_FWD, fNumber_of_samples_in_waveform_fwd);//length 10

	PndEmcHit *gevHit=new PndEmcHit();
	gevHit->SetEnergy(1.0);
	gevHit->SetTime(0.);
	tmpwaveform1->UpdateWaveform(gevHit, 0, false, 1., 0., fSampleRate, fPulseshape);
	tmpwaveform2->UpdateWaveform(gevHit, 0, false, 1., 0., fSampleRate_PMT, fPulseshape_pmt);
	tmpwaveform3->UpdateWaveform(gevHit, 0, false, 1., 0., fSampleRate_FWD, fPulseshape_fwd);

	fWfNormalisation = tmpwaveform1->Max();
	fWfNormalisation_pmt = tmpwaveform2->Max();
	fWfNormalisation_fwd = tmpwaveform3->Max();

	fFpgaPar->printParams();
	if(fVerbose > 2){
		cout<<"fWfNormalisation#"<<fWfNormalisation<<", I/A ="<<(tmpwaveform1->Integral()/fWfNormalisation)<<endl;
		cout<<"fWfNormalisation#"<<fWfNormalisation<<", I ="<<tmpwaveform1->Integral()<<endl;
		cout<<"fWfNormalisation_pmt#"<<fWfNormalisation_pmt<<", I/A ="<<(tmpwaveform2->Integral()/fWfNormalisation_pmt)<<endl;
		cout<<"fWfNormalisation_fwd#"<<fWfNormalisation_fwd<<", I ="<<(tmpwaveform3->Integral())<<endl;
		cout<<"fWfNormalisation_fwd#"<<fWfNormalisation_fwd<<", I/A ="<<(tmpwaveform3->Integral()/fWfNormalisation_fwd)<<endl;
	}
	//setup barrel backward
	PndEmcPSAFPGADigitalFilterAnalyser* theAlg2 = new PndEmcPSAFPGADigitalFilterAnalyser("BarBckFilter");
	PndEmcPSAFPGASampleAnalyser::SampleAnalyserParams newParams;
	newParams.ma_trig_M = fFpgaPar->GetBarrelMAFilterLength();
	newParams.cf_delay = fFpgaPar->GetBarrelCFFilterLength();
	newParams.cf_ratio = fFpgaPar->GetBarrelCFFilterRatio();
	newParams.cf_fitter_length = fFpgaPar->GetBarrelCFFitterLength();
	newParams.cf_fit_offset = fFpgaPar->GetBarrelCFDelayLength();
	newParams.mwd_length = fFpgaPar->GetBarrelMWDFilterLength();
	newParams.mwd_tau = fFpgaPar->GetBarrelMWDFilterLifeT();
	newParams.hit_threshold = fFpgaPar->GetBarrelPulseThreshold()*fWfNormalisation*0.001;
	newParams.clock_unit = 1./fSampleRate*1.e9;//in unit of ns
	newParams.iafactor = tmpwaveform1->Integral()/fWfNormalisation;//31.4166;
	theAlg2->enable_mwd_filter(fFpgaPar->GetBarrelMWDFilterUsed());
	theAlg2->init(newParams);
	theAlg2->setBaselineInterval(0,0);
	fpsaAlgorithm= theAlg2;
	//setup forward digital filter
	PndEmcPSAFPGADigitalFilterAnalyser* theAlg1 = new PndEmcPSAFPGADigitalFilterAnalyser("FwdFilter");
	//PndEmcPSAFPGASampleAnalyser::SampleAnalyserParams newParams;
	newParams.ma_trig_M = fFpgaPar->GetForwardMAFilterLength();
	newParams.cf_delay = fFpgaPar->GetForwardCFFilterLength();
	newParams.cf_ratio = fFpgaPar->GetForwardCFFilterRatio();
	newParams.cf_fitter_length = fFpgaPar->GetForwardCFFitterLength();
	newParams.cf_fit_offset = fFpgaPar->GetForwardCFDelayLength();
	newParams.mwd_length = fFpgaPar->GetForwardMWDFilterLength();
	newParams.mwd_tau = fFpgaPar->GetForwardMWDFilterLifeT();
	newParams.hit_threshold = fFpgaPar->GetForwardPulseThreshold()*fWfNormalisation_fwd*0.001;//five times larger than noise,
	newParams.clock_unit = 1./fSampleRate_FWD*1.e9;//in unit of ns
	newParams.iafactor = tmpwaveform3->Integral()/fWfNormalisation_fwd;//151.408;
	theAlg1->enable_mwd_filter(fFpgaPar->GetForwardMWDFilterUsed());
	theAlg1->init(newParams);
	theAlg1->setBaselineInterval(0,0);
	fpsaAlgorithm_fwd = theAlg1;

	//setup shashylik
	PndEmcPSAFPGADigitalFilterAnalyser* theAlg3 = new PndEmcPSAFPGADigitalFilterAnalyser("ShashylikFilter");
	//PndEmcPSAFPGASampleAnalyser::SampleAnalyserParams newParams;
	newParams.ma_trig_M = fFpgaPar->GetShashylikMAFilterLength();
	newParams.cf_delay = fFpgaPar->GetShashylikCFFilterLength();
	newParams.cf_ratio = fFpgaPar->GetShashylikCFFilterRatio();
	newParams.cf_fitter_length = fFpgaPar->GetShashylikCFFitterLength();
	newParams.cf_fit_offset = fFpgaPar->GetShashylikCFDelayLength();
	newParams.mwd_length = fFpgaPar->GetShashylikMWDFilterLength();
	newParams.mwd_tau = fFpgaPar->GetShashylikMWDFilterLifeT();
	newParams.hit_threshold = fFpgaPar->GetShashylikPulseThreshold()*fWfNormalisation_pmt*0.001;
	newParams.clock_unit = 1./fSampleRate_PMT*1.e9;//in unit of ns
	newParams.iafactor = tmpwaveform2->Integral()/fWfNormalisation_pmt;//5.08306;//this factor depends on the pulseshape function
	theAlg3->enable_mwd_filter(fFpgaPar->GetShashylikMWDFilterUsed());
	theAlg3->init(newParams);
	theAlg3->setBaselineInterval(0,0);
	fpsaAlgorithm_pmt = theAlg3;

	Int_t iMapperVersion;
	PndEmcMapper::Init(iMapperVersion=1);
	PndEmcStructure::Instance();

	fFunctor = new StopTime();
	fEventNo = 0;

	if(fDigitizationVersion2){
		// Matched digital filter
		// Parameters of the filter are hardcoded at the moment
		// For different pulseshape in barrel and endcaps different filters should be implemented
		std::vector<Double_t> params;
		params.push_back(30); // width
		params.push_back(fSampleRate); // Sample rate
		fpsaAlgorithm = new PndEmcPSAMatchedDigiFilter(params,fPulseshape);
		fpsaAlgorithm_fwd = fpsaAlgorithm;
		// Pulse shape analysis algorithm.
		// Simple parabolic fit.
		fpsaAlgorithm_pmt = new PndEmcPSAParabolic();
	}

  if (fCalibrator == nullptr) {
    PndEmcSimCrystalCalibrator *SimCalibrator = new PndEmcSimCrystalCalibrator();
		// Determine normalisation constant for PndEmcWaveform
		Double_t tmpPeakPosition;
		Double_t WfNormalisation;
		fpsaAlgorithm->Process(tmpwaveform1);
		fpsaAlgorithm->GetHit(0, WfNormalisation, tmpPeakPosition);
		SimCalibrator->SetCalibration(1,WfNormalisation); // Barrel
		SimCalibrator->SetCalibration(2,WfNormalisation); // Barrel
		SimCalibrator->SetCalibration(4,WfNormalisation); // Backward endcap
		fpsaAlgorithm_pmt->Process(tmpwaveform2);
		fpsaAlgorithm_pmt->GetHit(0, WfNormalisation, tmpPeakPosition);
		SimCalibrator->SetCalibration(5,WfNormalisation); // Shashlyk
		fpsaAlgorithm_fwd->Process(tmpwaveform3);
		fpsaAlgorithm_fwd->GetHit(0, WfNormalisation, tmpPeakPosition);
		SimCalibrator->SetCalibration(3,WfNormalisation); // Forward endcap
		fCalibrator=SimCalibrator;
  }
  fCalibrator->Init();


	delete tmpwaveform1;
	delete tmpwaveform2;
	delete tmpwaveform3;
	//delete tmpwaveform4;
	delete gevHit;

	totDigisAboveThreshold = 0;
	totHits = 0;
	totExpHits = 0;
	totNumOfWave = 0;

  LOG(info) << " PndEmcWaveformToDigi: Intialization successfull";
  return kSUCCESS;
}

/**
 * @brief Runs the task.
 * 
 * The task loops over the waveforms and uses the pulse shape analyser (PndEmcAbsPSA) to
 * extract signal height and timing. From this the PndEmcDigi are created.
 * 
 * @param opt unused
 * @return void
 */
void PndEmcWaveformToDigi::Exec(Option_t*)
{
	TStopwatch timer;
	if (fVerbose>2){
		timer.Start();
	}

	fDigiArray->Delete();

	Double_t fevtTime = FairRootManager::Instance()->GetEventTime();

	if(fVerbose>0){
		cout<<endl;
		cout<<"=======================PndEmcWaveformToDigi======================="<<endl;
		std::cout<<"Event NO. #"<<fEventNo<<", event time # "<<fevtTime <<std::endl;
	}
	if(fTimeOrderedDigi){
		if(FairRunAna::Instance()->IsTimeStamp()){
			fWaveformArray->Delete();
			Double_t time_length = 40.;//99.98%
			if(fVerbose >0)
				cout<<"--I-- time-based simulation, read data to later #"<<time_length<<" ns"<<endl;
			fWaveformArray  = FairRootManager::Instance()->GetData("EmcSortedWaveform"
					, fFunctor
					, fevtTime + time_length);
		}
		if(fVerbose>0)
			std::cout<<"fDigiArrayTBD size #"<<PndEmcDigi::fDigiArrayTBD->GetEntriesFast()<<std::endl;
	}
	Int_t nWaveforms = fWaveformArray->GetEntriesFast();
	if(fVerbose>0){
		std::cout<<"fWaveformArray size #"<<nWaveforms<<std::endl;
	}
	//std::cout<<"fEvtHeaderArray size #"<<fEvtHeaderArray->GetEntriesFast()<<std::endl;
	//Int_t evtNo = FairRunAna::Instance()->GetEventHeader()->GetMCEntryNumber();
	//for(Int_t iheader=0;iheader<fEvtHeaderArray->GetEntriesFast();++iheader){
	//	FairEventHeader* evtHeader= (FairEventHeader*) fEvtHeaderArray->At(iheader);
	//	cout<<"===> Event NO. #"<<evtHeader->GetMCEntryNumber()<<", event time#"<<evtHeader->GetEventTime()<<endl;
	//}


	Double_t fdigiEnergy(0.), fdigiTime(0.), theSampleRate, minT(1e10), maxT(-1e10);
	Int_t hitIndex, nHits, detId, trackId, i_digi(0), fMod;
  PndEmcAbsPSA *thePSA(nullptr);

  totNumOfWave += nWaveforms;
	for (Int_t iWaveform=0; iWaveform<nWaveforms; iWaveform++) 
	{
		PndEmcWaveform* theWaveform = (PndEmcWaveform*) fWaveformArray->At(iWaveform);
		hitIndex=theWaveform->GetHitIndex();
		detId=theWaveform->GetDetectorId();
		trackId=theWaveform->GetTrackId();
		fMod=theWaveform->GetModule();

		if(theWaveform->GetTimeStamp() < minT ) minT = theWaveform->GetTimeStamp();
		if(theWaveform->GetTimeStamp() > maxT ) maxT = theWaveform->GetTimeStamp();
		//Double_t timeshift; // how maximum is shifted
		if(fMod== 5){
			thePSA = fpsaAlgorithm_pmt;
			theSampleRate = fSampleRate_PMT;
		} else if(fMod == 3){
			thePSA = fpsaAlgorithm_fwd;
			theSampleRate = fSampleRate_FWD;
		}else
		{
			thePSA = fpsaAlgorithm;
			theSampleRate = fSampleRate;
		}
		//cout<<endl;
		const std::vector<Int_t>& evtList = theWaveform->GetEvtList();
		if(fVerbose > 1){
			cout<<endl;
			std::cout<<"The wave#"<<iWaveform<<" includes "<<evtList.size()<<" events #";
			for(size_t i=0; i< evtList.size(); ++i){
				std::cout<<evtList[i]<<" ";
			}
			cout<<endl;
		}

		nHits = thePSA->Process(theWaveform);

		if(fVerbose > 1){
			std::cout<<"detID#"<<theWaveform->GetDetectorId()<<", hits#"<<nHits<<std::endl;
		}
		totHits += nHits;
		totExpHits += evtList.size();

		for(Int_t i = 0 ; i< nHits; i++)
		{
			thePSA->GetHit(i, fdigiEnergy, fTimeShift);
			if(fCalibrator->Calibrate(fdigiEnergy,detId)!=PndEmcAbsCrystalCalibrator::kCALOK){
				continue;
			}
			fdigiTime = theWaveform->GetTimeStamp() + fTimeShift/theSampleRate*1.e9;//translate position to global time

			if(fVerbose>1){
				//if(fdigiTime-fevtTime>200. || fdigiTime - fevtTime < 0.)
				{
					cout<<"wave #"<<iWaveform<<", mod#"<<fMod<<", theSampleRate#"<<theSampleRate<<", shift#"<<fTimeShift<<endl;
					cout<<"fdigiEnergy#"<<fdigiEnergy<<", fdigiTime#"<<fdigiTime<<", evtTime#"<<fevtTime<<", diffT#"<<(fdigiTime-fevtTime)<<endl;
				}
			}

			if (fdigiEnergy>fEnergyDigiThreshold)
			{
				++totDigisAboveThreshold;

				PndEmcDigi* myDigi(0);
				//std::cout<<"PndEmcWaveformToDigi::trackId #"<<trackId<<endl;
				myDigi = new((*fDigiArray)[i_digi++]) PndEmcDigi(trackId,detId, fdigiEnergy, fdigiTime,fdigiTime-fevtTime, hitIndex);
				myDigi->fEvtNo = evtList[i];//fEventNo;
				myDigi->AddLink(FairLink("EmcWaveform", iWaveform));
			}
		}
	}
	Int_t nOldSize = fDigiArray->GetEntriesFast();
	Int_t nDigi =  PndEmcDigi::fDigiArrayTBD->GetEntriesFast();
	if(fTimeOrderedDigi){
		for(Int_t iDigi = 0; iDigi < nDigi; ++iDigi)
		{
			PndEmcDigi* copy = (PndEmcDigi*)PndEmcDigi::fDigiArrayTBD->At(iDigi);
	if(fFakeOnline) copy->SetEnergy(SmearFakeOnline(copy->GetEnergy()));
			new((*fDigiArray)[i_digi++]) PndEmcDigi(*copy);
		}
		//free buffer
		PndEmcDigi::fDigiArrayTBD->Delete();
	}
	if(fVerbose >0){
		Int_t nNewSize = fDigiArray->GetEntriesFast();
		std::cout<<nNewSize<<" digis, "<<nOldSize<<" <--waveform, "<<nDigi<<" <--previous event."<<std::endl;
		cout<<"=======================PndEmcWaveformToDigi======================="<<endl;
	}

	//if(fVerbose >1){
	//	cout<<"=======================PndEmcWaveformToDigi======================="<<endl;
	//	cout<<"time region #"<<minT<<", "<<maxT<<endl;
	//	cout<<"=================================================================="<<endl;
	//}

	if (fVerbose>2){
		timer.Stop();
		Double_t rtime = timer.RealTime();
		Double_t ctime = timer.CpuTime();
		cout << "PndEmcWaveformToDigi, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}
	fEventNo ++;
}

void PndEmcWaveformToDigi::SetParContainers() {

	// Get run and runtime database
	FairRun* run = FairRun::Instance();
	if ( ! run ) LOG(fatal) << "SetParContainers: No analysis run";

	FairRuntimeDb* db = run->GetRuntimeDb();
	if ( ! db ) LOG(fatal) << "SetParContainers: No runtime database";

	// Get Emc digitisation parameter container
	fDigiPar = (PndEmcDigiPar*) db->getContainer("PndEmcDigiPar");

	// Get Emc reconstruction parameter container
	fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");

	// Get Emc FPGA parameter container
	fFpgaPar = (PndEmcFpgaPar*) db->getContainer("PndEmcFpgaPar");
}

void PndEmcWaveformToDigi::SetStorageOfData(Bool_t val)
{
	SetPersistency(val);
	return;
}

/**
 * @brief Called at end of task.
 * 
 * Outputs some statistics.
 * 
 * @return void
 */
void PndEmcWaveformToDigi::FinishTask()
{

	//unmatchCount += fBufferofDigisToBeDetermined.size();
	std::cout<<"========================================================="<<std::endl;
	std::cout<<"PndEmcWaveformToDigi::FinishTask"<<std::endl;
	std::cout<<"========================================================="<<std::endl;
	std::cout<<"Total waveforms# "<<totNumOfWave<<std::endl;
	std::cout<<"Total expected hits# "<<totExpHits<<std::endl;
	std::cout<<"Total fpga hits# "<<totHits<<std::endl;
	std::cout<<"Total digis Above th. #"<<totDigisAboveThreshold<<", threshold# "<<fEnergyDigiThreshold<<std::endl;
	std::cout<<"========================================================="<<std::endl;
	//fpsaAlgorithm->Print();
	//fpsaAlgorithm_pmt->Print();
	//std::cout<<"pileup events #"<<PileupEventSet.size()<<std::endl;
	//std::cout<<"Correct Ana#"<<CorrectAna<<std::endl;
	//std::cout<<"UnCorrect Ana#"<<unCorrectAna<<std::endl;
	//std::set<Int_t>::iterator it = PileupEventSet.begin();
	//std::set<Int_t>::iterator end = PileupEventSet.end();
	//Int_t LineNo= 0;
	//for(; it != end; ++it){
	//	std::cout<<(*it)<<' ';
	//	if(++LineNo % 10 == 0)
	//		std::cout<<std::endl;
	//}
	//std::cout<<std::endl;

}

/*Double_t PndEmcWaveformToDigi::GetEventTimebyDigiTime(Double_t digiT, Double_t digiE, Int_t detID, bool PrintOut) const
	{
	PndEmcTwoCoordIndex* _tci = PndEmcMapper::Instance()->GetTCI(detID);
	PndEmcTciXtalMap& map = const_cast<PndEmcTciXtalMap&> ( PndEmcStructure::Instance()->GetTciXtalMap());
	TVector3 Where = PndEmcDigi::algPointer()(map.find(_tci)->second);
	Double_t path = Where.Mag();
//Double_t path = map[_tci]->frontCentre().Mag();


TVectorD vFunc(6);
vFunc[0]=1.;
vFunc[1]=path;
vFunc[2]=1./TMath::Sqrt(digiE);
vFunc[3]=1./digiE;
vFunc[4]=vFunc[3]*vFunc[3];
vFunc[5]=vFunc[4]*vFunc[3];

Double_t corrT(0.);
Int_t mod = detID/100000000;

if(mod == 3){
corrT = CoeffMod3*vFunc;
}else if( mod == 5){
corrT = CoeffMod5*vFunc;
}else{
corrT = CoeffModo*vFunc;
}
if(PrintOut){
//if(mod == 5){
//	vFunc.Print();
//	CoeffMod5.Print();
//}
cout<<"path = "<<path<<", mod = "<<mod<<", corrT = "<<corrT<<endl;
}
return digiT - corrT;
}
Int_t PndEmcWaveformToDigi::GetIdxByEnergy(Double_t energy) const
{
if(energy < 0.006) return 0;
if(energy < 0.008) return 1;
if(energy < 0.010) return 2;
if(energy < 0.1)
return 2 + Int_t(energy/0.01);//energy step 0.01GeV
if(energy >= 0.1){
//Int_t idx = Int_t(energy/0.1);
//if(idx > 2) idx = 2;
return 12;
}
}*/


 Float_t PndEmcWaveformToDigi::SmearFakeOnline(Float_t energy)
 { // Additional fake energy resolution to get about a factor 2 worse
   // dE/E =~ 1% + 1.63%/(E/GeV)
   if(!fFakeOnline) return energy;
   Float_t resolution=0.01*energy+0.0163*sqrt(energy);
   return gRandom->Gaus(energy,resolution);
 }


ClassImp(PndEmcWaveformToDigi)
