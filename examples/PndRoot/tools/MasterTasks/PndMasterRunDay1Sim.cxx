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

#include "PndMasterRunDay1Sim.h"
#include "PndMultiField.h"
#include "PndCave.h"
#include "PndMagnet.h"
#include "PndPipe.h"
#include "PndMvdDetector.h"
#include "PndDrc.h"
#include "PndFtof.h"
#include "PndGemDetector.h"
#include "PndFts.h"
#include "PndStt.h"
#include "PndStt2.h"
#include "PndEmc.h"
#include "PndMdt.h"
#include "PndSciT.h"
#include "PndProcessTask.h"
#include "BSEmcDetector.h"
#include "BSEmcShashlikDetector.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcBarrelSensorNameIdMap.h"
#include "BSEmcBwEndcapSensorNameIdMap.h"
#include "BSEmcFwEndcapSensorNameIdMap.h"
#include "BSEmcShashlikSensorNameIdMap.h"
#include "CmpPndBarrelSondersliceSensorNameIdMap.h"
#include "PndEventCounterTask.h"

#include "BSEmcJoinBranches.h"
#include "BSEmcBarrelZDependentMCDepositProductionProcess.h"
#include "BSEmcParameterLoadingProcess.h"

#include "PndMasterSimTask.h"

#include "BSEmcMCDepositProducerTask.h"
#include "BSEmcMCDepositProductionProcess.h"

PndMasterRunDay1Sim::PndMasterRunDay1Sim()
{
  LOG(info) << "PndMasterRunDay1Sim created";
}

PndMasterRunDay1Sim::~PndMasterRunDay1Sim()
{
  // TODO Auto-generated destructor stub
}

void PndMasterRunDay1Sim::CreateGeometry()
{
  //-------------------------  CAVE      -----------------
  FairModule *Cave = new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  AddModule(Cave);
  //-------------------------  Magnet   -----------------
  // This part is commented because the MDT geometry contains the magnet now
  // FairModule *Magnet= new PndMagnet("MAGNET");
  // Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  // Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  // AddModule(Magnet);
  FairModule *Dipole = new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  AddModule(Dipole);
  //-------------------------  Pipe     -----------------
  FairModule *Pipe = new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201309.root");
  AddModule(Pipe);
  //-------------------------  STT       -----------------
  if (fOptions.Contains("stt2")) {
    FairDetector *Stt = new PndStt2("STT2", kTRUE);
    Stt->SetGeometryFileName("stt2_full.geo");
    AddModule(Stt);
  } else {
    FairDetector *Stt = new PndStt("STT", kTRUE);
    Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
    AddModule(Stt);
  }
  //-------------------------  MVD       -----------------
  if (fOptions.Contains("strip") || fOptions.Contains("nopixels")) {
    FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
    Mvd->SetGeometryFileName("Mvd-2.1-Strips.root");
    AddModule(Mvd);
  } else {
    FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
    Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
    AddModule(Mvd);
  }
  //-------------------------  EMC       -----------------
  PndEmc *Emc = new PndEmc("EMC", kTRUE);
  Emc->SetGeometryVersion(1);
  Emc->SetStorageOfData(kFALSE);
  AddModule(Emc);
  //-------------------------  SCITIL    -----------------
  FairDetector *SciT = new PndSciT("SCIT", kTRUE);
  SciT->SetGeometryFileName("SciTil_201601.root");
  AddModule(SciT);
  //-------------------------  DRC       -----------------
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetGeometryFileName("dirc_e3_b3_l6_m40.root");
  Drc->SetRunCherenkov(kFALSE);
  AddModule(Drc);
  //-------------------------  MDT       -----------------
  PndMdt *Muo = new PndMdt("MDT", kTRUE);
  Muo->SetBarrel("fast");
  Muo->SetEndcap("fast");
  Muo->SetMuonFilter("fast");
  Muo->SetForward("fast");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtCoil(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  AddModule(Muo);
  //-------------------------  FTOF      -----------------
  FairDetector *FTof = new PndFtof("FTOF", kTRUE);
  FTof->SetGeometryFileName("ftofwall.root");
  AddModule(FTof);

  if (fOptions.Contains("nogem") || fOptions.Contains("gem0")) {
    // do nothing
  } else if (fOptions.Contains("gem3")) // GEM 3 Stations
  {
    FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
    Gem->SetGeometryFileName("gem_3Stations_realistic_v3.root");
    AddModule(Gem);
  } else // GEM 2 Stations
  {
    //-------------------------  GEM       -----------------
    FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
    Gem->SetGeometryFileName("gem_2Stations_realistic_v3.root");
    AddModule(Gem);
  }

  if (fOptions.Contains("fts1256")) {
    //-------------------------  FTS       -----------------
    FairDetector *Fts = new PndFts("FTS", kTRUE);
    Fts->SetGeometryFileName("fts_1256.geo");
    AddModule(Fts);
  } else {
    //-------------------------  FTS       -----------------
    FairDetector *Fts = new PndFts("FTS", kTRUE);
    Fts->SetGeometryFileName("fts_reduced.geo");
    AddModule(Fts);
  }
}

/** @cond CLASSIMP */
ClassImp(PndMasterRunDay1Sim);
/** @endcond */
