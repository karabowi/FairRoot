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

//---------------------------------------------------------------------- // File and Version Information:
//      $Id: Exp $
//
// Description:
//      Class PndEmcAnalysis. Module to take the hit list for the
//      calorimeter and make ADC waveforms from them.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother                  Original Author
// Dima Melnichuk - adaption for PANDA
//
// Copyright Information:
//      Copyright (C) 1996             Imperial College
//
//----------------------------------------------------------------------
//#pragma once
#ifndef PndEmcAnalysis_H
#define PndEmcAnalysis_H

#include "FairTask.h"
#include "PndEmcDigiWriteoutBuffer.h"
#include "FairTSBufferFunctional.h"
#include <string>
#include <map>
#include <vector>
#include <set>
#include "TTree.h"
#include "TFile.h"
#include "TVectorD.h"
#include "TLorentzVector.h"
#include "PndEmcDigiCalibrator.h"

class PndEmcHit;
class PndEmcTwoCoordIndex;
class PndEmcWaveform;

class TClonesArray;
class PndEmcDigiPar;
class PndEmcRecoPar;
class PndEmcFpgaPar;
class PndEmcAbsPSA;
class PndEmcAbsPulseshape;

using namespace std;

class PndEmcAnalysis : public FairTask {

 public:
  // Constructors

  PndEmcAnalysis(Int_t verbose = 0, Bool_t storeRootFile = kTRUE);

  // Destructor

  virtual ~PndEmcAnalysis();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetOutput(TString name) { fileName = name; }

  void RunTimeBased() { fTimeOrderedDigi = kTRUE; }

  virtual void FinishTask();

  Bool_t fSaveWave;
  Bool_t fSaveDigi;
  Bool_t fSaveBump;
  Bool_t fSaveTask;
  Bool_t fSaveHits;
  Bool_t fSaveMcTruth;
  Bool_t fSaveReco;
  Bool_t fSavePidCharged;
  Bool_t fSavePidNeutral;
  Bool_t fSaveJpsiTo3g;

 private:
  void SetParContainers();
  /** Input arrays **/
  TClonesArray *fEmcHitsArray;
  TClonesArray *fWaveformArray;
  TClonesArray *fDigiArray;
  TClonesArray *fSharedDigiArray;
  TClonesArray *fClusterArray;
  TClonesArray *fBumpArray;
  TClonesArray *fMcTrackArray;
  TClonesArray *fEvtHeaderArray;
  TClonesArray *fChargedCand;
  TClonesArray *fNeutralCand;

  Int_t fVerbose;
  Bool_t fStoreRooTFile;

  PndEmcAnalysis(const PndEmcAnalysis &L);
  PndEmcAnalysis &operator=(const PndEmcAnalysis &) { return *this; };

  ClassDef(PndEmcAnalysis, 1);

  BinaryFunctor *fFunctor;

  // for time-based analysis
  Bool_t fTimeOrderedDigi; ///< set to kTRUE to use the time ordering of the output data.
  // event no, how many waveforms in this event, how many pileup waveforms
  std::map<Int_t, std::pair<Int_t, Int_t>> evtMap;
  std::set<Int_t> pevtset; // pileup event numbers
  std::set<Int_t> evtset;  // total event numbers

  TString fileName;
  TFile *fRootFile;
  TTree *tHit;        // hit information
  TTree *tDigi;       // digi information
  TTree *tWave;       // wave information
  TTree *tBump;       // bump information
  TTree *tTask;       // task summary
  TTree *tEvtPileup;  // pileup at event level
  TTree *tTaskPileup; // pileup at task level
  TTree *tMcTruth;    // MC Truth
  TTree *tRec;        // reconstruction
  // for Tree tHit
  Double_t fHitTime;
  Double_t fHitEnergy;
  Double_t fTheta;
  Double_t fPhi;
  // variables for Tree tWave
  Double_t fEvtTime;
  Int_t fModule;
  Int_t fHitIndex;
  Double_t fWaveTime;
  Double_t fWavePeak;
  Int_t fWaveLen;
  Int_t fWaveIdx;
  Int_t fEventNo;
  Double_t fBaseline;
  Int_t fXcor;
  Int_t fYcor;
  Int_t fPileupCount;
  // for Tree tDigi;
  Double_t fDigiTime;
  // Double_t fEvtTime;
  // Int_t    fModule;
  // Double_t fWaveTime;
  // Double_t fHitEnergy;
  Double_t fDigiEnergy;
  Double_t fDistance;
  // Int_t    fWaveIdx;
  // Int_t    fEventNo;
  TVector3 fPosition;
  TVector3 fSeedPosition;
  // for Tree tBump;
  Double_t fEnergy1C;
  Double_t fEnergy2C;
  // Double_t fEvtTime;
  // Int_t    fEventNo        ,"EventNO/I");
  // Int_t    fModule         ,"Module/I");
  Double_t fBumpTime1;
  Double_t fBumpTime2;
  Double_t fBumpTime3;
  Double_t fSeedTime;
  Double_t fBumpEnergy;
  // TVector3 fPosition;
  // for Tree tTask
  Int_t fNumDigi;
  Int_t fNumBump;
  Int_t fClusterIdx;
  Int_t fNumOfSeed;
  Int_t fDigiMapSize;
  Int_t fNumOfMatch;
  Int_t fNumOfDismatch;
  // for tree task
  Double_t totDigiEnergy;
  Double_t totSharedDigiEnergy;
  // Int_t    fEventNo;
  Int_t fNumOfHits;
  Int_t fNumWaveform;
  // Int_t    fNumDigi;
  Int_t fNumSharedDigi;
  Int_t fNumCluster;
  // Int_t    fNumBump;
  Int_t fNumMcTrack;

  // for Tree tTaskPileup
  Int_t fTotWave;
  Int_t fPileupWave;
  Int_t totNumOfWave;
  Int_t totNumOfSubWave;
  Int_t totNumOfDigi;
  Int_t totNumOfBump;
  Int_t totNumOfSharedDigi;
  Int_t totNumOfEvents;
  Int_t totNumOfPileupEvents;

  // for Tree tRec
  const static Int_t NElement = 30;
  Double_t fp4[4 * NElement];
  Double_t fMcp4[4 * NElement];
  Double_t fPositionV[3 * NElement];
  Double_t fEnergy[NElement];
  Double_t fEnergyC1[NElement];
  Double_t fEnergyC2[NElement];
  Double_t fZ201[NElement];
  Double_t fZ531[NElement];
  Double_t fLat1[NElement];
  Int_t fModule1[NElement];
  Int_t fPDGCode[NElement];
  Double_t fTheta1[NElement];
  Double_t fPhi1[NElement];
  Double_t fRestEnergy1;
  Double_t fRestEnergy2;
  Double_t fRestEnergy3;
  Int_t fNumGT;
  // for Tree tMcTruth
  Int_t fDet1[NElement];
  Int_t fHitEmc1[NElement];
  Int_t fHitEmc2[NElement];
  Int_t fNPointV[NElement];

  // pid data

  Int_t fchar[NElement];
  //	Double_t fp4[NElement]  ;
  Double_t fpos[NElement];
  Double_t fmom[NElement];
  Int_t fmcidx[NElement];
  Double_t frawe[NElement];
  Double_t fcale[NElement];
  Int_t fndigi[NElement];
  Int_t fmod[NElement];
  Int_t fcluIdx[NElement];
  Double_t fe25[NElement];
  Double_t fe9[NElement];
  Int_t fnbump[NElement];

  TTree *tCharged; // charged track
  TTree *tNeutral; // photons

  TTree *tAna3g; // specified analysis
  // jpsi-->3 gamma
  Int_t fNumberOfGoodPhoton; //
  Double_t fExternalEnergy;  // other energy excluding 3 photon

  Double_t f4p1[4];   // photon 1
  Double_t fMc4p1[4]; // photon 1, MC truth
  Double_t fpos1[3];  // position
  Double_t fmom1;     // momentum amplitude
  Double_t fenergy1;
  Double_t femcraw1;
  Int_t fndigi1; // fired crystals by this photon
  Int_t fmod1;
  Int_t fMcTrack1;
  Double_t fe251;
  Double_t flat1;
  Double_t fz201;
  Double_t fz531;

  Double_t f4p2[4];   // photon 2
  Double_t fMc4p2[4]; // photon 2, MC truth
  Double_t fpos2[3];  // position
  Double_t fmom2;     // momentum amplitude
  Double_t fenergy2;
  Double_t femcraw2;
  Int_t fndigi2; // fired crystals by this photon
  Int_t fmod2;
  Int_t fMcTrack2;
  Double_t fe252;
  Double_t flat2;
  Double_t fz202;
  Double_t fz532;

  Double_t f4p3[4];   // photon 3
  Double_t fMc4p3[4]; // photon 3, MC truth
  Double_t fpos3[3];  // position
  Double_t fmom3;     // momentum amplitude
  Double_t fenergy3;
  Double_t femcraw3;
  Int_t fndigi3; // fired crystals by this photon
  Int_t fmod3;
  Int_t fMcTrack3;
  Double_t fe253;
  Double_t flat3;
  Double_t fz203;
  Double_t fz533;

  PndEmcDigiCalibrator digiCalibrator;
};

#endif
