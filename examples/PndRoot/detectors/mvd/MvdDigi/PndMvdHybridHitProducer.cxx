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
// -----                PndMvdHybridHitProducer source file             -----
// -------------------------------------------------------------------------

#include "PndMvdHybridHitProducer.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsTotChargeConversion.h"
#include "PndSdsIdealChargeConversion.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

// -----   Default constructor   -------------------------------------------
PndMvdHybridHitProducer::PndMvdHybridHitProducer() : PndSdsHybridHitProducer("MVD Hybrid Hit Producer")
{
  fPixelHits = 0;
  fEventNr = 0;
  fOverwriteParams = kFALSE;
  fPersistance = kTRUE;
  fDigiPixelMCInfo = kFALSE;
  if (fVerbose > 2)
    std::cout << "MVD Hybrid Digi Producer created, Parameters will be taken from RTDB" << std::endl;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdHybridHitProducer::~PndMvdHybridHitProducer() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdHybridHitProducer::Init()
{
  InitStatus status = PndSdsHybridHitProducer::Init();
  if (fDigiPar->GetChargeConvMethod() == 0) {
    if (fVerbose > 0)
      Info("Init()", "ideal charge conversion");
    fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
  } else if (fDigiPar->GetChargeConvMethod() == 1) {
    if (fVerbose > 0)
      Info("Init()", "use TOT charge conversion");
    fChargeConverter =
      new PndSdsTotChargeConversion(fTotDigiPar->GetChargingTime(), fTotDigiPar->GetConstCurrent(), fDigiPar->GetThreshold(), fTotDigiPar->GetClockFrequency(), fVerbose);
  } else
    Fatal("Init()", "charge conversion method not defined!");
  if (fVerbose > 0)
    Info("Init", "Intialisation successfull");
  return status;
}

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdHybridHitProducer::SetParContainers()
{
  // called before Init()
  // Get Base Container
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fDigiPar = (PndSdsPixelDigiPar *)(rtdb->getContainer("MVDPixelDigiPar"));
  fTotDigiPar = (PndSdsTotDigiPar *)(rtdb->getContainer("MVDPixelTotDigiPar"));
  PndSdsHybridHitProducer::SetParContainers();
}

// -------------------------------------------------------------------------
void PndMvdHybridHitProducer::SetBranchNames(TString inBranchname, TString outBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outBranchname;
  fFolderName = folderName;
  SetInBranchId();
}

// -------------------------------------------------------------------------
void PndMvdHybridHitProducer::SetBranchNames()
{
  if (fVerbose > 1)
    Info("SetBranchNames", "Set Mvd Pixel names.");
  fInBranchName = "MVDPoint";
  fOutBranchName = "MVDPixelDigis";
  fFolderName = "PndMvd";
  SetInBranchId();
}

ClassImp(PndMvdHybridHitProducer);
