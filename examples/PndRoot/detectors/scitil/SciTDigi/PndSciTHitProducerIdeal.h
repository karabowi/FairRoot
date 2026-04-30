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
// -----                 PndSciTHitProducerIdeal  header file          -----
// -----                  Created by Alicia Sanchez                    -----
// -----                   modified by D. Steinschaden                 -----
// -----                   last update    04.2015 		       -----
// -------------------------------------------------------------------------

/** PndSciTHitProducerIdeal.h
 *@author a.sanchez <a.sanchez@gsi.de>
 **
 ** The ideal hit producer produces hits of type PndSciTHitProducerIdeal.h by copying
 ** the MCPoint position.  Using the hits from
 ** this HitProducer is thus equivalent to using MC information
 ** directly, but with the correct data interface.
 **/

#ifndef PNDSCITHITPRODUCERIDEAL_H
#define PNDSCITHITPRODUCERIDEAL_H

#include <PndPersistencyTask.h>
#include "PndSciTPoint.h"
#include "PndGeoHandling.h"
#include "PndGeoSciTPar.h"

#include "FairMCEventHeader.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"

#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TRandom.h"
#include <string>

class TClonesArray;

class PndSciTHitProducerIdeal : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndSciTHitProducerIdeal();
  PndSciTHitProducerIdeal(Double_t dt);

  /** Destructor **/
  ~PndSciTHitProducerIdeal();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  void smear(Double_t &time, Double_t &dt);
  void SetPersistence(Bool_t persistence) { SetPersistency(persistence); }

 private:
  TString fInBranchName;

  /** Input array of PndSciTPoints **/
  TClonesArray *fPointArray;

  /** Output array of PndSciTHits **/
  TClonesArray *fHitArray;

  PndGeoSciTPar *fGeoPar;

  PndGeoHandling *fGeoH; // For converting sensor ID (shortID) into the Full volume path

  Double_t fdt, fdt2;

  void Register();

  void Reset();

  void ProduceHits();

  ClassDef(PndSciTHitProducerIdeal, 2);
};

#endif
