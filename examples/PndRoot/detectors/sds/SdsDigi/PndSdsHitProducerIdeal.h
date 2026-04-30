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
// -----                 PndSdsHitProducerIdeal header file             -----
// -----                 based on the CbmStsHitProducerIdeal
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------

/** @class PndSdsHitProducerIdeal
 ** @author V.Friese <v.friese@gsi.de>
 **
 ** The ideal hit producer produces hits of type PndSdsHit by copying
 ** the MCPoint position. The position can be smeared by a user given value.
 **/

#ifndef PNDSDSHITPRODUCERIDEAL_H
#define PNDSDSHITPRODUCERIDEAL_H

#include "PndSdsTask.h"
//#include "PndSdsGeoPar.h"
#include "PndDetectorList.h"

#include "TVector3.h"
#include "TRandom.h"

class TClonesArray;

class PndSdsHitProducerIdeal : public PndSdsTask {
 public:
  /** Default constructor **/
  PndSdsHitProducerIdeal();

  PndSdsHitProducerIdeal(PndSdsHitProducerIdeal &other) : PndSdsTask(), fPointArray(other.fPointArray), fHitArray(other.fHitArray){};
  /** Named constructor **/
  PndSdsHitProducerIdeal(const char *name);

  /** Destructor **/
  virtual ~PndSdsHitProducerIdeal();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** pure virtual method SetBranchNames
   **
   ** called by Init()
   ** function to set individual branch names
   **/
  virtual void SetBranchNames() = 0;

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetPersistance(Bool_t p = kTRUE) { SetPersistency(p); }
  void SetSmearing(Double_t x, Double_t y, Double_t z){ fDx= x; fDy=y; fDz=z;}

  PndSdsHitProducerIdeal &operator=(PndSdsHitProducerIdeal &other)
  {
    if (this != &other) // protect against invalid self-assignment
    {
      fPointArray = other.fPointArray;
      fHitArray = other.fHitArray;
    }
    return *this;
  }

 protected:
  /** Input array of PndSdsMCPoints **/
  TClonesArray *fPointArray;

  /** Output array of PndSdsHits **/
  TClonesArray *fHitArray;

  Double_t fDx = 0.0001;		//< Gaussian smearing in x (default set to 1 µm to have a non-zero value)
  Double_t fDy = 0.0001;		//< Gaussian smearing in y
  Double_t fDz = 0.0001;		//< Gaussian smearing in z

  // PndSdsGeoPar* fGeoPar;

  void Register();

  void Reset();

  void ProduceHits();

  ClassDef(PndSdsHitProducerIdeal, 4);
};

#endif
