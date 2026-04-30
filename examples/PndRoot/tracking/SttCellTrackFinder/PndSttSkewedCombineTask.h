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

#ifndef PndSttSkewedCombineTask_H_
#define PndSttSkewedCombineTask_H_

#include "FairTask.h"
//#include "PndGeoSttPar.h"
//#include "PndStt2GeoHandler.h"
#include "PndSttStrawMap.h"

//#include "PndSttGeometryMap.h"

#include <vector>
class TClonesArray;
class PndStt2GeoHandler;
class PndGeoSttPar;
//class PndSttGeometryMap;

class PndSttSkewedCombineTask : public FairTask {
 public:
  PndSttSkewedCombineTask() : FairTask("Stt Skewed Combine"), fVerbose(0), fPersistence(kTRUE){};

  virtual ~PndSttSkewedCombineTask(){};

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  // virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();

  // void AddHitBranch(TString branchName);//Has to be called before Init() is used otherwise the default names are taken!

  void SetVerbose(Int_t verbose) { fVerbose = verbose; };
  void SetPersistence(Bool_t val) { fPersistence = val; };

  //  void SetGeoH(PndGeoHandling geoH){ fGeoH=geoH;};

 private:
  TClonesArray *fSttHits;

  TClonesArray *fCombinedSkewedHits;

  PndStt2GeoHandler *fSttGeoH;       // STT geo handler
  //PndSttStrawMap fStrawMap;        // for getting more information about the tubes
  //PndSttGeometryMap *fGeometryMap; // for initializing the neighbors of each tube

  Int_t fVerbose;
  Bool_t fPersistence; // safe data?

  PndGeoSttPar *fSttParameters; // needed for SttStrawMap
  TClonesArray *fTubeArray;     // needed for SttStrawMap

  TClonesArray *fEventHeader;
  // PndGeoHandling* fGeoH;

  //	void Register();
  //	void Reset();
  //	void ProduceHits();

  ClassDef(PndSttSkewedCombineTask, 1);
};

#endif /*PndSttSkewedCombineTask_H_*/
