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

/////////////////////////////////////////////////////////////
//
//  PndSciT
//
//  created by A. Sanchez
//  modified by D. Steinschaden
//  last update  06.2015
///////////////////////////////////////////////////////////////

#include "PndSciT.h"
#include "PndSciTPoint.h"
//#include "PndGeoSciT.h"
#include "PndStack.h"
#include "PndDetectorList.h"

#include "FairGeoTransform.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoMedium.h"
#include "FairGeoInterface.h"
#include "FairGeoMedia.h"
//#include "FairGeoG3Builder.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TString.h"
#include "TList.h"
#include "TGeoBBox.h"
#include "TGeoMCGeometry.h"
#include "TObjArray.h"
#include "TGeoVoxelFinder.h"
#include "TGeoMatrix.h"

#include <string>
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;
using std::ostringstream;

// -----   Default constructor   -------------------------------------------
PndSciT::PndSciT() : FairDetector()
{

  fVerboseLevel = 0;
  fThreshold = 0.0001; // Standard Threshold set to 100 keV
  fGeoH = nullptr;
  fGeoH = PndGeoHandling::Instance();

  // Volumes containing "SENSOR" in the name will be processed in the process hit funktion
  fListOfSensitives.push_back("SENSOR");
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSciT::PndSciT(const char *name, Bool_t active) : FairDetector(name, active)
{
  fVerboseLevel = 0;
  fThreshold = 0.0001; // Standard Threshold set to 100 keV

  fGeoH = nullptr;
  std::cout << "ScitTil fGeoH is loading" << std::endl;
  fGeoH = PndGeoHandling::Instance();
  if (fGeoH == nullptr) {
    std::cout << "ScitTil fGeoH was loaded but is still nullptr" << std::endl;
  }

  // Volumes containing "SENSOR" in the name will be processed in the process hit funktion
  fListOfSensitives.push_back("SENSOR");
}
// -------------------------------------------------------------------------

PndSciT::PndSciT(const PndSciT& rhs)
    : FairDetector(rhs)
    , fPosIndex(0)
    , fGeoH(PndGeoHandling::Instance())
    , fListOfSensitives()
{
    fListOfSensitives.push_back("SENSOR");
}

// -----   Destructor   ----------------------------------------------------
PndSciT::~PndSciT()
{
}
// -------------------------------------------------------------------------

// -----   Public method Intialize   ---------------------------------------
void PndSciT::Initialize()
{

  std::cout << " -I- Initializing PndSciT()" << std::endl;
  FairDetector::Initialize();
  std::cout << " Fairdetector geht" << std::endl;
  // not mandatory ,but may someone can make use out of the stored parameters
  FairRun *sim = FairRun::Instance();
  FairRuntimeDb *rtdb = sim->GetRuntimeDb();
  par = (PndGeoSciTPar *)(rtdb->getContainer("PndGeoSciTPar"));
  par->setChanged();
  par->setInputVersion(sim->GetRunId(), 1);
  //-----------------------------------------------------------------

  if (0 == gGeoManager) {
    std::cout << " -E- No gGeoManager in PndSciT::Initialize()!" << std::endl;
    abort();
  }

  fGeoH->CreateUniqueSensorId("", fListOfSensitives);
  if (fVerboseLevel > 0)
    fGeoH->PrintSensorNames();

  std::cout << " -I- Initialized PndSciT()" << std::endl;
}
// -------------------------------------------------------------------------
void PndSciT::BeginEvent()
{
  // Begin of the event
}

// -----   Public method ProcessHits  --------------------------------------

Bool_t PndSciT::ProcessHits(FairVolume *) // vol //[R.K.03/2017] unused variable(s)
{

  if (0 == fGeoH) {
    std::cout << " -E- No PndGeoHandling loaded." << std::endl;
    abort();
  }

  // Set parameters at entrance of volume. Reset ELoss.
  if (gMC->IsTrackEntering()) {
    fELoss = 0.;
    fEventID = gMC->CurrentEvent();
    fTime = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume.
  // And create the PndSciTPoint.
  if ((gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())) {
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

    fdetPath = gMC->CurrentVolPath();
    fSensorID = fGeoH->GetShortID(gMC->CurrentVolPath());

    // fSensorID = vol->getCopyNo();

    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);

    // Cut on energy loss to reduce stored data Elos < 100 keV
    if (fELoss < fThreshold)
      return kFALSE;

    AddHit(fEventID, fTrackID, fSensorID, fdetPath, TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()), TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
           TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()), TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()), fTime, fLength, fELoss);

    PndStack *stack = (PndStack *)gMC->GetStack();
    stack->AddPoint(PndDetectorId::kTOF);

    ResetParameters();
  }

  return kTRUE;

} // ProcessHits

// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndSciT::EndOfEvent()
{
  if (fVerboseLevel)
    Print();

  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndSciT::Register()
{
  FairRootManager::Instance()->RegisterAny("SciTPoint", fPointVector, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray *PndSciT::GetCollection(Int_t iColl) const
{
    return nullptr;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndSciT::Print() const
{
  // Int_t nHits = fSciTCollection->GetEntriesFast();
  // LOG(info) << " PndSciT: " << nHits << " points registered in this event.";

  // if (fVerboseLevel > 1)
  //   for (Int_t i = 0; i < nHits; i++)
  //     (*fSciTCollection)[i]->Print();
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void PndSciT::Reset()
{
    fPointVector->clear();

  fPosIndex = 0;
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void PndSciT::CopyClones(TClonesArray *cl1, TClonesArray *cl2, Int_t offset)
{
}
// ----------------------------------------------------------------------------
// -----   Public method ConstructGeometry   ----------------------------------
void PndSciT::ConstructGeometry()
{

  TString fileName = GetGeometryFileName();

  if (fileName.EndsWith(".root")) {
    ConstructRootGeometry();
  } else {
    cout << " geometry format not supported" << endl;
  }
}

// -------------------------------------------------------------------------
bool PndSciT::CheckIfSensitive(std::string name)
{
  for (size_t i = 0; i < fListOfSensitives.size(); i++) {
    if (name.find(fListOfSensitives[i]) != std::string::npos)
      return true;
  }
  return false;
}

// -----   Private method AddHit   --------------------------------------------

void PndSciT::AddHit(Int_t eventID, Int_t trackID, Int_t sensorID, TString detName, TVector3 pos, TVector3 mom, TVector3 posout, TVector3 momout, Double_t time,
                              Double_t length, Double_t eLoss)
{
    fPointVector->push_back(PndSciTPoint(eventID, trackID, sensorID, detName, pos, mom, posout, momout, time, length, eLoss));
}

Bool_t PndSciT::IsSensitive(const std::string& name)
{
  for (size_t i = 0; i < fListOfSensitives.size(); i++) {
    if (name.find(fListOfSensitives[i]) != std::string::npos)
      return true;
  }
  return false;
}


FairModule* PndSciT::CloneModule() const
{
    return new PndSciT(*this);
}

// --------

ClassImp(PndSciT)
