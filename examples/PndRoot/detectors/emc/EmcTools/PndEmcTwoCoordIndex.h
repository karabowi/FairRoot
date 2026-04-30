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

//======================================================================
// Class PndEmcTwoCoordIndex
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Class to hold coordinate and index information
// for system in which two co-ordinates specifies a channel uniquely.
// (Simplified version of class taken from BABAR framework)
// It is convenient to use 2-coordinate index to describe barel part of EMC
// to search for neigbour crystals in clustering algorithms
//
//
//  Author List:
//
//         Phil Strother                         Imperial College
//         Stephen J. Gowdy                      University of Edinburgh
// Dima Melnichuk - adaption for PANDA
//
//======================================================================
//#pragma once
#ifndef PNDEMCTWOCOORDINDEX_H
#define PNDEMCTWOCOORDINDEX_H

//---------------
// C++ Headers --
//---------------
#include "iostream"
#include "stdlib.h"
//#include <vector>
#include "TObject.h"
#include "PndEmcDataTypes.h"

using std::cout;
using std::endl;

/**
 * @brief stores crystal index coordinates (x,y) or (theta,phi)
 * @ingroup PndEmc
 */
class PndEmcTwoCoordIndex : public TObject {
 public:
  // Constructors
  PndEmcTwoCoordIndex();
  PndEmcTwoCoordIndex(long theXcoord, long theYcoord, long theIndex);

  // Destructors

  // destructor in base class
  virtual ~PndEmcTwoCoordIndex();

  // Operators
  bool operator==(const PndEmcTwoCoordIndex &c) const { return fIndex == c.fIndex; }
  bool operator!=(const PndEmcTwoCoordIndex &c) const { return fIndex != c.fIndex; }

  bool operator<(const PndEmcTwoCoordIndex &) const;

  virtual const PndEmcCoordIndexSet GetNeighbours() const;

  long XCoord() const { return fCoords[0]; }
  long YCoord() const { return fCoords[1]; }
  long Index() const { return fIndex; }

  virtual void AddToNeighbourList(PndEmcTwoCoordIndex *);

  // check if 2 tci are neigbour or not
  bool IsNeighbour(PndEmcTwoCoordIndex *_tci);

 protected:
  long fCoords[2];
  long fIndex;
  PndEmcCoordIndexSet fNeighbours;

 private:
  ClassDef(PndEmcTwoCoordIndex, 1)
};

#endif // PNDEMCTWOCOORDINDEX_HH
