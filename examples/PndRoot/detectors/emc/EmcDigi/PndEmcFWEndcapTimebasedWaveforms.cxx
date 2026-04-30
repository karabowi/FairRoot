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
// Author List:
// 	Philippp Mahlberg
//----------------------------------------------------------------------
//
//
#include <PndPersistencyTask.h>
#include "PndEmcFWEndcapTimebasedWaveforms.h"

#include "FairRun.h"

#include "PndEmcWaveform.h"
#include "PndEmcWaveformData.h"
#include "PndEmcWaveformBuffer.h"
#include "PndEmcAbsWaveformModifier.h"
#include "PndEmcFullStackedWaveformSimulator.h"
#include "PndEmcMultiWaveformSimulator.h"
#include "PndEmcShapingNoiseAdder.h"
#include "PndEmcWaveformDigitizer.h"
#include "PndEmcHit.h"
#include "PndEmcFittedPulseshape.h"
#include "PndEmcFittedPulseshape_nw.h"
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcFWEndcapDigiPar.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDataTypes.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLink.h"
#include "FairLogger.h"

#include "TStopwatch.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TClonesArray.h"

#include <iostream>
#include <cassert>

using std::cerr;
using std::cout;
using std::endl;
using std::fstream;

PndEmcFWEndcapTimebasedWaveforms::PndEmcFWEndcapTimebasedWaveforms(Int_t verbose, Bool_t storewaves)
  : PndPersistencyTask("PndEmcFWEndcapTimebasedWaveforms", verbose), fHitArray(nullptr), fWaveformBuffer(nullptr), fStoreWaves(storewaves), fnewShape(kFALSE),
    fStoreDataClass(kFALSE), fActivateBuffering(kFALSE), fDigiPar(nullptr), fGeoPar(nullptr), fUse_photon_statistic(kFALSE), fNPhotoElectronsPerMeV(0), fExcessNoiseFactor(0.),
    fExternalSimulator(nullptr), fAPD_LOWHIGH(nullptr), fEnergyDigiThreshold(0)
{
  SetPersistency(storewaves);
}

//--------------
// Destructor --
//--------------
PndEmcFWEndcapTimebasedWaveforms::~PndEmcFWEndcapTimebasedWaveforms() {}

/**
 * @brief Init Task
 *
 * Prepares the TClonesArray of PndEmcHit for reading and PndEmcMultiWaveform for writing.
 * Also reads the EMC parameters and prepares the waveform simulator (PndEmcMultiWaveformSimulator).
 *
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcFWEndcapTimebasedWaveforms::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcFWEndcapTimebasedWaveforms::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = (TClonesArray *)ioman->GetObject("EmcHit");
  if (!fHitArray) {
    cout << "-W- PndEmcFWEndcapTimebasedWaveforms::Init: "
         << "No EmcHit array!" << endl;
    return kERROR;
  }

  // Create and activiate output Buffer....choose between PndEmcWaveform and PndEmcMultiWaveform
#ifndef MULTI
  fWaveformBuffer = new PndEmcWaveformBuffer("EmcWaveform", "PndEmcWaveform", "Emc", GetPersistency());
#else
  fWaveformBuffer = new PndEmcWaveformBuffer("EmcWaveform", "PndEmcMultiWaveform", "Emc", GetPersistency());
#endif

  fWaveformBuffer = (PndEmcWaveformBuffer *)ioman->RegisterWriteoutBuffer("EmcTimebasedWaveform", fWaveformBuffer);
  fWaveformBuffer->ActivateBuffering(fActivateBuffering);

  if (fStoreDataClass) {
    fWaveformBuffer->StoreWaveformData("EmcWaveformData", "Emc", GetPersistency());
  }

  if (!fDigiPar) {
    cout << "-E- PndEmcFWEndcapTimebasedWaveforms::Init: "
         << "no DigiPar containter found" << endl;
    return kERROR;
  }

  fUse_photon_statistic = fDigiPar->GetUse_photon_statistic();

  if (fUse_photon_statistic) {
    // rear surface of FWEndcap crystal: 676 mm^2
    fNPhotoElectronsPerMeV = fDigiPar->GetDetectedPhotonsPerMeV() * fDigiPar->GetSensitiveAreaAPD() / 676. * fDigiPar->GetQuantumEfficiencyAPD();
    fExcessNoiseFactor = fDigiPar->GetExcessNoiseFactorAPD();
  } else {
    fNPhotoElectronsPerMeV = 0;
    fExcessNoiseFactor = 1;
  }

  fGeoPar->InitEmcMapper();
  PndEmcStructure::Instance();

  const Double_t tBefore = fDigiPar->GetTimeBeforeHit();
  const Double_t tAfter = fDigiPar->GetTimeAfterHit();
  const Double_t cutoff = fDigiPar->GetWfCutOffEnergy(); // GeV 			//0.001
  const Double_t activeTimeIncrement = 2;                // ns			//50
  const Double_t sampleRate = fDigiPar->GetSampleRate(); // ns^-1 <-> GHz	//0.08
  const Int_t bits = fDigiPar->GetNBits();               // 1				//14

  const Double_t tau = fDigiPar->GetPulseshapeTau();                // ns			//68.7
  const Double_t N = fDigiPar->GetPulseshapeN();                    // N				//1.667
  const Double_t energyRange_HIGH = fDigiPar->GetEnergyRangeHigh(); // GeV		//1
  const Double_t energyRange_LOW = fDigiPar->GetEnergyRangeLow();   // GeV		//15
  const Double_t noiseWidth_HIGH = fDigiPar->GetNoiseWidthHigh();   // GeV		//0.0023
  const Double_t noiseWidth_LOW = fDigiPar->GetNoiseWidthLow();     // GeV		//0.0035
  // const Double_t noiseWidth_HIGH = fDigiPar->GetNoiseWidthLow();	//GeV		//0.0023
  // const Double_t noiseWidth_LOW = fDigiPar->GetNoiseWidthHigh();		//GeV		//0.0035
  fEnergyDigiThreshold = fDigiPar->GetEnergyDigiThreshold(); // GeV		//0.002

  if (fnewShape) {
    const Double_t tau2 = fDigiPar->GetPulseshapeTau2(); // ns			//68.7
    const Double_t N2 = fDigiPar->GetPulseshapeN2();
    const Double_t v1 = fDigiPar->GetPulseshapev1();   // = 0;
    const Double_t tcr = fDigiPar->GetPulseshapetcr(); // = 26.3995;//2.11196*12.5;
    const Double_t tcf = fDigiPar->GetPulseshapetcf(); // = 302.76625;//24.2213*12.5;
    const Double_t t_r = fDigiPar->GetPulseshapet_r(); // = 52.69625;//4.2157*12.5;
    const Double_t res = fDigiPar->GetPulseshapeRes(); // = 0;
    fPulseshape = new PndEmcFittedPulseshape_nw(v1, tcr, tcf, t_r, res, tau2, N2);
  } else {
    fPulseshape = new PndEmcFittedPulseshape(tau, N);
  }
  fAPD_LOWHIGH = new PndEmcMultiWaveformSimulator(sampleRate, fPulseshape, 2);

  fAPD_LOWHIGH->Init(tBefore, tAfter, cutoff, activeTimeIncrement);
  fAPD_LOWHIGH->AddModifier(new PndEmcShapingNoiseAdder(fAPD_LOWHIGH->GetPulseRiseTime(), sampleRate, noiseWidth_HIGH, fAPD_LOWHIGH->GetTotalScale(0)), 0);
  fAPD_LOWHIGH->AddModifier(new PndEmcShapingNoiseAdder(fAPD_LOWHIGH->GetPulseRiseTime(), sampleRate, noiseWidth_LOW, fAPD_LOWHIGH->GetTotalScale(1)), 1);
  fAPD_LOWHIGH->AddModifier(new PndEmcWaveformDigitizer(bits, energyRange_HIGH, fAPD_LOWHIGH->GetTotalScale(0)), 0);
  fAPD_LOWHIGH->AddModifier(new PndEmcWaveformDigitizer(bits, energyRange_LOW, fAPD_LOWHIGH->GetTotalScale(1)), 1);

  LOG(info) << " PndEmcFWEndcapTimebasedWaveforms:: sample time before pulse = " << tBefore << " ns.";
  LOG(info) << " PndEmcFWEndcapTimebasedWaveforms:: sample time after pulse =  " << tAfter << " ns.";
  LOG(info) << " PndEmcFWEndcapTimebasedWaveforms:: bitResolution = " << bits << " bit.";
  LOG(info) << " PndEmcFWEndcapTimebasedWaveforms:: SampleRate = " << sampleRate << " GHz.";
  LOG(info) << " PndEmcFWEndcapTimebasedWaveforms:: fAPD_LOWHIGH->GetTotalScale(0) = " << fAPD_LOWHIGH->GetTotalScale(0) << " GHz.";
  LOG(info) << " PndEmcFWEndcapTimebasedWaveforms:: fAPD_LOWHIGH->GetTotalScale(1) = " << fAPD_LOWHIGH->GetTotalScale(1) << " GHz.";
  LOG(info) << " PndEmcFWEndcapTimebasedWaveforms:: fAPD_LOWHIGH->GetPulseRiseTime() = " << fAPD_LOWHIGH->GetPulseRiseTime() << " GHz.";

  return kSUCCESS;
}

/**
 * @brief Runs the task.
 *
 * Uses the waveform simulator (PndEmcAbsWaveformSimulator) to generate a waveform
 * from the hit and fill the buffer (PndEmcWaveformBuffer).
 *
 * @param opt unused
 * @return void
 */
void PndEmcFWEndcapTimebasedWaveforms::Exec(Option_t *)
{
  FairRootManager *ioman = FairRootManager::Instance();

  TStopwatch timer;
  if (fVerbose > 0) {
    timer.Start();
  }

  // Variable declaration
  PndEmcHit *theHit = nullptr;

  // Loop over PndEmcHits to add them to correspondent waveforms
  Int_t nHits = fHitArray->GetEntriesFast();
  if (fVerbose > 2) {
    cout << "PndEmcFWEndcapTimebasedWaveforms:: Hit array contains " << nHits << " hits" << endl;
  }
  // cout << endl;
  SumEnergyofhits = 0;
  for (Int_t iHit = 0; iHit < nHits; iHit++) {

    theHit = dynamic_cast<PndEmcHit *>(fHitArray->At(iHit));

    if (theHit->GetModule() > 5)
      continue; // tackles invalid PndEmcHit information (valid module number = seq 1 5)
    if (theHit->GetEnergy() < fEnergyDigiThreshold)
      continue; // skip very low energy hits

    // select wf Simulator..
    // TODO Add realistic description for other emc modules and make simulator choice module dependent
    // switch(theHit->GetModule() {
    // 	case 1: wfSimulator = ...
    // 	case 2: wfSimulator = ...
    // 	case 3: wfSimulator = ...
    // 	case 4: wfSimulator = ...
    // 	case 5: wfSimulator = ...
    // 	default: continue;
    // }
    PndEmcAbsWaveformSimulator *wfSimulator = nullptr;

    if (fExternalSimulator) {
      wfSimulator = fExternalSimulator;
    } else {
      wfSimulator = fAPD_LOWHIGH;
    };

    Double_t energy = theHit->GetEnergy();

    if (fUse_photon_statistic) {
      Double_t crystalPhotonsMeV = 1.0e3 * energy * fNPhotoElectronsPerMeV;
      Double_t photonStatFactor = gRandom->Gaus(1, sqrt(fExcessNoiseFactor / crystalPhotonsMeV));
      energy *= photonStatFactor;
    }
    // cout << "hitEnergy:" << theHit->GetEnergy() << endl;
    // construct corresponding waveform data Object
    PndEmcWaveformData wfData(theHit->GetDetectorID(), wfSimulator);

    // register hit...timebased framework uses ns, whereas emc deals with seconds as time unit
    FairLink linkToHit(-1, ioman->GetEntryNr(), "EmcHit", iHit, 1.0);
    //		std::cout.precision(20);
    //		std::cout << "-I- PndEmcFWEndcapTimebasedWaveforms::Exec eventTime: " << ioman->GetEventTime() << " hit Time: " << theHit->GetTime()*1.0e9 << std::endl;
    wfData.AddHit(linkToHit, ioman->GetEventTime() + theHit->GetTime() * 1.0e9, theHit->GetEnergy());

    if (fVerbose >= 5)
      cout << "// --- Input hit #" << iHit << " with energy: " << theHit->GetEnergy() << /* " x: " << theHit->where().x() << " y: "
<< theHit->where().y() <<*/
        " t: " << ioman->GetEventTime() + theHit->GetTime() << endl;
    SumEnergyofhits += theHit->GetEnergy();
    fWaveformBuffer->FillNewData(&wfData);

    if (fVerbose > 3) {
      timer.Stop();
      Double_t rtime = timer.RealTime();
      Double_t ctime = timer.CpuTime();
      cout << "PndEmcFWEndcapTimebasedWaveforms, Real time " << rtime << " s, CPU time " << ctime << " s"
           << "Number of hit: " << iHit << endl;
    }
  }
  if (fVerbose > 3)
    cout << " Sum of hit energy " << SumEnergyofhits << endl;
}

void PndEmcFWEndcapTimebasedWaveforms::SetParContainers()
{
  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get Emc geometry parameter container
  fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");

  // Get Emc digitisation parameter container
  fDigiPar = dynamic_cast<PndEmcFWEndcapDigiPar *>(db->getContainer("PndEmcFWEndcapDigiPar"));
}

ClassImp(PndEmcFWEndcapTimebasedWaveforms)
