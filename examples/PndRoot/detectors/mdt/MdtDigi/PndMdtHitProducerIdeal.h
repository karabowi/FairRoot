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

#ifndef PNDMDTHITPRODUCERIDEAL_H
#define PNDMDTHITPRODUCERIDEAL_H 1

#include <PndPersistencyTask.h>
#include "PndMdtHit.h"
#include "TVector3.h"

class TClonesArray;

class PndMdtHitProducerIdeal : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndMdtHitProducerIdeal();

  /** Destructor **/
  ~PndMdtHitProducerIdeal();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  PndMdtHit *AddHit(Int_t detID, TVector3 &pos, TVector3 &dpos, Int_t index);

  /**
   * @brief Set the smearing of the PndMdtHit position
   * @param res Resolution in [cm]
   */
  void SetPositionSmearing(Float_t res) { fPosResolution = res; };

 private:
  Float_t fPosResolution; ///< Position smearing [cm]

  /** Input array of PndMdtPoint **/
  TClonesArray *fPointArray;

  /** Output array of PndMdtHit **/
  TClonesArray *fHitArray;

  ClassDef(PndMdtHitProducerIdeal, 1);
};

#endif
