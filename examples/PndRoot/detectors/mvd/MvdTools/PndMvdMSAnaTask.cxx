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
// -----                PndMvdMSAnaTask source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TVector3.h"

// framework includes
#include "FairRootManager.h"
#include "PndMvdMSAnaTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "PndMCTrack.h"
// PndMvd includes
#include "PndSdsMCPoint.h"
#include "FairTrackParH.h"
#include "FairTrackParP.h"

#include "TDatabasePDG.h"

#include <vector>
#include <map>

// -----   Default constructor   -------------------------------------------
PndMvdMSAnaTask::PndMvdMSAnaTask()
  : FairTask("Geane Task for PANDA PndMvd"), fMCHits(nullptr), fMCTracks(nullptr), fTrackParGeane(nullptr), fTrackParIni(nullptr), fTrackParFinal(nullptr), fDetName(nullptr),
    fPro(nullptr), fGeoH(nullptr), fEventNr(0), fUseMVDPoint(false), fTrackPixHitIdMap(), fTrackStripHitIdMap()
{
  fGeoH = PndGeoHandling::Instance();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdMSAnaTask::~PndMvdMSAnaTask() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdMSAnaTask::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndMvdMSAnaTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input arrays
  fMCTracks = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTracks) {
    std::cout << "-W- PndMvdMSAnaTask::Init: "
              << "No MCTrack"
              << " array!" << std::endl;
    return kERROR;
  }

  fMCHits = (TClonesArray *)ioman->GetObject("MVDPoint");
  if (!fMCHits) {
    std::cout << "-W- PndMvdMSAnaTask::Init: "
              << "No MVDPoint"
              << " array!" << std::endl;
    return kERROR;
  }

  fTrackParGeane = new TClonesArray("FairTrackParP");
  ioman->Register("GeaneTrackPar", "Geane", fTrackParGeane, kTRUE);

  fTrackParIni = new TClonesArray("FairTrackParP");
  ioman->Register("GeaneTrackIni", "Geane", fTrackParIni, kTRUE);

  fTrackParFinal = new TClonesArray("FairTrackParP");
  ioman->Register("GeaneTrackFinal", "Geane", fTrackParFinal, kTRUE);

  fDetName = new TClonesArray("TObjString");
  ioman->Register("DetName", "Geane", fDetName, kTRUE);

  fPro = new FairGeanePro();
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndMvdMSAnaTask::SetParContainers() {}

// -----   Public method Exec   --------------------------------------------
void PndMvdMSAnaTask::Exec(Option_t *)
{
  std::map<int, std::vector<int>> mcHitMap; // Track ->  MCHits
  fTrackParGeane->Delete();
  fTrackParIni->Delete();
  fTrackParFinal->Delete();
  fDetName->Delete();

  mcHitMap = AssignHitsToTracks();

  std::cout << "------------Event " << fEventNr << "-------------" << std::endl;
  fEventNr++;

  for (std::map<int, std::vector<int>>::const_iterator it = mcHitMap.begin(); it != mcHitMap.end(); it++) { // go through all tracks
    PndMCTrack *myTrack = (PndMCTrack *)(fMCTracks->At(it->first));
    std::vector<int> MChits = it->second;
    if (MChits.size() > 1 && myTrack->GetMotherID() < 0) {
      TVector3 StartPos, StartPosErr, StartMom, StartMomErr, StartO, StartU, StartV;
      int p = 0;
      if (fUseMVDPoint) {
        PndSdsMCPoint *startPoint = (PndSdsMCPoint *)(fMCHits->At(0));
        std::cout << "StartPoint: " << *startPoint << std::endl;
        StartPos = (startPoint->GetPosition());
        StartPosErr = TVector3(0.0, 0.0, 0.0);
        StartMom = TVector3(startPoint->GetPx(), startPoint->GetPy(), startPoint->GetPz());
        StartMomErr = TVector3(0.0, 0.0, 0.0);
        fGeoH->GetOUVShortId(startPoint->GetSensorID(), StartO, StartU, StartV);
        p = 1;
      } else {
        StartPos = myTrack->GetStartVertex();
        StartPosErr = TVector3(0.0, 0.0, 0.0);
        StartMom = myTrack->GetMomentum();
        StartMomErr = TVector3(0.0, 0.0, 0.0);
        StartO = myTrack->GetStartVertex();
        StartU = TVector3(1.0, 0.0, 0.0);
        StartV = TVector3(0.0, 1.0, 0.0);
        p = 0;
      }

      fPro->PropagateFromPlane(StartU, StartV);

      Int_t PDGCode = myTrack->GetPdgCode();
      TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(PDGCode);
      Int_t fCharge = (Int_t)fParticle->Charge();

      TClonesArray &clref1 = *fTrackParIni;
      Int_t size1 = clref1.GetEntriesFast();
      FairTrackParP *fStart = new (clref1[size1]) FairTrackParP(StartPos, StartMom, StartPosErr, StartMomErr, fCharge, StartO, StartU, StartV);

      for (; p < (int)MChits.size(); p++) { // go through all hits in track
        PndSdsMCPoint *myPoint = (PndSdsMCPoint *)(fMCHits->At(MChits[p]));
        std::cout << "PropagationPoint: " << *myPoint << std::endl;

        TVector3 StopPos(myPoint->GetPosition());
        TVector3 StopPosErr(0.0, 0.0, 0.0);
        TVector3 StopMom(myPoint->GetPx(), myPoint->GetPy(), myPoint->GetPz());
        TVector3 StopMomErr(0.0, 0.0, 0.0);

        TVector3 o, u, v;
        fGeoH->GetOUVShortId(myPoint->GetSensorID(), o, u, v);

        TClonesArray &clref2 = *fTrackParFinal;
        Int_t size2 = clref2.GetEntriesFast();
        // FairTrackParP *fStop=
        new (clref2[size2]) FairTrackParP(StopPos, StopMom, StopPosErr, StopMomErr, fCharge, o, u, v);

        TClonesArray &clref = *fTrackParGeane;
        Int_t size = clref.GetEntriesFast();
        FairTrackParP *fRes = new (clref[size]) FairTrackParP();

        // std::cout << "DetName: " << fGeoH->GetPath(myPoint->GetDetName()) << std::endl;
        TClonesArray &cDetRef = *fDetName;
        Int_t size3 = cDetRef.GetEntriesFast();
        new (cDetRef[size3]) TObjString(fGeoH->GetPath(myPoint->GetSensorID()));

        std::cout << "Propagation Plane:" << std::endl;
        std::cout << "o: " << o[0] << " " << o[1] << " " << o[2] << std::endl;
        std::cout << "u: " << u[0] << " " << u[1] << " " << u[2] << std::endl;
        std::cout << "v: " << v[0] << " " << v[1] << " " << v[2] << std::endl;

        TVector3 endPoint(myPoint->GetPosition());
        // fPro->SetPoint(endPoint);
        // fPro->PropagateToPCA(1);
        fPro->PropagateToPlane(o, u, v);
        fPro->Propagate(fStart, fRes, PDGCode);

        std::cout << std::endl;
        std::cout << "Propagation Points: " << std::endl;
        std::cout << fRes->GetX() << ", "; //" +/- " << fRes->GetDX() << std::endl;
        std::cout << fRes->GetY() << ", "; //" +/- " << fRes->GetDY() << std::endl;
        std::cout << fRes->GetZ();         //" +/- " << fRes->GetDZ() << std::endl;
        std::cout << std::endl;

        std::cout << "Propagation Momentum: " << std::endl;

        std::cout << fRes->GetPx() << ", ";      //" +/- " << fRes->GetDPx() << std::endl;
        std::cout << fRes->GetPy() << ", ";      //" +/- " << fRes->GetDPy() << std::endl;
        std::cout << fRes->GetPz() << std::endl; //<< " +/- " << fRes->GetDPz() << std::endl;

        std::cout << "Charge: " << fRes->GetQ() << std::endl;

        std::cout << "Porpagation Covariance Matrix: " << std::endl;

        Double_t CovMatrix[15];
        Double_t CovMatrix66[6][6];
        fRes->GetCov(CovMatrix);
        fRes->GetMARSCov(CovMatrix66);

        /*				for (int i = 0; i < 5; i++){
         for (int j = 0; j < 5-i; j++){
         std::cout << CovMatrix[i*5+j] << " ";
         }
         std::cout << std::endl;
         }
         */
        std::cout << CovMatrix66[3][3] << ", " << CovMatrix66[3][4] << ", " << CovMatrix66[3][5] << std::endl;
        std::cout << CovMatrix66[4][4] << ", " << CovMatrix66[4][5] << ", " << CovMatrix66[5][5] << std::endl;
        std::cout << CovMatrix66[0][3] << ", " << CovMatrix66[1][3] << ", " << CovMatrix66[2][3] << std::endl;
        std::cout << CovMatrix66[0][4] << ", " << CovMatrix66[1][4] << ", " << CovMatrix66[2][4] << std::endl;
        std::cout << CovMatrix66[0][5] << ", " << CovMatrix66[1][5] << ", " << CovMatrix66[2][5] << std::endl;
        std::cout << CovMatrix66[0][0] << ", " << CovMatrix66[0][1] << ", " << CovMatrix66[0][2] << std::endl;
        std::cout << CovMatrix66[1][1] << ", " << CovMatrix66[1][2] << ", " << CovMatrix66[2][2] << std::endl;
        std::cout << std::endl;

        /*				for (int i = 0; i < 6; i++){
         for (int j = 0; j < 6; j++){
         std::cout << CovMatrix66[i][j] << " ";
         }
         std::cout << std::endl;
         }
         */
        TVector3 global(fRes->GetX(), fRes->GetY(), fRes->GetZ());
        TVector3 local = fGeoH->MasterToLocalShortId(global, myPoint->GetSensorID());

        std::cout << "Propagation Point local: " << std::endl;
        std::cout << local[0] << " " << local[1] << " " << local[2] << std::endl;
        std::cout << std::endl;
      }
    }
  }
  fMCTracks->Delete();
  fMCHits->Delete();
}

void PndMvdMSAnaTask::Finish() {}

std::map<int, std::vector<int>> PndMvdMSAnaTask::AssignHitsToTracks()
{
  std::map<int, std::vector<int>> result;
  for (int i = 0; i < fMCHits->GetEntriesFast(); i++) {         // get all MC Hits
    PndSdsMCPoint *myPoint = (PndSdsMCPoint *)(fMCHits->At(i)); // sort MCHits with Tracks
    // PndMCTrack* myTrack = (PndMCTrack*)(fMCTracks->At(myPoint->GetTrackID()));
    result[myPoint->GetTrackID()].push_back(i);
  }
  return result;
}

ClassImp(PndMvdMSAnaTask);
