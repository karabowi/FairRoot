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

// -------------------------------------------------------------------------
// -----                PndMdtDigitization source file                  -----
// -----                  author: Jifeng Hu, hu@to.infn.it
// -------------------------------------------------------------------------

#include "PndMdtDigitization.h"
#include "PndMdtWaveform.h"
#include "PndMdtDigi.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TRandom.h"
#include <iostream>
#include "PndMdtIGeometry.h"
#include "FairEventHeader.h"

using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndMdtDigitization::PndMdtDigitization(Int_t verbose, Bool_t store) : PndPersistencyTask(" MDT Digitization"), fTimeOrderedDigi(kFALSE)
{
  SetVerbose(verbose);
  SetPersistency(store);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtDigitization::~PndMdtDigitization() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndMdtDigitization::Init()
{

  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndMdtDigitization::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fWaveformArray = (TClonesArray *)ioman->GetObject("MdtWaveform");
  if (!fWaveformArray) {
    cout << "-W- PndMdtDigitization::Init: "
         << "No MdtWaveform array!" << endl;
    return kERROR;
  }

  if (fTimeOrderedDigi) {
    fDigiArray = ioman->Register("MdtDigi", "PndMdtDigi", "Mdt", GetPersistency());
  } else {
    // Create and register output array
    fDigiBoxArray = ioman->Register("MdtDigiBox", "PndMdtDigi", "Mdt", GetPersistency());
    fDigiStripArray = ioman->Register("MdtDigiStrip", "PndMdtDigi", "Mdt", GetPersistency());
  }

  fWireNoiseSigma = 0.05;
  fStripNoiseSigma = 0.01;
  fSamplingInterval = 10.; // nano seconds

  fGeoIF = PndMdtIGeometry::Instance();
  fGeoIF->SetVerbose(3);
  fGeoIF->AddSensor("MDT");
  fGeoIF->AddSensor("GasCell");
  Bool_t fGoodGeo = fGeoIF->Init();

  HowManyWireWf = HowManyStripWf = 0;
  HowManyWireDigi = HowManyStripDigi = 0;
  LOG(info) << " PndMdtDigitization: Intialization " << (fGoodGeo ? "successful." : "failed.");

  return fGoodGeo ? kSUCCESS : kERROR;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMdtDigitization::Exec(Option_t *)
{
  if (fTimeOrderedDigi)
    exec_t();
  else
    exec_e();
}
void PndMdtDigitization::exec_e()
{
  // Reset output array
  fDigiBoxArray->Delete();
  fDigiStripArray->Delete();
  Int_t evtNo = FairRunAna::Instance()->GetEventHeader()->GetMCEntryNumber() - 1;

  Int_t nWaveform = fWaveformArray->GetEntriesFast();
  PndMdtWaveform *theWf(nullptr);
  Double_t fTimeStamp;
  Double_t fAmplitude;

  for (Int_t iW = 0; iW < nWaveform; iW++) {
    theWf = (PndMdtWaveform *)fWaveformArray->At(iW);
    if (theWf->IsWire()) {
      ++HowManyWireWf;
      if (Digitize(theWf, fTimeStamp, fAmplitude, kTRUE)) {
        TVector3 fPos;
        fGeoIF->GetTubeCenter(theWf->GetDetectorID(), fPos);
        PndMdtDigi *aDigi = AddDigiBox(theWf->GetDetectorID(), fPos, evtNo);
        aDigi->SetTimeStamp(theWf->GetTimeStamp() + fTimeStamp);
        ++HowManyWireDigi;
      }
    } else {
      ++HowManyStripWf;
      if (Digitize(theWf, fTimeStamp, fAmplitude, kFALSE)) {
        TVector3 fPos;
        fGeoIF->GetStripCenter(theWf->GetDetectorID(), fPos);
        PndMdtDigi *aDigi = AddDigiStrip(theWf->GetDetectorID(), fPos, evtNo);
        aDigi->SetTimeStamp(theWf->GetTimeStamp() + fTimeStamp);
        ++HowManyStripDigi;
      }
    }
  }
}
void PndMdtDigitization::exec_t()
{
  // Reset output array
  fDigiArray->Delete();
  Int_t evtNo = FairRunAna::Instance()->GetEventHeader()->GetMCEntryNumber() - 1;

  Int_t nWaveform = fWaveformArray->GetEntriesFast();
  PndMdtWaveform *theWf(nullptr);
  Double_t fTimeStamp;
  Double_t fAmplitude;

  // cout<<"event no "<<evtNo<<", load in "<<nWaveform<<endl;
  for (Int_t iW = 0; iW < nWaveform; iW++) {
    theWf = (PndMdtWaveform *)fWaveformArray->At(iW);
    // theWf->print();
    if (theWf->IsWire()) {
      ++HowManyWireWf;
      if (Digitize(theWf, fTimeStamp, fAmplitude, kTRUE)) {
        TVector3 fPos;
        fGeoIF->GetTubeCenter(theWf->GetDetectorID(), fPos);
        PndMdtDigi *aDigi = AddDigi(theWf->GetDetectorID(), fPos, evtNo);
        aDigi->SetTimeStamp(theWf->GetTimeStamp() + fTimeStamp);
        ++HowManyWireDigi;
      }
    } else {
      ++HowManyStripWf;
      if (Digitize(theWf, fTimeStamp, fAmplitude, kFALSE)) {
        TVector3 fPos;
        fGeoIF->GetStripCenter(theWf->GetDetectorID(), fPos);
        PndMdtDigi *aDigi = AddDigi(theWf->GetDetectorID(), fPos, evtNo);
        aDigi->SetTimeStamp(theWf->GetTimeStamp() + fTimeStamp);
        ++HowManyStripDigi;
      }
    }
  }
}
// -------------------------------------------------------------------------

// -----   Private method AddDigi   --------------------------------------------
PndMdtDigi *PndMdtDigitization::AddDigiBox(Int_t detID, TVector3 &pos, Int_t evtNo)
{
  // It fills the PndMdtDigi category
  TClonesArray &clref = *fDigiBoxArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndMdtDigi(detID, pos, evtNo);
}
// ----

// -----   Private method AddDigi   --------------------------------------------
PndMdtDigi *PndMdtDigitization::AddDigiStrip(Int_t detID, TVector3 &pos, Int_t evtNo)
{
  // It fills the PndMdtDigi category

  TClonesArray &clref = *fDigiStripArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndMdtDigi(detID, pos, evtNo);
}
PndMdtDigi *PndMdtDigitization::AddDigi(Int_t detID, TVector3 &pos, Int_t evtNo)
{
  // It fills the PndMdtDigi category

  TClonesArray &clref = *fDigiArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndMdtDigi(detID, pos, evtNo);
}

// --- threshold digitization ---
Bool_t PndMdtDigitization::Digitize(PndMdtWaveform *theWf, Double_t &time, Double_t &amp, Bool_t isWire)
{
  time = -1;
  amp = -9999;
  Double_t fPeak = 0.;
  Bool_t NotFound = kTRUE;
  // Double_t fNoiseSigma; //[R.K. 01/2017] unused variable?
  Double_t fThreshold(1e9);
  if (isWire)
    fThreshold = 5. * fWireNoiseSigma;
  else
    fThreshold = 5. * fStripNoiseSigma;

  const std::vector<Double_t> &fSignalData = theWf->GetSignal();
  for (size_t is = 0; is < fSignalData.size(); ++is) {
    if (TMath::Abs(fSignalData[is]) > TMath::Abs(fPeak))
      fPeak = fSignalData[is];
    if (TMath::Abs(fSignalData[is]) > fThreshold && NotFound) {
      time = is * fSamplingInterval; // nano seconds
      NotFound = kFALSE;
    }
  }
  amp = fPeak;
  return time > 0;
}

// ----
void PndMdtDigitization::FinishTask()
{
  if (fTimeOrderedDigi) {
    Int_t nWaveform = fWaveformArray->GetEntriesFast();
    if (fVerbose > 0) {
      std::cout << "PndMdtDigitization::FinishTask, fWaveformArray size #" << nWaveform << std::endl;
    }
    Exec("");
  }

  std::cout << "===================================================" << std::endl;
  std::cout << "PndMdtDigitization::FinishTask" << std::endl;
  std::cout << "***************************************************" << std::endl;
  std::cout << "Read waveforms#" << (HowManyWireWf + HowManyStripWf) << endl;
  std::cout << "Produce digis#" << (HowManyWireDigi + HowManyStripDigi) << endl;
  std::cout << "\twire digis# " << HowManyWireDigi << " from " << HowManyWireWf << " waveforms." << std::endl;
  std::cout << "\tstrip digis# " << HowManyStripDigi << " from " << HowManyStripWf << " waveforms." << std::endl;
  std::cout << "***************************************************" << std::endl;
}

ClassImp(PndMdtDigitization)
