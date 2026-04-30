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

/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

/** PndXYResidualTask.h
 **
 **/

#ifndef PndXYResidualTask_H
#define PndXYResidualTask_H

#include "FairTask.h" // for FairTask, InitStatus
#include "FairTSBufferFunctional.h"

#include "PndTrack.h"

#include "TH2.h"

#include "Rtypes.h"  // for Bool_t, Int_t, kTRUE, etc
#include "TString.h" // for TString

class FairTimeStamp;
class TClonesArray;

class PndXYResidualTask : public FairTask {
 public:
  /** Default constructor **/
  PndXYResidualTask()
    : FairTask("MvdXYResidualTask"), fPersistance(kTRUE), fTrackArray(nullptr), fHitArray(nullptr), fMissingHitArray(nullptr), fProjectedHitArray(nullptr), fEntryNr(0),
      fBranchName("MVDHitsPixelCorrectedSorted_event")
  {
    SetVerbose(0);
  }

  /** Named constructor **/
  PndXYResidualTask(const char *name)
    : FairTask(name), fPersistance(kTRUE), fTrackArray(nullptr), fHitArray(nullptr), fMissingHitArray(nullptr), fProjectedHitArray(nullptr), fEntryNr(0),
      fBranchName("MVDHitsPixelCorrectedSorted_event")
  {
    SetVerbose(0);
  }

  /** Destructor **/
  virtual ~PndXYResidualTask() {}

  /** Virtual method Init **/
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void FinishTask();

  virtual void SetParContainers(){};

  void SetPersistance(Bool_t p = kTRUE) { fPersistance = p; };
  Bool_t GetPersistance() { return fPersistance; };

  TVector3 PropagateToZ(PndTrack *aTrack, Double_t z);

  void SetBranchName(TString val) { fBranchName = val; }

 protected:
  /** switch to turn on/off storing the arrays to a file*/
  Bool_t fPersistance;
  /** Input array of PndSdsPixelDigis **/
  TClonesArray *fTrackArray;
  TClonesArray *fHitArray;

  TClonesArray *fMissingHitArray;
  TClonesArray *fProjectedHitArray;
  TClonesArray *fGoodHitArray;

  TString fBranchName;

  Int_t fEntryNr;

  TH2D *fHc0c0;
  TH2D *fHc0c1;
  TH2D *fHc0c2;
  TH2D *fHc0c3;
  TH2D *fHc0c0cut;
  TH2D *fHc0c1cut;
  TH2D *fHc0c2cut;
  TH2D *fHc0c3cut;

  TH1D *fHMissingHits;

  ClassDef(PndXYResidualTask, 2);
};

#endif
