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

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Software developed for the PANDA Detector at GSI.
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCRECOHIT_H
#define PNDEMCRECOHIT_H

//----------------------
// Base Class Headers --
//----------------------
#include "FairMultiLinkedData.h"
#include "TObject.h"
#include "TVector3.h"

/**
 * @brief represents the reconstructed hit in the emc
 * @ingroup PndEmc
 */
class PndEmcRecoHit : public FairMultiLinkedData {

 public:
  // Constructors
  PndEmcRecoHit();
  PndEmcRecoHit(Double_t energy, TVector3 position);

  // Destructor
  virtual ~PndEmcRecoHit();

  // Modifiers
  Double_t GetEnergy() const;
  Double_t GetEnergyCorrected() const;
  TVector3 GetPosition() const;

 private:
  // Data members
  Double_t fEnergy;
  Double_t fEnergyCorrected;
  TVector3 fPosition;

  ClassDef(PndEmcRecoHit, 1)
};

#endif // PNDEMCRECOHIT_H
