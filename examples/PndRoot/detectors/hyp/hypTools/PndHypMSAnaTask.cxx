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
// -----                PndHypMSAnaTask source file             -----
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
#include "PndHypMSAnaTask.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "PndMCTrack.h"
// PndHyp includes
#include "PndHypPoint.h"
#include "FairTrackParH.h"
#include "FairTrackParP.h"

#include "TDatabasePDG.h"

#include <vector>
#include <map>

// -----   Default constructor   -------------------------------------------
PndHypMSAnaTask::PndHypMSAnaTask() : FairTask("Geane Task for PANDA PndHyp"), fEventNr(0)
{
  histo = new TH1F("h1", "h1", 3000, 0, 20);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHypMSAnaTask::~PndHypMSAnaTask()
{
  delete (histo);
  delete fGeoH;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndHypMSAnaTask::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndHypMSAnaTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input arrays
  fMCTracks = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTracks) {
    std::cout << "-W- PndHypMSAnaTask::Init: "
              << "No MCTrack"
              << " array!" << std::endl;
    return kERROR;
  }

  fMCHits = (TClonesArray *)ioman->GetObject("HypPoint");
  if (!fMCHits) {
    std::cout << "-W- PndHypMSAnaTask::Init: "
              << "No MVDPoint"
              << " array!" << std::endl;
    return kERROR;
  }

  /* fTrackParGeane = new TClonesArray("FairTrackParP");
   ioman->Register("GeaneTrackPar","Geane", fTrackParGeane, kTRUE);

   fTrackParIni = new TClonesArray("FairTrackParP");
   ioman->Register("GeaneTrackIni","Geane", fTrackParIni, kTRUE);

   fTrackParFinal = new TClonesArray("FairTrackParP");
   ioman->Register("GeaneTrackFinal","Geane", fTrackParFinal, kTRUE);*/

  // fPro = new FairGeanePro();
  fGeoH = new PndHypGeoHandling(gGeoManager);

  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndHypMSAnaTask::SetParContainers()
{
  // Get Base Container
  // FairRunAna* ana = FairRunAna::Instance();
  // FairRuntimeDb* rtdb=ana->GetRuntimeDb();
}

// -----   Public method Exec   --------------------------------------------
void PndHypMSAnaTask::Exec(Option_t *)
{
  // mcHitMap.clear();						//Track ->  MCHits
  TVector3 vpi2;

  // 	fTrackParGeane->Delete();
  // 	fTrackParIni->Delete();
  // 	fTrackParFinal->Delete();

  mcHitMap = AssignHitsToTracks();

  std::cout << "------------Event " << fEventNr << "-------------" << std::endl;
  fEventNr++;
  std::cout << " size map " << mcHitMap.size() << std::endl;

  for (std::map<int, std::vector<int>>::const_iterator it = mcHitMap.begin(); it != mcHitMap.end(); it++) { // go through all tracks
    PndMCTrack *myTrack = (PndMCTrack *)(fMCTracks->At(it->first));
    std::vector<int> MChits = it->second;

    /*	if(myTrack==0)
      {
      it++;
      continue;
      }
    */

    // if(myTrack->GetPdgCode()>1010000000||myTrack->GetPdgCode()>1020000000)
    // std::cout<<" hypernuclei "<<std::endl;

    if (MChits.size() >= 3) {

      PndHypPoint *startPoint = (PndHypPoint *)(fMCHits->At(0));
      std::cout << " hits " << MChits.size() << (it->first) << std::endl;
      // std::cout << "StartPoint: " << *startPoint << std::endl;

      // PndMCTrack* mctruth=(PndMCTrack*)fMCTracks->At( startPoint->GetTrackID());
      // if(mctruth==0)continue;
      std::cout << " pid " << startPoint->GetTrackID() << " " << it->first << std::endl;
      Int_t MotherId, Motherpdg;
      MotherId = myTrack->GetMotherID();

      if (MotherId == -1)
        Motherpdg = myTrack->GetPdgCode();
      else {
        PndMCTrack *mother = (PndMCTrack *)fMCTracks->At(MotherId);
        Motherpdg = mother->GetPdgCode();
      }
      if ((myTrack->GetPdgCode() == -211) && (Motherpdg > 1010000000 && Motherpdg < 1020050130)) { // startPoint->MomentumIn(vpi2);//vpi2 = mctruth->GetMomentum();
        std::cout << " hits " << MChits.size() << " " << Motherpdg << std::endl;

        histo->Fill(MChits.size()); // vpi2.Mag());
      }

      /*
  TVector3 StartPos(startPoint->GetPosition());
  TVector3 StartPosErr(0.0, 0.0, 0.0);
  TVector3 StartMom(startPoint->GetPx(), startPoint->GetPy(), startPoint->GetPz());
  TVector3 StartMomErr(0.0, 0.0, 0.0);
  TVector3 startO, startU, startV;

  fGeoH->GetOUVId(startPoint->GetDetName(), startO, startU, startV);
  fPro->PropagateFromPlane(startU, startV);

  Int_t PDGCode = myTrack->GetPdgCode();
  TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
  TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
  Double_t  fCharge= fParticle->Charge();

  TClonesArray& clref1 = *fTrackParIni;
  Int_t size1 = clref1.GetEntriesFast();
  FairTrackParP *fStart= new (clref1[size1]) FairTrackParP(StartPos, StartMom, StartPosErr, StartMomErr, fCharge, startO, startU, startV);

  for (int p = 1; p < MChits.size(); p++){											//go through all hits in track
  PndHypPoint* myPoint = (PndHypPoint*)(fMCHits->At(MChits[p]));
  std::cout << "PropagationPoint: " << *myPoint << std::endl;

  TVector3 StopPos(myPoint->GetPosition());
  TVector3 StopPosErr(0.0, 0.0, 0.0);
  TVector3 StopMom(myPoint->GetPx(), myPoint->GetPy(), myPoint->GetPz());
  TVector3 StopMomErr(0.0, 0.0, 0.0);

  TVector3 o, u, v;
  fGeoH->GetOUVId(myPoint->GetDetName(), o, u, v);

  TClonesArray& clref2 = *fTrackParFinal;
  Int_t size2 = clref2.GetEntriesFast();
  FairTrackParP *fStop= new (clref2[size2]) FairTrackParP(StopPos, StopMom, StopPosErr, StopMomErr, fCharge, o, u, v);

  TClonesArray& clref = *fTrackParGeane;
  Int_t size = clref.GetEntriesFast();
  FairTrackParP *fRes=	new(clref[size]) FairTrackParP();

  std::cout << "Propagation Plane:" << std::endl;
  std::cout << "o: " << o[0] << " " << o[1] << " " << o[2] << std::endl;
  std::cout << "u: " << u[0] << " " << u[1] << " " << u[2] << std::endl;
  std::cout << "v: " << v[0] << " " << v[1] << " " << v[2] << std::endl;

  TVector3 endPoint(myPoint->GetPosition());
  //fPro->SetPoint(endPoint);
  //fPro->PropagateToPCA(1);

  fPro->PropagateToPlane(o, u, v);
  if(fCharge>0)fPro->Propagate(fStart, fRes, 211);
  if(fCharge<0)fPro->Propagate(fStart, fRes, -211);

  std::cout << std::endl;
  std::cout << "Propagation Points: " << std::endl;
  std::cout << fRes->GetX() << " +/- " << fRes->GetDX() << std::endl;
  std::cout << fRes->GetY() << " +/- " << fRes->GetDY() << std::endl;
  std::cout << fRes->GetZ() << " +/- " << fRes->GetDZ() << std::endl;
  std::cout << std::endl;

  TVector3 global(fRes->GetX(), fRes->GetY(), fRes->GetZ());
  TVector3 local = fGeoH->MasterToLocalId(global, myPoint->GetDetName());

  std::cout << "Propagation Point local: " << std::endl;
  std::cout << local[0] << " " << local[1] << " " << local[2] << std::endl;
  std::cout << std::endl;
  }
      */
    }
  }
  fMCTracks->Delete();
  fMCHits->Delete();
  Finish();
}

void PndHypMSAnaTask::Finish()
{
  fMCTracks->Delete();
  fMCHits->Delete();
}

std::map<int, std::vector<int>> PndHypMSAnaTask::AssignHitsToTracks()
{
  std::map<int, std::vector<int>> result;
  for (int i = 0; i < fMCHits->GetEntriesFast(); i++) {     // get all MC Hits
    PndHypPoint *myPoint = (PndHypPoint *)(fMCHits->At(i)); // sort MCHits with Tracks
    // if(myPoint->GetTrackID()>1000)continue;
    TVector3 mom;
    myPoint->Momentum(mom);

    //	if(mom.Mag()>0.05){
    // std::cout<<" hyp "<<myPoint->GetTrackID()<<std::endl;
    //}

    PndMCTrack *myTrack = (PndMCTrack *)(fMCTracks->At(myPoint->GetTrackID()));
    result[myPoint->GetTrackID()].push_back(i);
    //}
  }
  return result;
  // result.clear();
}

void PndHypMSAnaTask::WriteHistograms()
{

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();
    outfile->mkdir("Dedx_detid");
    outfile->cd("Dedx_detid");
    outfile->WriteTObject(histo);
    delete histo;
    histo = nullptr;
    outfile->cd("..");
  }

  //  TFile *file = FairRootManager::Instance()->GetOutFile();
  //  file->cd();
  //  file->mkdir("Dedx_detid");
  //  file->cd("Dedx_detid");
  //
  //  histo->Write();
  //  delete histo;
  //  histo = nullptr;
}

ClassImp(PndHypMSAnaTask);
