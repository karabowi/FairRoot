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

//* $Id: */

// -------------------------------------------------------------------------
// -----                     PndGemFindHits header file                -----
// -----                  Created 15/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemFindHits
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15/02/2009
 *@version 1.0
 **
 ** PANDA task class for finding hits in the GEM
 ** Task level RECO
 ** Produces objects of type PndGemHits out of PndGemDigi.
 **/

#ifndef PNDGEMFINDHITS_H
#define PNDGEMFINDHITS_H 1

#include <PndPersistencyTask.h>
#include "TStopwatch.h"
#include "TH2F.h"
#include "TVector3.h"

#include <map>
#include <set>
#include <vector>

class TH1F;
class TClonesArray;
class PndGemMonitor;
class PndGemDigiPar;
class PndGemSensor;
class PndGemStation;
class PndGemCluster;
class PndGemDigitize;

class PndGemFindHits : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndGemFindHits();

  /** Standard constructor **/
  PndGemFindHits(Int_t iVerbose);

  /** Constructor with task name **/
  PndGemFindHits(const char *name, Int_t iVerbose);

  /** Destructor **/
  virtual ~PndGemFindHits();
  void ConfirmHits();   // why in public??
  void ActivateDigis(); // why in public??

  /** Execution **/
  virtual void Exec(Option_t *opt);

  void SetUseClusters(Bool_t bt = kTRUE) { fUseClusters = bt; }

  void RunTimeBased(Bool_t bt = kTRUE) { fTimeOrderedDigi = bt; }

  Bool_t GetTimeBased() const { return fTimeOrderedDigi; }

  void SetHitWindow(Double_t dd = 1.5) { fHitWindow = dd; }

 private:
  PndGemMonitor *fMonitor;                             //!   /** GEM monitor **/
  PndGemDigiPar *fDigiPar;                             /** Digitisation parameters **/
  TClonesArray *fDigis;                                /** Input array of PndGemDigi **/
  TClonesArray *fHits;                                 /** Output array of PndGemHit **/
  TClonesArray *fHitsTemp;                             /** Output array of PndGemHit **/
  std::map<PndGemSensor *, std::set<Int_t>> fDigiMapF; /** sensor digis (front) **/
  std::map<PndGemSensor *, std::set<Int_t>> fDigiMapB; /** sensor digis (back)  **/

  Int_t fMCPointBranchId;
  Bool_t fUseClusters;
  Bool_t fTimeOrderedDigi;

  Double_t fPrepTime;
  Double_t fSortTime;
  Double_t fCreateTime;
  Double_t fConfirmTime;
  Double_t fActivateTime;
  Double_t fAllTime;
  Double_t fHitWindow;

  Int_t fTNofEvents;
  Int_t fTNofDigis;
  Int_t fTNofHits;
  Int_t fTNofHitsTemp;

  TStopwatch fTimer;

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  /** Make sensorwise sets for sigis  **/
  void MakeSets();

  /** Sort digis sensorwise  **/
  void SortDigis();

  /** Find hits in one sensor **/
  Int_t FindHits(PndGemSensor *sensor, std::set<Int_t> &fSet, std::set<Int_t> &bSet);

  /** Find hits in one sensor (store in fHitsTemp) **/
  Int_t FindHits2(PndGemSensor *sensor, std::set<Int_t> &fSet, std::set<Int_t> &bSet);

  /** Check matching of hits on both sensors and store matched hits in fHits  **/
  void ConfirmHits2();

  /** Finish at the end of each event **/
  virtual void Finish();

  ClassDef(PndGemFindHits, 1);
};

#endif
