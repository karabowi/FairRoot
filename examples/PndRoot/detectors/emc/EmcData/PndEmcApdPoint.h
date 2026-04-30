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

// ------------------------------------------------------------------------
// -----                  PndEmcApdPoint header file                  -----
// -----               Created 07/04/08  by S.Spataro                 -----
// ------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCAPDPOINT_H
#define PNDEMCAPDPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"

class PndEmcApdPoint : public FairMCPoint {

 public:
  /** Default constructor **/
  PndEmcApdPoint();

  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param posIn    Ccoordinates at entrance to active volume [cm]
   *@param posOut   Coordinates at exit of active volume [cm]
   *@param momIn    Momentum of track at entrance [GeV]
   *@param momOut   Momentum of track at exit [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/

  PndEmcApdPoint(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, Short_t mod, Short_t row, Short_t crys,
                 Short_t copy, Short_t flag);

  /** Copy constructor **/
  PndEmcApdPoint(const PndEmcApdPoint &point);

  /** Destructor **/
  virtual ~PndEmcApdPoint();

  /** Accessors **/
  Double_t GetTheta() const { return fX == 0.0 && fY == 0.0 && fZ == 0.0 ? 0.0 : TMath::ATan2(sqrt(fX * fX + fY * fY), fZ) * TMath::RadToDeg(); };
  Double_t GetPhi() const { return fX == 0.0 && fY == 0.0 ? 0.0 : TMath::ATan2(fY, fX) * TMath::RadToDeg(); };
  Short_t GetFlag() const { return fFlag; };

  Short_t GetXPad() const;
  Short_t GetYPad() const;

  void Position(TVector3 &pos) { pos.SetXYZ(fX, fY, fZ); };
  void Momentum(TVector3 &mom) { mom.SetXYZ(fPx, fPy, fPz); };
  Short_t GetModule() const { return (fDetectorID / 100000000); };
  Short_t GetRow() const { return ((fDetectorID / 1000000) % 100); };
  Short_t GetCrystal() const { return (fDetectorID % 10000); };
  Short_t GetCopy() const { return ((fDetectorID / 10000) % 100); };
  /** Modifiers **/
  void SetModule(Short_t mod) { nModule = mod; };
  void SetRow(Short_t row) { nRow = row; };
  void SetCrystal(Short_t crys) { nCrystal = crys; };
  void SetFlag(Short_t flag) { fFlag = flag; };

  /** Output to screen **/
  virtual void Print(const Option_t *opt) const;

 protected:
  Short_t nModule;  // Module number
  Short_t nRow;     // Row number
  Short_t nCrystal; // Crystal number
  Short_t nCopy;    // Copy number
  Short_t fFlag;    // MC Flag: -1 entering, 0 crossing, 1 exiting,

  ClassDef(PndEmcApdPoint, 1)
};

#endif
