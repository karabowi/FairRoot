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

/******************************************************
Class PndHypMicroWriter

Collects Micro infromation from Reconstruction and
writes out PndPidCandidates

Author: K.Goetzen, GSI, 06/2008

*******************************************************/

#ifndef PNDMICROIDEALWRITER_H
#define PNDMICROIDEALWRITER_H 1

#include "FairTask.h"
#include "TH1F.h"
#include "TFile.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;

class PndHypMicroIdealWriter : public FairTask {

 public:
  typedef std::map<Int_t, Float_t> mapper;

  /** Default constructor **/
  PndHypMicroIdealWriter();

  /** Destructor **/
  ~PndHypMicroIdealWriter();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void Finish();
  // void CreateStructure();

 protected:
  void propagate(TLorentzVector &l, TVector3 &p, float charge);

  TLorentzVector track1;
  TLorentzVector track2;

  /**book all the histograms**/

 private:
  int evtcnt; // event counter for output

  /** Input array of TpcLheTrack **/
  TClonesArray *fTrArray;

  /** Input array of EmcCandidates **/
  TClonesArray *fHitArray;

  /** Input array of MCTracks **/
  TClonesArray *fMCTrack;

  // Output arrays Candidates
  TClonesArray *fChargedCandidates;
  TClonesArray *fNeutralCandidates;
  TClonesArray *fMcCandidates;
  TClonesArray *fMicroIdealCandidates;

  // output array EventInfo
  TClonesArray *fEventInfo;

  bool fStoreNeutral;
  bool fStoreCharged;
  bool fStoreMC;

  Int_t mvd_hitidx[1000];
  Int_t stt_hitidx[1000];
  Int_t tpc_hitidx[1000];

  /** Geo file to use **/

  /** Get parameter containers **/
  virtual void SetParContainers();

  ClassDef(PndHypMicroIdealWriter, 1);
};

#endif
