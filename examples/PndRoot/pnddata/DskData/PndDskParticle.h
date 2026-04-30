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
// -----                 PndDskParticle header file                    -----
// -----                Created 21/03/08  by P. Koch                   -----
// -------------------------------------------------------------------------

#ifndef PNDDSKPARTICLE_H
#define PNDDSKPARTICLE_H

#include "PndMCPoint.h"

class PndDskParticle : public PndMCPoint {

 public:
  /** Default constructor **/
  PndDskParticle();

  /** Standard constructor with arguments
   *@param trackID        Index of MCTrack
   *@param detectorID     Detector ID where the first hit is produced
   *@param position       Position of Particle when created [cm]
   *@param momentum       Momentum of Particle when created  [eV]
   *@param time           Time since event start when created [ns]
   *@param pdgCode        Particles PDG code
   *@param pdgName        Particles PDG name according to pdgCode
   *@param energy         Energy of the Particle when created [GeV]
   *@param motherTrackID  Track ID of particle that created the Particle
   *@param motherPdgCode  PDG Code of particle that created the Particle
   *@param motherPdgName  PDG Name of particle that created the Particle
   **/
  PndDskParticle(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Int_t pdgCode, TString pdgName, Double_t energy, Int_t motherTrackID,
                 Int_t motherPdgCode, TString motherPdgName, Double_t mass, Double_t angIn, Double_t thetaC, Int_t nPhot);

  /** Copy constructor **/
  PndDskParticle(const PndDskParticle &particle);

  /** Destructor **/
  virtual ~PndDskParticle();

  /** Virtual method Print
   **
   ** Screen output of Particle
   **/
  virtual void Print(const Option_t *opt) const;

  /** Method SetFinalValues
   **
   ** Set the final values for a particle
   *@param exitPosition Position when particle disappears [cm]
   *@param exitMomentum Momentum when particle disappears [GeV]
   *@param exitTime     Time when particle disappears [ns]
   *@param exitEnergy   Energy when particle disappears [GeV]
   **/
  void SetFinalValues(TVector3 exitPosition, TVector3 exitMomentum, Double_t exitTime, Double_t exitEnergy);

  /** Accessors **/
  Int_t GetPdgCode() const { return fPdgCode; }
    TString GetPdgName() const { return "";}
  Double_t GetEnergy() const { return fEnergy; }
  Int_t GetMotherTrackID() const { return fMotherTrackID; }
  Int_t GetMotherPdgCode() const { return fMotherPdgCode; }
    TString GetMotherPdgName() const { return "";}
  Double_t GetExitTime() const { return fExitTime; }
  Double_t GetExitEnergy() const { return fExitEnergy; }
  Double_t GetMass() const { return fMass; };

  Double_t GetAngIn() const { return fAngIn; };
  Double_t GetThetaC() const { return fThetaC; };
  Int_t GetNPhot() const { return fNPhot; };

  /** Modifiers **/
  void SetAngIn(Double_t ang) { fAngIn = ang; };
  void SetThetaC(Double_t theta) { fThetaC = theta; };
  void SetNPhot(Int_t nPhot) { fNPhot = nPhot; };

 protected:
  Int_t fPdgCode = -1;   ///< PDG code of particle
  Double_t fEnergy = 0.; ///< Energy [GeV]
  Double_t fMass = 0.;   // Mass

  Int_t fMotherTrackID = -1; ///< Track ID of the first mother
  Int_t fMotherPdgCode = -1; ///< PDG code of the particle with fMotherTrackID

  Double_t fExitTime = 0.;   ///< Time when particle disappears [ns]
  Double_t fExitEnergy = 0.; ///< Energy when particle disappears [GeV]

  Double_t fThetaC = 0.; // Cherenkov Angle
  Double_t fAngIn = 0.;  // Rod number
  Int_t fNPhot = -1;     // number of produced photons

  ClassDef(PndDskParticle, 2)
};

#endif // PNDDSKPARTICLE_H
