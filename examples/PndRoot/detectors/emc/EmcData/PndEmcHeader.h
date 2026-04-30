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
// -----                     PndEmcHeader header file                     -----
// -----               Created 21/04/07  by S.Spataro                  ----
// -------------------------------------------------------------------------

/**  PndEmcHeader.h
 *@author S.Spataro
 **
 ** Header of the EMC (multiplicity and total energy inside the calorimeter per event)
 **/

//#pragma once
#ifndef PNDEMCHEADER_H
#define PNDEMCHEADER_H

#include "TObject.h"
#include "TVector3.h"

class PndEmcHit;
class PndEmcCluster;

class PndEmcHeader : public TObject {
 public:
  /** Default constructor **/
  PndEmcHeader();

  PndEmcHeader(Double32_t ene_hit, Int_t mult_hit, Int_t mult_digi, Double32_t ene_clu, Int_t mult_clu, TVector3 p_clu);

  /** Copy **/
  PndEmcHeader(const PndEmcHeader &);

  /** Destructor **/
  virtual ~PndEmcHeader();

  /** Output to screen **/
  virtual void Print(const Option_t *opt = "") const;

  /** Modifiers **/
  virtual void SetHitEnergy(Double32_t energy) { fHitEnergy = energy; };
  virtual void SetCluEnergy(Double32_t energy) { fCluEnergy = energy; };
  virtual void SetHitMult(Int_t mult) { nHitMult = mult; };
  virtual void SetDigiMult(Int_t mult) { nDigiMult = mult; };
  virtual void SetCluMult(Int_t mult) { nCluMult = mult; };
  virtual void SetPx(Double32_t px) { fPx = px; };
  virtual void SetPy(Double32_t py) { fPy = py; };
  virtual void SetPz(Double32_t pz) { fPz = pz; };

  /** Accessors **/
  virtual Double32_t GetHitEnergy() const { return fHitEnergy; };
  virtual Double32_t GetCluEnergy() const { return fCluEnergy; };
  virtual Int_t GetHitMult() const { return nHitMult; };
  virtual Int_t GetDigiMult() const { return nDigiMult; };
  virtual Int_t GetCluMult() const { return nCluMult; };
  virtual Double32_t GetPx() const { return fPx; };
  virtual Double32_t GetPy() const { return fPy; };
  virtual Double32_t GetPz() const { return fPz; };

 protected:
  Double32_t fPx;        // total px
  Double32_t fPy;        // total py
  Double32_t fPz;        // total py
  Double32_t fHitEnergy; // total energy in crystals
  Double32_t fCluEnergy; // total energy in clusters
  Int_t nHitMult;        // number of fired crystals
  Int_t nDigiMult;       // number of fired crystals above threshold and after digitization
  Int_t nCluMult;        // number of reconstructed clusters

  ClassDef(PndEmcHeader, 2)
};

#endif // PNDEMCHEADER_H
