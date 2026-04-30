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

// Fast Digitization

#ifndef PNDSTTHITPRODUCERREALFAST_H
#define PNDSTTHITPRODUCERREALFAST_H 1

#include <PndPersistencyTask.h>
#include "PndGeoSttPar.h"

#include "TVector3.h"

class PndSttHit;
class PndSttHitInfo;
class TClonesArray;
class TObjectArray;

class PndSttHitProducerRealFast : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndSttHitProducerRealFast();

  /** Destructor **/
  virtual ~PndSttHitProducerRealFast();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  PndSttHit *AddHit(Int_t detID, Int_t tubeID, Int_t iPoint, TVector3 &pos, TVector3 &dpos, Double_t p, Double_t rsim, Double_t closestDistanceError, Double_t depcharge);
  PndSttHit *AddHit(TClonesArray *hitarray, Int_t detID, Int_t tubeID, Int_t iPoint, TVector3 &pos, TVector3 &dpos, Double_t p, Double_t rsim, Double_t closestDistanceError,
                    Double_t depcharge);

  PndSttHitInfo *AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake);

  void FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3 localInPos, TVector3 localOutPos);

  Double_t GetError(Double_t, Int_t);

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { SetPersistency(persistence); }
  void SwitchOnOverlap() { fOverlap = kTRUE; }

  void SeparateHits() { fSeparate = kTRUE; }

 private:
  virtual void SetParContainers();

  /** Input array of PndSttPoints **/
  TClonesArray *fPointArray;

  /** Output array of PndSttHits **/
  TClonesArray *fHitArray;
  TClonesArray *fOverlapHitArray;

  Bool_t fSeparate;

  TClonesArray *fSttParalHitArray;
  TClonesArray *fSttParalOverlapHitArray;
  TClonesArray *fSttSkewHitArray;
  TClonesArray *fSttSkewOverlapHitArray;

  TObjArray *fVolumeArray;

  /** Output array of PndSttHitInfo **/
  TClonesArray *fHitInfoArray;
  Int_t fevtn;

  PndGeoSttPar *fSttParameters;

  /** object persistence **/
  Bool_t fPersistence;                        //!
  Bool_t fOverlap;                            //!
  /** from parameters array of PndSttTube **/ //  CHECK added
  TClonesArray *fTubeArray;
  Int_t fGeoType; //< Distinguishes between old geometry description (1) and new, root-based one (2)

  ClassDef(PndSttHitProducerRealFast, 1);
};

#endif
