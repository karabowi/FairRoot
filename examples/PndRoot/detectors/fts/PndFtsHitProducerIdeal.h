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
// -----                 CbmStsHitProducerIdel header file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------

/** CbmStsHitProducerIdeal.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** The ideal hit producer produces hits of type CbmStsMapsHit by copying
 ** the MCPoint position. The position error is set to 1 mum, much
 ** smaller than can be obtained by any detector. Using the hits from
 ** this HitProducer is thus equivalent to using MC information
 ** directly, but with the correct data interface.
 **/

#ifndef PNDFTSHITPRODUCERIDEAL_H
#define PNDFTSHITPRODUCERIDEAL_H

#include <PndPersistencyTask.h>
#include "PndGeoFtsPar.h"
#include "PndFtsMapCreator.h"

#include "TVector3.h"
//#include "TRandom.h"

class TClonesArray;

class PndFtsHitProducerIdeal : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndFtsHitProducerIdeal();

  /** Destructor **/
  ~PndFtsHitProducerIdeal();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetPersistence(Bool_t persistence) { SetPersistency(persistence); }

 private:
  /** Private method GetClostestApproachToWire;
   **
   ** Returns the measured values of the straw, radius and z position
   *@param closestDistanceinPos the radial position of the hit in straw frame
   *@param zPosInStrawFrame the z-position of the hit in straw frame
   *@param inPos entry point of the track into the straw
   *@param outPos exit point of the track into the straw
   **/
  void GetClostestApproachToWire(Double_t &closestDistance, Double_t &closestDistanceError, TVector3 inPos, TVector3 outPos);
  void FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3 localInPos, TVector3 localOutPos);

  Double_t GetRadialResolution(Double_t radius);
  Double_t GetLongitudinalResolution(Double_t zpos);

  void SetParContainers();

  /** Input array of PndFtsPoints **/
  TClonesArray *fPointArray;

  /** Output array of PndFtsHits **/
  TClonesArray *fHitArray;

  /** Output array of PndFtsHitInfo **/
  TClonesArray *fHitInfoArray;

  /** from parameters array of PndFtsTube **/ //  CHECK added
  TClonesArray *fTubeArray;

  PndGeoFtsPar *fFtsParameters; //  CHECK added

  PndFtsHitProducerIdeal(const PndFtsHitProducerIdeal &L);
  PndFtsHitProducerIdeal &operator=(const PndFtsHitProducerIdeal &) { return *this; }

  ClassDef(PndFtsHitProducerIdeal, 1);
};

#endif
