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

//-----------------------------------------------------------------------
// File and Version Information:
// 	$Id: $
//
// Description:
//	BSEmcXtal Class -
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//      Stephen J. Gowdy              University of Edinburgh
// Dima Melnichuk - adaption for PANDA
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//	Copyright (C) 1996	       CEA - Centre d'Etude de Saclay
//------------------------------------------------------------------------
#ifndef BSEMCXTAL_HH
#define BSEMCXTAL_HH

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TGeoArb8.h"
#include "TGeoMatrix.h"
#include "TVector3.h"

#include "BSEmcGeoItem.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcXtal
 * @brief represents coordinates of one crystal
 * @details helper class to represent one EMC crystal to determine position values of that crystal
 * @author Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
 * @author Stephen J. Gowdy              University of Edinburgh
 * @author Dima Melnichuk - adaption for PANDA
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcXtal : public BSEmcGeoItem {

 public:
  // Constructors
  BSEmcXtal();
  BSEmcXtal(Int_t t_id, const TGeoTrap &t_trap, const TVector3 &t_pos, const TGeoRotation &t_rot);

  // Destructor
  virtual ~BSEmcXtal();

  // Operators
  virtual Bool_t operator==(const BSEmcXtal &t_compare) const;
  virtual Bool_t operator<(const BSEmcXtal &t_compare) const;
  // Accessors (const)
  const TVector3 &GetCentre() const;
  const TVector3 &GetFrontCentre() const;
  const TVector3 &GetNormalToFrontFace() const;
  const TVector3 &GetAxisVector() const;
  const TGeoTrap &GetGeometry() const;
  const TGeoRotation &GetRotation() const;

  Double_t GetNPAngle() const;

 private:
  Double_t fLength{-1};
  const TGeoTrap fTrap{};
  const TGeoRotation fRotation{};
  TVector3 fCentre{0, 0, 0};
  TVector3 fFrontCentre{0, 0, 0};
  TVector3 fNormalToFrontFace{0, 0, 0};
  TVector3 fAxis{0, 0, 0};

  BSEmcXtal(const BSEmcXtal &t_other);
  BSEmcXtal &operator=(const BSEmcXtal & /*unused*/) { return *this; }

  ClassDef(BSEmcXtal, 1)
};

#endif /*BSEMCXTAL_HH*/
