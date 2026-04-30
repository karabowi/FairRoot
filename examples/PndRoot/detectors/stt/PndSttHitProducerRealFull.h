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

// Complete Digitization

#ifndef PNDSTTHITPRODUCERREALFULL_H
#define PNDSTTHITPRODUCERREALFULL_H 1

#include <PndPersistencyTask.h>
#include "PndGeoSttPar.h"

//#include "PndSttHit.h"
//#include "PndSttHitInfo.h"

#include "TVector3.h"

class PndSttHit;
class PndSttHitInfo;
class TClonesArray;
class TObjectArray;
class PndSttHitWriteoutBuffer;

class PndSttHitProducerRealFull : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndSttHitProducerRealFull();

  /** Destructor **/
  ~PndSttHitProducerRealFull();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  PndSttHit *AddHit(Int_t detID, Int_t tubeID, Int_t iPoint, TVector3 &pos, TVector3 &dpos, Double_t p, Double_t rsim, Double_t closestDistanceError, Double_t depcharge,
                    Double_t timeOfFlight);

  PndSttHitInfo *AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake);

  void FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3 localInPos, TVector3 localOutPos);

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { SetPersistency(persistence); }

  void SetParContainers();

  void RunTimeBased() { fTimeOrderedDigi = kTRUE; }

 private:
  /** Input array of PndSttPoints **/
  TClonesArray *fPointArray;

  /** Output array of PndSttHits **/
  TClonesArray *fHitArray;
  PndSttHitWriteoutBuffer *fDataBuffer;

  /** Output array of PndSttHitInfo **/
  TClonesArray *fHitInfoArray;

  /** from parameters array of PndSttTube **/ //  CHECK added
  TClonesArray *fTubeArray;

  Bool_t fTimeOrderedDigi; //!

  PndGeoSttPar *fSttParameters; //  CHECK added

  ClassDef(PndSttHitProducerRealFull, 2);
};

#endif
