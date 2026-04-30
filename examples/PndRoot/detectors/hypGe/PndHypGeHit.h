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
// -----                     CbmHypGeHit header file                     -----

// -------------------------------------------------------------------------

/**  CbmHypGeHit.h
 *@author a. sanchez
 **
 ** Hits in HYPGE
 **/

#ifndef PNDHYPGEHIT_H
#define PNDHYPGEHIT_H

#include "TObject.h"
#include "FairHit.h"
class PndHypGeHit : public FairHit {
 public:
  /** Default constructor **/
  PndHypGeHit();

  PndHypGeHit(Int_t trackId, Int_t detID, Double_t ener, Double_t den);

  /** Destructor **/
  virtual ~PndHypGeHit();

  /** Output to screen **/
  virtual void Print(const Option_t *opt = "") const;

  /** Modifiers **/
  virtual void SetEnergy(Double32_t ener) { fenergy = ener; };
  virtual void SetTrackId(Int_t detID) { fTrackId = detID; };
  virtual void SetDetectorId(Int_t detID) { fDetectorId = detID; };

  /** Accessors **/
  virtual Double_t GetEnergy() const { return fenergy; };
  virtual Int_t GetTrackId() const { return fTrackId; };
  virtual Int_t GetDetectorId() const { return fDetectorId; };

  Short_t GetCopy() const { return ((fDetectorId)); };

 protected:
  Double_t fenergy, fden; // hit amplitude
  Int_t fTrackId;
  Int_t fDetectorId;

  ClassDef(PndHypGeHit, 1)
};

#endif // PNDHYPGEHIT_H
