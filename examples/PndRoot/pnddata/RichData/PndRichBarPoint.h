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
// -----                      PndRichBarPoint header file                   -----
// -----                  Created 09/04/08  by E. Fioravanti and A. Cecchi               -----
// -------------------------------------------------------------------------

/**  PndRichBarPoint.h
 **/

#ifndef PNDRICHBARPOINT_H
#define PNDRICHBARPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"

class PndRichBarPoint : public FairMCPoint {

 public:
  /** Default constructor **/
  PndRichBarPoint();

  /** Constructor with arguments **/
  PndRichBarPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Int_t pdgCode, Double_t thetaC, Int_t eventID, Double_t mass);

  PndRichBarPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Int_t pdgCode, Double_t thetaC, Int_t eventID, Double_t mass,
                  TVector3 pos0, TVector3 mom0);

  PndRichBarPoint(TVector3 pos, TVector3 dir, Double_t time);

  /** Copy constructor **/
  // PndRichBarPoint(const PndRichBarPoint& point) { *this = point; }; // Not needed

  /** Destructor **/
  virtual ~PndRichBarPoint();

  /** Accessors **/
  Int_t GetPdgCode() const { return fPdgCode; };
  Double_t GetThetaC() const { return fThetaC; };
  Double_t GetMass() const { return fMass; };
  Int_t GetTrackStatus() const { return fTrackStatus; };
  // TVector3 GetPosition()   const { return TVector3(fX,fY,fZ);};
  // TVector3 GetMomentum()   const { return TVector3(fPx,fPy,fPz);};
  TVector3 GetPosition0() const { return fTrackPos0; };
  TVector3 GetMomentum0() const { return fTrackMom0; };
  Double_t GetTime0() const { return fTime; };
  TVector3 GetAxisX() const { return fAxisX; };
  TVector3 GetAxisY() const { return fAxisY; };
  TVector3 GetAxisZ() const { return fAxisZ; };

  /** Modifiers ** **/
  void SetPdgCode(Int_t id) { fPdgCode = id; };
  void SetThetaC(Double_t theta) { fThetaC = theta; };
  void SetTrackStatus(Int_t status) { fTrackStatus = status; };

  void SetPosition0(TVector3 pos) { fTrackPos0 = pos; };
  void SetMomentum0(TVector3 dir);

  /** Output to screen **/
  //  virtual void Print(const Option_t* opt = 0) const = 0;
  virtual void Print(const Option_t *opt) const;

 protected:
  Int_t fPdgCode;   // PDG code
  Double_t fThetaC; // Cherenkov Angle
  Double_t fMass;   // Mass
  Int_t fTrackStatus;
  TVector3 fTrackPos0;
  TVector3 fTrackMom0;
  Double_t fTime0;
  TVector3 fAxisX;
  TVector3 fAxisY;
  TVector3 fAxisZ;

  ClassDef(PndRichBarPoint, 3)
};

#endif
