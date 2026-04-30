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

#include "PndEmcShashlykDigi.h"

#include "PndEmcWaveformData.h"
#include "PndEmcPSAOptimalFilterAnalyser.h"
#include "PndEmcDigi.h"
#include "PndEmcShashlykDigiPar.h"
#include "PndEmcGeoPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcSimCrystalCalibrator.h"
#include "PndEmcShashlykPulseshape.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

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

PndEmcShashlykDigi::PndEmcShashlykDigi(Int_t verbose, Bool_t storedigis) : 
fWaveformArray(NULL), fDigiArray(NULL), fEnergyDigiThreshold(0), fDigiPosMethod(""), 
fEmcDigiRescaleFactor(0.), fEmcDigiPositionDepthPWO(0), fEmcDigiPositionDepthShashlyk(0), 
fOFAnalyser(NULL), fCalibrator(NULL), 
fDigiPar(NULL), fRecoPar(NULL), fGeoPar(NULL), fVerbose(verbose)
{
	SetPersistency(storedigis);
	fSingleAPDMode = kFALSE;
}

//--------------
// Destructor --
//--------------
PndEmcShashlykDigi::~PndEmcShashlykDigi()
{
	if (fDigiArray != 0)
		delete fDigiArray;

	if (fOFAnalyser) delete fOFAnalyser;
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
InitStatus PndEmcShashlykDigi::Init()
{
	/* Get RootManager */
	FairRootManager *ioman = FairRootManager::Instance();
	if (!ioman)
	{
		cout << "-E- PndEmcShashlykDigi::Init: "
			 << "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	/* Input and Output */
	// Get input array
	fWaveformArray = (TClonesArray *)ioman->GetObject("EmcWaveform");
	if (!fWaveformArray)
	{
		//check if EmcWaveform contains MultiWaveforms
		//fWaveformArray = (TClonesArray *)ioman->GetObject("EmcWaveform");
		//if ((!fWaveformArray) || (!fWaveformArray->GetClass()->InheritsFrom("PndEmcMultiWaveform")))
		//{
			cout << "-W- PndEmcShashlykDigi::Init: "
				 << "No PndEmcWaveform array containing multi waveforms!" << endl;
			return kERROR;
		//}
	}

	if (!fDigiPar)
	{
		cout << "-E- PndEmcShashlykTimebasedWaveforms::Init: "
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
		cout << "-W- PndEmcShashlykDigi::Init: "
			 << "Unknown digi position method!" << endl;
		return kERROR;
	}

	/* psa */
	//Double_t sample_rate = fDigiPar->GetSampleRate();
    Double_t fSamplingFactor = fDigiPar->GetSamplingFactor();
	Double_t cf_frac = fDigiPar->GetCFFraction();
	Int_t cf_tap = fDigiPar->GetCFTap();
	Int_t cf_n = fDigiPar->GetCFNSamples();
	const Double_t* cf_tcorr = fDigiPar->GetCFTCorr().GetArray();
	Int_t of_m = fDigiPar->GetOFM();
	Int_t of_b0 = fDigiPar->GetOFB0();
	const Double_t* of_a = fDigiPar->GetOFA().GetArray();
	const Double_t* of_b = fDigiPar->GetOFB().GetArray();
	Double_t hit_threshold = fDigiPar->GetADCHitThreshold();
    Double_t mu = fDigiPar->GetPulseshapeMu()-TMath::Log(fDigiPar->GetSampleRate());
    Double_t sigma = fDigiPar->GetPulseshapeSigma();
    PndEmcShashlykPulseshape* shape = new PndEmcShashlykPulseshape(mu, sigma);
    fOFAnalyser = new PndEmcPSAOptimalFilterAnalyser(cf_frac, cf_tap, cf_tcorr, cf_n, of_a, of_b, of_m, of_b0, shape, hit_threshold, 0., fVerbose);

	/* calibration */
	if (fCalibrator != NULL)
	{
		std::cout << "-W in PndEmcShashlykDigi::Init: Calibrator already set. Skipping default initialization" << std::endl;
	}
	else
	{
		fCalibrator = new PndEmcSimCrystalCalibrator();
		for (Int_t imod = 1; imod <= 5; ++imod)
		{
            if (imod <= 4) 
            {
			    fCalibrator->SetCalibration(imod, 1E3/fDigiPar->GetEnCalib());
            }
            else
            {
			    fCalibrator->SetCalibration(imod, 1E3/fDigiPar->GetEnCalib()/fSamplingFactor);
            }
            
		}
		fCalibrator->Init();
	}

	fEnergyDigiThreshold = fDigiPar->GetEnergyDigiThreshold();

	fGeoPar->InitEmcMapper();
	PndEmcStructure::Instance();

	cout << "-I- PndEmcShashlykDigi: Intialization successfull" << endl;
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
void PndEmcShashlykDigi::Exec(Option_t *)
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

		nHits = fOFAnalyser->Process(theWaveform);

		for (Int_t iHit = 0; iHit < nHits; ++iHit)
		{
			fOFAnalyser->GetHit(iHit, energy, digi_time);
			fCalibrator->Calibrate(energy, detId);

			if (energy > fEnergyDigiThreshold)
			{
				Double_t timestamp = digi_time;

				PndEmcDigi* myDigi = new ((*fDigiArray)[fDigiArray->GetEntriesFast()]) PndEmcDigi(trackId, detId, energy, timestamp, timestamp-FairRootManager::Instance()->GetEventTime(), hitIndex);
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
		cout << "PndEmcShashlykDigi, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}
}

void PndEmcShashlykDigi::SetParContainers()
{
	// Get run and runtime database
	FairRun *run = FairRun::Instance();
	if (!run)
		Fatal("SetParContainers", "No analysis run");

	FairRuntimeDb *db = run->GetRuntimeDb();
	if (!db)
		Fatal("SetParContainers", "No runtime database");

	// Get Emc digitisation parameter container
	fDigiPar = dynamic_cast<PndEmcShashlykDigiPar *>(db->getContainer("PndEmcShashlykDigiPar"));

	// Get Emc reconstruction parameter container
	fRecoPar = (PndEmcRecoPar *)db->getContainer("PndEmcRecoPar");

	// Get Emc geometry parameter container
	fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");
}

void PndEmcShashlykDigi::SetStorageOfData(Bool_t val)
{
	SetPersistency(val);
	return;
}

ClassImp(PndEmcShashlykDigi)
