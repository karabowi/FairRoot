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

// -------------------------------------------------------------------------
// -----                  PndEmcApdHit header file                     -----
// -----               Created 07/04/08  by S.Spataro                  -----
// -------------------------------------------------------------------------

/**  PndEmcApdHit.h
 *@author S.Spataro
 **
 ** Hits in EMC APD
 **/

//#pragma once
#ifndef PNDEMCAPDHIT_H
#define PNDEMCAPDHIT_H

#include "FairHit.h"
#include "TMath.h"
#include "TArrayI.h"
#include <vector>

class PndEmcApdPoint;

class PndEmcApdHit : public FairHit {
 public:
  /** Default constructor **/
  PndEmcApdHit();

  PndEmcApdHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z);

  PndEmcApdHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z, Int_t npoint, Int_t pointIndex[10]);

  PndEmcApdHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z, std::vector<PndEmcApdPoint *> PointList);

  /** Copy **/
  PndEmcApdHit(const PndEmcApdHit &);

  /** Destructor **/
  virtual ~PndEmcApdHit();

  /** Output to screen **/
  virtual void Print(const Option_t *opt = "") const;

  /** Modifiers **/
  virtual void SetEnergy(Double32_t energy) { fEnergy = energy; };
  virtual void SetTime(Double32_t time) { fTime = time; };

  /** Accessors **/
  virtual Double_t GetEnergy() const { return fEnergy; };
  virtual Double_t GetTime() const { return fTime; };
  Float_t GetTheta() const { return fX == 0.0 && fY == 0.0 && fZ == 0.0 ? 0.0 : TMath::ATan2(sqrt(fX * fX + fY * fY), fZ) * TMath::RadToDeg(); };
  Float_t GetPhi() const { return fX == 0.0 && fY == 0.0 ? 0.0 : TMath::ATan2(fY, fX) * TMath::RadToDeg(); };
  Short_t GetModule() const { return (fDetectorID / 100000000); };
  Short_t GetRow() const { return ((fDetectorID / 1000000) % 100); };
  Short_t GetCrystal() const { return (fDetectorID % 10000); };
  Short_t GetCopy() const { return ((fDetectorID / 10000) % 100); };

  Short_t GetXPad() const;
  Short_t GetYPad() const;

  Int_t GetNPoints() const { return fNPoints; };
  Int_t GetMCIndex(Short_t ind) const { return fPointIndex[ind]; };

  // Possible memory issue (FIXME).
  std::vector<PndEmcApdPoint *> &GetPointList() { return fPointList; }

  // TArrayI* GetPoindIndexes() { return fPointIndexes;};

 protected:
  Double32_t fTime;      // time
  Double32_t fEnergy;    // hit amplitude
  Int_t fPointIndex[10]; // index of ther first 10 particles entering

  // TArrayI *fPointIndexes;

  std::vector<PndEmcApdPoint *> fPointList; //  points contributed to hit
  Int_t fNPoints;                           // number of ApdPoints

  ClassDef(PndEmcApdHit, 1)
};

#endif // PNDEMCAPDHIT_H
