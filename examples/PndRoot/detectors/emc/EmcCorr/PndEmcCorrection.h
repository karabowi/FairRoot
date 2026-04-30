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

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class EmcCorrection
//      Do an energy and theta corrections
//      (at the moment for photons, 4.02.2010)
//
// Author List:
//      A. Biegun
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCCORRECTION_H
#define PNDEMCCORRECTION_H

//#include "FairTask.h"
#include "TObject.h"

#include "PndEmcCluster.h"

#include <TH2.h>
#include <TFile.h>
#include <TString.h>

class TClonesArray;
class TObjectArray;
class PndEmcDigiPar;
class PndEmcRecoPar;

class PndEmcCluster;
class PndEmcDigi;

class PndEmcCorrection : public TObject {

 public:
  // Constructors
  PndEmcCorrection();

  PndEmcCorrection(Int_t chosenModule, Double_t EnCorPhoton, Double_t EnCorElectron, Double_t EnCorPion, Double_t EnCorOther, Double_t ThCorPhoton, Double_t ThCorElectron,
                   Double_t ThCorPion, Double_t ThCorOther, Double_t valzEnPhoton, Double_t valzEnElectron, Double_t valzEnPion, Double_t valzEnOther, Double_t valzThPhoton,
                   Double_t valzThElectron, Double_t valzThPion, Double_t valzThOther);

  // Destructor
  virtual ~PndEmcCorrection();

  Int_t Module();

  Double_t EnergyCorrPhoton();
  Double_t EnergyCorrElectron();
  Double_t EnergyCorrPion();
  Double_t EnergyCorrOther();

  Double_t ThetaCorrPhoton();
  Double_t ThetaCorrElectron();
  Double_t ThetaCorrPion();
  Double_t ThetaCorrOther();

  Double_t EnCorrFactorPhoton();
  Double_t EnCorrFactorElectron();
  Double_t EnCorrFactorPion();
  Double_t EnCorrFactorOther();

  Double_t ThCorrFactorPhoton();
  Double_t ThCorrFactorElectron();
  Double_t ThCorrFactorPion();
  Double_t ThCorrFactorOther();

 private:
  // Copy const.
  PndEmcCorrection(const PndEmcCorrection &) = delete;
  // Assignment operator.
  PndEmcCorrection &operator=(const PndEmcCorrection &) = delete;

  Int_t fChosenModule;

  Double_t fEnergyCorrPhoton;
  Double_t fEnergyCorrElectron;
  Double_t fEnergyCorrPion;
  Double_t fEnergyCorrOther;

  Double_t fThetaCorrPhoton;
  Double_t fThetaCorrElectron;
  Double_t fThetaCorrPion;
  Double_t fThetaCorrOther;

  Double_t fEnCorrFactorPhoton;
  Double_t fEnCorrFactorElectron;
  Double_t fEnCorrFactorPion;
  Double_t fEnCorrFactorOther;

  Double_t fThCorrFactorPhoton;
  Double_t fThCorrFactorElectron;
  Double_t fThCorrFactorPion;
  Double_t fThCorrFactorOther;

  ClassDef(PndEmcCorrection, 1)
};
#endif // PNDEMCCORRECTION_HH
