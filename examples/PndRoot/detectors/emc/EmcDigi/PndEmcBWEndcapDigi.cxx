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

#include "PndEmcBWEndcapDigi.h"

#include "PndEmcWaveformData.h"
#include "PndEmcMultiWaveform.h"
#include "PndEmcMultiPSA.h"
#include "PndEmcPSATmaxAnalyser.h"
#include "PndEmcPSAOverflowCombinator.h"
#include "PndEmcDigi.h"
#include "PndEmcBWEndcapDigiPar.h"
#include "PndEmcGeoPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcSimCrystalCalibrator.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TF1.h"
#include "TObject.h"
#include "TList.h"

#include <iostream>
#include <vector>
#include <utility>

using std::cout;
using std::endl;

PndEmcBWEndcapDigi::PndEmcBWEndcapDigi(Int_t verbose, Bool_t storedigis)
  : fWaveformArray(nullptr), fDigiArray(nullptr), fEnergyDigiThreshold(0), fDigiPosMethod(""), fEmcDigiRescaleFactor(0.), fEmcDigiPositionDepthPWO(0),
    fEmcDigiPositionDepthShashlyk(0), fHighLowPSA(nullptr), fCombinator(nullptr), fHighgainPSA(nullptr), fLowgainPSA(nullptr), fCalibrator(nullptr), fDigiPar(nullptr),
    fRecoPar(nullptr), fGeoPar(nullptr), fVerbose(verbose)
{
	SetPersistency(storedigis);
	fSingleAPDMode = kFALSE;
}

//--------------
// Destructor --
//--------------
PndEmcBWEndcapDigi::~PndEmcBWEndcapDigi()
{
  if (fDigiArray != nullptr)
    delete fDigiArray;

	if (!fCombinator) delete fCombinator;
	if (!fHighLowPSA) delete fHighLowPSA;
	if (!fHighgainPSA) delete fHighgainPSA;
	if (!fLowgainPSA) delete fLowgainPSA;
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
InitStatus PndEmcBWEndcapDigi::Init()
{
	/* Get RootManager */
	FairRootManager *ioman = FairRootManager::Instance();
	if (!ioman)
	{
		cout << "-E- PndEmcBWEndcapDigi::Init: "
			 << "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	/* Input and Output */
	// Get input array
	fWaveformArray = (TClonesArray *)ioman->GetObject("EmcMultiWaveform");
	if (!fWaveformArray)
	{
		//check if EmcWaveform contains MultiWaveforms
		fWaveformArray = (TClonesArray *)ioman->GetObject("EmcWaveform");
		if ((!fWaveformArray) || (!fWaveformArray->GetClass()->InheritsFrom("PndEmcMultiWaveform")))
		{
			cout << "-W- PndEmcBWEndcapDigi::Init: "
				 << "No PndEmcWaveform array containing multi waveforms!" << endl;
			return kERROR;
		}
	}

	if (!fDigiPar)
	{
		cout << "-E- PndEmcBWEndcapTimebasedWaveforms::Init: "
			 << "no DigiPar containter found" << endl;
		return kFATAL;
	}

	// Create and register output array
	fDigiArray = ioman->Register("EmcDigi", "PndEmcDigi", "Emc", GetPersistency());

	/* position methods */
	fEmcDigiPositionDepthPWO = fRecoPar->GetEmcDigiPositionDepthPWO(); // 6.2
	fEmcDigiPositionDepthShashlyk = fRecoPar->GetEmcDigiPositionDepthShashlyk(); // 20.9

	//digi pos method
	fDigiPosMethod = "depth"; //ATTN: hardcoded...do we need this at all?
	fEmcDigiRescaleFactor = 1.08;
	if (!fDigiPosMethod.CompareTo("surface"))
	{
		PndEmcDigi::selectDigiPositionMethod(PndEmcDigi::surface, 0., 0., 1.0);
	}
	else if (!fDigiPosMethod.CompareTo("depth"))
	{
		PndEmcDigi::selectDigiPositionMethod(PndEmcDigi::depth, fEmcDigiPositionDepthPWO, fEmcDigiPositionDepthShashlyk, fEmcDigiRescaleFactor);
	}
	else
	{
		cout << "-W- PndEmcBWEndcapDigi::Init: "
			 << "Unknown digi position method!" << endl;
		return kERROR;
	}

	/* psa */
	// backward
	Double_t sample_rate = fDigiPar->GetSampleRate();
	const Double_t* fir_coeff = fDigiPar->GetFIRCoeff().GetArray();
	Int_t taps = fDigiPar->GetTmaxTaps();
	Int_t gap = fDigiPar->GetTmaxGap();
	Double_t hit_thr_hi = fDigiPar->GetTmaxHitThresholdHigh();
	Double_t hit_thr_lo = fDigiPar->GetTmaxHitThresholdLow();
	Double_t tut_peak = fDigiPar->GetTmaxTutPeak();
	Double_t hit_val = fDigiPar->GetTmaxHitVal();
	Double_t sig_overflow = fDigiPar->GetSignalOverflowHigh();
	Double_t sig_timediff = fDigiPar->GetPulseshapeTau() / sample_rate / 3.;

	if (!fCombinator) {
		if (!fSingleAPDMode) fCombinator = new PndEmcPSAOverflowCombinator(sig_overflow, sig_timediff);
		else fCombinator = new PndEmcPSAOverflowCombinator(sig_overflow, sig_timediff, kTRUE); // single APD
	}
	if (!fHighLowPSA)
		fHighLowPSA = new PndEmcMultiPSA(fCombinator);

	if (!fHighgainPSA)
		fHighgainPSA = new PndEmcPSATmaxAnalyser(fir_coeff, taps, gap, sample_rate, hit_thr_hi, tut_peak, hit_val, fVerbose);
	if (!fLowgainPSA)
		fLowgainPSA = new PndEmcPSATmaxAnalyser(fir_coeff, taps, gap, sample_rate, hit_thr_lo, tut_peak, hit_val, fVerbose);

	if (fHighgainPSA && fLowgainPSA)
	{
		if (!fSingleAPDMode) {
			fHighLowPSA->AddPSA(fHighgainPSA);
			fHighLowPSA->AddPSA(fLowgainPSA);
			fHighLowPSA->AddPSA(fHighgainPSA);
			fHighLowPSA->AddPSA(fLowgainPSA);
		}
		else { // single APD
			fHighLowPSA->AddPSA(fHighgainPSA);
			fHighLowPSA->AddPSA(fLowgainPSA);
		}
	}
	else
	{
		std::cerr << "-E- PndEmcBWEndcapDigi::Init "
				  << "No highgain and/or lowgain psa" << std::endl;
		return kFATAL;
	}

	/* calibration */
  if (fCalibrator != nullptr) {
    std::cout << "-W in PndEmcBWEndcapDigi::Init: Calibrator already set. Skipping default initialization" << std::endl;
  } else {
    fCalibrator = new PndEmcSimCrystalCalibrator();
		for (Int_t imod = 1; imod <= 5; ++imod)
		{																									   //TODO: different calibration constants for different modules
			fCalibrator->SetCalibration(imod, 1E3*fDigiPar->GetHGLGRatio()/fDigiPar->GetEnCalib(), 0, pow(2, fDigiPar->GetNBits())); //idx:0 <-> highgain
			fCalibrator->SetCalibration(imod, 1E3/fDigiPar->GetEnCalib(), 1, 0);								   //idx:1 <-> lowgain
		}
		fCalibrator->Init();
  }

  fEnergyDigiThreshold = fDigiPar->GetEnergyDigiThreshold();

	fGeoPar->InitEmcMapper();
	PndEmcStructure::Instance();

  LOG(info) << " PndEmcBWEndcapDigi: Intialization successfull";
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
void PndEmcBWEndcapDigi::Exec(Option_t *)
{
	TStopwatch timer;
	if (fVerbose > 2)
	{
		timer.Start();
	}

	fDigiArray->Delete();

	Double_t energy;
	Double_t digi_time;
	Int_t hitIndex;
	Int_t nHits;
	Int_t detId;
	Int_t trackId;
	Int_t nWaveforms = fWaveformArray->GetEntriesFast();
	PndEmcWaveform *theWaveform;

	for (Int_t iWaveform = 0; iWaveform < nWaveforms; iWaveform++)
	{

		theWaveform = (PndEmcWaveform *)fWaveformArray->At(iWaveform);
		hitIndex = theWaveform->GetHitIndex();
		detId = theWaveform->GetDetectorId();
		trackId = theWaveform->GetTrackId();

		nHits = fHighLowPSA->Process(theWaveform);

		for (Int_t iHit = 0; iHit < nHits; ++iHit)
		{
			fHighLowPSA->GetHit(iHit, energy, digi_time);

			fCalibrator->Calibrate(energy, detId, fHighLowPSA->GetAPDGainIndex());

			if (energy > fEnergyDigiThreshold)
			{
				Double_t timestamp = digi_time;

				PndEmcDigi *myDigi = new ((*fDigiArray)[fDigiArray->GetEntriesFast()]) PndEmcDigi(trackId, detId, energy, timestamp, timestamp-FairRootManager::Instance()->GetEventTime(), hitIndex);
				myDigi->ResetLinks();
				myDigi->AddLinks(theWaveform->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcHit")));
			}
		}
	}

	if (fVerbose > 2)
	{
		timer.Stop();
		Double_t rtime = timer.RealTime();
		Double_t ctime = timer.CpuTime();
		cout << "PndEmcBWEndcapDigi, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}
}

void PndEmcBWEndcapDigi::SetParContainers()
{
	// Get run and runtime database
	FairRun *run = FairRun::Instance();
	if (!run)
		LOG(fatal) << "SetParContainers: No analysis run";

	FairRuntimeDb *db = run->GetRuntimeDb();
	if (!db)
		LOG(fatal) << "SetParContainers: No runtime database";

	// Get Emc digitisation parameter container
	fDigiPar = dynamic_cast<PndEmcBWEndcapDigiPar *>(db->getContainer("PndEmcBWEndcapDigiPar"));

	// Get Emc reconstruction parameter container
	fRecoPar = (PndEmcRecoPar *)db->getContainer("PndEmcRecoPar");

	// Get Emc geometry parameter container
	fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");
}

void PndEmcBWEndcapDigi::SetStorageOfData(Bool_t val)
{
	SetPersistency(val);
	return;
}

ClassImp(PndEmcBWEndcapDigi)
