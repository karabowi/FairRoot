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

/** PndTSCorrectorTask.h
 **
 **/

#ifndef PndTSCorrectorTask_H
#define PndTSCorrectorTask_H

#include "FairTask.h" // for FairTask, InitStatus

#include "PndMapSorter.h"

#include "Rtypes.h"  // for Bool_t, Int_t, kTRUE, etc
#include "TString.h" // for TString

class FairTimeStamp;
class TClonesArray;

class PndTSCorrectorTask : public FairTask {
 public:
  /** Default constructor **/
  PndTSCorrectorTask()
    : FairTask("SorterTask"), fPersistance(kTRUE), fInputBranch("MVDHitsPixel"), fInputArray(nullptr), fOutputBranch("MVDHitsPixelCorrected"), fFolder(), fOutputArray(nullptr),
      fEntryNr(0)
  {
    SetVerbose(2);
  }

  /** Named constructor **/
  PndTSCorrectorTask(const char *name) : FairTask(name), fPersistance(kTRUE), fInputBranch(), fInputArray(nullptr), fOutputBranch(), fFolder(), fOutputArray(nullptr), fEntryNr(0)
  {
    SetVerbose(2);
  }

  PndTSCorrectorTask(TString inputBranch, TString outputBranch, TString folderName)
    : FairTask("Corrector"), fPersistance(kTRUE), fInputBranch(inputBranch), fInputArray(nullptr), fOutputBranch(outputBranch), fFolder(folderName), fOutputArray(nullptr),
      fEntryNr(0)
  {
    SetVerbose(2);
  }

  /** Destructor **/
  virtual ~PndTSCorrectorTask() {}

  /** Virtual method Init **/
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void FinishTask();

  void SetCorrectionValue(Int_t sensorId, Int_t value) { fTSCorrectionMap[sensorId] = value; }

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
  std::map<Int_t, Int_t> fTSCorrectionMap; //< first value is sensor ID, second parameter is correction value which is added to the time stamps
  PndTSCorrectorTask(const PndTSCorrectorTask &);
  PndTSCorrectorTask &operator=(const PndTSCorrectorTask &);

  ClassDef(PndTSCorrectorTask, 2);
};

#endif
