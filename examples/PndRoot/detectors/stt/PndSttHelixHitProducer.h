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

#ifndef PNDSTTHELIXHITPRODUCER_H
#define PNDSTTHELIXHITPRODUCER_H 1

#include <PndPersistencyTask.h>
#include "PndSttHelixHit.h"
#include "PndGeoSttPar.h"

#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

class TClonesArray;
class TObjectArray;

class PndSttHelixHitProducer : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndSttHelixHitProducer();

  PndSttHelixHitProducer(Int_t verbose);

  /** Destructor **/
  ~PndSttHelixHitProducer();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void WriteHistograms();

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { SetPersistency(persistence); }

  void SetParContainers();

 private:
  /** Input array of PndSttPoints **/
  TClonesArray *fPointArray;
  /** Input array of PndSttHits **/
  TClonesArray *fHitArray;
  /** Input array of PndSttTracks **/
  TClonesArray *fTrackArray;
  /** Input array of PndTracksCand **/
  TClonesArray *fTrackCandArray;

  /** Output array of PndSttHelixHits **/
  TClonesArray *fHelixHitArray;

  /** from parameters array of PndSttTube **/ //  CHECK added
  TClonesArray *fTubeArray;

  // histograms
  TH1F *hx, *hy, *hz;
  TH1F *hxs, *hys, *hzs;
  TH2F *hzresvsslope;

  PndGeoSttPar *fSttParameters; //  CHECK added

  ClassDef(PndSttHelixHitProducer, 1);
};

#endif
