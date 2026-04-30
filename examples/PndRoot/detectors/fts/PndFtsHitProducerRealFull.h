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

#ifndef PNDFTSHITPRODUCERREALFULL_H
#define PNDFTSHITPRODUCERREALFULL_H 1

#include <PndPersistencyTask.h>
#include "PndGeoFtsPar.h"
#include "TVector3.h"

#include "PndFtsHitWriteoutBuffer.h"

class PndFtsHit;
class PndFtsHitInfo;
class TClonesArray;
class TObjectArray;

class PndFtsHitProducerRealFull : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndFtsHitProducerRealFull();

  /** Destructor **/
  ~PndFtsHitProducerRealFull();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  PndFtsHit *AddHit(Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, Int_t skew, Int_t iPoint, TVector3 &pos, TVector3 &dpos, Double_t p, Double_t rsim,
                    Double_t closestDistanceError, Double_t depcharge, Double_t timeOfFlight);

  PndFtsHitInfo *AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake);

  void FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3 localInPos, TVector3 localOutPos);

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { SetPersistency(persistence); }

  void SetParContainers();

  void RunTimeBased() { fTimeOrderedDigi = kTRUE; }

 private:
  /** Input array of PndFtsPoints **/
  TClonesArray *fPointArray;

  /** Output array of PndFtsHits **/
  // TClonesArray* fHitArray;
  PndFtsHitWriteoutBuffer *fDataBuffer;

  /** Output array of PndFtsHitInfo **/
  TClonesArray *fHitInfoArray;

  PndGeoFtsPar *fFtsParameters;

  Bool_t fTimeOrderedDigi;

  /** from parameters array of PndSttTube **/ //  CHECK added
  TClonesArray *fTubeArray;

  PndFtsHitProducerRealFull(const PndFtsHitProducerRealFull &L);
  PndFtsHitProducerRealFull &operator=(const PndFtsHitProducerRealFull &) { return *this; }

  ClassDef(PndFtsHitProducerRealFull, 1);
};

#endif
