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

#include "PndEmcFWEndcapDigi.h"

#include "PndEmcWaveformData.h"
#include "PndEmcMultiWaveform.h"
#include "PndEmcPSAFPGAPileupAnalyser.h"
#include "PndEmcHighLowPSA.h"
#include "PndEmcDigi.h"
#include "PndEmcFWEndcapDigiPar.h"		
#include "PndEmcGeoPar.h"		
#include "PndEmcRecoPar.h"				
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcSimCrystalCalibrator.h"
#include "PndEmcFWEndcapTimebasedWaveforms.h"
#include "PndEmcHit.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TF1.h"

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>

using std::cout;
using std::endl;

PndEmcFWEndcapDigi::PndEmcFWEndcapDigi(Int_t verbose, Bool_t storedigis)
  : fWaveformArray(nullptr), fDigiArray(nullptr), fDigiArray2(nullptr), fEnergyDigiThreshold(0), fDigiPosMethod(""), fEmcDigiRescaleFactor(0.), fEmcDigiPositionDepthPWO(0),
    fEmcDigiPositionDepthShashlyk(0), fHighgainPSA(nullptr), fLowgainPSA(nullptr), fHighLowPSA(verbose), fCalibrator(nullptr), fDigiPar(nullptr), fRecoPar(nullptr),
    fGeoPar(nullptr), fVerbose(verbose), fStoreDigis(storedigis), fTimeOrderedDigi(kFALSE), fNrOfEvents(0), evtCounter(0), nFwDigiProg(0)
{
	SetPersistency(storedigis);
}

//--------------
// Destructor --
//--------------
PndEmcFWEndcapDigi::~PndEmcFWEndcapDigi()
{
  if (fDigiArray != nullptr)
    delete fDigiArray;
  if (fDigiArray2 != nullptr)
    delete fDigiArray2;
}

/**
 * @brief Init Task
 * 
 * Prepares the TClonesArray of PndEmcMultiWaveform for reading and PndEmcDigi for writing.
 * Also reads the EMC parameters and prepares the pulseshapes
 * (PndEmcAbsPulseshape) and pulse shape analyser (PndEmcAbsPSA) as well as the
 * calibrator (PndEmcSimCrystalCalibrator).
 * 
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcFWEndcapDigi::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcFWEndcapDigi::Init: "
				<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	// Get nr of events	(divide by 100 for progress counter)
	fNrOfEvents = ((ioman->GetInTree())->GetEntriesFast())/100;

	// Get input array
	fWaveformArray = (TClonesArray*) ioman->GetObject("EmcMultiWaveform");
	if (!fWaveformArray) {
		//check if EmcWaveform contains MultiWaveforms
		fWaveformArray = (TClonesArray*) ioman->GetObject("EmcWaveform");
		if((!fWaveformArray) || (!fWaveformArray->GetClass()->InheritsFrom("PndEmcMultiWaveform"))){
			cout << "-W- PndEmcFWEndcapDigi::Init: "
					<< "No PndEmcWaveform array containing multi waveforms!" << endl;
			return kERROR;
		}
		if (fWaveformArray) cout << "[INFO\t] Waveform array present!"<<"    PndEmcFWEndcapDigi::Init started" << endl;

	}

	if(!fDigiPar) {
		cout << "-E- PndEmcFWEndcapTimebasedWaveforms::Init: "
				<< "no DigiPar containter found" << endl;
		return kFATAL;
	}
	fHitArray = (TClonesArray*) ioman->GetObject("EmcHit");
	// Create and register output array
	fDigiArray = ioman->Register("EmcDigi","PndEmcDigi", "Emc", GetPersistency());
	fDigiArray2 = ioman->Register("EmcDigiPile","PndEmcDigi", "Emc", GetPersistency());
	//position methods
	fEmcDigiPositionDepthPWO=fRecoPar->GetEmcDigiPositionDepthPWO();
	fEmcDigiPositionDepthShashlyk=fRecoPar->GetEmcDigiPositionDepthShashlyk();

	cout<<"fEmcDigiPositionDepthPWO: "<<fEmcDigiPositionDepthPWO<<endl;
	cout<<"fEmcDigiPositionDepthShashlyk: "<<fEmcDigiPositionDepthShashlyk<<endl;

	//digi pos method				
	fDigiPosMethod = "depth";	//ATTN: hardcoded...do we need this at all?
	fEmcDigiRescaleFactor = 1.08;	
	if (!fDigiPosMethod.CompareTo("surface")) {
		PndEmcDigi::selectDigiPositionMethod( PndEmcDigi::surface, 0., 0., 1.0 );
	} else if (!fDigiPosMethod.CompareTo("depth")) {
		PndEmcDigi::selectDigiPositionMethod(PndEmcDigi::depth, fEmcDigiPositionDepthPWO, fEmcDigiPositionDepthShashlyk, fEmcDigiRescaleFactor);
	} else {
    LOG(warn) << " PndEmcFWEndcapDigi::Init: "
              << "Unknown digi position method!";
    return kERROR;
	}

	// psa
	if(!fDigiPar->GetPsaTypeLow().IsNull()) {

		if(fLowgainPSA) {
			std::cout << "-W in PndEmcFWEndcapDigi::Init: Lowgain PSA already set. Skipping default initialization" << std::endl;
		} else if(!fDigiPar->GetPsaTypeLow().CompareTo("PSAFPGAPileupAnalyser")) {
			// std::cout << "Sample?" << std::endl;
			PndEmcPSAFPGAPileupAnalyser* psa = new PndEmcPSAFPGAPileupAnalyser();
			psa->SetVerbose(fVerbose);
			TObjArray* rparas = fDigiPar->GetRValueParLow().Tokenize(";");
			TF1 R_thres("R_thres_Low", rparas->GetEntriesFast()>0 ? rparas->UncheckedAt(0)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));				
			TF1 R_mean("R_mean_Low", rparas->GetEntriesFast()>1 ? rparas->UncheckedAt(1)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));				

			delete rparas;
			std::cout << "-W psa->Init Lowgain" << std::endl;
      psa->Init(std::vector<Double_t>(fDigiPar->GetPsaParLow().GetArray(), fDigiPar->GetPsaParLow().GetArray() + fDigiPar->GetPsaParLow().GetSize()),
                R_thres.IsZombie() ? nullptr : &R_thres, R_mean.IsZombie() ? nullptr : &R_mean);

      fLowgainPSA = psa;

		} else {
			std::cerr << "-E in PndEmcFWEndcapDigi::Init could not find PSA of type: " << fDigiPar->GetPsaTypeLow() << std::endl;
		}
	}

	if(!fDigiPar->GetPsaTypeHigh().IsNull()) {

		if(fHighgainPSA) {
			std::cout << "-W in PndEmcFWEndcapDigi::Init: Highgain PSA already set. Skipping default initialization" << std::endl;
		} else if(!fDigiPar->GetPsaTypeHigh().CompareTo("PSAFPGAPileupAnalyser")) {
			PndEmcPSAFPGAPileupAnalyser* psa = new PndEmcPSAFPGAPileupAnalyser();
			psa->SetVerbose(fVerbose);

			TObjArray* rparas = fDigiPar->GetRValueParHigh().Tokenize(";");
			TF1 R_thres("R_thres_High", rparas->GetEntriesFast()>0 ? rparas->UncheckedAt(0)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));				
			TF1 R_mean("R_mean_High", rparas->GetEntriesFast()>1 ? rparas->UncheckedAt(1)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));				

			delete rparas;
			std::cout << "-W psa->Init Highgain" << std::endl;
      psa->Init(std::vector<Double_t>(fDigiPar->GetPsaParHigh().GetArray(), fDigiPar->GetPsaParHigh().GetArray() + fDigiPar->GetPsaParHigh().GetSize()),
                R_thres.IsZombie() ? nullptr : &R_thres, R_mean.IsZombie() ? nullptr : &R_mean);

      fHighgainPSA = psa;
		} else {
			std::cerr << "-E in PndEmcFWEndcapDigi::Init could not find PSA of type: " << fDigiPar->GetPsaTypeHigh() << std::endl;
		}
	}


	if(fHighgainPSA && fLowgainPSA) {
		fHighLowPSA.Init(fHighgainPSA, fLowgainPSA, fDigiPar->GetSignalOverflowHigh(), 0, 1);
    LOG(info) << " HighLowPSA: Intialization successful";
  } else {
		std::cerr << "-E- PndEmcFWEndcapDigi::Init " << "No highgain and/or lowgain psa" << std::endl;
		return kFATAL;
	}

	//calibration
  if (fCalibrator != nullptr) {
    std::cout << "-W in PndEmcFWEndcapDigi::Init: Calibrator already set. Skipping default initialization"  << std::endl;
  } else {
    fCalibrator = new PndEmcSimCrystalCalibrator();
		for(Int_t imod=1; imod<=5; ++imod) { //TODO: different calibration constants for different modules
			fCalibrator->SetCalibration(imod, fDigiPar->GetCalibHigh(), 0, fDigiPar->GetSignalOverflowHigh()); 	//idx:0 <-> highgain
			fCalibrator->SetCalibration(imod, fDigiPar->GetCalibLow(), 1, 0); 					//idx:1 <-> lowgain
		}
		fCalibrator->Init();
  }

  fEnergyDigiThreshold=fDigiPar->GetEnergyDigiThreshold();

	fGeoPar->InitEmcMapper();
	PndEmcStructure::Instance();

  LOG(info) << " PndEmcFWEndcapDigi: Intialization successfull";
  return kSUCCESS;
}

/**
 * @brief Runs the task.
 * 
 * The task loops over the waveforms and uses the pulse shape analyser (PndEmcAbsPSA) to
 * extract signal height and timing. The calibrator (PndEmcAbsCrystalCalibrator) is then
 * used to calculate the energy. If the energy is above the threshold (@ref fEnergyDigiThreshold),
 * a PndEmcDigi is created.
 * 
 * @param opt unused
 * @return void
 */
void PndEmcFWEndcapDigi::Exec(Option_t*)
{
	TStopwatch timer;
	if (fVerbose>2){
		timer.Start();
	}
	//cout << ">> event: " << evtCounter << endl;
	if (evtCounter-1==nFwDigiProg*fNrOfEvents && nFwDigiProg<100) {
		nFwDigiProg += 1;
		cout << "\r                                                                                                         " << std::flush;	
		cout << "\r[INFO\t] PndEmcFWEndcapDigi: " << nFwDigiProg << "\% completed." << std::flush;
	}
	evtCounter++;

	fDigiArray->Delete();
	fDigiArray2->Delete();
	Double_t MinDiff=0;
	Int_t MinDiffIdx=0;
	std::vector<Double_t> diffVector;
	FairRootManager* ioman = FairRootManager::Instance();

	Double_t energy;
	Double_t digi_time;
	Int_t hitIndex;
	Int_t pileIndex;
	Int_t nHits;
	Int_t detId;
	Int_t trackId;
	// Variable declaration
  PndEmcHit *theHit = nullptr;

  // Loop over PndEmcHits to add them to correspondent waveforms
	/*Int_t Hits = fHitArray->GetEntriesFast();
		cout<< "PndEmc:: Hit array contains " << Hits << " hits" <<endl;
//cout << endl;
	for (Int_t iHit=0; iHit<Hits; iHit++) {

		theHit = dynamic_cast<PndEmcHit*>(fHitArray->At(iHit));

		if(theHit->GetModule() > 5 ) continue;	//tackles invalid PndEmcHit information (valid module number = seq 1 5)
		if(theHit->GetEnergy() < fEnergyDigiThreshold) continue; // skip very low energy hits
             }*/
	Int_t nWaveforms = fWaveformArray->GetEntriesFast();
	Int_t nRawhits = fHitArray->GetEntriesFast();
	PndEmcWaveform* theWaveform;
	if (fVerbose>1){
		cout << "PndEmcFWEndcapDigi, #input Emc hits: " << nRawhits << " in Event: " << FairRootManager::Instance()->GetEntryNr() << endl;
		cout << "PndEmcFWEndcapDigi, #input waveforms: " << nWaveforms << " in Event: " << FairRootManager::Instance()->GetEntryNr() << endl<<endl;
	}
	for (Int_t iWaveform=0; iWaveform<nWaveforms; iWaveform++) {

		theWaveform = (PndEmcWaveform*) fWaveformArray->At(iWaveform);
		hitIndex=theWaveform->GetHitIndex();
		if (fVerbose>5) cout<<endl<<"[EndCapDigi..............Hit index........"<<hitIndex<<endl;
		detId=theWaveform->GetDetectorId();
		trackId=theWaveform->GetTrackId();

		//Double_t timeshift; // how maximum is shifted (not used?)

		nHits = fHighLowPSA.Process(theWaveform);

		if (fVerbose>5) cout<<"PndEmcFWEndcapDigi............nHits........"<<nHits<<"   "<<endl;
		for(Int_t iHit=0; iHit<nHits; ++iHit) {
			if (fVerbose>5) cout<<"EndCapDigi............GetHit........"<<endl;
			//fHighLowPSA.GetHit(iHit, energy, digi_time);
			fHighLowPSA.GetHit2(iHit, energy, digi_time, pileIndex);
			//PndEmcAbsCrystalCalibrator::CalibrationStatus_t CalS //[R.K. 03/2017] unused variable?tat;
			if (fVerbose>5){
				cout << "#" << iHit << ".\tDetId: " << detId << "\tsample time: " << digi_time << "\traw energy: " << energy <<"  pileIndex:  "<<pileIndex<< "\t"<<endl;
				//CalStat =  //[R.K. 03/2017] unused variable?
				printf("Begin time  %f    digi time %f    Raw energy %f fEnergyDigiThreshold %f  \n",theWaveform->GetTimeStamp(),digi_time,energy,fEnergyDigiThreshold);}
			fCalibrator->Calibrate(energy, detId, fHighLowPSA.GetWaveformIdx(iHit));

			Double_t sampleRate = theWaveform->GetSampleRate(); // 8e7 Hz

			digi_time/=sampleRate;//Hz
			digi_time*=1e9;		// back to ns

			if (energy>fEnergyDigiThreshold) {
				if(pileIndex==2){
					//if(detId==101130005){
					if (fVerbose>7){
						theHit = dynamic_cast<PndEmcHit*>(fHitArray->At(hitIndex));
						Energy.open("Energy_hit", std::ofstream::app);
						Energy<<theHit->GetEnergy()<<std::endl;
						Energy.close();
					}
					Double_t timestamp=theWaveform->GetTimeStamp() + digi_time;
					//Double_t timestamp=theWaveform->GetTimeStamp();
					//cout.precision(20);
					if (fVerbose>5) cout << "WF time:"<< theWaveform->GetTimeStamp() << " digitime:"<<digi_time<<" timestamp: "<<timestamp<<", energy:"<<energy<<endl;
					//std::cout << "PndEmcFWEndcapDigi::Exec Waveform TS: " << theWaveform->GetTimeStamp() << " digiTime: " << digi_time << std::endl;

					PndEmcDigi* myDigi = new((*fDigiArray2)[fDigiArray2->GetEntriesFast()]) PndEmcDigi(trackId, detId, energy, timestamp, digi_time, hitIndex, pileIndex);
					//PndEmcDigi* myDigi = new PndEmcDigi(trackId, detId, energy, timestamp, hitIndex);
					//cout << "// --- Added digi with energy: " << myDigi->GetEnergy() << " t: " << myDigi->GetTimeStamp() << endl;
					myDigi->ResetLinks();
					myDigi->AddLinks(theWaveform->GetLinksWithType(ioman->GetBranchId("EmcHit")));
					//Double_t flighttime = myDigi->where().Mag()/29.9792458; // flight time in ns = distance to IP / c
					//myDigi->SetTimeStamp(timestamp-flighttime);

					// select hit with energy closest to extracted digi energy to use in new timestamp
					/*FairMultiLinkedData digiLinks = myDigi->GetLinksWithType(ioman->GetBranchId("EmcHit"));
				diffVector.clear();
				//cout << "links:" << digiLinks.GetNLinks() << std::flush;
				for (int iLink=0; iLink<digiLinks.GetNLinks(); iLink++){
					PndEmcHit* theHit = (PndEmcHit*)ioman->GetCloneOfLinkData(digiLinks.GetLink(iLink));
					if(theHit) {
						diffVector.push_back( TMath::Abs( theHit->GetEnergy()-energy ) );	
						//diffVector.push_back( TMath::Abs( ioman->GetEventTime() + theHit->GetTime()*1.0e9-timestamp ) );	
//cout << "Hittime:" << ioman->GetEventTime() + theHit->GetTime()*1e9	<< ", timestamp:" << timestamp << ", energy:" << theHit->GetEnergy() << endl;
						//PndEmcDigi* theDigi = new((*fDigiArray)[fDigiArray->GetEntriesFast()]) PndEmcDigi(trackId, detId, theHit->GetEnergy(), ioman->GetEventTime() + theHit->GetTime()*1.0e9, hitIndex);
					} else {
						cout << "-E in PndEmcViewClusters::Exec FairLink: " << digiLinks.GetLink(iLink) << " to EmcHit delivers null" << std::endl;
					}
				}
				if (diffVector.size()>0) {
					MinDiff = diffVector[0];
					MinDiffIdx = 0;
					for (int e=0; e<diffVector.size(); e++) { // get smallest difference
						if (diffVector[e] < MinDiff) {
							MinDiff = diffVector[e];	
							MinDiffIdx = e;
						}
					}
//if(diffVector.size()==1)cout << MinDiff << " (" << diffVector.size() << ")" << endl;
					PndEmcHit* bestHit = (PndEmcHit*)ioman->GetCloneOfLinkData(digiLinks.GetLink(MinDiffIdx));
					PndEmcDigi* theDigi = new((*fDigiArray)[fDigiArray->GetEntriesFast()]) PndEmcDigi(trackId, detId, bestHit->GetEnergy(), ioman->GetEventTime() + bestHit->GetTime()*1.0e9, hitIndex);
//cout << ioman->GetEventTime() + bestHit->GetTime()*1.0e9 << "-" << timestamp << "=\t" << (ioman->GetEventTime() + bestHit->GetTime()*1.0e9-timestamp) << endl;
					//myDigi->SetTimeStamp( rand.Gaus( ioman->GetEventTime() + bestHit->GetTime()*1.0e9, GetEnergyDepTimeResolution(energy) ) ); // assign timestamp to digi using time of hit, smeared with energy-dependent Gaussian (only if the digi was not a pure noise hit, i.e. it came from an EmcHit. In that case, leave the timestamp as defined by the waveform)
				}*/
					// finished selecting hit
				} //else{
				Double_t timestamp=theWaveform->GetTimeStamp() + digi_time;                       
				//Double_t timestamp=theWaveform->GetTimeStamp();
				//cout.precision(20);
				if (fVerbose>5) cout << "WF time:"<< theWaveform->GetTimeStamp() << " digitime:"<<digi_time<<" timestamp: "<<timestamp<<", energy:"<<energy<<endl;
				//std::cout << "PndEmcFWEndcapDigi::Exec Waveform TS: " << theWaveform->GetTimeStamp() << " digiTime: " << digi_time << std::endl;

				PndEmcDigi* myDigi = new((*fDigiArray)[fDigiArray->GetEntriesFast()]) PndEmcDigi(trackId, detId, energy, timestamp, digi_time, hitIndex, pileIndex);
				//PndEmcDigi* myDigi = new PndEmcDigi(trackId, detId, energy, timestamp, hitIndex);
				//cout << "// --- Added digi with energy: " << myDigi->GetEnergy() << " t: " << myDigi->GetTimeStamp() << endl;
				myDigi->ResetLinks();
				myDigi->AddLinks(theWaveform->GetLinksWithType(ioman->GetBranchId("EmcHit")));
				//}
			}

		}

	}
	if (fVerbose>1)cout << "PndEmcFWEndcapDigi, #output digis: " << fDigiArray->GetEntriesFast() << endl<<endl;
	if (fVerbose>3) {
		timer.Stop();
		Double_t rtime = timer.RealTime();
		Double_t ctime = timer.CpuTime();
		cout << "PndEmcFWEndcapDigi, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}
}

Double_t PndEmcFWEndcapDigi::GetEnergyDepTimeResolution(Double_t E)
{	
	// calculate energy-dependent time resolution. Formula taken from http://dx.doi.org/10.1016/j.nima.2011.06.044
	return 0.55+5.5*TMath::Exp(-(TMath::Log(2)/0.0250)*E);
}

void PndEmcFWEndcapDigi::SetParContainers() 
{
	// Get run and runtime database
	FairRun* run = FairRun::Instance();
	if ( ! run ) LOG(fatal) << "SetParContainers: No analysis run";

	FairRuntimeDb* db = run->GetRuntimeDb();
	if ( ! db ) LOG(fatal) << "SetParContainers: No runtime database";

	// Get Emc digitisation parameter container
	fDigiPar = dynamic_cast<PndEmcFWEndcapDigiPar*>(db->getContainer("PndEmcFWEndcapDigiPar"));

	// Get Emc reconstruction parameter container
	fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");

	// Get Emc geometry parameter container
	fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");
}

void PndEmcFWEndcapDigi::SetStorageOfData(Bool_t val)
{
	SetPersistency(val);
	return;
}

ClassImp(PndEmcFWEndcapDigi);

