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
// -----                       PndStt source file                      -----
// -------------------------------------------------------------------------

#include "PndStt.h"

#include "PndGeoSttPar.h"
#include "PndGeoStt.h"
#include "PndSttPoint.h"
#include "PndDetectorList.h"
#include "PndStack.h"
#include "PndSttMapCreator.h"
#include "PndGeoHandling.h"

#include "FairRun.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TObjArray.h"
#include "TGeoTube.h"
#include "TGeoMedium.h"
#include "TGeoVolume.h"

#include <iostream>
#include <math.h>

using std::cout;
using std::endl;
using std::string;

// TODO: read this from geant initialization
#define innerStrawDiameter 1.
//#define redefineLambdaChargedDecay 0

// -----   Default constructor   -------------------------------------------
PndStt::PndStt()
  : fTrackID(0), fVolumeID(0), fPosInLocal(0, 0, 0), fPosOutLocal(0, 0, 0), fMomIn(0, 0, 0), fMomOut(0, 0, 0), fTime(0), fLength(0), fELoss(0), fMass(0), fIsInitialized(kFALSE),
    fPosIndex(0), fSttCollection(nullptr), fpostot(0, 0, 0), fpostotin(0, 0, 0), fpostotout(0, 0, 0), fPassNodes(new TObjArray()), fGeoType(0), fInFlag(0)
{
  fSttCollection = new TClonesArray("PndSttPoint");
  fVerboseLevel = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndStt::PndStt(const char *name, Bool_t active)
  : FairDetector(name, active), fTrackID(0), fVolumeID(0), fPosInLocal(0, 0, 0), fPosOutLocal(0, 0, 0), fMomIn(0, 0, 0), fMomOut(0, 0, 0), fTime(0), fLength(0), fELoss(0),
    fMass(0), fIsInitialized(kFALSE), fPosIndex(0), fSttCollection(nullptr), fpostot(0, 0, 0), fpostotin(0, 0, 0), fpostotout(0, 0, 0), fPassNodes(new TObjArray()), fGeoType(0),
    fInFlag(0)
{
  fSttCollection = new TClonesArray("PndSttPoint");
  fVerboseLevel = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndStt::~PndStt()
{
  if (fSttCollection) {
    fSttCollection->Delete();
    delete fSttCollection;
  }
}
// -------------------------------------------------------------------------

void PndStt::Initialize()
{
  LOG(info) << " Initializing PndStt()";

  FairDetector::Initialize();
  if (0 == gGeoManager) {
    std::cout << " -E- No gGeoManager in PndStt::Initialize()! aborting" << std::endl;
    abort();
  }
}

// -----   Private method GetSquaredDistanceFromWire -----------------------
float PndStt::GetSquaredDistanceFromWire()
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

bool PndStt::Split(string &aDest, string &aSrc, char aDelim)
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

string PndStt::GetStringPart(string &aSrc, Int_t part, char aDelim)
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
Bool_t PndStt::ProcessHits(FairVolume *vol)
{
  TString vol_name(gMC->CurrentVolName());
  TGeoHMatrix M;
  gMC->GetTransformation(gMC->CurrentVolPath(), M);
  TString name(gMC->CurrentVolName());

  Double_t globalPos[3] = {0., 0., 0.};
  Double_t localPos[3] = {0., 0., 0.};
  TLorentzVector mom;

  if (gMC->TrackCharge() != 0.) {
    if (gMC->IsTrackEntering()) // not sure why this was here:  && fabs(sqrt(GetSquaredDistanceFromWire()) - (innerStrawDiameter / 2.)) < epsil)
    {
      fInFlag = kTRUE;
      // Set parameters at entrance of volume. Reset ELoss.
      fELoss = 0.;
      fTime = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      gMC->TrackPosition(globalPos[0], globalPos[1], globalPos[2]);
      gMC->TrackMomentum(mom);

      fpostotin.SetXYZ(globalPos[0], globalPos[1], globalPos[2]);
      fMomIn.SetXYZ(mom.X(), mom.Y(), mom.Z());

      gMC->Gmtod(globalPos, localPos, 1);
      fPosInLocal.SetXYZ(localPos[0], localPos[1], localPos[2]);
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Create PndSttPoint at exit of active volume -- but not into the wire -- with eloss in the tube (to make it work with TGeant4)
    if ((gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) &&
        fInFlag == kTRUE // not sure why this was here: fabs(sqrt(GetSquaredDistanceFromWire()) - (innerStrawDiameter / 2.)) < epsil
        && fELoss != 0) {
      fInFlag = kFALSE;
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      fVolumeID = static_cast<int>(PndDetectorId::kSTT); // vol->getMCid();
      fMass = gMC->TrackMass();                       // mass (GeV)
      gMC->TrackPosition(globalPos[0], globalPos[1], globalPos[2]);
      gMC->TrackMomentum(mom);

      fpostotout.SetXYZ(globalPos[0], globalPos[1], globalPos[2]);
      fMomOut.SetXYZ(mom.X(), mom.Y(), mom.Z());

      gMC->Gmtod(globalPos, localPos, 1);
      fPosOutLocal.SetXYZ(localPos[0], localPos[1], localPos[2]);

      fpostot.SetXYZ((fpostotin.X() + fpostotout.X()) / 2., (fpostotin.Y() + fpostotout.Y()) / 2.,
                     (fpostotin.Z() + fpostotout.Z()) / 2.); // CHECK (delete this?)

      Int_t tubeID = -1;

      // CHECK -----------------------------------------------------------
      if (fGeoType == 1) {
        PndSttMapCreator mapper(fGeoType);
        tubeID = mapper.GetTubeIDFromPath(gMC->CurrentVolPath());
      } else if (fGeoType == 2) {
        tubeID = PndGeoHandling::Instance()->GetShortID(gMC->CurrentVolPath());
      }
      // -----------------------------------------------------------------

      //            std::cout << "SttPoint: " << gMC->CurrentVolPath()  << std::endl;
      //            std::cout << "PosIn: " << fpostotin.X() << "/" << fpostotin.Y() << "/" << fpostotin.Z() << std::endl;
      //            std::cout << "PosOut: " << fpostotout.X() << "/" << fpostotout.Y() << "/" << fpostotout.Z() << std::endl;
      //            std::cout << "PosToT: " << fpostot.X() << "/" << fpostot.Y() << "/" << fpostot.Z() << std::endl;
      //            std::cout << "PosInLoc: " << fPosInLocal.X() << "/" << fPosInLocal.Y() << "/" << fPosInLocal.Z() << std::endl;
      //            std::cout << "PosOutLoc: " << fPosOutLocal.X() << "/" << fPosOutLocal.Y() << "/" << fPosOutLocal.Z() << std::endl;

      AddHit(fTrackID, fVolumeID, tubeID, fpostot, fPosInLocal, fPosOutLocal, fMomIn, fMomOut, fTime, fLength, fELoss, fMass);

      // Increment number of stt points for TParticle
      PndStack *stack = (PndStack *)gMC->GetStack();
      stack->AddPoint(PndDetectorId::kSTT);
      ResetParameters();
    }
  }

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Public method EndOfEvent   --------------------------------------
void PndStt::EndOfEvent()
{
  if (fVerboseLevel)
    Print();
  fSttCollection->Delete();
  fPosIndex = 0;
}
// -------------------------------------------------------------------------

// -----   Public method Register   ----------------------------------------
void PndStt::Register()
{
  FairRootManager::Instance()->Register("STTPoint", "Stt", fSttCollection, kTRUE);
}
// -------------------------------------------------------------------------

// -----   Public method GetCollection   -----------------------------------
TClonesArray *PndStt::GetCollection(Int_t iColl) const
{
  if (iColl == 0)
    return fSttCollection;
  else
    return nullptr;
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndStt::Print() const
{
  Int_t nHits = fSttCollection->GetEntriesFast();

  LOG(info) << " PndStt: " << nHits << " points registered in this event.";

  if (fVerboseLevel > 1)
    for (Int_t i = 0; i < nHits; i++)
      (*fSttCollection)[i]->Print();
}
// -------------------------------------------------------------------------

// -----   Public method Reset   -------------------------------------------
void PndStt::Reset()
{
  fSttCollection->Delete();
  ResetParameters();
}
// -------------------------------------------------------------------------

// -----   Public method CopyClones   --------------------------------------
void PndStt::CopyClones(TClonesArray *cl1, TClonesArray *cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();

  LOG(info) << " PndStt: " << nEntries << " entries to add.";

  TClonesArray &clref = *cl2;

  PndSttPoint *oldpoint = nullptr;
  for (Int_t i = 0; i < nEntries; i++) {
    oldpoint = (PndSttPoint *)cl1->At(i);

    Int_t index = oldpoint->GetTrackID() + offset;

    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndSttPoint(*oldpoint);
    fPosIndex++;
  }
  LOG(info) << " PndStt: " << cl2->GetEntriesFast() << " merged entries.";
}
// -------------------------------------------------------------------------

// -----   Public method ConstructGeometry   -------------------------------
void PndStt::ConstructGeometry()
{
  LOG(info) << "ConstructGeometry called" << std::endl;
  TString fileName = GetGeometryFileName();
  if (fileName.EndsWith(".root")) {
    fGeoType = 2;
  } else if (fileName.Contains(".geo")) {
    fGeoType = 1;
  } else {
    LOG(error) << " STT: this geometry is not supported now";
    return;
  }

  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
  PndGeoSttPar *par = (PndGeoSttPar *)(rtdb->getContainer("PndGeoSttPar"));
  PndSttMapCreator mapper(fGeoType);
  int tubecounter = 0;

  if (fGeoType == 2) {
    // Set what is sensitive before creating geometry
    if (fListOfSensitives.size() == 0)
      SetDefaultSensorNames();
    ConstructRootGeometry();
    PndGeoHandling::Instance()->CreateUniqueSensorId("", fListOfSensitives);
    // if(fVerboseLevel>0)
    PndGeoHandling::Instance()->PrintSensorNames();
    tubecounter = mapper.FillSttTubeParametersType2(par);

  } else if (fGeoType == 1) {

    FairGeoLoader *geoLoad = FairGeoLoader::Instance();
    FairGeoInterface *geoFace = geoLoad->getGeoInterface();
    PndGeoStt *Geo = new PndGeoStt();
    Geo->setGeomFile(GetGeometryFileName());
    geoFace->addGeoModule(Geo);

    Bool_t rc = geoFace->readSet(Geo);
    if (rc)
      Geo->create(geoLoad->getGeoBuilder());

    // store geo parameter with PndSttMapCreator
    TList *volList = Geo->getListOfVolumes();
    FairRun *fRun = FairRun::Instance();
    FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
    PndGeoSttPar *par = (PndGeoSttPar *)(rtdb->getContainer("PndGeoSttPar"));
    TListIter iter(volList);
    // CHECK
    if (GetGeometryFileName().Contains(".geo"))
      fGeoType = 1;
    else
      LOG(error) << " STT: this geometry is not supported now";

    PndSttMapCreator mapper(fGeoType);
    int tubecounter = mapper.FillSttTubeParameters(par, volList);
    LOG(info) << " STT total number of tubes: " << tubecounter;
    par->setChanged();
    par->setInputVersion(fRun->GetRunId(), 1);

    ProcessNodes(volList);
  } else {
	  if (fListOfSensitives.size() == 0)
	      SetDefaultSensorNames();
  }

  LOG(info) << " PndStt::ConstructGeometry : STT total number of tubes: " << tubecounter;
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(), 1);
}
// -------------------------------------------------------------------------

// -----   Private method AddHit   -----------------------------------------
PndSttPoint *PndStt::AddHit(Int_t trackID, Int_t detID, Int_t tubeID, TVector3 pos, TVector3 posInLocal, TVector3 posOutLocal, TVector3 momIn, TVector3 momOut, Double_t time,
                            Double_t length, Double_t eLoss, Double_t mass)
{
  TClonesArray &clref = *fSttCollection;

  Int_t size = clref.GetEntriesFast();

  PndSttPoint *pointnew = new (clref[size]) PndSttPoint(trackID, detID, pos, posInLocal, posOutLocal, momIn, momOut, time, length, eLoss, mass);
  pointnew->SetTubeID(tubeID);
  return pointnew;
}
// -------------------------------------------------------------------------
bool PndStt::CheckIfSensitive(std::string name)
{
  for (UInt_t i = 0; i < fListOfSensitives.size(); i++) {
    if (name.find(fListOfSensitives[i]) != std::string::npos)
      return true;
  }
  return false;
}
void PndStt::SetDefaultSensorNames()
{
  fListOfSensitives.push_back("ArCO2Sensitive"); // Root_Test.root
  fListOfSensitives.push_back("stt01gas");

  if (fVerboseLevel > 0) {
    std::cout << "- I - PndSTTDetector: fListOfSensitives contains:";
    for (UInt_t k = 0; k < fListOfSensitives.size(); k++)
      std::cout << "\n\t" << fListOfSensitives[k];
    std::cout << std::endl;
  }
}

ClassImp(PndStt)
