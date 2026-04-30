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
//      Class PndEmcMultiWaveformToCalibratedDigi. Module to take the ADC waveforms and produces digi.
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

#include "PndEmcMultiWaveformToCalibratedDigi.h"

#include "PndEmcMultiWaveform.h"
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

PndEmcMultiWaveformToCalibratedDigi::PndEmcMultiWaveformToCalibratedDigi(Int_t verbose, Bool_t storedigis)
{
  fVerbose = verbose;
  fDigiPosMethod = "depth"; // "surface" or "depth"
  fEmcDigiRescaleFactor = 1.08;
  SetPersistency(storedigis);
  fCalibrationFileName = "";
  // fPndEmcDigiPositionDepth=6.2;
}

//--------------
// Destructor --
//--------------

PndEmcMultiWaveformToCalibratedDigi::~PndEmcMultiWaveformToCalibratedDigi() {}

InitStatus PndEmcMultiWaveformToCalibratedDigi::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcMultiWaveformToCalibratedDigi::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fWaveformArray = (TClonesArray *)ioman->GetObject("EmcMultiWaveform");
  if (!fWaveformArray) {
    cout << "-W- PndEmcMultiWaveformToCalibratedDigi::Init: "
         << "No PndEmcMultiWaveform array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fDigiArray = new TClonesArray("PndEmcDigi");

  ioman->Register("EmcDigi", "Emc", fDigiArray, GetPersistency());
  fSampleRate = fDigiPar->GetSampleRate();
  fSampleRate_PMT = fDigiPar->GetSampleRate_PMT();
  fASIC_Shaping_int_time = fDigiPar->GetASIC_Shaping_int_time();   // s
  fPMT_Shaping_int_time = fDigiPar->GetPMT_Shaping_int_time();     // s
  fPMT_Shaping_diff_time = fDigiPar->GetPMT_Shaping_diff_time();   // s
  fCrystal_time_constant = fDigiPar->GetCrystal_time_constant();   // s
  fShashlyk_time_constant = fDigiPar->GetShashlyk_time_constant(); // s
  fNumber_of_samples_in_waveform = fDigiPar->GetNumber_of_samples_in_waveform();
  fNumber_of_samples_in_waveform_pmt = fDigiPar->GetNumber_of_samples_in_waveform_pmt();
  fEnergyDigiThreshold = fDigiPar->GetEnergyDigiThreshold();
  fEmcDigiPositionDepthPWO = fRecoPar->GetEmcDigiPositionDepthPWO();
  fEmcDigiPositionDepthShashlyk = fRecoPar->GetEmcDigiPositionDepthShashlyk();

  cout << "fEmcDigiPositionDepthPWO: " << fEmcDigiPositionDepthPWO << endl;
  cout << "fEmcDigiPositionDepthShashlyk: " << fEmcDigiPositionDepthShashlyk << endl;

  if (!fDigiPosMethod.CompareTo("surface")) {
    PndEmcDigi::selectDigiPositionMethod(PndEmcDigi::surface, 0., 0., 1.0);
  } else if (!fDigiPosMethod.CompareTo("depth")) {
    PndEmcDigi::selectDigiPositionMethod(PndEmcDigi::depth, fEmcDigiPositionDepthPWO, fEmcDigiPositionDepthShashlyk, fEmcDigiRescaleFactor);
  } else {
    cout << "-W- PndEmcMultiWaveformToCalibratedDigi::Init: "
         << "Unknown digi position method!" << endl;
    return kERROR;
  }

  fPulseshape = new PndEmcAsicPulseshape(fASIC_Shaping_int_time, fCrystal_time_constant);
  fPulseshape_pmt = new PndEmcCRRCPulseshape(fPMT_Shaping_int_time, fPMT_Shaping_diff_time, fShashlyk_time_constant);

  if (fpsaAlgorithm == nullptr) {
    // Matched digital filter
    // Parameters of the filter are hardcoded at the moment
    // For different pulseshape in barrel and endcaps different filters should be implemented
    std::vector<Double_t> params;
    params.push_back(30);          // width
    params.push_back(fSampleRate); // Sample rate
    fpsaAlgorithm = new PndEmcPSAMatchedDigiFilter(params, fPulseshape);
  }

  if (fpsaAlgorithm_pmt == nullptr) {
    // Pulse shape analysis algorithm.
    // Simple parabolic fit.
    fpsaAlgorithm_pmt = new PndEmcPSAParabolic();
  }

  // Determine normalisation constant for PndEmcMultiWaveform
  PndEmcWaveform *tmpwaveform = new PndEmcWaveform(0, 101010001, fNumber_of_samples_in_waveform);
  PndEmcWaveform *tmpwaveform2 = new PndEmcWaveform(0, 101010001, fNumber_of_samples_in_waveform_pmt);

  PndEmcHit *gevHit = new PndEmcHit();
  gevHit->SetEnergy(1.0);
  gevHit->SetTime(0.);
  tmpwaveform->UpdateWaveform(gevHit, 0, false, 1., 0., fSampleRate, fPulseshape);
  tmpwaveform2->UpdateWaveform(gevHit, 0, false, 1., 0., fSampleRate_PMT, fPulseshape_pmt);
  Double_t tmpPeakPosition;
  Double_t tmpPeakPosition2;
  fpsaAlgorithm->Process(tmpwaveform, fWfNormalisation, tmpPeakPosition);
  fpsaAlgorithm_pmt->Process(tmpwaveform2, fWfNormalisation_pmt, tmpPeakPosition2);

  fWfNormalisation_proto192 = 1;

  if (fCalibrationFileName != "")
    ReadCalibrationFile();

  LOG(info) << " PndEmcMultiWaveformToCalibratedDigi: Read " << fCalibrationMap.size() << " Calibration Entries";

  LOG(info) << " PndEmcMultiWaveformToCalibratedDigi: Intialization successfull";

  return kSUCCESS;
}

void PndEmcMultiWaveformToCalibratedDigi::Exec(Option_t *)
{
  TStopwatch timer;
  if (fVerbose > 2) {
    timer.Start();
  }
  // Reset output array
  if (!fDigiArray)
    Fatal("Exec", "No Digi Array");
  fDigiArray->Delete();
  Double_t peakPosition;
  Double_t energy;
  Double_t digi_time;
  Int_t i_digi = 0; // index of digi in TClonesArray
  Int_t hitIndex;
  Int_t detId;
  Int_t trackId;
  Int_t module;
  Int_t nWaveforms = fWaveformArray->GetEntriesFast();
  Int_t nSignal;
  Bool_t highgain;
  PndEmcAbsPSA *thePSA;
  Double_t theEnergyNorm;
  Double_t theSampleRate;
  Int_t nHits[4];
  Double_t hittimes[4][32];
  Double_t hitenergies[4][32];
  std::map<Int_t, Double_t>::iterator it;
  // cout<<"PndEmcMultiWaveformToCalibratedDigi: "<<nWaveforms<<" waveforms to convert"<<endl;
  for (Int_t iWaveform = 0; iWaveform < nWaveforms; iWaveform++) {
    PndEmcMultiWaveform *theWaveform = (PndEmcMultiWaveform *)fWaveformArray->At(iWaveform);
    hitIndex = theWaveform->GetHitIndex();
    detId = theWaveform->GetDetectorId();
    trackId = theWaveform->GetTrackId();
    module = theWaveform->GetModule();
    nSignal = theWaveform->GetNumberOfWaveforms();
    if (module == 5) {
      thePSA = fpsaAlgorithm_pmt;
      theEnergyNorm = fWfNormalisation_pmt;
      theSampleRate = fSampleRate_PMT;
    } else {
      thePSA = fpsaAlgorithm;
      theEnergyNorm = fWfNormalisation_proto192;
      ;
      theSampleRate = fSampleRate;
    }
    for (Int_t iSignal = 0; iSignal < nSignal; iSignal++) {
      theWaveform->SetActiveWaveform(iSignal);
      nHits[iSignal] = thePSA->Process(theWaveform);
      //            std::cout << "Signal " << iSignal << ": " << nHits[iSignal] << " Hits" << std::endl;
      for (Int_t iHit = 0; iHit < nHits[iSignal]; iHit++) {
        thePSA->GetHit(iHit, hitenergies[iSignal][iHit], hittimes[iSignal][iHit]);
        //                std::cout << "Signal " << iSignal << ", Hit " << iHit << ": Energy " << hitenergies[iSignal][iHit] <<std::endl;
      }
    }
    //        std::cout << "found "<<nHits[0] << " hits for detid " << detId << std::endl;
    for (Int_t iHit = 0; iHit < nHits[0]; iHit++) {
      highgain = kTRUE;
      peakPosition = hittimes[0][iHit];
      energy = hitenergies[0][iHit];
      if (nSignal > 1 && energy > 1500.) { // todo make highgain limit a config option
        // look for hit in lowgain
        //                std::cout << "Hit " << iHit << " is too high (" << energy << ") checking lowgain." << std::endl;
        //                std::cout << "highgain time: " << hittimes[0][iHit] << std::endl;
        for (Int_t iHit1 = 0; iHit1 < nHits[1]; iHit1++) {
          //                    std::cout << "lowgain hit time: " << hittimes[1][iHit] << std::endl;
          if (TMath::Abs(peakPosition - hittimes[1][iHit1]) < 10.) { // todo make timediff a config option
            //                        std::cout << "Found matching highgain hit " << iHit1 << std::endl;
            energy = hitenergies[1][iHit1];
            peakPosition = hittimes[0][iHit];
            highgain = kFALSE;
            break;
          }
        }
      }

      energy /= theEnergyNorm;
      digi_time = peakPosition / theSampleRate;
      //            std::cout << "looking for calibration of detid " << detId << std::endl;
      it = fCalibrationMap.find(detId);
      if (it != fCalibrationMap.end()) {
        //                            if(hitIndex == 45) std::cout << "found calibration value of " << it->second << " for hitIndex " << hitIndex << std::endl;
        energy *= it->second;
      } else {
        //                            if(hitIndex == 45) std::cout << "no calibration value for hitIndex " << hitIndex << std::endl;
      }
      if (!highgain) {
        //                            if(hitIndex == 45) std::cout << "highgain" << std::endl;
        it = fGainMap.find(detId);
        if (it != fGainMap.end()) {
          if (hitIndex == 45)
            std::cout << "found highgain value of " << it->second << " for hitIndex " << hitIndex << std::endl;
          energy *= it->second;
        }
      }
      //        if(energy>1000){
      //            if(hitIndex == 45) std::cout << "energy: " << energy << " threshold: "<< fEnergyDigiThreshold << endl;
      //            if(hitIndex == 45) std::cout << "creating digi for detid: " << detId << "hitIndex: " << hitIndex <<std::endl;
      //        }
      if (energy > fEnergyDigiThreshold) {
        //                            if(hitIndex == 45) std::cout << "energy: " << energy << endl;
        PndEmcDigi *myDigi = new ((*fDigiArray)[i_digi]) PndEmcDigi(trackId, detId, energy, digi_time, hitIndex);
        myDigi->AddLink(FairLink("EmcMultiWaveform", iWaveform));
        i_digi++;
      }
    }
  }
  if (fVerbose > 2) {
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << "PndEmcMultiWaveformToCalibratedDigi, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  }
}

void PndEmcMultiWaveformToCalibratedDigi::SetParContainers()
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

void PndEmcMultiWaveformToCalibratedDigi::SetStorageOfData(Bool_t val)
{
   SetPersistency(val);
  return;
}

void PndEmcMultiWaveformToCalibratedDigi::ReadCalibrationFile()
{
  std::ifstream in(fCalibrationFileName, std::ifstream::in);
  //    in.open(fCalibrationFileName);
  if (!in.good()) {
    std::cerr << "Cannot open calibration file!" << endl;
    return;
  }

  char buf[255];
  while (in.getline(buf, 255)) {
    TString tmp = buf;
    TObjArray *tokens = tmp.Tokenize(" \t;");
    if (tokens->GetEntries() < 2) {
      continue;
    }
    tmp = tokens->UncheckedAt(0)->GetName();
    TString tmp2 = tokens->UncheckedAt(1)->GetName();
    if (tmp.IsDigit() && tmp2.IsFloat()) {
      fCalibrationMap.insert(std::pair<Int_t, Double_t>(tmp.Atoi(), tmp2.Atof()));
    }
    if (tokens->GetEntries() < 3) {
      continue;
    }
    tmp2 = tokens->UncheckedAt(2)->GetName();
    if (tmp.IsDigit() && tmp2.IsFloat()) {
      fGainMap.insert(std::pair<Int_t, Double_t>(tmp.Atoi(), tmp2.Atof()));
    }
    delete tokens;
  }
  if (fVerbose > 1) {
    std::cout << "calibration values:" << std::endl;
    std::map<Int_t, Double_t>::iterator it;
    std::map<Int_t, Double_t>::iterator it2;
    Int_t detid;
    for (it = fCalibrationMap.begin(); it != fCalibrationMap.end(); it++) {
      detid = (*it).first;
      std::cout << detid << "\t" << (*it).second << "\t";
      it2 = fGainMap.find(detid);
      if (it2 != fGainMap.end()) {
        std::cout << (*it2).second;
      }
      std::cout << endl;
    }
  }
}

ClassImp(PndEmcMultiWaveformToCalibratedDigi)
