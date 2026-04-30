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
// -----                PndMvdStripClusterTask source file             -----
// -------------------------------------------------------------------------

#include "PndMvdStripClusterTask.h"
#include "PndMvdContFact.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "PndSdsTotChargeConversion.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsChargeWeightingAlgorithms.h"
#include "PndSdsSimpleStripClusterFinder.h"
#include "PndSdsStripAdvClusterFinder.h"
#include "PndSdsTotDigiPar.h"

// -----   Default constructor   -------------------------------------------
PndMvdStripClusterTask::PndMvdStripClusterTask() : PndSdsStripClusterTask("MVD Strip Clusterisation Task") {}

// -----   Destructor   ----------------------------------------------------
PndMvdStripClusterTask::~PndMvdStripClusterTask() {}

void PndMvdStripClusterTask::GetParList(TList *tempList)
{
  PndSdsStripDigiPar *digipar = new PndSdsStripDigiPar("MVDStripDigiParRect");
  tempList->Add(digipar);

  digipar = new PndSdsStripDigiPar("MVDStripDigiParTrap");
  tempList->Add(digipar);

  PndSdsTotDigiPar *totpar = new PndSdsTotDigiPar("MVDStripTotDigiParRect");
  tempList->Add(totpar);

  totpar = new PndSdsTotDigiPar("MVDStripTotDigiParTrap");
  tempList->Add(totpar);

  fSensorNamePar = new PndSensorNamePar("PndSensorNamePar");
  tempList->Add(fSensorNamePar);
}

void PndMvdStripClusterTask::SetParContainersMQ(TList *tempList)
{
  fDigiParameterList->Add((PndSdsStripDigiPar *)tempList->FindObject("MVDStripDigiParTrap"));
  fDigiParameterList->Add((PndSdsStripDigiPar *)tempList->FindObject("MVDStripDigiParRect"));

  fChargeDigiParameterList->Add((PndSdsTotDigiPar *)tempList->FindObject("MVDStripTotDigiParTrap"));
  fChargeDigiParameterList->Add((PndSdsTotDigiPar *)tempList->FindObject("MVDStripTotDigiParRect"));
}

// -----   Manual I/O folders/branches   ----------------------------------------------------
void PndMvdStripClusterTask::SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outHitBranchname;
  fClustBranchName = outClustBranchname;
  fFolderName = folderName;
}

// -----   Default I/O folder/branches   ----------------------------------------------------
void PndMvdStripClusterTask::SetBranchNames()
{
  if (FairRunAna::Instance() != 0 && FairRunAna::Instance()->IsTimeStamp())
    fInBranchName = "MVDSortedStripDigis";
  else
    fInBranchName = "MVDStripDigis";
  //  fInBranchName = "MVDStripDigis";
  fOutBranchName = "MVDHitsStrip";
  fClustBranchName = "MVDStripClusterCand";
  fFolderName = "PndMvd";
}

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdStripClusterTask::SetParContainers()
{
  // called from the FairRun::Init()
  // Caution: The Parameter Set is not filled from the DB IO, yet.
  // This will be done just before this Tasks Init() is called.
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  PndMvdContFact *themvdcontfact = (PndMvdContFact *)rtdb->getContFactory("PndMvdContFact");
  fDigiParameterList->Clear();
  fChargeDigiParameterList->Clear();
  TList *theContNames = themvdcontfact->GetDigiParNames();
  if (fVerbose > 0)
    Info("SetParContainers()", "The container names list contains %i entries", theContNames->GetEntries());
  TIter cfIter(theContNames);
  fDigiParameterList->Clear();
  fChargeDigiParameterList->Clear();
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
  } // while
  PndSdsStripClusterTask::SetParContainers();
}

void PndMvdStripClusterTask::SetCalculators()
{
  Info("SetCalculators", "mvd");
  PndSdsStripClusterTask::SetCalculators();
  TIter params(fDigiParameterList);
  TIter totparams(fChargeDigiParameterList);
  while (PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)params()) {
    PndSdsTotDigiPar *totdigipar = (PndSdsTotDigiPar *)totparams();
    if (0 == digipar)
      continue;
    const char *senstype = digipar->GetSensType();
    if (digipar->GetChargeConvMethod() == 1) {
      if (fVerbose > 0)
        Info("SetCalculators()", "Use Tot charge conversion for %s sensors", senstype);
      fChargeConverter[senstype] =
        new PndSdsTotChargeConversion(totdigipar->GetChargingTime(), totdigipar->GetConstCurrent(), digipar->GetThreshold(), totdigipar->GetClockFrequency(), fVerbose);
    } else {
      if (fVerbose > 0)
        Info("SetCalculators()", "Use Ideal charge conversion for %s sensors", senstype);
      fChargeConverter[senstype] = new PndSdsIdealChargeConversion(digipar->GetNoise());
    }
    // fChargeAlgos[senstype] = new PndSdsChargeWeightingAlgorithms(fDigiArray, digipar->GetRaisingTime(),digipar->GetFallingRatio(),digipar->GetThreshold());
    Int_t ClusterMod = digipar->GetClusterMod();
    Int_t RadChannel = digipar->GetRadChannel();
    Int_t RadTime = digipar->GetRadTime();
    if (0 == ClusterMod) {
      fClusterFinderList[senstype] = new PndSdsSimpleStripClusterFinder(fInBranchId, RadChannel); // search radius in channel no.
    } else if (1 == ClusterMod) {
      fClusterFinderList[senstype] = new PndSdsStripAdvClusterFinder(fInBranchId, RadChannel, RadTime);
    }
  }
}

ClassImp(PndMvdStripClusterTask);
