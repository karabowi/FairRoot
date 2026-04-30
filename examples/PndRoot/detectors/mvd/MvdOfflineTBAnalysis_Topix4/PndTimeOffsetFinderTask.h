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

/** PndTimeOffsetFinderTask.h
 **
 **/

#ifndef PndTimeOffsetFinderTask_H
#define PndTimeOffsetFinderTask_H

#include "FairTask.h" // for FairTask, InitStatus
#include "FairTSBufferFunctional.h"

#include "TH1.h"

#include "Rtypes.h"  // for Bool_t, Int_t, kTRUE, etc
#include "TString.h" // for TString

class FairTimeStamp;
class TClonesArray;

class PndTimeOffsetFinderTask : public FairTask {
 public:
  /** Default constructor **/
  PndTimeOffsetFinderTask()
    : FairTask("TimeOffsetFinderTask"), fPersistance(kTRUE), fInputBranch("MVDHitsPixel"), fInputArray(nullptr), fOutputBranch(), fFolder(), fOutputArray(nullptr), fEntryNr(0),
      fFunctor(new StopTime()), fLastTimeStamp(0), fTimeOffset(100000000), fTerminateTime(0), fFirstExecute(kTRUE), fStartIndex(0)
  {
    SetVerbose(0);
  }

  /** Named constructor **/
  PndTimeOffsetFinderTask(const char *name)
    : FairTask(name), fPersistance(kTRUE), fInputBranch("MVDHitsPixel"), fInputArray(nullptr), fOutputBranch(), fFolder(), fOutputArray(nullptr), fEntryNr(0),
      fFunctor(new StopTime()), fLastTimeStamp(0), fTimeOffset(100000000), fTerminateTime(0), fFirstExecute(kTRUE), fStartIndex(0)
  {
    SetVerbose(0);
  }

  PndTimeOffsetFinderTask(TString inputBranch, TString outputBranch, TString folderName)
    : FairTask("Sorter"), fPersistance(kTRUE), fInputBranch(inputBranch), fInputArray(nullptr), fOutputBranch(outputBranch), fFolder(folderName), fOutputArray(nullptr),
      fEntryNr(0), fFunctor(new StopTime()), fLastTimeStamp(0), fTimeOffset(100000000), fTerminateTime(0), fFirstExecute(kTRUE), fStartIndex(0)
  {
    SetVerbose(0);
  }

  /** Destructor **/
  virtual ~PndTimeOffsetFinderTask() {}

  /** Virtual method Init **/
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void FinishTask();

  virtual void SetParContainers(){};
  void SetStartTime(Double_t val) { fLastTimeStamp = val; }
  void SetTerminateTime(Double_t val) { fTerminateTime = val; }
  void SetInputBranch(TString val) { fInputBranch = val; }

  void SetStartIndex(Int_t val) { fStartIndex = val; }

  void SetPersistance(Bool_t p = kTRUE) { fPersistance = p; };
  Bool_t GetPersistance() { return fPersistance; };

 protected:
  /** switch to turn on/off storing the arrays to a file*/
  Bool_t fPersistance;
  /** Input array of PndSdsPixelDigis **/
  TString fInputBranch;
  TClonesArray *fInputArray;
  /** Output array of sorted PndSdsDigis **/
  TString fOutputBranch;
  TString fFolder;
  TClonesArray *fOutputArray;
  Int_t fEntryNr;
  BinaryFunctor *fFunctor;
  Double_t fLastTimeStamp;
  Double_t fTimeOffset;

  Double_t fTerminateTime;
  Bool_t fFirstExecute;
  Int_t fStartIndex;

  TH1D *fHc0c1;
  TH1D *fHc0c2;
  TH1D *fHc0c3;
  TH1D *fHc1c2;
  TH1D *fHc1c3;
  TH1D *fHc2c3;

  ClassDef(PndTimeOffsetFinderTask, 2);
};

#endif
