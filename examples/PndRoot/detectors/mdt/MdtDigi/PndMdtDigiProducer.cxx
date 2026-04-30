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
// -----                PndMdtDigiProducer source file                  -----
// -----                  Created 29/03/11  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMdtDigiProducer.h"

#include "PndMdtDigi.h"
#include "PndMdtPoint.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
#include "TRandom.h"

#include <iostream>

using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndMdtDigiProducer::PndMdtDigiProducer() : FairTask(" MDT Digi Producer")
{
  fStripMode = kTRUE;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtDigiProducer::~PndMdtDigiProducer() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndMdtDigiProducer::Init()
{

  cout << "-I- PndMdtDigiProducer::Init: "
       << "INITIALIZATION *********************" << endl;

  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndMdtDigiProducer::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("MdtPoint");
  if (!fPointArray) {
    cout << "-W- PndMdtDigiProducer::Init: "
         << "No MdtPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fDigiBoxArray = new TClonesArray("PndMdtDigi");
  ioman->Register("MdtDigiBox", "Mdt", fDigiBoxArray, kTRUE);

  if (fStripMode) {
    fDigiStripArray = new TClonesArray("PndMdtDigi");
    ioman->Register("MdtDigiStrip", "Mdt", fDigiStripArray, kTRUE);
  }

  TGeoVolume *volBarrel = (TGeoVolume *)gGeoManager->FindVolumeFast("MdtBarrel");
  TGeoBBox *boxBarrel = (TGeoBBox *)volBarrel->GetShape();
  const Double_t *orBarrel = boxBarrel->GetOrigin();
  fBarrelStart = orBarrel[2] + boxBarrel->GetDZ();
  TGeoVolume *volEndcap = (TGeoVolume *)gGeoManager->FindVolumeFast("MdtEndcap");
  TGeoBBox *boxEndcap = (TGeoBBox *)volEndcap->GetShape();
  const Double_t *orEndcap = boxEndcap->GetOrigin();
  fEndcapStart = orEndcap[0] + boxEndcap->GetDX();
  TGeoVolume *volMF = (TGeoVolume *)gGeoManager->FindVolumeFast("MdtMF");
  TGeoBBox *boxMF = (TGeoBBox *)volMF->GetShape();
  const Double_t *orMF = boxMF->GetOrigin();
  fMFStart = orMF[0] + boxMF->GetDX();

  LOG(info) << " PndMdtDigiProducer: Intialization successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMdtDigiProducer::Exec(Option_t *)
{
  // Reset output array
  fDigiBoxArray->Delete();
  if (fStripMode)
    fDigiStripArray->Delete();

  map<Int_t, std::vector<Int_t>> mapStripPoint; // PointIds with same DetId - strip
  map<Int_t, std::vector<Int_t>> mapBoxPoint;   // PointIds with same DetId - box
  map<Int_t, TVector3> mapStripPos;             // Position for strip
  map<Int_t, TVector3> mapBoxPos;               // Position for box

  mapStripPoint.clear();
  mapBoxPoint.clear();
  mapStripPos.clear();
  mapBoxPos.clear();

  TVector3 mdtSize(0.5, 0.5, 0.5);

  // Loop over MdtPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  PndMdtPoint *point = 0;
  TVector3 inPos, outPos, meanPos;

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndMdtPoint *)fPointArray->At(iPoint);
    if (point->GetEnergyLoss() == 0)
      continue;

    point->Position(inPos);
    outPos = point->GetPosIn();

    if ((inPos - outPos).Mag() < 0.2)
      continue; // skipped points

    meanPos = 0.5 * (inPos + outPos);
    gGeoManager->FindNode(meanPos.X(), meanPos.Y(), meanPos.Z()); // TGeoNode *mdtNode = (TGeoNode*) //[R.K.03/2017] unused variable
    TGeoMatrix *mdtMat = (TGeoMatrix *)gGeoManager->GetCurrentMatrix();
    const Double_t *matM = mdtMat->GetTranslation();
    TVector3 tubePos(matM[0], matM[1], matM[2]);
    TVector3 stripPos1, stripPos2;
    Int_t stripNum1 = -10, stripNum2 = -10, stripId1 = -10, stripId2 = -10;
    if (point->GetModule() == 1) {
      stripNum1 = (Int_t)(fBarrelStart - point->GetZ() - 0.25);
      stripNum2 = (Int_t)(fBarrelStart - point->GetZ() + 0.25);
      if (stripNum1 < 0)
        stripNum1 = 0;
      if (stripNum2 < 0)
        stripNum2 = 0;
      stripPos1.SetXYZ(matM[0], matM[1], fBarrelStart - stripNum1 - 0.5);
      stripPos2.SetXYZ(matM[0], matM[1], fBarrelStart - stripNum2 - 0.5);
    }
    if (point->GetModule() == 2) {
      stripNum1 = (Int_t)(fEndcapStart - point->GetX() - 0.25);
      stripNum2 = (Int_t)(fEndcapStart - point->GetX() + 0.25);
      if (stripNum1 < 0)
        stripNum1 = 0;
      if (stripNum2 < 0)
        stripNum2 = 0;
      stripPos1.SetXYZ(fEndcapStart - stripNum1 - 0.5, matM[1], matM[2]);
      stripPos2.SetXYZ(fEndcapStart - stripNum2 - 0.5, matM[1], matM[2]);
    }
    if (point->GetModule() == 3) {
      stripNum1 = (Int_t)(fMFStart - point->GetX() - 0.25);
      stripNum2 = (Int_t)(fMFStart - point->GetX() + 0.25);
      if (stripNum1 < 0)
        stripNum1 = 0;
      if (stripNum2 < 0)
        stripNum2 = 0;
      stripPos1.SetXYZ(fMFStart - stripNum1 - 0.5, matM[1], matM[2]);
      stripPos2.SetXYZ(fMFStart - stripNum2 - 0.5, matM[1], matM[2]);
    }

    stripId1 = stripNum1 + 1000 * point->GetLayerID() + 100000 * point->GetSector() + 1000000 * point->GetModule();
    stripId2 = stripNum2 + 1000 * point->GetLayerID() + 100000 * point->GetSector() + 1000000 * point->GetModule();

    if (fStripMode) {
      mapBoxPoint[point->GetDetectorID()].push_back(iPoint);
      mapBoxPos[point->GetDetectorID()] = tubePos;
      mapStripPoint[stripId1].push_back(iPoint);
      mapStripPos[stripId1] = stripPos1;
      if (stripNum1 != stripNum2) {
        mapStripPoint[stripId2].push_back(iPoint);
        mapStripPos[stripId2] = stripPos2;
      }
    } else {
      mapBoxPoint[point->GetDetectorID()].push_back(iPoint);
      mapBoxPos[point->GetDetectorID()] = stripPos1;
    }

  } // Loop over MdtPoints

  map<Int_t, std::vector<Int_t>>::const_iterator boxIter;
  for (boxIter = mapBoxPoint.begin(); boxIter != mapBoxPoint.end(); ++boxIter) {
    AddDigiBox((*boxIter).first, mapBoxPos[(*boxIter).first], (*boxIter).second);
  }
  map<Int_t, std::vector<Int_t>>::const_iterator stripIter;
  for (stripIter = mapStripPoint.begin(); stripIter != mapStripPoint.end(); ++stripIter) {
    AddDigiStrip((*stripIter).first, mapStripPos[(*stripIter).first], (*stripIter).second);
  }
}
// -------------------------------------------------------------------------

// -----   Private method AddDigi   --------------------------------------------
PndMdtDigi *PndMdtDigiProducer::AddDigiBox(Int_t detID, TVector3 &pos, std::vector<Int_t> pointList)
{
  // It fills the PndMdtDigi category
  TClonesArray &clref = *fDigiBoxArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndMdtDigi(detID, pos, pointList);
}
// ----

// -----   Private method AddDigi   --------------------------------------------
PndMdtDigi *PndMdtDigiProducer::AddDigiStrip(Int_t detID, TVector3 &pos, std::vector<Int_t> pointList)
{
  // It fills the PndMdtDigi category

  TClonesArray &clref = *fDigiStripArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndMdtDigi(detID, pos, pointList);
}
// ----

ClassImp(PndMdtDigiProducer)
