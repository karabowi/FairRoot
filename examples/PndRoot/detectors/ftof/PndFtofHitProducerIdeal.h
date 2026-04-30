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
// -----                 PndFtofHitProducerIdeal  header file             -----
// -----                  Created by Alicia Sanchez              -----
// -------------------------------------------------------------------------

/** PndFtofHitProducerIdeal.h
 *@author a.sanchez <a.sanchez@gsi.de>
 **
 ** The ideal hit producer produces hits of type PndFtofHitProducerIdeal.h by copying
 ** the MCPoint position.  Using the hits from
 ** this HitProducer is thus equivalent to using MC information
 ** directly, but with the correct data interface.
 **/

#ifndef PNDFTOFHITPRODUCERIDEAL_H
#define PNDFTOFHITPRODUCERIDEAL_H

#include <PndPersistencyTask.h>
#include "PndGeoFtofPar.h"
#include "PndFtofPoint.h"

#include "FairGeoVector.h"
#include "FairGeoTransform.h"

#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TRandom.h"
#include <string>

class TClonesArray;

class PndFtofHitProducerIdeal : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndFtofHitProducerIdeal();
  PndFtofHitProducerIdeal(Double_t dt, Double_t dt2);

  /** Destructor **/
  ~PndFtofHitProducerIdeal();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  void smear(Double_t &, Double_t &){}; // time dt //[R.K.03/2017] unused variable(s)

  void RunTimeBased() { fTimeOrderedDigi = kTRUE; }

 private:
  TString fBranchName;
  Bool_t fTimeOrderedDigi;

  /** Input array of PndFtofPoints **/
  TClonesArray *fPointArray;

  /** Output array of PndFtofHits **/
  TClonesArray *fHitArray;

  PndGeoFtofPar *fGeoPar;
  Double_t fdt, fdt2;

  void Register();

  void Reset();

  void ProduceHits();
  /* TGeoHMatrix GetTransformation (std::string detName) const; */
  /* void GetLocalHitPoints(PndFtofPoint* myPoint, FairGeoVector& myHitIn,FairGeoVector& myInL); */
  /* TVector3 GetSensorDimensions(std::string detName) const; */
  ClassDef(PndFtofHitProducerIdeal, 3);
};

#endif
