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
//
// Class PndEmcTwoCoordIndex
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Class to hold coordinate and index information
// for system in which two co-ordinates specifies a channel uniquely.
// (Based on class from BABAR framework)
//
//  Author List:
//
//         Phil Strother                         Imperial College
//         Stephen J. Gowdy                      University of Edinburgh
// Dima Melnichuk - adaption for PANDA
//
//======================================================================

#include "PndEmcTwoCoordIndex.h"
#include <iostream>
#include "math.h"

using namespace std;

//----------------
// Constructors --
//----------------
PndEmcTwoCoordIndex::PndEmcTwoCoordIndex() : fIndex(-1), fNeighbours(PndEmcCoordIndexSet())
{
  fCoords[0] = -1;
  fCoords[1] = -1;
}

PndEmcTwoCoordIndex::PndEmcTwoCoordIndex(long x, long y, long ind) : fIndex(ind), fNeighbours(PndEmcCoordIndexSet())
{
  fCoords[0] = x;
  fCoords[1] = y;
}

//--------------
// Destructor --
//--------------
PndEmcTwoCoordIndex::~PndEmcTwoCoordIndex() {}

//-------------
// Operators --
//-------------
bool PndEmcTwoCoordIndex::operator<(const PndEmcTwoCoordIndex &compare) const
{
  bool answer = (fIndex < compare.fIndex);
  return answer;
}

//-------------
// Methods   --
//-------------
const PndEmcCoordIndexSet PndEmcTwoCoordIndex::GetNeighbours() const
{
  return fNeighbours;
}

void PndEmcTwoCoordIndex::AddToNeighbourList(PndEmcTwoCoordIndex *addition)
{
  fNeighbours.insert(addition);
}

bool PndEmcTwoCoordIndex::IsNeighbour(PndEmcTwoCoordIndex *tci)
{

  if (abs(fCoords[0] - tci->XCoord()) > 1)
    return false;
  if (fCoords[1] < 200) {
    if ((abs(fCoords[1] - tci->YCoord()) > 1) && (abs(fCoords[1] - tci->YCoord()) != 159))
      return false;
  } else if (abs(fCoords[1] - tci->YCoord()) > 1) {
    return false;
  }

  return true;
}

ClassImp(PndEmcTwoCoordIndex)
