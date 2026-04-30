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
// -----                 PndEmcHdrFiller header file                 -----
// -----                 Created 21/04/07  by S.Spataro                -----
// -------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCHDRFILLER_H
#define PNDEMCHDRFILLER_H

#include "FairTask.h"
#include "PndEmcHit.h"
#include "PndEmcCluster.h"
#include "TVector3.h"

class TClonesArray;
class PndEmcHeader;

/**
 * @brief Task to create Emc header
 * @ingroup PndEmc
 */
class PndEmcHdrFiller : public FairTask {

 public:
  /** Default constructor **/
  PndEmcHdrFiller();

  /** Destructor **/
  ~PndEmcHdrFiller();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  PndEmcHeader *AddHit(Double32_t ene_hit, Int_t mult_hit, Int_t mult_digi, Double32_t ene_clu, Int_t mult_clu, TVector3 p_clu);

 private:
  /** Input array of EmcHit **/
  TClonesArray *fHitArray;

  /** Input array of EmcDigi **/
  TClonesArray *fDigiArray;

  /** Input array of EmcCluster **/
  TClonesArray *fCluArray;

  /** Output array of PndEmcHeader **/
  TClonesArray *fHdrArray;

  PndEmcHdrFiller(const PndEmcHdrFiller &L);
  PndEmcHdrFiller &operator=(const PndEmcHdrFiller &) { return *this; };

  ClassDef(PndEmcHdrFiller, 1);
};

#endif
