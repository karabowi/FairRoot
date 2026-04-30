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
// -----                PndMvdStripHitProducer source file             -----
// -----                Ralf Kliemt                                    -----
// -------------------------------------------------------------------------

// This Class
#include "PndMvdStripHitProducer.h"
// MVD
#include "PndMvdContFact.h"
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
PndMvdStripHitProducer::PndMvdStripHitProducer() : PndSdsStripHitProducer("MVD Strip Digi Producer(PndMvdStripHitProducer)") {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdStripHitProducer::~PndMvdStripHitProducer() {}

// -------------------------------------------------------------------------
void PndMvdStripHitProducer::SetBranchNames(TString inBranchName, TString outBranchName, TString folderName)
{
  fInBranchName = inBranchName;
  fOutBranchName = outBranchName;
  fFolderName = folderName;
  SetInBranchId();
}
// -------------------------------------------------------------------------
void PndMvdStripHitProducer::SetCalculators()
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
    if (digipar->GetChargeConvMethod() == 0) {
      if (fVerbose > 0)
        Info("SetCalculators()", "Use Ideal charge conversion for %s sensors", senstype);
      fChargeConverter[senstype] = new PndSdsIdealChargeConversion(digipar->GetNoise());
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
void PndMvdStripHitProducer::SetParContainers()
{
  // called from the FairRun::Init()
  // Caution: The Parameter Set is not filled from the DB IO, yet.
  // This will be done just before this Tasks Init() is called.

  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  PndMvdContFact *themvdcontfact = (PndMvdContFact *)rtdb->getContFactory("PndMvdContFact");
  TList *theContNames = themvdcontfact->GetDigiParNames();
  if (fVerbose > 0)
    Info("SetParContainers()", "The container names list contains %i entries", theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString *contname = (TObjString *)cfIter()) {
    TString parsetname = contname->String();
    if (fVerbose > 0)
      Info("SetParContainers()", "%s", parsetname.Data());
    if (parsetname.BeginsWith("MVDStripDigiPar")) {
      PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)(rtdb->getContainer(parsetname.Data()));
      if (!digipar)
        Fatal("SetParContainers", "No DIGI parameter found: %s", parsetname.Data());
      fDigiParameterList->Add(digipar);
    }
    if (parsetname.BeginsWith("MVDStripTotDigiPar")) {
      PndSdsTotDigiPar *totdigipar = (PndSdsTotDigiPar *)(rtdb->getContainer(parsetname.Data()));
      if (!totdigipar)
        Fatal("SetParContainers", "No TOT parameter found: %s", parsetname.Data());
      fChargeDigiParameterList->Add(totdigipar);
    }
  }
  PndSdsStripHitProducer::SetParContainers();
}

// -------------------------------------------------------------------------
ClassImp(PndMvdStripHitProducer);
