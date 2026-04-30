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

/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#include "IdealTubeDetector.h"
#include "PndStack.h" // for FairStack

#include "FairDetectorList.h" // for DetectorId::kTutDet
#include "FairLogger.h"       // for logging
#include "FairRootManager.h"  // for FairRootManager
#include "FairMCPoint.h"
#include "FairVolume.h" // for FairVolume

#include <TClonesArray.h>    // for TClonesArray
#include <TVirtualMC.h>      // for TVirtualMC
#include <TVirtualMCStack.h> // for TVirtualMCStack

IdealTubeDetector::IdealTubeDetector()
  : FairDetector("TutorialDet", kTRUE, kTutDet), fTrackID(-1), fVolumeID(-1), fPos(), fMom(), fTime(-1.), fLength(-1.), fELoss(-1),
    fIdealTubeDetectorPointCollection(new TClonesArray("FairMCPoint"))
{
}

IdealTubeDetector::IdealTubeDetector(const char *name, Bool_t active)
  : FairDetector(name, active, kTutDet), fTrackID(-1), fVolumeID(-1), fPos(), fMom(), fTime(-1.), fLength(-1.), fELoss(-1),
    fIdealTubeDetectorPointCollection(new TClonesArray("FairMCPoint"))
{
}

IdealTubeDetector::IdealTubeDetector(const IdealTubeDetector &rhs)
  : FairDetector(rhs), fTrackID(-1), fVolumeID(-1), fPos(), fMom(), fTime(-1.), fLength(-1.), fELoss(-1), fIdealTubeDetectorPointCollection(new TClonesArray("FairMCPoint"))
{
}

IdealTubeDetector::~IdealTubeDetector()
{
  if (fIdealTubeDetectorPointCollection) {
    fIdealTubeDetectorPointCollection->Delete();
    delete fIdealTubeDetectorPointCollection;
  }
}

void IdealTubeDetector::Initialize()
{
  FairDetector::Initialize();
  /*
FairRuntimeDb* rtdb= FairRun::Instance()->GetRuntimeDb();
IdealTubeDetectorGeoPar* par=(IdealTubeDetectorGeoPar*)(rtdb->getContainer("IdealTubeDetectorGeoPar"));
*/
}

Bool_t IdealTubeDetector::ProcessHits(FairVolume *vol)
{
  /** This method is called from the MC stepping */

  LOG(debug) << "In IdealTubeDetector::ProcessHits";
  // Set parameters at entrance of volume. Reset ELoss.
  if (TVirtualMC::GetMC()->IsTrackEntering()) {
    fELoss = 0.;
    fTime = TVirtualMC::GetMC()->TrackTime() * 1.0e09;
    fLength = TVirtualMC::GetMC()->TrackLength();
    TVirtualMC::GetMC()->TrackPosition(fPos);
    TVirtualMC::GetMC()->TrackMomentum(fMom);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += TVirtualMC::GetMC()->Edep();

  // Create IdealTubeDetectorPoint at exit of active volume
  if (TVirtualMC::GetMC()->IsTrackExiting() || TVirtualMC::GetMC()->IsTrackStop() || TVirtualMC::GetMC()->IsTrackDisappeared()) {
    fTrackID = TVirtualMC::GetMC()->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    //    if (fELoss == 0.) {
    //      return kFALSE;
    //    }
    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(), fPos.Y(), fPos.Z()), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, fELoss);

    // Increment number of tutorial det points in TParticle
    PndStack *stack = (PndStack *)gMC->GetStack();
    stack->AddPoint(DetectorId::kSTT);
  }

  return kTRUE;
}

void IdealTubeDetector::EndOfEvent()
{
  fIdealTubeDetectorPointCollection->Clear();
}

void IdealTubeDetector::Register()
{
  /** This will create a branch in the output tree called
    IdealTubeDetectorPoint, setting the last parameter to kFALSE means:
    this collection will not be written to the file, it will exist
    only during the simulation.
*/

  FairRootManager::Instance()->Register("IdealTubePoint", "IdealTubeDet", fIdealTubeDetectorPointCollection, kTRUE);
}

TClonesArray *IdealTubeDetector::GetCollection(Int_t iColl) const
{
  if (iColl == 0) {
    return fIdealTubeDetectorPointCollection;
  } else {
    return nullptr;
  }
}

void IdealTubeDetector::Reset()
{
  fIdealTubeDetectorPointCollection->Clear();
}

Bool_t IdealTubeDetector::IsSensitive(const std::string &name)
{
  if (name.find("IdealTube") != std::string::npos) {
    return kTRUE;
  }
  return kFALSE;
}

void IdealTubeDetector::ConstructGeometry()
{
  /** If you are using the standard ASCII input for the geometry
    just copy this and use it for your detector, otherwise you can
    implement here you own way of constructing the geometry. */

  ConstructRootGeometry();
}

FairMCPoint *IdealTubeDetector::AddHit(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss)
{
  TClonesArray &clref = *fIdealTubeDetectorPointCollection;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) FairMCPoint(trackID, detID, pos, mom, time, length, eLoss);
}

FairModule *IdealTubeDetector::CloneModule() const
{
  return new IdealTubeDetector(*this);
}

ClassImp(IdealTubeDetector);
