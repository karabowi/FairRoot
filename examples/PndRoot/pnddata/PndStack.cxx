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
// -----                       PndStack source file                    -----
// -----             Created 10/08/04  by D. Bertini / V. Friese       -----
// -------------------------------------------------------------------------
#include "PndStack.h"

#include "FairDetector.h"
#include "FairMCPoint.h"
#include "PndMCTrack.h"
#include "FairRootManager.h"
#include "FairMCEventHeader.h"
#include "FairLogger.h"

#include "TError.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TRefArray.h"

#include <list>
#include <iostream>

using std::cout;
using std::endl;
using std::pair;

// -----   Default constructor   -------------------------------------------
PndStack::PndStack(Int_t size)
  : fStack(), fStoreMap(), fStoreIter(), fIndexMap(), fIndexIter(), fPointsMap(), fCurrentTrack(-1), fNPrimaries(0), fNParticles(0), fNTracks(0),
    fIndex(0), fStoreSecondaries(kTRUE), fMinPoints(1), fEnergyCut(0), fStoreMothers(kTRUE), fPersistence(kTRUE)
{
  fStoreMap.clear();
  fIndexMap.clear();
  fPointsMap.clear();
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndStack::~PndStack()
{
  if (fParticles) {
    fParticles->Delete();
    delete fParticles;
  }
  fTrackVector->clear();
  fGeoTrIdVector->clear();
  fGeoTrXVector->clear();
  fGeoTrYVector->clear();
  fGeoTrZVector->clear();
  fGeoTrPxVector->clear();
  fGeoTrPyVector->clear();
  fGeoTrPzVector->clear();

  if (fTracks) {
    fTracks->Delete();
    delete fTracks;
  }
}
// -------------------------------------------------------------------------

// -----   Virtual public method PushTrack   -------------------------------
void PndStack::PushTrack(Int_t toBeDone, Int_t parentId, Int_t pdgCode, Double_t px, Double_t py, Double_t pz, Double_t e, Double_t vx, Double_t vy, Double_t vz, Double_t time,
                         Double_t polx, Double_t poly, Double_t polz, TMCProcess proc, Int_t &ntr, Double_t weight, Int_t is)
{

  PushTrack(toBeDone, parentId, pdgCode, px, py, pz, e, vx, vy, vz, time, polx, poly, polz, proc, ntr, weight, is, -1);
}

// -----   Virtual public method PushTrack   -------------------------------
void PndStack::PushTrack(Int_t toBeDone, Int_t parentId, Int_t pdgCode, Double_t px, Double_t py, Double_t pz, Double_t e, Double_t vx, Double_t vy, Double_t vz, Double_t time,
                         Double_t polx, Double_t poly, Double_t polz, TMCProcess proc, Int_t &ntr, Double_t weight, Int_t is, Int_t secondparentID)
{

  (void)is; // To remove "unused" warnings
  // --> Get TParticle array
  TClonesArray &partArray = *fParticles;

  // --> Create new TParticle and add it to the TParticle array
  Int_t trackId = fNParticles;
  Int_t nPoints = 0;
  Int_t daughter1Id = -1;
  Int_t daughter2Id = -1;
  //  LOG(info) << "[" << trackId << "] PARTICLE PUSHED " << pdgCode << " @ (" << vx << "," << vy << "," << vz << ") mom = (" << px << "," << py << "," << pz << ")";
  TParticle *particle = new (partArray[fNParticles++]) TParticle(pdgCode, trackId, parentId, nPoints, daughter1Id, daughter2Id, px, py, pz, e, vx, vy, vz, time);
  particle->SetLastMother(secondparentID);
  particle->SetPolarisation(polx, poly, polz);
  particle->SetWeight(weight);
  particle->SetUniqueID(proc);

  // --> Increment counter
  if (parentId < 0)
    fNPrimaries++;

  // --> Set argument variable
  ntr = trackId;

  // --> Push particle on the stack if toBeDone is set
  if (toBeDone == 1) {
    particle->SetBit(kDoneBit);
    fStack.push(particle);
  }
}
// -------------------------------------------------------------------------

// -----   Virtual method PopNextTrack   -----------------------------------
TParticle *PndStack::PopNextTrack(Int_t &iTrack)
{

  // If end of stack: Return empty pointer
  if (fStack.empty()) {
    iTrack = -1;
    return nullptr;
  }

  // If not, get next particle from stack
  TParticle *thisParticle = fStack.top();
  fStack.pop();

  if (!thisParticle) {
    iTrack = 0;
    return nullptr;
  }

  fCurrentTrack = thisParticle->GetStatusCode();
  iTrack = fCurrentTrack;

  return thisParticle;
}
// -------------------------------------------------------------------------

// -----   Virtual method PopPrimaryForTracking   --------------------------
TParticle *PndStack::PopPrimaryForTracking(Int_t iPrim)
{

  // Get the iPrimth particle from the fStack TClonesArray. This
  // should be a primary (if the index is correct).

  // Test for index
  if (iPrim < 0 || iPrim >= fNPrimaries) {
    LOG(error) << "PndStack: Primary index out of range! " << iPrim;
    Fatal("PndStack::PopPrimaryForTracking", "Index out of range");
  }

  // Return the iPrim-th TParticle from the fParticle array. This should be
  // a primary.
  TParticle *part = (TParticle *)fParticles->At(iPrim);

  if (!(part->GetMother(0) < 0)) {
    LOG(error) << "PndStack:: Not a primary track! ,  " << iPrim;
    Fatal("PndStack::PopPrimaryForTracking", "Not a primary track");
  }

  if (!part->TestBit(kDoneBit))
    return nullptr;
  else
    return part;
}
// -------------------------------------------------------------------------

// -----   Virtual public method GetCurrentTrack   -------------------------
TParticle *PndStack::GetCurrentTrack() const
{
  TParticle *currentPart = GetParticle(fCurrentTrack);
  if (!currentPart) {
    LOG(warning) << "PndStack: Current track not found in stack!";
    Warning("PndStack::GetCurrentTrack", "Track not found in stack");
  }
  return currentPart;
}
// -------------------------------------------------------------------------

// -----   Public method AddParticle   -------------------------------------
void PndStack::AddParticle(TParticle *oldPart)
{
  TClonesArray &array = *fParticles;
  TParticle *newPart = new (array[fIndex]) TParticle(*oldPart);
  newPart->SetWeight(oldPart->GetWeight());
  newPart->SetUniqueID(oldPart->GetUniqueID());
  fIndex++;
}
// -------------------------------------------------------------------------


void PndStack::AddGeoPoint(int trID, double trX, double trY, double trZ, double moX, double moY, double moZ)
{
    fGeoTrIdVector->push_back(trID);
    fGeoTrXVector->push_back(trX);
    fGeoTrYVector->push_back(trY);
    fGeoTrZVector->push_back(trZ);
    fGeoTrPxVector->push_back(moX);
    fGeoTrPyVector->push_back(moY);
    fGeoTrPzVector->push_back(moZ);

}

// -----   Public method FillTrackArray   ----------------------------------
void PndStack::FillTrackArray()
{

  LOG(debug) << "PndStack: Filling MCTrack array...";

  // --> Reset index map and number of output tracks
  fIndexMap.clear();
  fNTracks = 0;

  // --> Check tracks for selection criteria
  SelectTracks();

  // --> Loop over fParticles array and copy selected tracks
  for (Int_t iPart = 0; iPart < fNParticles; iPart++) {

    fStoreIter = fStoreMap.find(iPart);
    if (fStoreIter == fStoreMap.end()) {
      LOG(error) << "PndStack: Particle  " << iPart << "  not found in storage map!";
      Fatal("PndStack::FillTrackArray", "Particle not found in storage map.");
    }
    Bool_t store = (*fStoreIter).second;

    //    LOG(info) << "PARTICLE " << iPart << " @ " << GetParticle(iPart)->Vx() << "," << GetParticle(iPart)->Vy() << "," << GetParticle(iPart)->Vz() << (store?" + ACCEPTED":" - REJECTED");
    if (store) {
        fTrackVector->push_back(PndMCTrack(GetParticle(iPart)));
        PndMCTrack track = fTrackVector->back();
            //new ((*fTracks)[fNTracks]) PndMCTrack(GetParticle(iPart));
      fIndexMap[iPart] = fNTracks;
      // --> Set the number of points in the detectors for this track
      for (int iDet = static_cast<int>(PndDetectorId::kRICH); iDet < static_cast<int>(PndDetectorId::kHYP); iDet++) {
        pair<Int_t, Int_t> a(iPart, iDet);
        track.SetNPoints(static_cast<PndDetectorId>(iDet), fPointsMap[a]);
      }

      SetGeneratorFlags(iPart);

      fNTracks++;

    } else {
      fIndexMap[iPart] = -2;
    }
  }

  // --> Map index for primary mothers
  fIndexMap[-1] = -1;

  // --> Screen output
  Print(0);
}
// -------------------------------------------------------------------------

void PndStack::SetGeneratorFlags(Int_t myid)
{
  if (myid < 0)
    return;

  PndMCTrack mytrack;
  {
    Int_t myid2 = fIndexMap[myid];
    if (myid2 < 0) {
      LOG(error) << "=== This should not happen negative index in MAP!!";
      return;
    }

    mytrack = fTrackVector->at(myid2);
    //    mytrack = (PndMCTrack *)fTracks->At(myid2);
  }

  Int_t n;
  Int_t daughters = 0, daughtersp = 0;
  // fParticles; // TParticle
  n = fParticles->GetEntries();
  for (Int_t i = 0; i < n; i++) {
    TParticle *part = (TParticle *)fParticles->At(i);
    Int_t m;
    m = part->GetMother(0);
    if (myid == m) {
      daughters++;
    } else if (m == -1) {
      m = part->GetMother(1);
      if (myid == m) {
        daughtersp++;
      }
    } else if (m == -2) {
      // removed should not happen before this is called
      // and anyway not on the TParticle Level
      LOG(error) << "=== Problem!!! part mother -2";
    }
  }

  Int_t mymo1 = mytrack.GetMotherID();

  if (((TParticle *)fParticles->At(myid))->GetMother(0) != mymo1) {
    LOG(error) << "=== Problem: Mothers != " << myid;
  }
  if (mymo1 == -1) {
    if (daughters != 0 && daughtersp != 0) {
      LOG(error) << "=== Problem: particle with index " << myid << " has  daughters= " << daughters << "  && daughtersp= " << daughtersp;
    }

    mytrack.SetGeneratorCreated();
    if (daughtersp > 0)
      mytrack.SetGeneratorDecayed();
    //		cout << myid <<" ("<<mytrack->GetPdgCode()<<"): "<<daughters<<","<<daughtersp<<" ==> " <<mytrack->IsGeneratorCreated()<<" "<<mytrack->IsGeneratorDecayed()<<"
    //"<<mytrack->IsGeneratorLast()<<endl;
  }
}

// -----   Public method UpdateTrackIndex   --------------------------------
void PndStack::UpdateTrackIndex(TRefArray *detList)
{

  LOG(debug) << "PndStack: Updating track indizes...";
  Int_t nColl = 0;

  const FairMCEventHeader *header = FairRootManager::Instance()->InitObjectAs<FairMCEventHeader const*>("MCEventHeader");
  //  fTracksArray = ioman->InitObjectAs<std::vector<FairMCTrack> const*>("MCTrack");
  
  // First update mother ID in MCTracks
  for (Int_t i = 0; i < fNTracks; i++) {
      PndMCTrack track = fTrackVector->at(i);
          //(PndMCTrack *)fTracks->At(i);
    
    Int_t iMotherOld = track.GetMotherID();
    fIndexIter = fIndexMap.find(iMotherOld);
    if (fIndexIter == fIndexMap.end()) {
      LOG(error) << "PndStack: Particle index  " << iMotherOld << " not found in dex map! ";
      Fatal("PndStack::UpdateTrackIndex", "Particle index not found in map");
    }
    track.SetMotherID((*fIndexIter).second);
    if (iMotherOld == -1) {
      iMotherOld = track.GetSecondMotherID();
      fIndexIter = fIndexMap.find(iMotherOld);
      if (fIndexIter == fIndexMap.end()) {
        LOG(error) << "PndStack: Particle index  " << iMotherOld << " not found in dex map! (second mother id)";
        Fatal("PndStack::UpdateTrackIndex", "Particle index not found in map");
      }
      track.SetSecondMotherID((*fIndexIter).second);
    }
  }

  TIterator* detIter = detList->MakeIterator();
  detIter->Reset();
  FairDetector* det = nullptr;

  while ((det = static_cast<FairDetector*>(detIter->Next()))) {

    Int_t iColl = 0;
    std::vector<FairMCPoint*> pointPtrs;

    while (!(pointPtrs = det->GetPointVectorForIndex(iColl++)).empty()) {
      nColl++;

      int npoints = 0;
      for (FairMCPoint* point : pointPtrs) {

        Int_t iTrack = point->GetTrackID();
        fIndexIter = fIndexMap.find(iTrack);

        if (fIndexIter == fIndexMap.end()) {
          LOG(error) << "PndStack: Particle index " << iTrack
                     << " not found in index map!";
          Fatal("PndStack::UpdateTrackIndex", "Particle index not found in map");
        }

        //        LOG(info) << det->GetName() << " [" << npoints << "] TrId = " << point->GetTrackID() << " -> " << fIndexIter->second;
        point->SetTrackID(fIndexIter->second);
        point->SetLink(FairLink(-1, (header->GetEventID() - 1),
                                "MCTrack", fIndexIter->second));
      }
    } // collections of this detector
  }   // list of active detectors

  LOG(debug) << "...stack and " << nColl << " collections updated.";
  delete detIter;
}
// -------------------------------------------------------------------------

// -----   Public method Reset   -------------------------------------------
void PndStack::Reset()
{
  fIndex = 0;
  fCurrentTrack = -1;
  fNPrimaries = fNParticles = fNTracks = 0;
  while (!fStack.empty())
    fStack.pop();
  fParticles->Clear();
  fTracks->Clear();
  fTrackVector->clear();
  fGeoTrIdVector->clear();
  fGeoTrXVector->clear();
  fGeoTrYVector->clear();
  fGeoTrZVector->clear();
  fGeoTrPxVector->clear();
  fGeoTrPyVector->clear();
  fGeoTrPzVector->clear();

  fPointsMap.clear();
}
// -------------------------------------------------------------------------

// -----   Public method Register   ----------------------------------------
void PndStack::Register()
{
    FairRootManager::Instance()->RegisterAny("MCTrack", fTrackVector, kTRUE);
    /*
    FairRootManager::Instance()->RegisterAny("geoId", fGeoTrIdVector, kTRUE);
    FairRootManager::Instance()->RegisterAny("geoX", fGeoTrXVector, kTRUE);
    FairRootManager::Instance()->RegisterAny("geoY", fGeoTrYVector, kTRUE);
    FairRootManager::Instance()->RegisterAny("geoZ", fGeoTrZVector, kTRUE);
    FairRootManager::Instance()->RegisterAny("geoPx", fGeoTrPxVector, kTRUE);
    FairRootManager::Instance()->RegisterAny("geoPy", fGeoTrPyVector, kTRUE);
    FairRootManager::Instance()->RegisterAny("geoPz", fGeoTrPzVector, kTRUE);
    */


}
// -------------------------------------------------------------------------

// -----   Public method Print  --------------------------------------------
void PndStack::Print(Int_t iVerbose) const
{
  LOG(debug) << "  PndStack: Number of primaries  = " << fNPrimaries << "\n  Total number of particles  = " << fNParticles << "\n  Number of tracks in output = " << fNTracks;

  if (iVerbose) {
      //    for (Int_t iTrack = 0; iTrack < fNTracks; iTrack++)
        //        fTrackVector[iTrack].Print(iTrack);
        //((PndMCTrack *)fTracks->At(iTrack))->Print(iTrack);
  }
}
// -------------------------------------------------------------------------

// -----   Public method AddPoint (for current track)   --------------------
void PndStack::AddPoint(PndDetectorId detId)
{
  Int_t iDet = static_cast<int>(detId);
  pair<Int_t, Int_t> a(fCurrentTrack, iDet);
  if (fPointsMap.find(a) == fPointsMap.end())
    fPointsMap[a] = 1;
  else
    fPointsMap[a]++;
}
// -------------------------------------------------------------------------

// -----   Public method AddPoint (for arbitrary track)  -------------------
void PndStack::AddPoint(PndDetectorId detId, Int_t iTrack)
{
  if (iTrack < 0)
    return;
  Int_t iDet = static_cast<int>(detId);
  pair<Int_t, Int_t> a(iTrack, iDet);
  if (fPointsMap.find(a) == fPointsMap.end())
    fPointsMap[a] = 1;
  else
    fPointsMap[a]++;
}
// -------------------------------------------------------------------------

// -----   Virtual method GetCurrentParentTrackNumber   --------------------
Int_t PndStack::GetCurrentParentTrackNumber() const
{
  TParticle *currentPart = GetCurrentTrack();
  if (currentPart)
    return currentPart->GetFirstMother();
  else
    return -1;
}
// -------------------------------------------------------------------------

// -----   Public method GetParticle   -------------------------------------
TParticle *PndStack::GetParticle(Int_t trackID) const
{
  if (trackID < 0 || trackID >= fNParticles) {
    LOG(error) << "PndStack: Particle index out of range." << trackID;
    Fatal("PndStack::GetParticle", "Index out of range");
  }
  return (TParticle *)fParticles->At(trackID);
}
// -------------------------------------------------------------------------

// -----   Private method SelectTracks   -----------------------------------
void PndStack::SelectTracks()
{
    //    LOG(info) << "PndStack::SelectTracks()";
  // --> Clear storage map
  fStoreMap.clear();

  // --> Check particles in the fParticle array
  for (Int_t i = 0; i < fNParticles; i++) {

    TParticle *thisPart = GetParticle(i);
    Bool_t store = kTRUE;

    // --> Get track parameters
    Int_t iMother = thisPart->GetMother(0);
    TLorentzVector p;
    thisPart->Momentum(p);
    Double_t energy = p.E();
    Double_t mass = p.M();
    //    Double_t mass   = thisPart->GetMass();// Why?? Mass (given by generator) is inside by Lorentzvector!!! I dont care about PSG mass!
    Double_t eKin = energy - mass;
    if (eKin < 0.0)
      eKin = 0.0; // sometimes due to different PDG masses between ROOT and G4!!!!!!
    // --> Calculate number of points
    Int_t nPoints = 0;
    for (int iDet = static_cast<int>(PndDetectorId::kRICH); iDet < static_cast<int>(PndDetectorId::kHYP); iDet++) {
      pair<Int_t, Int_t> a(i, iDet);
      if (fPointsMap.find(a) != fPointsMap.end())
        nPoints += fPointsMap[a];
    }

    // --> Check for cuts (store primaries in any case)
    if (iMother < 0)
      store = kTRUE;
    else {
      if (!fStoreSecondaries)
        store = kFALSE;
      if (nPoints < fMinPoints)
        store = kFALSE;
      if (eKin < fEnergyCut)
        store = kFALSE;
    }

    // --> Set storage flag

    //    LOG(info) << "[" << FairRootManager::Instance()->GetInstanceId() << "]" << " <" << i << "> @ " << thisPart->Vz() << " / " << thisPart->Pz() << " has " << nPoints << " points " << (store?"   ---   ---   >>>   WILL BE STORED":"will not be stored");
    fStoreMap[i] = store;
  }

  // --> If flag is set, flag recursively mothers of selected tracks
  if (fStoreMothers) {
    for (Int_t i = 0; i < fNParticles; i++) {
      if (fStoreMap[i]) {
        Int_t iMother = GetParticle(i)->GetMother(0);
        while (iMother >= 0) {
          fStoreMap[iMother] = kTRUE;
          iMother = GetParticle(iMother)->GetMother(0);
        }
      }
    }
  }
}
// -------------------------------------------------------------------------

ClassImp(PndStack)
