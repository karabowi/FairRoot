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

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndLmdHitPair
//      see PndLmdHitPair.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//		Roman Klasen (HIM), roklasen@uni-mainz.de using template by Tobias Stockmanns (IKP - Juelich)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndLmdHitPair.h"
//#include "FairRootManager.h"
//#include "FairEventHeader.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include <iostream>

ClassImp(PndLmdHitPair);

PndLmdHitPair::~PndLmdHitPair() {}

bool PndLmdHitPair::operator==(const PndLmdHitPair &rhs)
{
  if (_hit1 == rhs._hit1 && _hit2 == rhs._hit2 && _id1 == rhs._id1 && _id2 == rhs._id2) {
    return true;
  }
  if (_hit1 == rhs._hit2 && _hit2 == rhs._hit1 && _id1 == rhs._id2 && _id2 == rhs._id1) {
    return true;
  }
  return false;
}

void PndLmdHitPair::PrintPair() const
{
  std::cout << "=========== PndLmdHitPair::Print() ==========" << std::endl;
  std::cout << "=hitPair= col1: " << _col1 << ", col2: " << _col2 << ", row1: " << _row1 << ", row2: ";
  std::cout << _row2 << ", id1: " << _id1 << ", id2: " << _id2 << "\n moduleID: " << _moduleID << ", overlap ID: " << _overlapID << ", distance " << _distance << std::endl;
  std::cout << "------ hit 1 ------ " << std::endl;
  std::cout << "x: " << _hit1.x() << std::endl;
  std::cout << "y: " << _hit1.y() << std::endl;
  std::cout << "z: " << _hit1.z() << std::endl;
  std::cout << "------ hit 2 ------ " << std::endl;
  std::cout << "x: " << _hit2.x() << std::endl;
  std::cout << "y: " << _hit2.y() << std::endl;
  std::cout << "z: " << _hit2.z() << std::endl;
}

/*
 * checks this pair for decing errors.
 * this does not apply any cuts, the PairFinderTask is responsible for that.
 */
bool PndLmdHitPair::check()
{

  bool colSane = false, rowSane = false, idsane = false, allVarsSet = false; //, distanceOk=false; //[R.K. 9/2018] unused
  if (std::isinf(_col1) || std::isinf(_col2) || std::isnan(_col1) || std::isnan(_col2)) {
    colSane = false;
  } else {
    colSane = true;
  }
  if (std::isinf(_row1) || std::isinf(_row2) || std::isnan(_row1) || std::isnan(_row2)) {
    rowSane = false;
  } else {
    rowSane = true;
  }
  if (std::isinf(_id1) || std::isinf(_id2) || std::isnan(_id1) || std::isnan(_id2)) {
    idsane = false;
  } else {
    idsane = true;
  }

  // no row and col info, no valid pair
  if (_row1 < 0 || _row2 < 0 || _col1 < 0 || _col2 < 0) {
    rowSane = false;
    colSane = false;
    return false;
  }

  if (colSane && rowSane && idsane) {
    sane = true;
    checked = true;
    return true;
  } else {
    std::cerr << "colsane " << colSane << ", rowsane " << rowSane << ", idsane " << idsane << ", allVArsSet " << allVarsSet << std::endl;
  }
}

PndLmdHitPair::PndLmdHitPair(const TVector3 &hit1, const TVector3 &hit2, Int_t id1, Int_t id2)
{
  _row1 = _row2 = _col1 = _col2 = -1;
  _hit1 = hit1;
  _hit2 = hit2;
  _id1 = id1;
  _id2 = id2;
  checked = false;
  sane = false;
  _moduleID = -1;
  hit1present = true;
  hit2present = true;
  _distance = -1;
  _overlapID = -1;
}

PndLmdHitPair::PndLmdHitPair(Double_t col1, Double_t row1, Int_t id1, Double_t col2, Double_t row2, Int_t id2)
{
  _row1 = row1;
  _row2 = row2;
  _col1 = col1;
  _col2 = col2;
  _id1 = id1;
  _id2 = id2;
  checked = false;
  sane = false;
  _moduleID = -1;
  hit1present = false;
  hit2present = false;
  _distance = -1;
  _overlapID = -1;
}

PndLmdHitPair::PndLmdHitPair()
{
  _id1 = _id2 = _row1 = _row2 = _col1 = _col2 = -1;
  checked = false;
  sane = false;
  _moduleID = -1;
  hit1present = false;
  hit2present = false;
  _distance = -1;
  _overlapID = -1;
}

bool PndLmdHitPair::hitSensors(Int_t first, Int_t second)
{

  if (first == _id1) {
    if (second == _id2) {
      return true;
    }
  } else if (first == _id2) {
    if (second == _id1) {
      return true;
    }
  }
  return false;
}

void PndLmdHitPair::calculateDistance()
{

  // calculate absolute distance between hit1 and hit2, keeping the z distance of 250 present.
  if (hit1present && hit2present) {
    TVector3 distV = _hit1 - _hit2;
    _distance = distV.Mag();
  } else {
    _distance = -1;
  }
}

void PndLmdHitPair::swapHits()
{

  checked = false;
  std::swap(_id1, _id2);
  std::swap(_hit1, _hit2);
  std::swap(_row1, _row2);
  std::swap(_col1, _col2);
}
