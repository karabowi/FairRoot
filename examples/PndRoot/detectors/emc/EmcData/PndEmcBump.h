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
//	Class PndEmcBump. This call provides a standard interface for
//          Emc bump classes.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Stephen J. Gowdy	Originator
// Copyright Information:
//	Copyright (C) 1997	University of Edinburgh
//
// Dima Melnychuk, adaption for PANDA
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCBUMP_H
#define PNDEMCBUMP_H

//----------------------
// Base Class Headers --
//----------------------
#include "PndEmcCluster.h"
#include "TObject.h"

/**
 * @brief represents a reconstructed (splitted) emc cluster
 * @ingroup PndEmc
 */
class PndEmcBump : public PndEmcCluster {

 public:
  // Constructors
  PndEmcBump();

  // Destructor
  virtual ~PndEmcBump();

  /** Copy **/ // use compiler generated copy constructor
  // PndEmcBump(const PndEmcBump& copy);

  // Modifiers
  virtual void MadeFrom(Int_t clusterIndex);
  // 	virtual Int_t NumberOfDigis() const;
  // 	virtual Double_t RnumberOfDigis() const;
  virtual void Print(Int_t iBump) const;

  Int_t GetClusterIndex() { return fClusterIndex; };

  Int_t GetEventNo() const { return fEvtNo; }
  void SetEventNo(Int_t evtNo) { fEvtNo = evtNo; }

 protected:
  // Data members
  Int_t fClusterIndex; // Index of cluster the bump is made in TClonesArray
  Int_t fEvtNo;        // event number of seed digi
  	Double_t fTimeStamp;//three different weighted time

  ClassDef(PndEmcBump, 2) private : PndEmcBump &operator=(const PndEmcBump &rv);
};
#endif // PNDEMCBUMP_HH
