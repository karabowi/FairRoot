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
// -----                 CbmHypGeHitProducerIdel header file             -----
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

#ifndef PNDHYPGEHITPRODUCERIDEAL_H
#define PNDHYPGEHITPRODUCERIDEAL_H

#include "FairTask.h"
#include "PndGeoHypGePar.h"
#include "PndHypGeHit.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TFile.h"

class TClonesArray;

class PndHypGeHitProducerIdeal : public FairTask {
 public:
  /** Default constructor **/
  PndHypGeHitProducerIdeal();

  /** Destructor **/
  ~PndHypGeHitProducerIdeal();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec */
  virtual void Exec(Option_t *opt);
  void smear(Double_t &ener, Double_t &den);

 private:
  TString fBranchName;
  /** Input array of PndHypGePoints **/
  TClonesArray *fPointArray;

  /** Output array of PndHypGeHits **/
  TClonesArray *fHitArray;

  PndGeoHypGePar *fGeoPar;

  void Register();

  void Reset();

  void ProduceHits();

  ClassDef(PndHypGeHitProducerIdeal, 3);
};

#endif
