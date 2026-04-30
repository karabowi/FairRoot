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

#include "BSEmcDetector.h"

#include <stdlib.h>
#include <string.h>
#include <string>

#include "TClonesArray.h"
#include "TGenericClassInfo.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TLorentzVector.h"
#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
#include "TVirtualMC.h"
#include "TVirtualMCStack.h"

#include "FairDetector.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "fairlogger/Logger.h"

#include "PndDetectorList.h"
#include "PndGeoHandling.h"
#include "PndSensorNameIdMap.h"
#include "PndStack.h"

#include "BSEmcMCPoint.h"

// -----   Default constructor   -------------------------------------------
BSEmcDetector::BSEmcDetector()
  : FairDetector(), fListOfSensitives(), fGeoH(PndGeoHandling::Instance()), fIdMap(nullptr), fPosIndex(0), fOutBranchName("EmcMCPoint"), fFolderName("Emc"),
    fPndEmcCollection(nullptr), fTrackID(0), fVolumeID(0), fCopyNumber(0), fEventID(0), fPos(), fMom(), fTime(0), fLength(0), fELoss(0), fTransformMatrix(nullptr)

{
  fPndEmcCollection = new TClonesArray("BSEmcMCPoint");
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BSEmcDetector::BSEmcDetector(const char *t_name, Bool_t t_active)
  : FairDetector(t_name, t_active), fListOfSensitives(), fGeoH(PndGeoHandling::Instance()), fIdMap(nullptr), fPosIndex(0), fOutBranchName("EmcMCPoint"), fFolderName("Emc"),
    fPndEmcCollection(nullptr), fTrackID(0), fVolumeID(0), fCopyNumber(0), fEventID(0), fPos(), fMom(), fTime(0), fLength(0), fELoss(0), fTransformMatrix(nullptr)
{
  fPndEmcCollection = new TClonesArray("BSEmcMCPoint");
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BSEmcDetector::~BSEmcDetector()
{
  if (fPndEmcCollection != nullptr) {
    fPndEmcCollection->Delete();
    delete fPndEmcCollection;
  }
  if (fTransformMatrix != nullptr) {
    delete fTransformMatrix;
  }

  if (fIdMap != nullptr) {
    delete fIdMap;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Initialize  ---------------------------------------
void BSEmcDetector::Initialize()
{
  LOG(debug) << "Initializing BSEmcDetector: " << GetName();
  FairDetector::Initialize();
  if (gGeoManager == nullptr) {
    LOG(error) << "Initializing BSEmcDetector " << GetName() << ": Failed to initialize, missing gGeoManager.";
    abort();
  }

  fGeoH->CreateUniqueSensorId("", fListOfSensitives, fIdMap);
}
// -------------------------------------------------------------------------

// -----   Public method ProcessHits  --------------------------------------
Bool_t BSEmcDetector::ProcessHits(FairVolume * /*t_vol*/)
{
  if (gMC->Edep() <= 0) {
    // skip all the points which have no energy loss (i.e. Entering)
    // problem for MC truth!
    // ((Idea: Check if particle was produced inside crystal or outside))
    // ANY particle ENTERING and not being a NEW TRACK the crystal produces a hit
    if (gMC->IsNewTrack()) {
      return kTRUE;
    }
    if (!gMC->IsTrackEntering() && !gMC->IsTrackExiting()) {
      return kTRUE;
    }
  }
  fTrackID = gMC->GetStack()->GetCurrentTrackNumber(); // trk ID
  fEventID = gMC->CurrentEvent();
  fELoss = gMC->Edep();
  fLength = gMC->TrackLength();
  fTime = gMC->TrackTime();
  gMC->TrackPosition(fPos); // cm
  gMC->TrackMomentum(fMom); // GeV
  fVolumeID = GetDetectorId();

  AddHit(fTrackID, fVolumeID, fEventID, fPos.Vect(), fMom.Vect(), fTime, fLength, fELoss, gMC->IsTrackEntering() && !gMC->IsNewTrack(), gMC->IsTrackExiting());

  PndStack *stack = dynamic_cast<PndStack *>(gMC->GetStack());
  stack->AddPoint(DetectorId::kEMC);

  ResetParameters();

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Public method EndOfEvent   --------------------------------------
void BSEmcDetector::EndOfEvent()
{
  LOG_IF(info, fVerboseLevel) << "BSEmcDetector::EndOfEvent()";
  if (fVerboseLevel > 0) {
    Print();
  }
  if (fPndEmcCollection != nullptr) {
    fPndEmcCollection->Delete();
  } else {
    LOG(warn) << "EndOfEvent BSEmcDetector " << GetName() << ": BSEmcDetector::EndOfEvent: no fPndEmcCollection pointer!";
  }
  fPosIndex = 0;
}
// -------------------------------------------------------------------------

// -----   Public method FinishRun   ---------------------------------------
void BSEmcDetector::FinishRun()
{
  LOG(info) << " BSEmcDetector - " << GetName() << " - FinishRun():" << fNumberOfPoints << " Points were created.";
}
// -------------------------------------------------------------------------

// -----   Public method Register   ----------------------------------------
void BSEmcDetector::Register()
{
  FairRootManager::Instance()->Register(fOutBranchName, "Emc", fPndEmcCollection, fPersistency);
}
// -------------------------------------------------------------------------

// -----   Public method GetCollection   -----------------------------------
TClonesArray *BSEmcDetector::GetCollection(Int_t t_iColl) const
{
  if (t_iColl == 0) {
    return fPndEmcCollection;
  }
  return nullptr;
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void BSEmcDetector::Print() const
{
  Int_t nHits = fPndEmcCollection->GetEntriesFast();
  LOG(info) << "Print BSEmcDetector " << GetName() << ": points registered in this event.";

  for (Int_t i = 0; i < nHits; i++) {
    (*fPndEmcCollection)[i]->Print();
  }
}
// -------------------------------------------------------------------------

// -----   Public method SetSpecialPhysicsCuts  ----------------------------
void BSEmcDetector::SetSpecialPhysicsCuts()
{
  FairRun *fRun = FairRun::Instance();

  // check for GEANT3, else abort
  if (strcmp(fRun->GetName(), "TGeant3") == 0) {
    LOG(debug) << "BSEmcDetector::SetSpecialPhysicsCuts(): gMC->SetMaxNStep((Int_t)1E6);";
    gMC->SetMaxNStep((Int_t)1E6);
  }
  // Switched off.
  return;
}
// -------------------------------------------------------------------------

// -----   Public method Reset   -------------------------------------------
void BSEmcDetector::Reset()
{
  fPndEmcCollection->Delete();
  ResetParameters();
}
// -------------------------------------------------------------------------

// -----   Public method CopyClones   --------------------------------------
void BSEmcDetector::CopyClones(TClonesArray *t_origin, TClonesArray *t_target, Int_t t_offset)
{
  Int_t nEntries = t_origin->GetEntriesFast();
  LOG(debug) << "CopyClones BSEmcDetector " << GetName() << ":" << nEntries << " entries to add.";

  TClonesArray &clref = *t_target;

  BSEmcMCPoint *oldpoint = nullptr;
  for (Int_t i = 0; i < nEntries; i++) {
    oldpoint = dynamic_cast<BSEmcMCPoint *>(t_origin->At(i));
    Int_t index = oldpoint->GetTrackID() + t_offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) BSEmcMCPoint(*oldpoint);
    fPosIndex++;
  }
  LOG(debug) << "CopyClones BSEmcDetector " << GetName() << ":" << t_target->GetEntriesFast() << " merged entries.";
}
// -------------------------------------------------------------------------

// -----   Public method ConstructGeometry   -------------------------------
void BSEmcDetector::ConstructGeometry()
{
  LOG(info) << "ConstructGeometry BSEmcDetector " << GetName();
  // Set what is sensitive before creating geometry
  if (fListOfSensitives.size() == 0) {
    SetDefaultSensitiveNames();
  }
  TString fileName = GetGeometryFileName();
  if (fileName.EndsWith(".geo")) {
    ConstructASCIIGeometry();
  } else if (fileName.EndsWith(".root")) {
    ConstructRootGeometry(fTransformMatrix);
  } else {
    LOG(warn) << "ConstructGeometry BSEmcDetector " << GetName() << ":"
              << "Geometry format not supported ";
  }
  LOG(info) << "ConstructGeometry BSEmcDetector " << GetName() << " done.";
}
// -------------------------------------------------------------------------

// -----   Public method ConstructGeometry   -------------------------------
void BSEmcDetector::ConstructASCIIGeometry()
{
  LOG(warn) << "ConstructASCIIGeometry() BSEmcDetector " << GetName() << ":"
            << "Not implemented.";
}
// -------------------------------------------------------------------------

// -----   Protected method GetDetectorId   --------------------------------------
Int_t BSEmcDetector::GetDetectorId()
{
  TString tmp{gMC->CurrentVolPath()};
  return fIdMap->GetId(tmp);
  // return fGeoH->GetShortID(gMC->CurrentVolPath());
}
// -------------------------------------------------------------------------

// -----   Protected method CheckIfSensitive   --------------------------------
Bool_t BSEmcDetector::CheckIfSensitive(std::string t_name)
{

  for (auto &sensitive : fListOfSensitives) {
    if (t_name.find(sensitive) != std::string::npos) {
      LOG(trace) << "BSEmcDetector " << GetName() << "::CheckIfSensitive(" << t_name << "): Is sensitive.";
      return kTRUE;
    }
  }
  LOG(trace) << "BSEmcDetector " << GetName() << "::CheckIfSensitive(" << t_name << "): Is not sensitive.";
  return kFALSE;
}
// -------------------------------------------------------------------------

// -----   Protected method AddHit   -----------------------------------------
BSEmcMCPoint *BSEmcDetector::AddHit(Int_t t_trackID, Int_t t_detID, Int_t t_evtID, TVector3 t_pos, TVector3 t_mom, Double_t t_time, Double_t t_length, Double_t t_eLoss,
                                    Bool_t t_entering, Bool_t t_exiting)
{
  ++fNumberOfPoints;
  LOG(trace) << "AddHit() BSEmcDetector " << GetName() << ":"
             << "Adding Point at IN (" << t_pos.X() << ", " << t_pos.Y() << ", " << t_pos.Z() << ") cm, detector " << t_detID << ", evt " << t_evtID << ", track " << t_trackID
             << ", energy loss " << t_eLoss * 1e06 << " keV,"
             << " entering " << t_entering << ", exiting " << t_exiting << ", PID: " << gMC->TrackPid();

  BSEmcMCPoint *myPoint =
    new ((*fPndEmcCollection)[fPndEmcCollection->GetEntriesFast()]) BSEmcMCPoint(t_trackID, t_detID, t_evtID, t_pos, t_mom, t_time, t_length, t_eLoss, t_entering, t_exiting);
  return myPoint;
}
// -------------------------------------------------------------------------

// -----   Protected method ResetParameters   -----------------------------------------
void BSEmcDetector::ResetParameters()
{
  fPosIndex = 0;
  fTrackID = -999;
  fVolumeID = -999;
  fCopyNumber = -999;
  fEventID = -999;
  fPos.SetXYZT(0., 0., 0., 0.);
  fMom.SetXYZT(0., 0., 0., 0.);
  fTime = -999;
  fLength = -999;
  fELoss = -999;
}
// -------------------------------------------------------------------------

ClassImp(BSEmcDetector);
