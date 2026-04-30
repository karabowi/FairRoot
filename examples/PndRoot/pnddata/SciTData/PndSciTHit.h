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

// --------------------------------------------------------------------------
// -----                    Forward tof header file                	-----
// -----                    created by A. Sanchez                  	-----
// -----                   modified by D. Steinschaden                 	-----
// -----                   last update    05.2015 		       	-----
// --------------------------------------------------------------------------

/** PndSciTHit.h
 *@author D. Steinschaden <dominik.steinschaden@oeaw.ac.at>
 **
 ** A hit in a tof wall station of SciT. In addition to the base class
 ** FairHit, it holds the energy deposit in one detector tile.
 ** All coordinates are in the LAB frame.
 **
 **/

#ifndef PNDSCITHIT_H
#define PNDSCITHIT_H

#include "TVector3.h"
#include "TString.h"
#include "FairHit.h"

#include <stdio.h>
#include <iostream>

class PndSciTHit : public FairHit {

 public:
  //** Default constructor (not for use) **/
  PndSciTHit();

  PndSciTHit(Int_t detID, TString detName, Double_t time, Double_t dt, Double_t sipm1, Double_t dsipm1, Double_t sipm2, Double_t dsipm2, TVector3 &pos, TVector3 &dpos, Int_t index,
             Double_t charge);

  // PndSciTHit(PndSciTHit& c);
  //** Destructor **/
  virtual ~PndSciTHit();

  //** Accessors **/

  TString GetDetName() const { return fDetName; };
  Double_t GetCharge() { return fCharge; };

  Double_t GetSiPm1() { return fSiPm1; };
  Double_t GetDSiPm1() { return fDSiPm1; };
  Double_t GetSiPm2() { return fSiPm2; };
  Double_t GetDSiPm2() { return fDSiPm2; };

  // Following functions are redundant and should be removed.
  // use the base funktion provided by FairHit instead!!

  Double_t GetTime() { return GetTimeStamp(); };
  Double_t GetDt() { return GetTimeStampError(); };
  TVector3 GetPosition() const { return TVector3(fX, fY, fZ); };

  /** Modifiers **/

  void SetDetName(TString name) { fDetName = name; };
  void SetCharge(Double_t charge) { fCharge = charge; };
  void AddCharge(double charge) { fCharge += charge; }

  void SetSiPm1(Double_t sipm1) { fSiPm1 = sipm1; };
  void SetDSiPm1(Double_t dsipm1) { fDSiPm1 = dsipm1; };
  void SetSiPm2(Double_t sipm2) { fSiPm2 = sipm2; };
  void SetDSiPm2(Double_t dsipm2) { fDSiPm2 = dsipm2; };

  //** Operator overload**/

  friend std::ostream &operator<<(std::ostream &out, PndSciTHit &hit);

  virtual bool operator<(const PndSciTHit &right) const;
  virtual bool operator>(const PndSciTHit &right) const;

  virtual bool equal(FairTimeStamp *data);

  /** Screen output **/
  virtual void Print(const Option_t *opt = nullptr) const;

 public:
  TString fDetName; // Detector name
  Double_t fCharge; //, Eloss [GeV]

  Double_t fSiPm1;  // Timestamp for SiPm1  [ns]
  Double_t fDSiPm1; // Timing error for SiPm1
  Double_t fSiPm2;  // Timestamp for SiPm2
  Double_t fDSiPm2; // Timing error for SiPm2

  ClassDef(PndSciTHit, 3);
};

#endif
