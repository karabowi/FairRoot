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
// -----                 PndDskCerenkov header file                    -----
// -----                Created 21/03/08  by P. Koch                   -----
// -------------------------------------------------------------------------

#ifndef PNDDSKCERENKOV_H
#define PNDDSKCERENKOV_H

#include "FairMCPoint.h"

class PndDskCerenkov : public FairMCPoint {

 public:
  /** Default constructor **/
  PndDskCerenkov();

  /** Standard constructor with arguments
   *@param trackID         Index of MCTrack
   *@param detectorID      Detector ID where the first hit is produced
   *@param position        Position of Cerenkov when created [cm]
   *@param momentum        Momentum of Cerenkov when created  [eV]
   *@param time            Time since event start when created [ns]
   *@param energy          Energy of the Cerenkov when created [eV]
   *@param wavelength      Vacuum wavelength hc/energy [nm]
   *@param motherTrackID   Track ID of particle that emitted the Cerenkov
   *@param motherPdgCode   PDG Code of particle that emitted the Cerenkov
   *@param motherPdgName   PDG Name of particle that emitted the Cerenkov
   **/
  PndDskCerenkov(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Double_t energy, Double_t wavelength, Int_t motherTrackID,
                 Int_t motherPdgCode, TString motherPdgName);

  /** Copy constructor **/
  PndDskCerenkov(const PndDskCerenkov &cerenkov);

  /** Destructor **/
  virtual ~PndDskCerenkov();

  /** Virtual method Print
   **
   ** Screen output of Cerenkov
   **/
  virtual void Print(const Option_t *opt) const;

  /** Accessors **/
  Double_t GetEnergy() const { return fEnergy; }
  Double_t GetWavelength() const { return fWavelength; }
  Int_t GetMotherTrackID() const { return fMotherTrackID; }
  Int_t GetMotherPdgCode() const { return fMotherPdgCode; }
    TString GetMotherPdgName() const { return "";}
  Int_t GetDetNumber() const { return fDetNumber; }
  Short_t GetDetType() const { return fDetType; }
  Double_t GetDetTime() const { return fDetTime; }
  TVector3 GetDetMomentum() const { return fDetMomentum; }
  Double_t GetPrimaryHitAngle() const { return fPrimaryHitAngle; }
  Double_t GetPrimaryAngleToCerenkov() const { return fPrimaryAngleToCerenkov; }
  Double_t GetNofReflections() const { return fNofReflections; }
  Double_t GetPWay() const { return fPWay; }
  TVector3 Get5RefPosition() const { return f5RefPosition; }
  Double_t GetTotalRefAngle() const { return fTotalRefAngle; }
  TVector3 GetLastPosition() const { return fLastPos; }

  /** Modifiers **/
  void AddReflection() { fNofReflections++; }
  void AddPWay(TVector3 pos);
  void SetFinalValues(Int_t detNumber, Short_t detType, Double_t detTime, TVector3 detMomentum, Double_t length, Double_t primaryHitAngle, Double_t primaryAngleToCerenkov);
  void Set5RefPosition(TVector3 pos) { f5RefPosition = pos; }
  void SetTotalRefAngle(Double_t angle) { fTotalRefAngle = angle; }

 protected:
  Double_t fEnergy;     ///< Energy of Cerenkov when created [eV]
  Double_t fWavelength; ///< Vacuum wavelength hc/energy [nm]
  Double_t fPWay;       ///< Projected Way [cm]
    TVector3 fLastPos;   //! The Last known position of the Cerenkov - needed to calculate the fPWay

  Int_t fMotherTrackID;   ///< Track ID of particle that emitted the Cerenkov
  Int_t fMotherPdgCode;   ///< PDG Code of particle that emitted the Cerenkov

  Int_t fDetNumber;      ///< Detector number that finally registered the photon.
  Short_t fDetType;      ///< Detector type
  Double_t fDetTime;     ///< Global time when detected [ns]
  TVector3 fDetMomentum; //! RK removed Momentum when detected [eV]

  Int_t fNofReflections; ///< Number of total reflections the Cerenkov did in the radiator

  Double_t fPrimaryHitAngle;        ///< Angle to the z-Axis when first registered
  Double_t fPrimaryAngleToCerenkov; ///< Angle between momentum of eachs first appearance

  TVector3 f5RefPosition;  //! RK removed Just for debug. Stores position afer 5 refelctions
  Double_t fTotalRefAngle; ///< Total reflection angle

  ClassDef(PndDskCerenkov, 1)
};

#endif // PNDDSKCERENKOV_H
