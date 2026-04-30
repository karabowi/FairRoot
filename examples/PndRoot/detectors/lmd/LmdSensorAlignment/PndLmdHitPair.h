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
//      Header for PndLmdHitPair
//      It has fields for exactly two sensor hits (after cluster finder),
//		each hit has one row and col. It also has exactly two TVector3 for
//		hit coordinate in the LMD coordinate system. This info must be
//		supplied by LmdPairFinderTask (or user) and cannot be decoded in
//		this class itsel.
//
// Environment:
//      Software developed for the PANDA Luminosity Detector at FAIR.
//
// Author List:
//      R. Klasen, roklasen@uni-mainz.de or r.klasen@gsi.de or r.klasen@ep1.rub.de
//
//
//-----------------------------------------------------------

#ifndef PNDLMDHITPAIR_HH
#define PNDLMDHITPAIR_HH

// Root Class Headers ----------------
//#include "FairTimeStamp.h"

#include "TVector3.h"
#include <TObject.h>
#include <iostream>

class PndLmdHitPair : public TObject { //: public FairTimeStamp {
 public:
  // Constructors/Destructors ---------
  PndLmdHitPair();
  PndLmdHitPair(Double_t col1, Double_t row1, Int_t id1, Double_t col2, Double_t row2, Int_t id2);
  PndLmdHitPair(const TVector3 &hit1, const TVector3 &hit2, Int_t id1, Int_t id2);
  ~PndLmdHitPair();

  // operators
  bool operator==(const PndLmdHitPair &rhs);
  void PrintPair() const;

 private:
  // Private Data Members ------------
  Double_t _col1, _col2, _row1, _row2, _distance;
  TVector3 _hit1, _hit2;
  Int_t _id1, _id2;
  Int_t _moduleID, _overlapID;
  bool sane, checked, hit1present, hit2present;

 public:
  ClassDef(PndLmdHitPair, 16);

  Int_t getId1() const { return _id1; }
  Int_t getId2() const { return _id2; }

  void setId1(Int_t ID1)
  {
    _id1 = ID1;
    checked = false;
  }

  void setId2(Int_t ID2)
  {
    _id2 = ID2;
    checked = false;
  }

  // sanity check applies only to valid numeric entries, so no entries are nan. or the like
  bool isSane() const { return sane; }

  const TVector3 &getHit1() const { return _hit1; }

  void setHit1(const TVector3 &hit1)
  {
    _hit1 = hit1;
    checked = false;
    hit1present = true;
  }

  const TVector3 &getHit2() const { return _hit2; }

  void setHit2(const TVector3 &hit2)
  {
    _hit2 = hit2;
    checked = false;
    hit2present = true;
  }

  void setPixelHits(double col1, double row1, double col2, double row2)
  {
    _row1 = row1;
    _col1 = col1;

    _row2 = row2;
    _col2 = col2;
  }

  bool check();

  bool hitSensors(Int_t first, Int_t second);

  Double_t getCol1() const { return _col1; }

  void setCol1(Double_t col1)
  {
    _col1 = col1;
    checked = false;
  }

  Double_t getCol2() const { return _col2; }

  void setCol2(Double_t col2)
  {
    _col2 = col2;
    checked = false;
  }

  Double_t getRow1() const { return _row1; }

  void setRow1(Double_t row1)
  {
    _row1 = row1;
    checked = false;
  }

  Double_t getRow2() const { return _row2; }

  void setRow2(Double_t row2)
  {
    _row2 = row2;
    checked = false;
  }

  Int_t getModuleId() const { return _moduleID; }

  void setModuleId(Int_t moduleId)
  {
    _moduleID = moduleId;
    checked = false;
  }

  bool isHit1present() const { return hit1present; }

  bool isHit2present() const { return hit2present; }

  Double_t getDistance() const { return _distance; }

  void calculateDistance();

  void setDistance(Double_t distance)
  {
    _distance = distance;
    checked = false;
  }

  Int_t getOverlapId() const { return _overlapID; }

  void setOverlapId(Int_t overlapId)
  {
    _overlapID = overlapId;
    checked = false;
  }
  void swapHits();
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
