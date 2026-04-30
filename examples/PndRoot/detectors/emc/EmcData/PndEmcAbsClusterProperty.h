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
//
// Description:
//	Class EmcAbsClusterProperty.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Stephen J. Gowdy	Originator
//	Phil Strother   	Originator
//
// Copyright Information:
//	Copyright (C) 1998	University of Edinburgh
//
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCABSCLUSTERPROPERTY_H
#define PNDEMCABSCLUSTERPROPERTY_H

#include "TObject.h"
#include "PndEmcCluster.h"

class PndEmcCluster;
class PndEmcDigi;
class PndEmcTwoCoordIndex;
class TClonesArray;

//		---------------------
// 		-- Class Interface --
//		---------------------

class PndEmcAbsClusterProperty : public TObject {

 public:
  // Constructors
  PndEmcAbsClusterProperty(const PndEmcCluster &cluster, const TClonesArray *digiArray);

  // Destructor
  virtual ~PndEmcAbsClusterProperty();

 protected:
  // Helper functions
  // Possible memory problems (FIXME)
  const PndEmcCluster &MyCluster() const;
  const std::map<Int_t, Int_t> &Members() const; // Map <detId,digiIndex>
  const TClonesArray *DigiArray() const;

  PndEmcAbsClusterProperty(const PndEmcAbsClusterProperty &L) : TObject(L), fMyCluster(L.fMyCluster), fMembers(L.fMembers){};
  PndEmcAbsClusterProperty &operator=(const PndEmcAbsClusterProperty &) { return *this; };

 private:
  //  PndEmcAbsClusterProperty(const  PndEmcAbsClusterProperty& L);
  //  PndEmcAbsClusterProperty &operator=(const PndEmcAbsClusterProperty&) {return *this;};
  //  PndEmc& operator= (const  PndEmc&) {return *this;};
  // Friends

  // Data members
  const PndEmcCluster &fMyCluster;
  const std::map<Int_t, Int_t> &fMembers; // Map <detId,digiIndex>
  const TClonesArray *fDigiArray;
};

#endif // PNDEMCABSCLUSTERPROPERTY_HH
