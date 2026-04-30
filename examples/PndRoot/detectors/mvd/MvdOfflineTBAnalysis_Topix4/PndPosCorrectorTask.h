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

/** PndPosCorrectorTask.h
 **
 **/

#ifndef PndPosCorrectorTask_H
#define PndPosCorrectorTask_H

#include "FairTask.h" // for FairTask, InitStatus

#include "PndMapSorter.h"

#include "Rtypes.h"  // for Bool_t, Int_t, kTRUE, etc
#include "TString.h" // for TString

class FairTimeStamp;
class TClonesArray;

class PndPosCorrectorTask : public FairTask {
 public:
  /** Default constructor **/
  PndPosCorrectorTask()
    : FairTask("SorterTask"), fPersistance(kTRUE), fInputBranch("MVDHitsPixelCorrectedSorted_event"), fInputArray(nullptr), fOutputBranch("MVDHitsPixelPosCorrected"), fFolder(),
      fOutputArray(nullptr), fEntryNr(0)
  {
    SetVerbose(2);
  }

  /** Named constructor **/
  PndPosCorrectorTask(const char *name) : FairTask(name), fPersistance(kTRUE), fInputBranch(), fInputArray(nullptr), fOutputBranch(), fFolder(), fOutputArray(nullptr), fEntryNr(0)
  {
    SetVerbose(2);
  }

  PndPosCorrectorTask(TString inputBranch, TString outputBranch, TString folderName)
    : FairTask("Corrector"), fPersistance(kTRUE), fInputBranch(inputBranch), fInputArray(nullptr), fOutputBranch(outputBranch), fFolder(folderName), fOutputArray(nullptr),
      fEntryNr(0)
  {
    SetVerbose(2);
  }

  /** Destructor **/
  virtual ~PndPosCorrectorTask() {}

  /** Virtual method Init **/
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void FinishTask();

  void SetCorrectionValue(Int_t sensorId, Double_t xvalue, Double_t yvalue) { fPosCorrectionMap[sensorId] = std::make_pair(xvalue, yvalue); }

  virtual void SetParContainers(){};

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
  std::map<Int_t, std::pair<Double_t, Double_t>> fPosCorrectionMap; //< first value is sensor ID, second parameter is correction values for x and y
  PndPosCorrectorTask(const PndPosCorrectorTask &);
  PndPosCorrectorTask &operator=(const PndPosCorrectorTask &);

  ClassDef(PndPosCorrectorTask, 1);
};

#endif
