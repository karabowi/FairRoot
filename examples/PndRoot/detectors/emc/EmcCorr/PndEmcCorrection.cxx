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
//
//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcCorrection.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PndEmcStructure.h"
#include "PndEmcDataTypes.h"

#include "PndEmcMapper.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"
#include "PndEmcBump.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TObject.h"
#include "TH2.h"
#include "TVector3.h"

#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;

//----------------
// Constructors --
//----------------
PndEmcCorrection::PndEmcCorrection(Int_t chosenModule, Double_t EnCorPhoton, Double_t EnCorElectron, Double_t EnCorPion, Double_t EnCorOther, Double_t ThCorPhoton,
                                   Double_t ThCorElectron, Double_t ThCorPion, Double_t ThCorOther, Double_t valzEnPhoton, Double_t valzEnElectron, Double_t valzEnPion,
                                   Double_t valzEnOther, Double_t valzThPhoton, Double_t valzThElectron, Double_t valzThPion, Double_t valzThOther)
  : fChosenModule(chosenModule),

    fEnergyCorrPhoton(EnCorPhoton), fEnergyCorrElectron(EnCorElectron), fEnergyCorrPion(EnCorPion), fEnergyCorrOther(EnCorOther),

    fThetaCorrPhoton(ThCorPhoton), fThetaCorrElectron(ThCorElectron), fThetaCorrPion(ThCorPion), fThetaCorrOther(ThCorOther),

    fEnCorrFactorPhoton(valzEnPhoton), fEnCorrFactorElectron(valzEnElectron), fEnCorrFactorPion(valzEnPion), fEnCorrFactorOther(valzEnOther),

    fThCorrFactorPhoton(valzThPhoton), fThCorrFactorElectron(valzThElectron), fThCorrFactorPion(valzThPion), fThCorrFactorOther(valzThOther)
{
}

PndEmcCorrection::PndEmcCorrection()
  : fChosenModule(0),

    fEnergyCorrPhoton(0), fEnergyCorrElectron(0), fEnergyCorrPion(0), fEnergyCorrOther(0),

    fThetaCorrPhoton(0), fThetaCorrElectron(0), fThetaCorrPion(0), fThetaCorrOther(0),

    fEnCorrFactorPhoton(0), fEnCorrFactorElectron(0), fEnCorrFactorPion(0), fEnCorrFactorOther(0),

    fThCorrFactorPhoton(0), fThCorrFactorElectron(0), fThCorrFactorPion(0), fThCorrFactorOther(0)
{
}

//--------------
// Destructor --
//--------------

PndEmcCorrection::~PndEmcCorrection() {}
//
// Module number
//
Int_t PndEmcCorrection::Module()
{
  return fChosenModule;
}

//
// Corrected Energy
//
Double_t PndEmcCorrection::EnergyCorrPhoton()
{
  return fEnergyCorrPhoton;
}

Double_t PndEmcCorrection::EnergyCorrElectron()
{
  return fEnergyCorrElectron;
}

Double_t PndEmcCorrection::EnergyCorrPion()
{
  return fEnergyCorrPion;
}

Double_t PndEmcCorrection::EnergyCorrOther()
{
  return fEnergyCorrOther;
}
//
// Corrected Theta
//
Double_t PndEmcCorrection::ThetaCorrPhoton()
{
  return fThetaCorrPhoton;
}

Double_t PndEmcCorrection::ThetaCorrElectron()
{
  return fThetaCorrElectron;
}

Double_t PndEmcCorrection::ThetaCorrPion()
{
  return fThetaCorrPion;
}

Double_t PndEmcCorrection::ThetaCorrOther()
{
  return fThetaCorrOther;
}

//
// Correction factors for Energy
//
Double_t PndEmcCorrection::EnCorrFactorPhoton()
{
  return fEnCorrFactorPhoton;
}

Double_t PndEmcCorrection::EnCorrFactorElectron()
{
  return fEnCorrFactorElectron;
}

Double_t PndEmcCorrection::EnCorrFactorPion()
{
  return fEnCorrFactorPion;
}

Double_t PndEmcCorrection::EnCorrFactorOther()
{
  return fEnCorrFactorOther;
}

//
// Correction factors for Theta
//
Double_t PndEmcCorrection::ThCorrFactorPhoton()
{
  return fThCorrFactorPhoton;
}

Double_t PndEmcCorrection::ThCorrFactorElectron()
{
  return fThCorrFactorElectron;
}

Double_t PndEmcCorrection::ThCorrFactorPion()
{
  return fThCorrFactorPion;
}

Double_t PndEmcCorrection::ThCorrFactorOther()
{
  return fThCorrFactorOther;
}

ClassImp(PndEmcCorrection)
