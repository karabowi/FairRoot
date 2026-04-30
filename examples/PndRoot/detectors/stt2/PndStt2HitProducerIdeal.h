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
 
#ifndef PNDSTT2HITPRODUCERIDEAL_H
#define PNDSTT2HITPRODUCERIDEAL_H

#include "PndPersistencyTask.h"

#include "TVector3.h"
//#include "TRandom.h"

class TClonesArray;
class PndStt2GeoHandler;
class PndStt2DigiPar;
//class PndStt2GeoPar;
class PndGeoSttPar;

class PndStt2HitProducerIdeal : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndStt2HitProducerIdeal();

  /** Destructor **/
  ~PndStt2HitProducerIdeal();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetPersistence(Bool_t persistence) { SetPersistency(persistence); }

 private:
  Double_t GetDistToWire(Int_t tubeID, TVector3 gPosIn, TVector3 gPosOut );
  //Double_t GetDistToWire(TVector3 lPosIn, TVector3 lPosOut);
  void SetParContainers();

  /** Input array of PndSttPoints **/
  TClonesArray *fPointArray;

  /** Output array of PndStt2Hit **/
  TClonesArray *fHitArray;

  //PndStt2GeoPar  *fSttGeoPar; 
  PndGeoSttPar  *fSttGeoPar; 
  PndStt2DigiPar *fSttDigiPar; 
  PndStt2GeoHandler *fSttGeoH;   //! the STT geo handler
  
  Double_t fResIsoIdeal;         // resolution [cm] of isochrone radius (from fSttDigiPar)

  ClassDef(PndStt2HitProducerIdeal, 1);
};

#endif
