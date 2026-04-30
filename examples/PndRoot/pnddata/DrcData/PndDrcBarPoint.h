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
// -----                      PndDrcBarPoint header file                   -----
// -----                  Created 09/04/08  by E. Fioravanti and A. Cecchi               -----
// -------------------------------------------------------------------------

/**  PndDrcBarPoint.h
 **/

#ifndef PNDDRCBARPOINT_H
#define PNDDRCBARPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"

class PndDrcBarPoint : public FairMCPoint {

 public:
  /** Default constructor **/
  PndDrcBarPoint();

  /** Constructor with arguments **/
  PndDrcBarPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Int_t pdgCode, Double_t thetaC, Int_t nBar, Int_t eventID, Double_t mass);

  /** Copy constructor **/
  // PndDrcBarPoint(const PndDrcBarPoint& point) { *this = point; }; // Not needed

  /** Destructor **/
  virtual ~PndDrcBarPoint();

  /** Accessors **/
  Int_t GetPdgCode() const { return fPdgCode; };
  Double_t GetThetaC() const { return fThetaC; };
  Int_t GetBarId() const { return fBarN % 10; };
  Int_t GetBoxId() const { return fBarN / 10; };
  Double_t GetMass() const { return fMass; };
  Int_t GetTrackStatus() const { return fTrackStatus; };

  /** Modifiers ** **/
  void SetPdgCode(Int_t id) { fPdgCode = id; };
  void SetThetaC(Double_t theta) { fThetaC = theta; };
  void SetBarN(Int_t bar) { fBarN = bar; };
  void SetTrackStatus(Int_t status) { fTrackStatus = status; };

  /** Output to screen **/
  //  virtual void Print(const Option_t* opt = 0) const = 0;
  virtual void Print(const Option_t *opt) const;

 protected:
  Int_t fPdgCode;   // PDG code
  Double_t fThetaC; // Cherenkov Angle
  Int_t fBarN;      // Bar id number
  Double_t fMass;   // Mass
  Int_t fTrackStatus;

  ClassDef(PndDrcBarPoint, 3)
};

#endif
