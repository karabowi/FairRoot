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

#include "PndMasterRunCompactSim.h"
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

PndMasterRunCompactSim::PndMasterRunCompactSim()
{
  // TODO Auto-generated constructor stub
}

PndMasterRunCompactSim::~PndMasterRunCompactSim()
{
  // TODO Auto-generated destructor stub
}

void PndMasterRunCompactSim::SetField()
{
  PndMultiField *field = new PndMultiField("COMPACT");
  FairRunSim::SetField(field);
}

void PndMasterRunCompactSim::CreateGeometry()
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
  Dipole->SetGeometryFileName("magnet_zeus.root");
  AddModule(Dipole);
  //-------------------------  Pipe     -----------------
  FairModule *Pipe = new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201309.root");
  AddModule(Pipe);
  //-------------------------  STT       -----------------
  FairDetector *Stt = new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("stt2_red.geo");
  AddModule(Stt);
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
  AddEmcGeometry();
  //-------------------------  SCITIL    -----------------
  FairDetector *SciT = new PndSciT("SCIT", kTRUE);
  SciT->SetGeometryFileName("SciTil_201601_compact.root");
  AddModule(SciT);
  //-------------------------  DRC       -----------------
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetGeometryFileName("dirc_e3_b3_l6_m80_zeus.root");
  Drc->SetRunCherenkov(kFALSE);
  AddModule(Drc);
  //-------------------------  MDT       -----------------
  //  PndMdt *Muo = new PndMdt("MDT", kTRUE);
  //  Muo->SetBarrel("fast");
  //  Muo->SetEndcap("fast");
  //  Muo->SetMuonFilter("fast");
  //  Muo->SetForward("fast");
  //  Muo->SetMdtMagnet(kTRUE);
  //  Muo->SetMdtCoil(kTRUE);
  //  Muo->SetMdtMFIron(kTRUE);
  //  AddModule(Muo);
  //-------------------------  FTOF      -----------------
  //	  FairDetector *FTof = new PndFtof("FTOF", kTRUE);
  //	  FTof->SetGeometryFileName("ftofwall.root");
  //	  AddModule(FTof);

  if (fOptions.Contains("nogem") || fOptions.Contains("gem0")) {
    // do nothing
  } else {
    FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
    Gem->SetGeometryFileName("gem_3Stations_zeus.root");
    AddModule(Gem);
  }

  //	  if (fOptions.Contains("fts1256")) {
  //	    //-------------------------  FTS       -----------------
  //	    FairDetector *Fts = new PndFts("FTS", kTRUE);
  //	    Fts->SetGeometryFileName("fts_1256.geo");
  //	    AddModule(Fts);
  //	  } else {
  //	    //-------------------------  FTS       -----------------
  //	    FairDetector *Fts = new PndFts("FTS", kTRUE);
  //	    Fts->SetGeometryFileName("fts_reduced.geo");
  //	    AddModule(Fts);
  //	  }
}

void PndMasterRunCompactSim::AddEmcGeometry()
{
  BSEmcDetector *barrel = new BSEmcDetector("EmcBarrel", kTRUE);
  barrel->SetGeometryFileName("ReducedBarrelWithFrontPlate.root");
  BSEmcBarrelSensorNameIdMap *barrelMap = new BSEmcBarrelSensorNameIdMap();
  barrel->SetIdMap(barrelMap);
  barrel->SetSensitiveNames({"Crystal-"});
  barrel->SetBranchOutName(BSEmcDataBranchNames::fgMCPointBranchName + "BarrelSlices");
  barrel->SetFolderName("EmcBarrel");
  barrel->SetPersistency(kFALSE);
  {
    Double_t zshift = 0;
    TGeoRotation rotBarrel;
    rotBarrel.RotateZ(90);
    TGeoCombiTrans *TransRotBarrel = new TGeoCombiTrans(0., 0., zshift, new TGeoRotation(rotBarrel));
    barrel->SetTransformMatrix(TransRotBarrel);
  }
  AddModule(barrel);

  BSEmcDetector *sonderslices = new BSEmcDetector("EmcBarrelSonderslices", kTRUE);
  sonderslices->SetGeometryFileName("CompactPandaBarrelSonderslices.root");
  CmpPndBarrelSondersliceSensorNameIdMap *sonderslicesMap = new CmpPndBarrelSondersliceSensorNameIdMap();
  sonderslices->SetIdMap(sonderslicesMap);
  sonderslices->SetSensitiveNames({"vol_shape_Sonderslice_Crystal_"});
  sonderslices->SetBranchOutName(BSEmcDataBranchNames::fgMCPointBranchName + "BarrelSonderslices");
  sonderslices->SetFolderName("EmcBarrel");
  sonderslices->SetPersistency(kFALSE);
  {
    Double_t zshift = 0;
    TGeoRotation rotBarrel;
    // rotBarrel.RotateZ(90);
    TGeoCombiTrans *TransRotBarrel = new TGeoCombiTrans(0., 0., zshift, new TGeoRotation(rotBarrel));
    sonderslices->SetTransformMatrix(TransRotBarrel);
  }

  AddModule(sonderslices);

  //-------------------------   BwEndcap    -----------------
  LOG(info) << "Before adding BWEC";
  const Double_t BWECHalfZ = 21.9000000000;      // cm
  const Double_t distTargetBWEC = 56.0000000000; // cm
  TGeoCombiTrans *bwTransRotMatrix = new TGeoCombiTrans(0., 0., -(distTargetBWEC + BWECHalfZ),
                                                        new TGeoRotation()); // this is not the correct distance to the center.
  BSEmcDetector *bwendcap = new BSEmcDetector("EmcBwEndcap", kTRUE);
  bwendcap->SetGeometryFileName("ReducedBwEndcap.root");
  bwendcap->SetTransformMatrix(bwTransRotMatrix);
  bwendcap->SetSensitiveNames({"PWOCrystal"});
  BSEmcBwEndcapSensorNameIdMap *bwMap = new BSEmcBwEndcapSensorNameIdMap();
  bwendcap->SetIdMap(bwMap);
  bwendcap->SetBranchOutName(BSEmcDataBranchNames::fgMCPointBranchName + "BwEndcap");
  bwendcap->SetFolderName("EmcBwEndcap");
  bwendcap->SetPersistency(kTRUE);
  AddModule(bwendcap);
  LOG(info) << "Added BWEC";

  //-------------------------   FwEndcap    -----------------
  LOG(info) << "Before adding FWEC1";
  TGeoRotation rotVolume1;
  BSEmcDetector *fwendcap1 = new BSEmcDetector("EmcFwEndcap", kTRUE);
  fwendcap1->SetGeometryFileName("emc_module3_2012_new_EmcDetector.root"); // Geometry
  rotVolume1.RotateY(180.);
  PndSensorNameIdMap *fwMap1 = new BSEmcFwEndcapSensorNameIdMap(); // How do we want to decode them?
  fwendcap1->SetSensitiveNames({"CrystalVol"});                    // What are the sensitive parts
  fwendcap1->SetIdMap(fwMap1);
  TGeoCombiTrans *TransRotMatrix1 = new TGeoCombiTrans(0., 0., 213.9 + 60, new TGeoRotation(rotVolume1)); // distance of the FwEndCap
                                                                                                          // module to the target
                                                                                                          // point was obtained to be
                                                                                                          // around 2139 mm.}
  fwendcap1->SetTransformMatrix(TransRotMatrix1);                                                         // Where to put the geometry
  fwendcap1->SetBranchOutName(BSEmcDataBranchNames::fgMCPointBranchName + "FwEndcap");                    // Where do we want to store
                                                                                                          // the resultsíng
                                                                                                          // BSEmcMCPoints?
  fwendcap1->SetFolderName("EmcFwEndcap");
  fwendcap1->SetPersistency(kTRUE);
  AddModule(fwendcap1);
  LOG(info) << "Added FWEC1";

  //-------------------------   Shashlik    -----------------
  LOG(info) << "Before adding Shashlik";
  TGeoRotation rotVolumeSh;
  rotVolumeSh.RotateY(0);
  TGeoCombiTrans *TransRotMatrixSh = new TGeoCombiTrans(0., 0., 818.775,
                                                        new TGeoRotation(rotVolumeSh)); // distance of the FwEndCap module to the
                                                                                        // targetpoint was obtained to be around
                                                                                        // 2139 mm.
  BSEmcDetector *shashlik = new BSEmcShashlikDetector("EmcShashlik", kTRUE);
  shashlik->SetGeometryFileName("emc_module5_fsc_EmcDetector.root"); // Geometry
  shashlik->SetTransformMatrix(TransRotMatrixSh);                    // Where to put the geometry
  shashlik->SetSensitiveNames({"FscSciVolume"});                     // What are the sensitive
                                                                     // parts, "FscFiberVolume",
                                                                     // "FscModuleVolume",
  // "FscFiber", "FscLeadVolume"
  BSEmcShashlikSensorNameIdMap *shashlikMap = new BSEmcShashlikSensorNameIdMap(); // How do we want to decode them ?
  shashlik->SetIdMap(shashlikMap);
  shashlik->SetBranchOutName(BSEmcDataBranchNames::fgMCPointBranchName + "Shashlik"); // Where do we want to store the results?
  shashlik->SetFolderName("EmcShashlik");
  shashlik->SetPersistency(kFALSE);
  AddModule(shashlik);
  LOG(info) << "Added Shashlik";
  //---------------------------------------------------------
}

void PndMasterRunCompactSim::AddSimTasks()
{
  // -----   Emc Tasks ------
  TString vmcworkdir = gSystem->Getenv("VMCWORKDIR");
  PndProcessTask *general = new BSEmcMCDepositProducerTask("General");
  general->AddProcess(new BSEmcParameterLoadingProcess(TString{vmcworkdir + TString{"/macro/params/EmcCrystalsNeighbouringRelationsWithRedBarrel.txt"}}.Data()/*,
																			  TString{vmcworkdir + TString{"/macro/params/EmcCrystalPositions.txt"}}.Data()*/));
  AddTask(general);

  std::vector<TString> sourcebranches = {
    BSEmcDataBranchNames::fgMCPointBranchName + "BarrelSlices",
    BSEmcDataBranchNames::fgMCPointBranchName + "BarrelSonderslices",
  };

  BSEmcJoinBranches *branchJoiner = new BSEmcJoinBranches(sourcebranches, BSEmcDataBranchNames::fgMCPointBranchName + "Barrel");
  AddTask(branchJoiner);

  PndProcessTask *barrelHitProducer = new BSEmcMCDepositProducerTask("Barrel");
  {
    BSEmcMCDepositProductionProcess *barrelHitProducerProcess = new BSEmcBarrelZDependentMCDepositProductionProcess();
    barrelHitProducerProcess->SetVolume(new BSEmcVolumeDisk(54.2 - 11.4, 94 - 11.4, -90, 170));
    barrelHitProducer->AddProcess(barrelHitProducerProcess);
  }
  AddTask(barrelHitProducer);
  LOG(INFO) << "Added BarrelZDependentHitProducer(\"BSEmcHitParBarrel\")";

  PndProcessTask *bwDepositProducer = new BSEmcMCDepositProducerTask("BwEndcap");
  BSEmcMCDepositProductionProcess *bwDepositProducerProcess = new BSEmcMCDepositProductionProcess();
  bwDepositProducerProcess->SetVolume(new BSEmcVolumeDisk(17, 44, -98, -55));
  bwDepositProducer->AddProcess(bwDepositProducerProcess);
  AddTask(bwDepositProducer);
  LOG(info) << "Added BSEmcMCDepositProducerTask(\"BSEmcMCParBwEndcap\")";

  PndProcessTask *fwDepositProducer = new BSEmcMCDepositProducerTask("FwEndcap");
  BSEmcMCDepositProductionProcess *fwDepositProducerProcess = new BSEmcMCDepositProductionProcess();
  fwDepositProducerProcess->SetVolume(new BSEmcVolumeDisk(17, 101, 200, 230));
  fwDepositProducer->AddProcess(fwDepositProducerProcess);
  AddTask(fwDepositProducer);
  LOG(info) << "Added BSEmcMCDepositProducerTask(\"BSEmcMCParFwEndcap\")";

  PndProcessTask *shashlikDepositProducer = new BSEmcMCDepositProducerTask("Shashlik");
  BSEmcMCDepositProductionProcess *shashlikDepositProducerProcess = new BSEmcMCDepositProductionProcess();
  shashlikDepositProducerProcess->SetVolume(new BSEmcVolumeBox(-150, 150, -78, 78, 783, 853));
  shashlikDepositProducer->AddProcess(shashlikDepositProducerProcess);
  AddTask(shashlikDepositProducer);
  LOG(info) << "Added BSEmcMCDepositProducerTask(\"BSEmcMCParShashlik\")";

  // -----   Event Counter   --------------------------------
  AddTask(new PndEventCounterTask("Event Counter", fNEvents, fEventCounterRate));

  PndMasterSimTask *sim = new PndMasterSimTask(fOptions);
  AddTask(sim);

  return;
}

/** @cond CLASSIMP */
ClassImp(PndMasterRunCompactSim);
/** @endcond */
