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
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                PndXYResidualTaskT source file             -----
// -------------------------------------------------------------------------

#include "PndXYResidualTask.h"

#include "FairLink.h"        // for FairLink
#include "FairRootManager.h" // for FairRootManager
#include "FairTimeStamp.h"   // for FairTimeStamp
#include "FairRunAna.h"

#include "PndSdsHit.h"

#include <iosfwd>         // for ostream
#include "TClass.h"       // for TClass
#include "TClonesArray.h" // for TClonesArray

#include <iostream> // for operator<<, cout, ostream, etc
#include <iomanip>
#include <vector> // for vector

InitStatus PndXYResidualTask::ReInit()
{
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndXYResidualTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndXYResidualTaskT::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Create and register output array
  fHitArray = (TClonesArray *)FairRootManager::Instance()->GetObject(fBranchName);
  fTrackArray = (TClonesArray *)FairRootManager::Instance()->GetObject("MvdTrack");

  fMissingHitArray = FairRootManager::Instance()->Register("MvdMissingHit", "PndSdsHit", "MVD", kTRUE);
  fProjectedHitArray = FairRootManager::Instance()->Register("MvdProjectedHit", "PndSdsHit", "MVD", kTRUE);
  fGoodHitArray = FairRootManager::Instance()->Register("GoodHit", "PndSdsHit", "MVD", kTRUE);

  // if(fVerbose>1) { Info("Init","Registering this branch: %s/%s",fFolder.Data(),fOutputBranch.Data()); }
  // fOutputArray = ioman->Register(fOutputBranch, fInputArray->GetClass()->GetName(), fFolder, fPersistance);
  fHc0c0 = new TH2D("fHc0c0", "fHc0c0", 200, -0.1, 0.1, 200, -0.1, 0.1);
  fHc0c1 = new TH2D("fHc0c1", "fHc0c1", 200, -0.1, 0.1, 200, -0.1, 0.1);
  fHc0c2 = new TH2D("fHc0c2", "fHc0c2", 200, -0.1, 0.1, 200, -0.1, 0.1);
  fHc0c3 = new TH2D("fHc0c3", "fHc0c3", 200, -0.1, 0.1, 200, -0.1, 0.1);
  fHc0c0cut = new TH2D("fHc0c0cut", "fHc0c0cut", 400, -0.02, 0.02, 400, -0.02, 0.02);
  fHc0c1cut = new TH2D("fHc0c1cut", "fHc0c1cut", 400, -0.02, 0.02, 400, -0.02, 0.02);
  fHc0c2cut = new TH2D("fHc0c2cut", "fHc0c2cut", 400, -0.02, 0.02, 400, -0.02, 0.02);
  fHc0c3cut = new TH2D("fHc0c3cut", "fHc0c3cut", 400, -0.02, 0.02, 400, -0.02, 0.02);
  fHMissingHits = new TH1D("fHMIssingHits", "MissingHits", 4, -0.5, 3.5);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndXYResidualTask::Exec(Option_t *)
{
  SetVerbose(2);
  if (fVerbose > 1 && ++fEntryNr % 1000 == 0) {
    LOG(info) << " PndXYResidualTask: Event " << fEntryNr;
  }
  std::set<int> sensorIDs;
  fMissingHitArray->Delete();
  fProjectedHitArray->Delete();

  for (int i = 0; i < 4; i++)
    sensorIDs.insert(i);

  if (fTrackArray->GetEntriesFast() > 0) {
    for (int i = 0; i < fTrackArray->GetEntriesFast(); i++) {
      PndTrack *myTrack = (PndTrack *)fTrackArray->At(i);
      std::vector<PndTrackCandHit> hits = myTrack->GetTrackCandPtr()->GetSortedHits();
      std::set<int> tempIDs = sensorIDs;
      for (int iHits = 0; iHits < hits.size(); iHits++) {
        PndSdsHit *myHit = (PndSdsHit *)fHitArray->At(hits[iHits].GetIndex());
        // std::cout << "HitValue: " << myHit->GetPosition().X() << "/" << myHit->GetPosition().Y()<< "/" << myHit->GetPosition().Z() << std::endl;

        TVector3 predicted = PropagateToZ(myTrack, myHit->GetZ());
        // std::cout << "PredictedValue: " <<predicted.X() << "/" << predicted.Y()<< "/" << predicted.Z() << std::endl;
        TVector3 res = predicted - myHit->GetPosition();
        tempIDs.erase(myHit->GetSensorID());
        if (myHit->GetSensorID() == 0)
          fHc0c0->Fill(res.X(), res.Y());
        if (myHit->GetSensorID() == 1)
          fHc0c1->Fill(res.X(), res.Y());
        if (myHit->GetSensorID() == 2)
          fHc0c2->Fill(res.X(), res.Y());
        if (myHit->GetSensorID() == 3)
          fHc0c3->Fill(res.X(), res.Y());
        if (myTrack->GetChi2() / myTrack->GetNDF() < 2) {
          if (myHit->GetSensorID() == 0)
            fHc0c0cut->Fill(res.X(), res.Y());
          if (myHit->GetSensorID() == 1)
            fHc0c1cut->Fill(res.X(), res.Y());
          if (myHit->GetSensorID() == 2)
            fHc0c2cut->Fill(res.X(), res.Y());
          if (myHit->GetSensorID() == 3)
            fHc0c3cut->Fill(res.X(), res.Y());
        }
        //				if (myTrack->GetChi2() / myTrack->GetNDF() < 2){
        //					PndSdsHit* goodHit = new ((*fGoodHitArray)[fGoodHitArray->GetEntriesFast()]) PndSdsHit(*myHit);
        //					TVector3 correction;
        //					TVector3 errorMHit;
        //					Int_t j = goodHit->GetSensorID();
        //					if (j == 0) correction.SetXYZ(-1.348e-3, -4.01e-3, 0);
        //					else if (j == 1) correction.SetXYZ(+4.97e-3,  4.978e-3, 0);
        //					else if (j == 2) correction.SetXYZ(-7.21e-3, 0.42e-3, 0);
        //					else if (j == 3) correction.SetXYZ(+3.556e-3, -7.569e-3, 0);
        //					errorMHit = goodHit->GetPosition() + correction;
        //					goodHit->SetPositionError(errorMHit);
        //				}
      }
      if (tempIDs.size() > 0) {
        for (std::set<int>::iterator it = tempIDs.begin(); it != tempIDs.end(); it++) {
          fHMissingHits->Fill(*it);
          int sensID = *it;
          TVector3 missingHit = PropagateToZ(myTrack, 10 + 6 * sensID);
          TVector3 errorMHit(0, 0, 0);
          new ((*fMissingHitArray)[fMissingHitArray->GetEntriesFast()]) PndSdsHit(-1, *it, missingHit, errorMHit, -1, -1, -1, -1);
        }
      }
      if (myTrack->GetChi2() / myTrack->GetNDF() < 2) {
        for (int j = 0; j < 4; j++) {
          TVector3 projectedHit = PropagateToZ(myTrack, 10 + 6 * j);

          TVector3 correction;
          TVector3 errorMHit;
          if (j == 0)
            correction.SetXYZ(-1.348e-3, -4.01e-3, 0);
          else if (j == 1)
            correction.SetXYZ(+4.97e-3, 4.978e-3, 0);
          else if (j == 2)
            correction.SetXYZ(-7.21e-3, 0.42e-3, 0);
          else if (j == 3)
            correction.SetXYZ(+3.556e-3, -7.569e-3, 0);
          errorMHit = projectedHit + correction;

          new ((*fProjectedHitArray)[fProjectedHitArray->GetEntriesFast()]) PndSdsHit(-1, j, projectedHit, errorMHit, -1, -1, hits.size(), -1);
        }
      }
    }
  }
}

TVector3 PndXYResidualTask::PropagateToZ(PndTrack *aTrack, Double_t z)
{
  TVector3 origin = aTrack->GetParamFirst().GetOrigin();
  TVector3 dir = aTrack->GetParamFirst().GetMomentum();

  // std::cout << "OriginValue: " <<origin.X() << "/" << origin.Y()<< "/" << origin.Z() << std::endl;
  // std::cout << "DirValue: " <<dir.X() << "/" << dir.Y()<< "/" << dir.Z() << std::endl;

  Double_t t = (z - origin.Z()) / dir.Z();

  return origin + t * dir;
}

// -------------------------------------------------------------------------

void PndXYResidualTask::FinishEvent()
{
  // fOutputArray->Delete();
}

void PndXYResidualTask::FinishTask()
{
  fHc0c0->Write();
  fHc0c1->Write();
  fHc0c2->Write();
  fHc0c3->Write();
  fHc0c0cut->Write();
  fHc0c1cut->Write();
  fHc0c2cut->Write();
  fHc0c3cut->Write();
}

ClassImp(PndXYResidualTask);
