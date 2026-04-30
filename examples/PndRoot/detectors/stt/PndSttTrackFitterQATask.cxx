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
// -----                PndSttTrackFitterQATask source file             -----
// -------------------------------------------------------------------------

#include "PndSttTrackFitterQATask.h"

#include "PndSttHit.h"
#include "PndSttHitInfo.h"
#include "PndSttPoint.h"
//#include "PndSttSingleStraw.h"
#include "PndSttMapCreator.h"
#include "PndSttTube.h"
#include "PndSttTrack.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndTrackCandHit.h"
#include "PndSttHelixHit.h"
#include "PndMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "FairRootFileSink.h"

#include "TClonesArray.h"
#include "TRandom.h"
#include <TFile.h>
#include "TH1F.h"
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::sqrt;

// -----   Default constructor   -------------------------------------------
PndSttTrackFitterQATask::PndSttTrackFitterQATask() : FairTask("") {}
// -------------------------------------------------------------------------

PndSttTrackFitterQATask::PndSttTrackFitterQATask(Int_t verbose) : FairTask("")
{
  fVerbose = verbose;
}

// -----   Destructor   ----------------------------------------------------
PndSttTrackFitterQATask::~PndSttTrackFitterQATask() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndSttTrackFitterQATask::Init()
{

  hptfit = new TH1F("hptfit", "pt fit: reco - mc", 100, -0.3, 0.3);
  hplfit = new TH1F("hplfit", "pl fit: reco - mc", 100, -0.4, 0.4);
  hptotfit = new TH1F("hptotfit", "ptot fit: reco - mc", 100, -0.3, 0.3);

  hptfit_perc = new TH1F("hptfit_perc", "pt fit: (reco - mc) / mc", 100, -0.3, 0.3);
  hplfit_perc = new TH1F("hplfit_perc", "pl fit: (reco - mc) / mc", 100, -0.4, 0.4);
  hptotfit_perc = new TH1F("hptotfit_perc", "ptot fit: (reco - mc) / mc", 100, -0.3, 0.3);

  hDist = new TH1F("hDist", "Dist fit: reco - mc", 100, -1., 1.);
  hRad = new TH1F("hRad", "Rad fit: reco - mc", 100, -15., 15.);
  hPhi = new TH1F("hPhi", "Phi fit: reco - mc", 100, -0.1, 0.1);
  hTanL = new TH1F("hTanL", "TanL fit: reco - mc", 100, -0.2, 0.2);
  hZ = new TH1F("hZ", "Z fit: reco - mc", 100, -0.3, 0.3);

  hQ = new TH1F("hQ", "Q fit: mc + reco", 6, -3, 3);

  hptfound = new TH1F("hptfound", "pt found: reco - mc", 100, -0.3, 0.3);
  hplfound = new TH1F("hplfound", "pl found: reco - mc", 100, -0.4, 0.4);
  hptotfound = new TH1F("hptotfound", "ptot found: reco - mc", 100, -0.3, 0.3);

  hptfound_perc = new TH1F("hptfound_perc", "pt found: (reco - mc) / mc", 100, -0.3, 0.3);
  hplfound_perc = new TH1F("hplfound_perc", "pl found: (reco - mc) / mc", 100, -0.4, 0.4);
  hptotfound_perc = new TH1F("hptotfound_perc", "ptot found: (reco - mc) / mc", 100, -0.3, 0.3);

  hpxfit = new TH1F("hpxfit", "px fit: reco - mc", 100, -0.3, 0.3);
  hpyfit = new TH1F("hpyfit", "py fit: reco - mc", 100, -0.3, 0.3);
  hpzfit = new TH1F("hpzfit", "pz fit: reco - mc", 100, -0.4, 0.4);

  hresx = new TH1F("hresx", "x: reco - mc", 100, -0.2, 0.2);
  hresy = new TH1F("hresy", "y: reco - mc", 100, -0.2, 0.2);
  hresz = new TH1F("hresz", "z: reco - mc", 100, -3, 3);

  hx = new TH1F("hx", "x no skewed: reco - mc", 100, -0.2, 0.2);
  hy = new TH1F("hy", "y no skewed: reco - mc", 100, -0.2, 0.2);
  hz = new TH1F("hz", "z no skewed: reco - mc", 100, -3, 3);

  hxs = new TH1F("hxs", "x skewed: reco - mc", 100, -0.2, 0.2);
  hys = new TH1F("hys", "y skewed: reco - mc", 100, -0.2, 0.2);
  hzs = new TH1F("hzs", "z skewed: reco - mc", 100, -3, 3);

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndSttHelixHitProducer::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get MCTrack array
  fMCTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTrackArray) {
    LOG(error) << " PndSttTrackFitterQATask::Init: No MCTrack array!";
    return kERROR;
  }

  // Get SttTrack array
  fTrackArray = (TClonesArray *)ioman->GetObject("STTTrack");
  if (!fTrackArray) {
    LOG(error) << " PndSttTrackFitterQATask::Init: No SttTrack array!";
    return kERROR;
  }

  // Get SttFoundTrack array
  fFoundTrackArray = (TClonesArray *)ioman->GetObject("STTFoundTrack");
  if (!fFoundTrackArray) {
    LOG(error) << " PndSttTrackFitterQATask::Init: No SttFoundTrack array!";
    return kERROR;
  }

  // Get SttTrackCand array
  fTrackCandArray = (TClonesArray *)ioman->GetObject("STTTrackCand");
  if (!fTrackCandArray) {
    LOG(error) << " PndSttTrackFitterQATask::Init: No SttTrack Cand  array!";
    return kERROR;
  }

  // MC points
  fPointArray = (TClonesArray *)ioman->GetObject("STTPoint");
  if (!fPointArray) {
    cout << "-W- PndSttTrackFitterQATask::Init: "
         << "No STTPoint array!" << endl;
    return kERROR;
  }

  // hits
  fHitArray = (TClonesArray *)ioman->GetObject("STTHit");
  if (!fHitArray) {
    cout << "-W- PndSttTrackFitterQATask::Init: "
         << "No STTHit array!" << endl;
    return kERROR;
  }

  // helix hits
  fHelixHitArray = (TClonesArray *)ioman->GetObject("SttHelixHit");
  if (!fHelixHitArray) {
    cout << "-W- PndSttTrackFitterQATask::Init: "
         << "No SttHelixHit array!" << endl;
    return kERROR;
  }

  // CHECK added
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();

  LOG(info) << " PndSttTrackFitterQATask: Intialization successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// CHECK added
void PndSttTrackFitterQATask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -----   Public method Exec   --------------------------------------------
void PndSttTrackFitterQATask::Exec(Option_t *)
{
  // Declare some variables
  PndSttTrack *pTrack = nullptr;
  PndTrackCand *pTrackCand = nullptr;
  PndTrack *pFoundTrack = nullptr;
  PndMCTrack *mcTrack = nullptr;

  if (!fTrackArray)
    Fatal("Exec", "No fTrackArray");

  // Loop over fitted tracks
  Int_t nTracks = fTrackArray->GetEntriesFast();

  for (Int_t j = 0; j < nTracks; j++) {
    pTrack = (PndSttTrack *)fTrackArray->At(j);
    if (!pTrack)
      continue;
    Int_t trackCandID = pTrack->GetTrackCandIndex();
    pTrackCand = (PndTrackCand *)fTrackCandArray->At(trackCandID);
    if (!pTrackCand)
      continue;

    bool foundtrack = false;
    for (int k = 0; k < fFoundTrackArray->GetEntriesFast(); k++) {
      pFoundTrack = (PndTrack *)fFoundTrackArray->At(k);
      if (!pFoundTrack)
        continue;
      if (pFoundTrack->GetRefIndex() == trackCandID) {
        foundtrack = true;
        break;
      }
    }
    if (foundtrack == false)
      continue;

    // ================= momentum residual ========================
    if (pTrack->GetFlag() < 3)
      continue; // only prefit-fit-zfit CHECK
    // -------------------------------------------------------------------
    // FITTED track
    // xy
    Int_t h = -(Int_t)pTrack->GetCharge();
    Double_t d0 = pTrack->GetDist();
    Double_t phi0 = pTrack->GetPhi();
    Double_t Rad = pTrack->GetRad();
    // z
    // Double_t z0 = pTrack->GetZ(); //[R.K. 01/2017] unused variable?
    Double_t zslope = pTrack->GetTanL();
    // center of curvature of helix
    TVector2 vec((d0 + Rad) * cos(phi0), (d0 + Rad) * sin(phi0));

    Double_t ptran = 0.003 * 2 * Rad;
    Double_t plong = ptran * zslope;
    Double_t ptot = sqrt(plong * plong + ptran * ptran);

    // -------------------------------------------------------------------
    // FOUND track
    TVector3 foundMom = pFoundTrack->GetParamFirst().GetMomentum();
    // Double_t momMag = foundMom.Mag(); //[R.K. 01/2017] unused variable?

    // -------------------------------------------------------------------
    // MC track
    Int_t mcIndex = pTrackCand->getMcTrackId();
    mcTrack = (PndMCTrack *)fMCTrackArray->At(mcIndex);
    if (!mcTrack)
      continue;
    // if(mcTrack->GetMotherID() != -1)  continue;
    TVector3 mcmom = mcTrack->GetMomentum();
    TVector3 vertex = mcTrack->GetStartVertex();

    int mcQ = (int)((TDatabasePDG::Instance())->GetParticle(mcTrack->GetPdgCode())->Charge() / 3.);
    hQ->Fill(-h + mcQ);

    hptfit->Fill(ptran - mcmom.Perp());
    hplfit->Fill(plong - mcmom.Z());
    hptotfit->Fill(ptot - mcmom.Mag());

    hptfound->Fill(foundMom.Perp() - mcmom.Perp());
    hplfound->Fill(foundMom.Z() - mcmom.Z());
    hptotfound->Fill(foundMom.Mag() - mcmom.Mag());

    hptfit_perc->Fill((ptran - mcmom.Perp()) / mcmom.Perp());
    hplfit_perc->Fill((plong - mcmom.Z()) / mcmom.Z());
    hptotfit_perc->Fill((ptot - mcmom.Mag()) / mcmom.Mag());

    hptfound_perc->Fill((foundMom.Perp() - mcmom.Perp()) / mcmom.Perp());
    hplfound_perc->Fill((foundMom.Z() - mcmom.Z()) / mcmom.Z());
    hptotfound->Fill((foundMom.Mag() - mcmom.Mag()) / mcmom.Mag());

    //     cout << "PT   : " << mcmom.Perp() << " " << foundMom.Perp() << " " << ptran << endl;
    //     cout << "PL   : " << mcmom.Z()    << " " << foundMom.Z()    << " " << plong << endl;
    //     cout << "PTOT : " << mcmom.Mag()  << " " << foundMom.Mag()  << " " << ptot << endl;
    //     cout << "--------------------------------------------------" << endl;

    // === momentum coordinates ===
    TVector3 fitmom(ptran * TMath::Cos(phi0 - h * TMath::Pi() / 2.), ptran * TMath::Sin(phi0 - h * TMath::Pi() / 2.), plong);

    hpxfit->Fill(fitmom.X() - mcmom.X());
    hpyfit->Fill(fitmom.Y() - mcmom.Y());
    hpzfit->Fill(fitmom.Z() - mcmom.Z());

    // ================= parameters residual ========================
    double mcRad = mcmom.Perp() / 0.006;

    // track from tangent ---------------------
    // double mc_m1 = mcmom.Y() / mcmom.X(); //[R.K. 01/2017] unused variable?
    // double mc_q1 = vertex.Y() - vertex.X() * mc_m1; //[R.K. 01/2017] unused variable?
    // double mc_m2 = -1./mc_m1; //[R.K. 01/2017] unused variable?
    // double mc_q2 = vertex.Y() - vertex.X() * mc_m2; //[R.K. 01/2017] unused variable?

    double alpha = TMath::ATan2(mcmom.X(), mcmom.Y());
    double mcX0, mcY0;
    if (-h > 0) {
      mcX0 = vertex.X() + mcRad * TMath::Cos(alpha);
      mcY0 = vertex.Y() - mcRad * TMath::Sin(alpha);
    } else {
      mcX0 = vertex.X() - mcRad * TMath::Cos(alpha);
      mcY0 = vertex.Y() + mcRad * TMath::Sin(alpha);
    }

    Double_t mcDist, mcPhi;

    mcDist = TMath::Sqrt(mcX0 * mcX0 + mcY0 * mcY0) - mcRad;
    mcPhi = atan2(mcY0, mcX0);

    Double_t mcTanL; //[R.K. 01/2017] unused variable?//, mcZ;
    mcTanL = mcmom.Z() / mcmom.Perp();

    // mcZ = ??

    if (pTrack->GetFlag() >= 2)
      hDist->Fill(d0 - mcDist);
    if (pTrack->GetFlag() >= 2)
      hRad->Fill(Rad - mcRad);
    if (pTrack->GetFlag() >= 2)
      hPhi->Fill(phi0 - mcPhi);
    if (pTrack->GetFlag() >= 3)
      hTanL->Fill(zslope - mcTanL);
    //    if(pTrack->GetFlag() >= 3)  hZ->Fill(z0 - mcZ); // ??

    // ========================= helix hits ==============================
    Int_t hitcounter = pTrack->GetNofHelixHits();
    TVector2 point; // point

    for (Int_t k = 0; k < hitcounter; k++) {
      Int_t iHHit = pTrack->GetHelixHitIndex(k);
      PndSttHelixHit *helixhit = (PndSttHelixHit *)fHelixHitArray->At(iHHit);
      Int_t hitindex = helixhit->GetHitIndex();
      PndSttHit *hit = (PndSttHit *)fHitArray->At(hitindex);

      // tubeID  CHECK added
      Int_t tubeID = hit->GetTubeID();
      PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

      PndSttPoint *mcpoint = (PndSttPoint *)fPointArray->At(hit->GetRefIndex());

      hresx->Fill(helixhit->GetX() - mcpoint->GetX());
      hresy->Fill(helixhit->GetY() - mcpoint->GetY());
      hresz->Fill(helixhit->GetZ() - mcpoint->GetZ());

      if (tube->GetWireDirection() == TVector3(0, 0, 1)) {
        hx->Fill(helixhit->GetX() - mcpoint->GetX());
        hy->Fill(helixhit->GetY() - mcpoint->GetY());
        hz->Fill(helixhit->GetZ() - mcpoint->GetZ());
      } else {
        hxs->Fill(helixhit->GetX() - mcpoint->GetX());
        hys->Fill(helixhit->GetY() - mcpoint->GetY());
        hzs->Fill(helixhit->GetZ() - mcpoint->GetZ());
      }
    }
  }
}
// -------------------------------------------------------------------------

void PndSttTrackFitterQATask::WriteHistograms()
{
//  TFile *file = FairRootManager::Instance()->GetOutFile();
//  file->cd();
//  file->mkdir("SttFitterQATask");
//  file->cd("SttFitterQATask");

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();
    outfile->mkdir("SttFitterQATask");
    outfile->cd("SttFitterQATask");

    outfile->WriteTObject(hptfit);
    delete hptfit;
    outfile->WriteTObject(hplfit);
    delete hplfit;
    outfile->WriteTObject(hptotfit);
    delete hptotfit;

    outfile->WriteTObject(hptfit_perc);
    delete hptfit_perc;
    outfile->WriteTObject(hplfit_perc);
    delete hplfit_perc;
    outfile->WriteTObject(hptotfit_perc);
    delete hptotfit_perc;

    outfile->WriteTObject(hpxfit);
    delete hpxfit;
    outfile->WriteTObject(hpyfit);
    delete hpyfit;
    outfile->WriteTObject(hpzfit);
    delete hpzfit;

    outfile->WriteTObject(hDist);
    delete hDist;
    outfile->WriteTObject(hRad);
    delete hRad;
    outfile->WriteTObject(hPhi);
    delete hPhi;
    outfile->WriteTObject(hTanL);
    delete hTanL;
    outfile->WriteTObject(hZ);
    delete hZ;
    outfile->WriteTObject(hQ);
    delete hQ;

    outfile->WriteTObject(hptfound);
    delete hptfound;
    outfile->WriteTObject(hplfound);
    delete hplfound;
    outfile->WriteTObject(hptotfound);
    delete hptotfound;

    outfile->WriteTObject(hptfound_perc);
    delete hptfound_perc;
    outfile->WriteTObject(hplfound_perc);
    delete hplfound_perc;
    outfile->WriteTObject(hptotfound_perc);
    delete hptotfound_perc;

    outfile->WriteTObject(hresx);
    delete hresx;
    outfile->WriteTObject(hresy);
    delete hresy;
    outfile->WriteTObject(hresz);
    delete hresz;

    outfile->WriteTObject(hx);
    delete hx;
    outfile->WriteTObject(hy);
    delete hy;
    outfile->WriteTObject(hz);
    delete hz;

    outfile->WriteTObject(hxs);
    delete hxs;
    outfile->WriteTObject(hys);
    delete hys;
    outfile->WriteTObject(hzs);
    delete hzs;
  }
}

ClassImp(PndSttTrackFitterQATask)
