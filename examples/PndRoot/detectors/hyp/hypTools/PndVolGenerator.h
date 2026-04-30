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
// -----                PndVolGenerator header file                    -----
// -----       Created 22/05/07  by A. Sanchez and S. Bleser           -----
// -------------------------------------------------------------------------

/**  PndVolGenerator.h based on PnBoxGenerator.h (created
 *@author Stefano Spataro <stefano.spataro@exp2.physik.uni-giessen.de> )
 *
 * The PndVolGenerator allows to select sensitive volumes to
 * generates particles within given range
 * over pt, phi, eta, y or theta a fixed multiplicity per event.
 * Is it possible to have flat distribution on theta or on cos(theta)
 * Derived from FairGenerator
 * Original file (first import): FairVolGenerator.h
 * author Yuri Kharlov <Yuri.Kharlov@cern.ch>
 **/

#ifndef PND_VOLGENERATOR_H
#define PND_VOLGENERATOR_H

#include "FairGenerator.h"
#include <iostream>
//#include "PndGeoHandling.h"
// TGeoHMatrix* fCurrentTransMat;
#include "TGeoMatrix.h"
#include "TF1.h"

class FairPrimaryGenerator;

class PndVolGenerator : public FairGenerator {
 public:
  /** Default constructor. **/
  PndVolGenerator();

  /** Constructor with PDG-ID, multiplicity
   **@param pdgid Particle type (PDG encoding)
   **@param mult  Multiplicity (default is 1)
   **/
  PndVolGenerator(Int_t pdgid, Int_t mult = 1);

  /** Destructor **/
  virtual ~PndVolGenerator(){};

  /** Modifiers **/
  void SetPDGType(Int_t pdg) { fPDGType = pdg; };

  void SetMultiplicity(Int_t mult) { fMult = mult; };

  void SetPRange(Double32_t pmin = 0, Double32_t pmax = 10)
  {
    fPMin = pmin;
    fPMax = pmax;
    fPRangeIsSet = kTRUE;
  }

  void SetPtRange(Double32_t ptmin = 0, Double32_t ptmax = 10)
  {
    fPtMin = ptmin;
    fPtMax = ptmax;
    fPtRangeIsSet = kTRUE;
  };

  void SetPhiRange(Double32_t phimin = 0, Double32_t phimax = 360)
  {
    fPhiMin = phimin;
    fPhiMax = phimax;
  };

  void SetEtaRange(Double32_t etamin = -5, Double32_t etamax = 7)
  {
    fEtaMin = etamin;
    fEtaMax = etamax;
    fEtaRangeIsSet = kTRUE;
  };

  void SetYRange(Double32_t ymin = -5, Double32_t ymax = 7)
  {
    fYMin = ymin;
    fYMax = ymax;
    fYRangeIsSet = kTRUE;
  };

  void SetThetaRange(Double32_t thetamin = 0, Double32_t thetamax = 90)
  {
    fThetaMin = thetamin;
    fThetaMax = thetamax;
    fThetaRangeIsSet = kTRUE;
  };

  void SetCosTheta() { fCosThetaIsSet = kTRUE; };

  void SetInverseP() { fInversePIsSet = kTRUE; };

  void SetXYZ(Double32_t x = 0, Double32_t y = 0, Double32_t z = 0)
  {
    fX = x;
    fY = y;
    fZ = z;
    fPointVtxIsSet = kTRUE;
  }

  void SetVolXYZ(Double32_t x1 = 0, Double32_t y1 = 0, Double32_t x2 = 0, Double32_t y2 = 0, Double32_t z = 0)
  {
    fX1 = x1;
    fY1 = y1;
    fX2 = x2;
    fY2 = y2;
    fZ = z;
    fVolVtxIsSet = kTRUE;
  }

  void SetDebug(Bool_t debug = 0) { fDebug = debug; }

  void SetVolTarg(Int_t quad = 2, Int_t abs = 5)
  {
    fQuad = quad;
    fAbs = abs;
    fExt = kTRUE;
  }; // Absorber Volume Setter

  void SetVolTgFc(Int_t quad = 2, TF1 *func = nullptr)
  {
    fQuad = quad;
    fFunc = func;
    fExt = kFALSE;
  }; // Absorber Volume Setter

  /** Initializer **/
  Bool_t Init();

  /** Creates an event with given type and multiplicity.
   **@param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator *primGen);

 private:
  Int_t fPDGType; // Particle type (PDG encoding)
  Int_t fMult;    // Multiplicity
  Int_t fQuad;    // # quadrant
  Int_t fAbs;     // # Absorber Layer
  TF1 *fFunc;
  TGeoHMatrix *fCurrentTransMat;

  Double32_t fPDGMass;             // Particle mass [GeV]
  Double32_t fPtMin, fPtMax;       // Transverse momentum range [GeV]
  Double32_t fPhiMin, fPhiMax;     // Azimuth angle range [degree]
  Double32_t fEtaMin, fEtaMax;     // Pseudorapidity range in lab system
  Double32_t fYMin, fYMax;         // Rapidity range in lab system
  Double32_t fPMin, fPMax;         // Momentum range in lab system
  Double32_t fThetaMin, fThetaMax; // Polar angle range in lab system [degree]
  Double32_t fX, fY, fZ;           // Point vertex coordinates [cm]
  Double32_t fX1, fY1, fX2, fY2;   // Vol vertex coords (x1,y1)->(x2,y2)

  Bool_t fEtaRangeIsSet;   // True if eta range is set
  Bool_t fYRangeIsSet;     // True if rapidity range is set
  Bool_t fThetaRangeIsSet; // True if theta range is set
  Bool_t fCosThetaIsSet;   // True if uniform distribution in cos(theta) is set (default -> uniform theta)
  Bool_t fInversePIsSet;   // True if uniform distribution in 1/p
  Bool_t fPtRangeIsSet;    // True if transverse momentum range is set
  Bool_t fPRangeIsSet;     // True if abs.momentum range is set
  Bool_t fPointVtxIsSet;   // True if point vertex is set
  Bool_t fVolVtxIsSet;     // True if box vertex is set
  Bool_t fDebug;           // Debug switch
  Bool_t fExt;             // True if abs is set externally
  // PndGeoHandling* fGeoH;
  ClassDef(PndVolGenerator, 3);
};

#endif
