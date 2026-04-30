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
//      Class PndEmcTmpWaveformToDigi. Module to take the ADC waveforms and produces digi.
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

#include "PndEmcTmpWaveformToDigi.h"

#include "PndEmcWaveform.h"
#include "PndEmcDigi.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TStopwatch.h"

#include "IfdProxyDict.h"
#include "IfdStdVectorProxy.h"
#include "Ifd.h"

#include "AbsEnv.h"

#include <iostream>
#include <assert.h>
//#include <map>

using std::cout;
using std::endl;
using std::fstream;

PndEmcTmpWaveformToDigi::PndEmcTmpWaveformToDigi(Int_t verbose, Bool_t storedigis)
{
  fVerbose = verbose;
  fDigiPosMethod = "depth"; // "surface" or "depth"
  fEmcDigiRescaleFactor = 1.08;
  SetPersistency(storedigis);
  // fPndEmcDigiPositionDepth=6.2;
}

//--------------
// Destructor --
//--------------

PndEmcTmpWaveformToDigi::~PndEmcTmpWaveformToDigi() {}

InitStatus PndEmcTmpWaveformToDigi::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcTmpWaveformToDigi::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  // 	fWaveformArray = (TClonesArray*) ioman->GetObject("EmcWaveform");
  // 	if ( ! fWaveformArray ) {
  // 		cout << "-W- PndEmcTmpWaveformToDigi::Init: "
  // 		<< "No PndEmcWaveform array!" << endl;
  // 		return kERROR;
  // 	}

  // Create and register output array
  fDigiArray = new TClonesArray("PndEmcDigi");

  ioman->Register("EmcDigi", "Emc", fDigiArray, GetPersistency());
  fSampleRate = fDigiPar->GetSampleRate();
  fEnergyDigiThreshold = fDigiPar->GetEnergyDigiThreshold();
  fEmcDigiPositionDepth = fRecoPar->GetEmcDigiPositionDepth();

  cout << "fEmcDigiPositionDepth: " << fEmcDigiPositionDepth << endl;

  if (!fDigiPosMethod.compare("surface")) {
    PndEmcDigi::selectDigiPositionMethod(PndEmcDigi::surface, 1., 0.);
  } else if (!fDigiPosMethod.compare("depth")) {
    PndEmcDigi::selectDigiPositionMethod(PndEmcDigi::depth, fEmcDigiRescaleFactor, fEmcDigiPositionDepth);
  } else {
    cout << "-W- PndEmcTmpWaveformToDigi::Init: "
         << "Unknown digi position method!" << endl;
    return kERROR;
  }

  LOG(info) << " PndEmcTmpWaveformToDigi: Intialization successfull";

  return kSUCCESS;
}

void PndEmcTmpWaveformToDigi::Exec(Option_t *)
{
  TStopwatch timer;
  if (fVerbose > 0) {
    timer.Start();
  }
  // Reset output array
  if (!fDigiArray)
    Fatal("Exec", "No Digi Array");
  fDigiArray->Delete();
  Double_t peakPosition;
  Double_t max_energy;
  Double_t digi_time;
  Int_t i_digi = 0; // index of digi in TClonesArray
  Int_t hitIndex;
  // 	Int_t nWaveforms = fWaveformArray->GetEntriesFast();
  // cout<<"PndEmcTmpWaveformToDigi: "<<nWaveforms<<" waveforms to convert"<<endl;

  // 	for (Int_t iWaveform=0; iWaveform<nWaveforms; iWaveform++) {
  // 		PndEmcWaveform* theWaveform = (PndEmcWaveform*) fWaveformArray->At(iWaveform);
  // 		hitIndex=theWaveform->GetHitIndex();
  // 		Int_t detId=theWaveform->GetDetectorId();
  // 		Int_t trackId=theWaveform->GetTrackId();
  // 		theWaveform->fitPeak(max_energy,peakPosition);
  // 		max_energy/=theWaveform->get_scale();
  // 		digi_time=peakPosition/fSampleRate;
  // 		if (max_energy>fEnergyDigiThreshold)
  // 		{
  // 			new((*fDigiArray)[i_digi]) PndEmcDigi(trackId,detId, max_energy, peakPosition,hitIndex);
  // 			i_digi++;

  // 		}
  // 	}

  vector<PndEmcWaveform *> *waveformList = Ifd<std::vector<PndEmcWaveform *>>::get(gblEvtDict);

  Int_t nWaveforms = waveformList->size();
  if (0 == waveformList) {
    cout << "Failed to recover std::vector from the dictionary" << endl;
    assert(0);
  }

  vector<PndEmcWaveform *>::const_iterator it = waveformList->begin();
  for (it; it != waveformList->end(); it++) {
    hitIndex = (*it)->GetHitIndex();
    Int_t detId = (*it)->GetDetectorId();
    Int_t trackId = (*it)->GetTrackId();
    (*it)->fitPeak(max_energy, peakPosition);
    max_energy /= (*it)->get_scale();
    digi_time = peakPosition / fSampleRate;
    if (max_energy > fEnergyDigiThreshold) {
      new ((*fDigiArray)[i_digi]) PndEmcDigi(trackId, detId, max_energy, peakPosition, hitIndex);
      i_digi++;
    }
  }

  if (fVerbose > 0) {
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << "PndEmcTmpWaveformToDigi, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  }
}

void PndEmcTmpWaveformToDigi::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar *)db->getContainer("PndEmcDigiPar");

  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar *)db->getContainer("PndEmcRecoPar");
}

void PndEmcTmpWaveformToDigi::SetStorageOfData(Bool_t val)
{
  SetPersistency(val);
  return;
}

ClassImp(PndEmcTmpWaveformToDigi)
