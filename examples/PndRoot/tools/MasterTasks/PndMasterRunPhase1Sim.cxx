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

#include "PndMasterRunPhase1Sim.h"
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

PndMasterRunPhase1Sim::PndMasterRunPhase1Sim()
{
  LOG(info) << "PndMasterRunPhase1Sim created!";
}

PndMasterRunPhase1Sim::~PndMasterRunPhase1Sim()
{
  // TODO Auto-generated destructor stub
}

void PndMasterRunPhase1Sim::CreateGeometry()
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
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  AddModule(Mvd);
  //-------------------------  GEM       -----------------
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations_realistic_v3.root");
  AddModule(Gem);
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
  //-------------------------  DISC      -----------------
  // PndDsk* Dsk = new PndDsk("DSK", kTRUE);
  // Dsk->SetStoreCerenkovs(kFALSE);
  // Dsk->SetStoreTrackPoints(kFALSE);
  // AddModule(Dsk);
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
  //-------------------------  FTS       -----------------
  FairDetector *Fts = new PndFts("FTS", kTRUE);
  Fts->SetGeometryFileName("fts.geo");
  AddModule(Fts);
  //-------------------------  FTOF      -----------------
  FairDetector *FTof = new PndFtof("FTOF", kTRUE);
  FTof->SetGeometryFileName("ftofwall.root");
  AddModule(FTof);
  //-------------------------  RICH       ----------------
  // PndRich *Rich= new PndRich("RICH",kTRUE);
  // Rich->SetGeometryFileName("rich_v313.root");
  // AddModule(Rich);
}

/** @cond CLASSIMP */
ClassImp(PndMasterRunPhase1Sim);
/** @endcond */
