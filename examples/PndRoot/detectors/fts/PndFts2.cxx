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
// -----                       PndFts source file                      -----
// -------------------------------------------------------------------------

#include "PndFts2.h"

#include "PndGeoFtsPar.h"
#include "PndGeoFts.h"
#include "PndDetectorList.h"
#include "PndStack.h"
#include "PndFtsMapCreator.h"

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

using std::cout;
using std::endl;
using std::string;

// TODO: read this from geant initialization
#define innerStrawDiameter 1.
//#define redefineLambdaChargedDecay 0

// -----   Default constructor   -------------------------------------------
PndFts2::PndFts2()
  : fTrackID(0), fVolumeID(0), fPos(0, 0, 0, 0), fPosIn(0, 0, 0, 0), fPosOut(0, 0, 0, 0), fPosInLocal(0, 0, 0, 0), fPosOutLocal(0, 0, 0, 0), fMomIn(0, 0, 0, 0),
    fMomOut(0, 0, 0, 0), fTime(0), fLength(0), fELoss(0), fMass(0), fIsInitialized(kFALSE), fPosIndex(0), fFtsCollection(nullptr), fpostot(0, 0, 0, 0), fpostotin(0, 0, 0, 0),
    fpostotout(0, 0, 0, 0), fPassNodes(new TObjArray()), valid(kFALSE), fGeoType(0)
{
  fFtsCollection = new TClonesArray("PndFtsPoint");
  fVerboseLevel = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndFts2::PndFts2(const char *name, Bool_t active)
  : FairDetector(name, active), fTrackID(0), fVolumeID(0), fPos(0, 0, 0, 0), fPosIn(0, 0, 0, 0), fPosOut(0, 0, 0, 0), fPosInLocal(0, 0, 0, 0), fPosOutLocal(0, 0, 0, 0),
    fMomIn(0, 0, 0, 0), fMomOut(0, 0, 0, 0), fTime(0), fLength(0), fELoss(0), fMass(0), fIsInitialized(kFALSE), fPosIndex(0), fFtsCollection(nullptr), fpostot(0, 0, 0, 0),
    fpostotin(0, 0, 0, 0), fpostotout(0, 0, 0, 0), fPassNodes(new TObjArray()), valid(kFALSE), fGeoType(0)
{
  fFtsCollection = new TClonesArray("PndFtsPoint");
  fVerboseLevel = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFts2::~PndFts2()
{
  if (fFtsCollection) {
    fFtsCollection->Delete();
    delete fFtsCollection;
  }
}
// -------------------------------------------------------------------------

// -----   Private method GetSquaredDistanceFromWire -----------------------
float PndFts2::GetSquaredDistanceFromWire()
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

bool PndFts2::Split(string &aDest, string &aSrc, char aDelim)
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

string PndFts2::GetStringPart(string &aSrc, Int_t part, char aDelim)
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
Bool_t PndFts2::ProcessHits(FairVolume *vol)
{

  // std::cout<<"ProcessHit PndFts####################################################"<<std::endl;

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

    // std::cout<<"bool valid = "<<valid<<std::endl;

    // Create PndFtsPoint at exit of active volume -- but not into the wire
    if (gMC->IsTrackExiting() && valid == kTRUE) {
      // std::cout<<"sono nel ciclo"<<std::endl;
      valid = kFALSE;
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      fVolumeID = static_cast<int>(PndDetectorId::kFTS); // vol->getMCid();

      if (fTrackID != 0) {
        if (fVerboseLevel > 2)
          std::cout << "test Vol----------" << vol->getMCid() << std::endl;
        if (fVerboseLevel > 2)
          std::cout << "fTrackID-----" << fTrackID << std::endl;
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
      /*
  // string basename("stt1tube");
  string basename;
  TString volumename;
  string
    hashmark("#"),
    volName,
    fullName,
    number,
    specialname,
    volPath(gMC->CurrentVolPath()),
    volPath2(gMC->CurrentVolPath());

  volumename = volPath;
  volName = GetStringPart(volPath, 2, '/');
  number = GetStringPart(volName, 1, '_');

  Int_t start =volPath2.find("fts",0);
  specialname = volPath2.substr(start,volPath2.find("_",start)-start);

  if(volumename.Contains("fts01")) basename = "fts01tube";
  if(volumename.Contains("fts02")) basename = "fts02tube";
  if(volumename.Contains("fts31")) basename = "fts31tube";
  if(volumename.Contains("fts32")) basename = "fts32tube";
  if(volumename.Contains("fts33")) basename = "fts33tube";
  if(volumename.Contains("fts34")) basename = "fts34tube";
  if(volumename.Contains("fts35")) basename = "fts35tube";
  if(volumename.Contains("fts36")) basename = "fts36tube";
  if(volumename.Contains("fts37")) basename = "fts37tube";
  if(volumename.Contains("fts38")) basename = "fts38tube";
        if(volumename.Contains("fts05")) basename = "fts05tube";
  if(volumename.Contains("fts06")) basename = "fts06tube";

  fullName = basename + hashmark + number;

  FairGeoNode
    *volnode = dynamic_cast<FairGeoNode*> (fPassNodes->FindObject(fullName.c_str()));
  if (!volnode)
    {
      LOG(info) << " PndFts: No volume " << fullName.c_str() << " found in geometry container."  ;
      return kFALSE;
    }


  if(fTrackID!=0){
    if (fVerboseLevel>2) std::cout<<"befor Mapper------"<<std::endl;
  }

  fpostot.SetXYZM((fpostotin.X() +  fpostotout.X())/2., (fpostotin.Y() + fpostotout.Y())/2.,(fpostotin.Z() + fpostotout.Z())/2.,0.0);


       //CHECK map creator-------------------------------------------------
       PndFtsMapCreator *mapper = new PndFtsMapCreator(fGeoType);
  Int_t tubeID=0;
 //testTubeID=tube number as in the geometry file....
 //we need to calculate the real number in order to have a different
 //number for each tube (also for up and down short tubes)
 Int_t testTubeID = mapper->GetTubeIDFromPath(gMC->CurrentVolPath());
 Int_t chamberID=mapper->GetChamberIDFromPath(gMC->CurrentVolPath());
 Int_t layerID=mapper->GetLayerID(chamberID,testTubeID,gMC->CurrentVolPath());
 //std::cout<<"LAYERID==========="<<layerID<<std::endl;
 //tubeID=at each tube corresponds only one id number.....
 tubeID = mapper->GetTubeIDTot(chamberID,layerID,testTubeID,gMC->CurrentVolPath());
 //std::cout<<"PndFts.cxx. chamber, tube ID = "<<chamberID<<" "<<tubeID<<" "<<testTubeID<<std::endl;
 ////----------------------------------------------------------------
*/
      fpostot.SetXYZM((fpostotin.X() + fpostotout.X()) / 2., (fpostotin.Y() + fpostotout.Y()) / 2., (fpostotin.Z() + fpostotout.Z()) / 2., 0.0);
      Int_t tubeID = 0, chamberID = 0, layerID = 0;
      AddHit(fTrackID, fVolumeID, tubeID, chamberID, layerID, TVector3(fpostot.X(), fpostot.Y(), fpostot.Z()), TVector3(fPosInLocal.X(), fPosInLocal.Y(), fPosInLocal.Z()),
             TVector3(fPosOutLocal.X(), fPosOutLocal.Y(), fPosOutLocal.Z()), TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()), TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
             fTime, fLength, fELoss, fMass);

      if (fTrackID != 0) {
        if (fVerboseLevel > 2)
          std::cout << "AddHit PndFts.cxx= " << fTrackID << std::endl;
      }

      // Increment number of stt points for TParticle
      PndStack *stack = (PndStack *)gMC->GetStack();
      stack->AddPoint(PndDetectorId::kFTS);
      ResetParameters();
    }
  }

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Public method EndOfEvent   --------------------------------------
void PndFts2::EndOfEvent()
{
  if (fVerboseLevel)
    Print();
  fFtsCollection->Delete();
  fPosIndex = 0;
}
// -------------------------------------------------------------------------

// -----   Public method Register   ----------------------------------------
void PndFts2::Register()
{
  FairRootManager::Instance()->Register("FTSPoint", "Fts", fFtsCollection, kTRUE);
}
// -------------------------------------------------------------------------

// -----   Public method GetCollection   -----------------------------------
TClonesArray *PndFts2::GetCollection(Int_t iColl) const
{
  if (iColl == 0)
    return fFtsCollection;
  else
    return nullptr;
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndFts2::Print() const
{
  Int_t nHits = fFtsCollection->GetEntriesFast();

  LOG(info) << " PndFts: " << nHits << " points registered in this event.";

  if (fVerboseLevel > 1)
    for (Int_t i = 0; i < nHits; i++)
      (*fFtsCollection)[i]->Print();
}
// -------------------------------------------------------------------------

// -----   Public method Reset   -------------------------------------------
void PndFts2::Reset()
{
  fFtsCollection->Delete();
  ResetParameters();
}
// -------------------------------------------------------------------------

// -----   Public method CopyClones   --------------------------------------
void PndFts2::CopyClones(TClonesArray *cl1, TClonesArray *cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();

  LOG(info) << " PndFts: " << nEntries << " entries to add.";

  TClonesArray &clref = *cl2;

  PndFtsPoint *oldpoint = nullptr;
  for (Int_t i = 0; i < nEntries; i++) {
    oldpoint = (PndFtsPoint *)cl1->At(i);

    Int_t index = oldpoint->GetTrackID() + offset;

    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndFtsPoint(*oldpoint);
    fPosIndex++;
  }
  LOG(info) << " PndFts: " << cl2->GetEntriesFast() << " merged entries.";
}
// -------------------------------------------------------------------------

void PndFts2::Initialize()
{
  cout << " -I- Initializing PndFts()" << endl;
  FairDetector::Initialize();
}

// -----   Public method ConstructGeometry   -------------------------------
void PndFts2::ConstructGeometry()
{
  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  PndGeoFts *Geo = new PndGeoFts();
  Geo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(Geo);

  Bool_t rc = geoFace->readSet(Geo);
  if (rc)
    Geo->create(geoLoad->getGeoBuilder());

  TList *volList = Geo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
  PndGeoFtsPar *par = (PndGeoFtsPar *)(rtdb->getContainer("PndGeoFtsPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode *node = nullptr;
  FairGeoVolume *aVol = nullptr;

  while ((node = (FairGeoNode *)iter.Next())) {
    aVol = dynamic_cast<FairGeoVolume *>(node);
    if (node->isSensitive()) {
      fSensNodes->AddLast(aVol);
    } else {
      fPassNodes->AddLast(aVol);
    }
  }

  // std::cout<<"BBBB->"<<fSensNodes->GetEntries()<<std::endl;
  // std::cout<<"cccc->"<<fPassNodes->GetEntries()<<std::endl;

  fGeoType = 1; // CHECK
  par->SetGeometryType(fGeoType);
  par->SetTubeInRad(0.5);    // cm
  par->SetTubeOutRad(0.001); // cm

  par->setChanged();
  par->setInputVersion(fRun->GetRunId(), 1);
  ProcessNodes(volList);
}
// -------------------------------------------------------------------------

// -----   Private method AddHit   -----------------------------------------
PndFtsPoint *PndFts2::AddHit(Int_t trackID, Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, TVector3 pos, TVector3 posInLocal, TVector3 posOutLocal, TVector3 momIn,
                             TVector3 momOut, Double_t time, Double_t length, Double_t eLoss, Double_t mass) // da cancellare postot
{
  TClonesArray &clref = *fFtsCollection;

  Int_t size = clref.GetEntriesFast();

  PndFtsPoint *pointnew = new (clref[size]) PndFtsPoint(trackID, detID, tubeID, chamberID, layerID, pos, posInLocal, posOutLocal, momIn, momOut, time, length, eLoss, mass);

  pointnew->SetTubeID(tubeID);
  pointnew->SetChamberID(chamberID);
  pointnew->SetLayerID(layerID);

  return pointnew;
}
// -------------------------------------------------------------------------

ClassImp(PndFts2)
