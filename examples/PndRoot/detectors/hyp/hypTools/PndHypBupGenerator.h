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
// -----                PndHypBupGenerator header file                 -----
// -----                Created  by Alicia Sanchez                 -----
// -------------------------------------------------------------------------

/** PndHypBupGenerator
 *@author Alicia Sanchez  <a.sanchez@gsi.de>
 *
 *@version 1.0
 *
 ** The PndHypBupGenerator  uses the
 ** root output of the RAZHYP_GEN as input for simulation.
 ** The format of the event header is a THParticle ttree:

**/

#ifndef PNDHYPBUPGENERATOR_H
#define PNDHYPBUPGENERATOR_H 1

#include "THParticle.h"

#include "FairGenerator.h"

class TClonesArray;
class TFile;
class TTree;
class TVirtualMCStack;

class PndPrimaryGenerator;
// class CbmParticle;

class PndHypBupGenerator : public FairGenerator {

 public:
  /** Default constructor without arguments should not be used. **/
  PndHypBupGenerator();

  /** Standard constructor.
   ** @param fileName The input file name
   **/
  PndHypBupGenerator(const char *fileName);

  /** Destructor. **/
  virtual ~PndHypBupGenerator();

  /** Modifiers **/

  void SetPhiRange(Double32_t phimin = 0, Double32_t phimax = 360)
  {
    fPhiMin = phimin;
    fPhiMax = phimax;
  };

  void SetThetaRange(Double32_t thetamin = 0, Double32_t thetamax = 90)
  {
    fThetaMin = thetamin;
    fThetaMax = thetamax;
    fThetaRangeIsSet = kTRUE;
  };

  void SetPRange(Double32_t pmin = 0, Double32_t pmax = 10)
  {
    fPMin = pmin;
    fPMax = pmax;
    fPRangeIsSet = kTRUE;
  }

  void GammaEmissPar(Bool_t On = kFALSE) { fGamOn = On; }

  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the CbmPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator *primGen);

 private:
  //!  PDG database
  Int_t iEvent;             //! Event number
  const Char_t *fFileName;  //! Input file name
  TFile *fInputFile;        //! Pointer to input file
  TTree *fInputTree;        //! Pointer to input tree
  TClonesArray *fParticles; //! Particle array

  Double32_t fPMin, fPMax;         //! Transverse momentum range [GeV]
  Double32_t fPhiMin, fPhiMax;     //! Azimuth angle range [degree]
  Double32_t fThetaMin, fThetaMax; //! Polar angle range in lab system [degree]
  Bool_t fThetaRangeIsSet;         // True if theta range is set
  Bool_t fPRangeIsSet;             // True if abs.momentum range is set
  Bool_t fGamOn;                   // True if. gamma emission is selected

  /** Private method CloseInput. Just for convenience. Closes the
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();

  /** Private method RegisterIons. Goes through the input file and registers
   ** any ion needed. **/

  // Int_t RegisterIons();

  /** STL map from ion name to CbmIon **/
  // std::map<TString,CbmParticle*> fIonMap;       //!

  ClassDef(PndHypBupGenerator, 2);
};

#endif
