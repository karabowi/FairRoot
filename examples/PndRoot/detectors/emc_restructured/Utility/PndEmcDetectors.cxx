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


#include "PndEmcDetectors.h"

#include "TGeoMatrix.h"

#include "FairRunSim.h"

#include "PndSensorNameIdMap.h"

#include "BSEmcBarrelSensorNameIdMap.h"
#include "BSEmcBwEndcapSensorNameIdMap.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcDetector.h"
#include "BSEmcFwEndcapSensorNameIdMap.h"
#include "BSEmcShashlikDetector.h"
#include "BSEmcShashlikSensorNameIdMap.h"

//---------------------------------------------------------------

BSEmcBarrel::BSEmcBarrel(Bool_t t_persistency) : BSEmcDetector("EmcBarrel", kTRUE)
{
  this->SetGeometryFileName("emc_module12_2018v1_EmcDetector.root");
  BSEmcBarrelSensorNameIdMap *barrelMap = new BSEmcBarrelSensorNameIdMap();
  this->SetIdMap(barrelMap);
  this->SetSensitiveNames({"Crystal-"});
  this->SetBranchOutName(BSEmcDataBranchNames::fgMCPointBranchName + "Barrel");
  this->SetFolderName("EmcBarrel");
  this->SetPersistency(t_persistency);
}

//---------------------------------------------------------------
BSEmcBwEndcap::BSEmcBwEndcap(Bool_t t_persistency) : BSEmcDetector("EmcBwEndcap", kTRUE)
{
  const Double_t BWECHalfZ = 21.9000000000;      // cm
  const Double_t distTargetBWEC = 56.0000000000; // cm
  TGeoCombiTrans *bwTransRotMatrix = new TGeoCombiTrans(0., 0., -(distTargetBWEC + BWECHalfZ),
                                                        new TGeoRotation()); // this is not the correct distance to the center.
  this->SetGeometryFileName("emc_module4_2017.root");
  this->SetTransformMatrix(bwTransRotMatrix);
  this->SetSensitiveNames({"PWOCrystal"});
  BSEmcBwEndcapSensorNameIdMap *bwMap = new BSEmcBwEndcapSensorNameIdMap();
  this->SetIdMap(bwMap);
  this->SetBranchOutName(BSEmcDataBranchNames::fgMCPointBranchName + "BwEndcap");
  this->SetFolderName("EmcBwEndcap");
  this->SetPersistency(t_persistency);
}

//---------------------------------------------------------------

BSEmcFwEndcap::BSEmcFwEndcap(Bool_t t_persistency) : BSEmcDetector("EmcFwEndcap", kTRUE)
{
  TGeoRotation rotVolume1;
  this->SetGeometryFileName("emc_module3_2012_new_EmcDetector.root"); // Geometry
  rotVolume1.RotateY(180.);
  PndSensorNameIdMap *fwMap1 = new BSEmcFwEndcapSensorNameIdMap(); // How do we want to decode them?
  this->SetSensitiveNames({"CrystalVol"});                         // What are the sensitive parts
  this->SetIdMap(fwMap1);
  TGeoCombiTrans *TransRotMatrix1 = new TGeoCombiTrans(0., 0., 213.9, new TGeoRotation(rotVolume1)); // distance of the FwEndCap
                                                                                                     // module to the target
                                                                                                     // point was obtained to be
                                                                                                     // around 2139 mm.}
  this->SetTransformMatrix(TransRotMatrix1);                                                         // Where to put the geometry
  this->SetBranchOutName(BSEmcDataBranchNames::fgMCPointBranchName + "FwEndcap");                    // Where do we want to store
                                                                                                     // the resultsíng
                                                                                                     // BSEmcMCPoints?
  this->SetFolderName("EmcFwEndcap");
  this->SetPersistency(t_persistency);
}

BSEmcShashlik::BSEmcShashlik(Bool_t t_persistency) : BSEmcShashlikDetector("EmcShashlik", kTRUE)
{
  TGeoRotation rotVolumeSh;
  rotVolumeSh.RotateY(0);
  TGeoCombiTrans *TransRotMatrixSh = new TGeoCombiTrans(0., 0., 818.775,
                                                        new TGeoRotation(rotVolumeSh)); // distance of the FwEndCap module to the
                                                                                        // targetpoint was obtained to be around
                                                                                        // 2139 mm.

  this->SetGeometryFileName("emc_module5_fsc_EmcDetector.root"); // Geometry
  this->SetTransformMatrix(TransRotMatrixSh);                    // Where to put the geometry
  this->SetSensitiveNames({"FscSciVolume"});                     // What are the sensitive
                                                                 // parts, "FscFiberVolume",
                                                                 // "FscModuleVolume",
  // "FscFiber", "FscLeadVolume"
  BSEmcShashlikSensorNameIdMap *shashlikMap = new BSEmcShashlikSensorNameIdMap(); // How do we want to decode them ?
  this->SetIdMap(shashlikMap);
  this->SetBranchOutName(BSEmcDataBranchNames::fgMCPointBranchName + "Shashlik"); // Where do we want to store the results?
  this->SetFolderName("EmcShashlik");
  this->SetPersistency(t_persistency);
}

void PndEmcDetectors::AddEmcSetup(FairRunSim *t_run, Bool_t t_persistency)
{
  t_run->AddModule(new BSEmcBarrel(t_persistency));
  t_run->AddModule(new BSEmcBwEndcap(t_persistency));
  t_run->AddModule(new BSEmcFwEndcap(t_persistency));
  t_run->AddModule(new BSEmcShashlik(t_persistency));
}
