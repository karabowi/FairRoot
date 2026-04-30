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
// -----                 PndSciTDigiTask  header file          -----
// -----                  Created by D. Steinschaden                   -----
// -----                   last update    06.2015 		       -----
// -------------------------------------------------------------------------

/** PndSciTDigiTask.h
 **@author d.steinschaden <dominik.steinschaden@oeaw.ac.at>
 **
 ** The hit producer produces hits of type PndSciTDigiTask.h in timebased and event based simulation.
 ** If Timebased is activated also the Sorter Task is started automatically
 **/

#ifndef PNDSCITDIGITASK_H
#define PNDSCITDIGITASK_H

#include <PndPersistencyTask.h>
#include "PndSciTPoint.h"
#include "PndGeoHandling.h"
#include "PndGeoSciTPar.h"
#include "PndSciTHitWriteoutBuffer.h"

#include "FairMCEventHeader.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"

#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TRandom.h"
#include <string>

class TClonesArray;

class PndSciTDigiTask : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndSciTDigiTask();
  PndSciTDigiTask(Double_t dt, Double_t deadtime);

  /** Destructor **/
  ~PndSciTDigiTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  void RunTimeBased();

  void SetDeadTime(Double_t deadtime) { fDeadtime = deadtime; };         // in ns  default val = 1000
  void SetTimeResolution(Double_t dt) { fdt = dt; };                     // in ns  default val = 0.075
  void SetPileupTime(Double_t pileuptime) { fPileupTime = pileuptime; }; // in ns  default val = 0.1

  void SetBuffering(Bool_t B) { fActivateBuffering = B; };

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  void smear(Double_t &time, Double_t &dt);

 protected:
  TString fInBranchName;
  TString fOutBranchName;
  TString fSortedOutBranchName;

  /** Input array of PndSciTPoints **/
  TClonesArray *fPointArray;

  /** Output array of PndSciTHits **/
  TClonesArray *fHitArray;

  PndSciTHitWriteoutBuffer *fDataBuffer;
  PndGeoSciTPar *fGeoPar;
  PndGeoHandling *fGeoH; // For converting sensor ID (shortID) into the Full volume path

  Double_t fdt, fDeadtime, fPileupTime;

  Bool_t fTimeOrderedDigi;   ///< set to kTRUE to use the time ordering of the output data.
  Bool_t fActivateBuffering; // set to kFALSE to deaktivate Buffering and PileUP

  void Register();
  void Reset();
  void ProduceHits();

  ClassDef(PndSciTDigiTask, 1);
};

#endif
