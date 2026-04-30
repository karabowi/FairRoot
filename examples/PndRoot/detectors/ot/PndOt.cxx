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

////////////////////////////////////////////////////////////////////////////
// PndOt source file
//
// Class for simulation of OT
//
// authors: Radoslaw Karabowicz, GSI, 2024
//
// modified from PndOt by Nafija Ibrišimović in 2023
////////////////////////////////////////////////////////////////////////////

#include "PndOt.h"
// from PandaRoot, this library
#include "PndGeoOtPar.h"
#include "PndGeoOt.h"
#include "PndDetectorList.h"
#include "PndStack.h"
#include "PndOtMapCreator.h"
// from FairRoot
#include <FairRun.h>
#include <FairGeoInterface.h>
#include <FairGeoLoader.h>
#include <FairGeoNode.h>
#include <FairGeoRootBuilder.h>
#include <FairRootManager.h>
#include <FairRuntimeDb.h>
#include <FairVolume.h>
#include <FairLogger.h>
// from ROOT
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TParticle.h>
#include <TVirtualMC.h>
#include <TGeoMatrix.h>
#include <TObjArray.h>
#include <TGeoTube.h>
#include <TGeoMedium.h>
#include <TGeoVolume.h>
// standard
#include <iostream>

using std::string;

// TODO: read this from geant initialization
#define innerStrawDiameter 1.
// #define redefineLambdaChargedDecay 0

// -----   Default constructor   -------------------------------------------
PndOt::PndOt()
  : fTrackID(0), fVolumeID(0), fPos(0, 0, 0, 0), fPosIn(0, 0, 0, 0), fPosOut(0, 0, 0, 0), fPosInLocal(0, 0, 0, 0), fPosOutLocal(0, 0, 0, 0), fMomIn(0, 0, 0, 0),
    fMomOut(0, 0, 0, 0), fTime(0), fLength(0), fELoss(0), fMass(0), fIsInitialized(kFALSE), fPosIndex(0), fOtCollection(nullptr), fpostot(0, 0, 0, 0), fpostotin(0, 0, 0, 0),
    fpostotout(0, 0, 0, 0), fPassNodes(), valid(kFALSE), fGeoType(0)
{
  fOtCollection = new TClonesArray("PndOtPoint");
  fVerboseLevel = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndOt::PndOt(const char *name, Bool_t active)
  : FairDetector(name, active), fTrackID(0), fVolumeID(0), fPos(0, 0, 0, 0), fPosIn(0, 0, 0, 0), fPosOut(0, 0, 0, 0), fPosInLocal(0, 0, 0, 0), fPosOutLocal(0, 0, 0, 0),
    fMomIn(0, 0, 0, 0), fMomOut(0, 0, 0, 0), fTime(0), fLength(0), fELoss(0), fMass(0), fIsInitialized(kFALSE), fPosIndex(0), fOtCollection(nullptr), fpostot(0, 0, 0, 0),
    fpostotin(0, 0, 0, 0), fpostotout(0, 0, 0, 0), fPassNodes(), valid(kFALSE), fGeoType(0)
{
  fOtCollection = new TClonesArray("PndOtPoint");
  fVerboseLevel = 0;
  fGeoType = 1; // CHECK
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndOt::~PndOt()
{
  if (fOtCollection) {
    fOtCollection->Delete();
    delete fOtCollection;
  }
}
// -------------------------------------------------------------------------

// -----   Private method GetSquaredDistanceFromWire -----------------------
float PndOt::GetSquaredDistanceFromWire()
{
  TLorentzVector entryPosition;

  float positionInMother[3], positionInStraw[3];

  gMC->TrackPosition(entryPosition);
  positionInMother[0] = entryPosition.X();
  positionInMother[1] = entryPosition.Y();
  positionInMother[2] = entryPosition.Z();
  gMC->Gmtod(positionInMother, positionInStraw, 1);

  return positionInStraw[0] * positionInStraw[0] + positionInStraw[1] * positionInStraw[1];
}
// -------------------------------------------------------------------------

bool PndOt::Split(string &aDest, string &aSrc, char aDelim)
{
  if (aSrc.empty())
    return false;

  string::size_type pos = aSrc.find(aDelim);

  aDest = aSrc.substr(0, pos);

  if (pos != string::npos)
    aSrc = aSrc.substr(pos + 1);
  else
    aSrc = "";

  return true;
}

string PndOt::GetStringPart(string &aSrc, Int_t part, char aDelim)
{
  string retval = "", sub;

  int counter = 0;

  while (Split(sub, aSrc, aDelim)) {
    if (counter == part) {
      retval = sub;
      break;
    }
    counter++;
  }

  return retval;
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t PndOt::ProcessHits(FairVolume *vol)
{

  // TParticle* particle =  gMC->GetStack()->GetCurrentTrack(); //[R.K. 01/2017] unused variable?
  // TGeoMedium *medium = (TGeoMedium*) vol->getGeoNode()->getRootVolume()->GetMedium(); //[R.K. 01/2017] unused variable?
  // Double_t epsil = medium->GetParam(6); //[R.K. 01/2017] unused variable?

  TString vol_name(gMC->CurrentVolName());
  TGeoHMatrix M;
  gMC->GetTransformation(gMC->CurrentVolPath(), M);
  TString name(gMC->CurrentVolName());

  if (gMC->TrackCharge() != 0.) {

    if (gMC->IsTrackEntering()) {
      valid = kTRUE;

      // Set parameters at entrance of volume. Reset ELoss.
      fELoss = 0.;
      fTime = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMomIn);
      gMC->TrackPosition(fpostotin);        // da cancellare
      Double_t globalPos[3] = {0., 0., 0.}; // stt1 modified
      Double_t localPos[3] = {0., 0., 0.};  // stt1 modified

      globalPos[0] = fPos.X();
      globalPos[1] = fPos.Y();
      globalPos[2] = fPos.Z();

      gMC->Gmtod(globalPos, localPos, 1);
      fPosInLocal.SetXYZM(localPos[0], localPos[1], localPos[2], 0.0);
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Create PndOtPoint at exit of active volume -- but not into the wire
    if (gMC->IsTrackExiting() && valid == kTRUE) {
      valid = kFALSE;
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      fVolumeID = static_cast<int>(DetectorId::kOT); // vol->getMCid();

      if (fTrackID != 0) {
        if (fVerboseLevel > 2)
          LOG(info) << "[OT] test Vol----------" << vol->getMCid();
        if (fVerboseLevel > 2)
          LOG(info) << "fTrackID-----" << fTrackID;
      }

      fMass = gMC->TrackMass(); // mass (GeV)
      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);
      gMC->TrackPosition(fpostotout);       // da cancellare
      Double_t globalPos[3] = {0., 0., 0.}; // stt1 modified
      Double_t localPos[3] = {0., 0., 0.};  // stt1 modified

      gMC->Gdtom(localPos, globalPos, 1);

      fPos.SetXYZM(globalPos[0], globalPos[1], globalPos[2], 0.0);

      globalPos[0] = fPosOut.X();
      globalPos[1] = fPosOut.Y();
      globalPos[2] = fPosOut.Z();

      gMC->Gmtod(globalPos, localPos, 1);
      fPosOutLocal.SetXYZM(localPos[0], localPos[1], localPos[2], 0.0);

      // string basename("stt1tube");
      string basename;
      TString volumename;
      string hashmark("#"), volName, fullName, number, specialname, volPath(gMC->CurrentVolPath()), volPath2(gMC->CurrentVolPath());

      volumename = volPath;
      volName = GetStringPart(volPath, 2, '/');
      number = GetStringPart(volName, 1, '_');

      if (fTrackID != 0) {
        if (fVerboseLevel > 2)
          LOG(info) << "[OT] befor Mapper------";
      }

      fpostot.SetXYZM((fpostotin.X() + fpostotout.X()) / 2., (fpostotin.Y() + fpostotout.Y()) / 2., (fpostotin.Z() + fpostotout.Z()) / 2., 0.0);

      // CHECK map creator-------------------------------------------------
      // PndOtMapCreator *mapper = new PndOtMapCreator(fGeoType);
      // testTubeID=tube number as in the geometry file....
      // we need to calculate the real number in order to have a different
      // number for each tube (also for up and down short tubes)
      //      Int_t tubeID    = fMapper->GetTubeIDFromPath(gMC->CurrentVolPath());
      Int_t chamberID = fMapper->GetChamberIDFromPath(gMC->CurrentVolPath());
      Int_t layerID = fMapper->GetLayerID(chamberID, gMC->CurrentVolPath());
      // LOG(info)<<"LAYERID==========="<<layerID;
      // tubeID=at each tube corresponds only one id number.....
      Int_t totTubeID = fMapper->GetTubeIDTot(chamberID, layerID, gMC->CurrentVolPath());
      // LOG(info)<<gMC->CurrentVolPath() << " -> " << "PndOt.cxx. layer + chamber + tubeID -> totTubeId = "<<layerID<<" + "<<chamberID<<" + "<<tubeID<<" -> "<<totTubeID;
      ////----------------------------------------------------------------

      AddHit(fTrackID, fVolumeID, totTubeID, chamberID, layerID, TVector3(fpostot.X(), fpostot.Y(), fpostot.Z()), TVector3(fPosInLocal.X(), fPosInLocal.Y(), fPosInLocal.Z()),
             TVector3(fPosOutLocal.X(), fPosOutLocal.Y(), fPosOutLocal.Z()), TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()), TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
             fTime, fLength, fELoss, fMass);

      if (fTrackID != 0) {
        if (fVerboseLevel > 2)
          LOG(info) << "AddHit PndOt.cxx= " << fTrackID;
      }

      // Increment number of stt points for TParticle
      PndStack *stack = (PndStack *)gMC->GetStack();
      stack->AddPoint(DetectorId::kOT);
      ResetParameters();
    }
  }

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Public method EndOfEvent   --------------------------------------
void PndOt::EndOfEvent()
{
  if (fVerboseLevel)
    Print();
  fOtCollection->Delete();
  fPosIndex = 0;
}
// -------------------------------------------------------------------------

// -----   Public method Register   ----------------------------------------
void PndOt::Register()
{
  FairRootManager::Instance()->Register("OTPoint", "Ot", fOtCollection, kTRUE);
}
// -------------------------------------------------------------------------

// -----   Public method GetCollection   -----------------------------------
TClonesArray *PndOt::GetCollection(Int_t iColl) const
{
  if (iColl == 0)
    return fOtCollection;
  else
    return nullptr;
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndOt::Print(Option_t *) const
{
  Int_t nHits = fOtCollection->GetEntriesFast();

  LOG(info) << " PndOt: " << nHits << " points registered in this event.";

  if (fVerboseLevel > 1)
    for (Int_t i = 0; i < nHits; i++)
      (*fOtCollection)[i]->Print();
}
// -------------------------------------------------------------------------

// -----   Public method Reset   -------------------------------------------
void PndOt::Reset()
{
  fOtCollection->Delete();
  ResetParameters();
}
// -------------------------------------------------------------------------

// -----   Public method CopyClones   --------------------------------------
void PndOt::CopyClones(TClonesArray *cl1, TClonesArray *cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();

  LOG(info) << " PndOt: " << nEntries << " entries to add.";

  TClonesArray &clref = *cl2;

  PndOtPoint *oldpoint = nullptr;
  for (Int_t i = 0; i < nEntries; i++) {
    oldpoint = (PndOtPoint *)cl1->At(i);

    Int_t index = oldpoint->GetTrackID() + offset;

    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndOtPoint(*oldpoint);
    fPosIndex++;
  }
  LOG(info) << " PndOt: " << cl2->GetEntriesFast() << " merged entries.";
}
// -------------------------------------------------------------------------

void PndOt::Initialize()
{
  LOG(info) << " -I- Initializing PndOt()";
  FairDetector::Initialize();
}

// -----   Public method ConstructGeometry   -------------------------------
void PndOt::ConstructGeometry()
{
  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  PndGeoOt *Geo = new PndGeoOt();
  Geo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(Geo);

  Bool_t rc = geoFace->readSet(Geo);
  if (rc)
    Geo->create(geoLoad->getGeoBuilder());

  TList *volList = Geo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
  PndGeoOtPar *par;
  par = (PndGeoOtPar *)(rtdb->getContainer("PndGeoOtPar"));
  rtdb->getListOfContainers()->Print();
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode *node = nullptr;
  FairGeoVolume *aVol = nullptr;

  while ((node = (FairGeoNode *)iter.Next())) {
    aVol = dynamic_cast<FairGeoVolume *>(node);
    // if (fGeoType == 2)
    //  LOG(info) << "Volume " << aVol->GetName() << " is" << (node->isSensitive() ? "" : " not") << " sensitive";
    if (node->isSensitive()) {
      fSensNodes->AddLast(aVol);
    } else {
      fPassNodes->AddLast(aVol);
    }
  }

  par->SetGeometryType(fGeoType);
  if (fGeoType == 1) {
    par->SetTubeInRad(0.4903 / 2.); // cm
    par->SetTubeOutRad(0.00127);    // cm
  } else {
    LOG(fatal) << "[OT] GeoType " << fGeoType << " not supported";
  }

  par->setChanged();
  par->setInputVersion(fRun->GetRunId(), 1);
  ProcessNodes(volList);

  fMapper = new PndOtMapCreator(fGeoType);
}
// -------------------------------------------------------------------------

// -----   Private method AddHit   -----------------------------------------
PndOtPoint *PndOt::AddHit(Int_t trackID, Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, TVector3 pos, TVector3 posInLocal, TVector3 posOutLocal, TVector3 momIn,
                          TVector3 momOut, Double_t time, Double_t length, Double_t eLoss, Double_t mass) // da cancellare postot
{
  TClonesArray &clref = *fOtCollection;

  Int_t size = clref.GetEntriesFast();

  PndOtPoint *pointnew = new (clref[size]) PndOtPoint(trackID, detID, tubeID, chamberID, layerID, pos, posInLocal, posOutLocal, momIn, momOut, time, length, eLoss, mass);

  pointnew->SetTubeID(tubeID);
  pointnew->SetChamberID(chamberID);
  pointnew->SetLayerID(layerID);

  return pointnew;
}
// -------------------------------------------------------------------------

ClassImp(PndOt)
