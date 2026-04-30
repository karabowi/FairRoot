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

////////////////////////////////////////////////////////////////////////////
// PndOtHitProducerRealFast
//
// Class for digitalization for OT
//
// authors: Radoslaw Karabowicz, GSI, 2024
//
// modified from PndFtsHitProducerRealFast by Nafija Ibrišimović in 2023
////////////////////////////////////////////////////////////////////////////

// Fast Digitization

#ifndef PNDOTHITPRODUCERREALFAST_H
#define PNDOTHITPRODUCERREALFAST_H 1

// from PandaRoot, this library
#include "PndGeoOtPar.h"
#include "PndGeoOtPar.h"
// from PandaRoot, pnddata
#include <PndPersistencyTask.h>
// from ROOT
#include <TVector3.h>

class PndOtHit;
class PndOtHitInfo;
class TClonesArray;
class TObjectArray;

class PndOtHitProducerRealFast : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndOtHitProducerRealFast();

  /** Destructor **/
  ~PndOtHitProducerRealFast();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  PndOtHit *AddHit(Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, Int_t skew, Int_t iPoint, TVector3 &pos, TVector3 &dpos, Double_t p, Double_t rsim,
                   Double_t closestDistanceError, Double_t depcharge);

  PndOtHitInfo *AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake);

  void FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3 localInPos, TVector3 localOutPos);

  Double_t GetError(Double_t);

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { SetPersistency(persistence); }
  void SwitchOnOverlap() { fOverlap = kTRUE; }

 private:
  void SetParContainers();

  /** Input array of PndOtPoints **/
  TClonesArray *fPointArray;

  /** Input array of OT PndOtPoints **/
  TClonesArray *fOtPointArray;

  /** Output array of PndOtHits **/
  TClonesArray *fHitArray;
  TClonesArray *fOverlapHitArray;

  TObjArray *fVolumeArray;

  /** Output array of PndOtHitInfo **/
  TClonesArray *fHitInfoArray;
  Int_t fevtn;

  PndGeoOtPar *fOtParameters;

  Bool_t fOverlap; //!

  TClonesArray *fOtTubeArray;

  PndOtHitProducerRealFast(const PndOtHitProducerRealFast &L);
  PndOtHitProducerRealFast &operator=(const PndOtHitProducerRealFast &) { return *this; }

  ClassDef(PndOtHitProducerRealFast, 1);
};

#endif
