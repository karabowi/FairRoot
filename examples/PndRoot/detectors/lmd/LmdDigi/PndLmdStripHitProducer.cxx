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
// -----                PndLmdStripHitProducer source file             -----
// -------------------------------------------------------------------------

// LUMI
#include "PndLmdStripHitProducer.h"
// LUMI
#include "PndLmdContFact.h"
//#include "PndLmdAlignPar.h"
// SDS
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsTotChargeConversion.h"
#include "PndSdsTotDigiPar.h"
// FAIR
#include "FairRun.h"
#include "FairRuntimeDb.h"
// ROOT
#include "TList.h"

// -----   Default constructor   -------------------------------------------
PndLmdStripHitProducer::PndLmdStripHitProducer() {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndLmdStripHitProducer::~PndLmdStripHitProducer()
{
  delete fGeoH;
}
// -------------------------------------------------------------------------

void PndLmdStripHitProducer::SetBranchNames(TString inBranchname, TString outBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outBranchname;
  fFolderName = folderName;
}

void PndLmdStripHitProducer::SetBranchNames()
{
  fInBranchName = "LMDPoint";
  fOutBranchName = "LMDStripDigis";
  fFolderName = "PndLmd";
  SetInBranchId();
}
// -------------------------------------------------------------------------
void PndLmdStripHitProducer::SetCalculators()
{
  PndSdsStripHitProducer::SetCalculators();
  // calculator for TOT charge conversion:
  TIter params(fDigiParameterList);
  TIter totparams(fChargeDigiParameterList);
  while (PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)params()) {
    PndSdsTotDigiPar *totdigipar = (PndSdsTotDigiPar *)totparams();
    if (0 == digipar)
      continue;
    const char *senstype = digipar->GetSensType();
    // cout<<"digipar->GetChargeConvMethod() = "<<digipar->GetChargeConvMethod()<<endl;
    if (digipar->GetChargeConvMethod() == 0) {
      if (fVerbose > 0)
        Info("SetCalculators()", "Use Ideal charge conversion for %s sensors", senstype);
      fChargeConverter[senstype] = new PndSdsIdealChargeConversion();
    } else if (digipar->GetChargeConvMethod() == 1) {
      if (fVerbose > 0)
        Info("SetCalculators()", "Use Tot charge conversion for %s sensors", senstype);
      fChargeConverter[senstype] =
        new PndSdsTotChargeConversion(totdigipar->GetChargingTime(), totdigipar->GetConstCurrent(), digipar->GetThreshold(), totdigipar->GetClockFrequency(), fVerbose);
    } else
      Fatal("SetCalculators()", "charge conversion method not defined!");
  }
}
// -------------------------------------------------------------------------
// -----   Initialization  of Parameter Containers -------------------------
void PndLmdStripHitProducer::SetParContainers()
{
  if (fGeoH == nullptr) {
    fGeoH = PndGeoHandling::Instance();
    fGeoH->SetParContainers();
  }
  // called from the FairRun::Init()
  // Caution: The Parameter Set is not filled from the DB IO, yet.
  // This will be done just before this Tasks Init() is called.

  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  PndLmdContFact *thelmdcontfact = (PndLmdContFact *)rtdb->getContFactory("PndLmdContFact");
  TList *theContNames = thelmdcontfact->GetDigiParNames();
  Info("SetParContainers()", "The container names list contains %i entries", theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString *contname = (TObjString *)cfIter()) {
    TString parsetname = contname->String();
    Info("SetParContainers()", "%s", parsetname.Data());
    if (parsetname.BeginsWith("SDSStripDigiPar")) {
      PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)(rtdb->getContainer(parsetname.Data()));
      fDigiParameterList->Add(digipar);
    }
    if (parsetname.BeginsWith("SDSStripTotDigiPar")) {
      PndSdsTotDigiPar *totdigipar = (PndSdsTotDigiPar *)(rtdb->getContainer(parsetname.Data()));
      if (!totdigipar)
        Fatal("SetParContainers", "No TOT parameter found: %s", parsetname.Data());
      fChargeDigiParameterList->Add(totdigipar);
    }
  }
  PndSdsStripHitProducer::SetParContainers();
}

//------------------------------------------------------------------
ClassImp(PndLmdStripHitProducer);
