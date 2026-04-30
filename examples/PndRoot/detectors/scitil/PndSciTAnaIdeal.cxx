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
// -----                PndSciTAnaIdeal source file             -----
// -----                  Created 10/01/06 by A.Sanchez             -----
// -------------------------------------------------------------------------

#include <cmath>

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "FairRootManager.h"
#include "FairGeoNode.h"
#include "PndSciTAnaIdeal.h"
#include "PndSciTPoint.h"

#include "PndRiemannHit.h"

#include "RhoCandidate.h"
#include "PndPidCandidate.h"
#include "RhoEventShape.h"
#include "RhoCandList.h"

#include "RhoFactory.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"
#include "FairLogger.h"
// Root includes
#include "TROOT.h"
#include "TString.h"

// -----   Default constructor   -------------------------------------------
PndSciTAnaIdeal::PndSciTAnaIdeal() : FairTask("Ideal PndSciT Hit Producer"), evt(0), friemann(false), fLHcut(false), fScFtime(1.), fstt(false)
{
  fFileName = "";

  fBranchName = "SciTPoint";

  histo = new TH2F("h1", "h1", 300, -2, 2, 300, 0, 10);
  histpi = new TH2F("h", "h", 300, 0, 1.5, 300, 0, 10);
  histk = new TH2F("hk", "hk", 200, 0, 2, 200, 0, 1.5);
  histp = new TH2F("hp", "hp", 300, 0, 1.5, 300, 0, 10);
  // histo = new TH2F("h1","h1",3000,0,0.02,3000,0,0.02);
  map1 = new TH2F("h2", "h2", 3000, 0, 30, 3000, 0, 1);
  map2 = new TH2F("h3", "h3", 100, -4, 4, 100, -4, 4);
  map3 = new TH2F("h4", "h4", 100, -4, 4, 100, -4, 4);
  map4 = new TH2F("h5", "h5", 100, -4, 4, 100, -4, 4);

  Dpi1 = new TH1F("hpi1", "hpi1", 300, 0, 2.);
  Dpi2 = new TH1F("hpi2", "hpi2", 300, 0, 40.);
  Dp2 = new TH1F("hp2", "hp2", 300, 0, 2);
  Dpix = new TH1F("hpix", "hpix", 300, 0, 2.);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSciTAnaIdeal::~PndSciTAnaIdeal()
{
  delete (histo);
  delete fread;
  delete fGeoH;
}

void PndSciTAnaIdeal::SetParContainers()
{
  // Get Base Container
  //  FairRunAna* ana = FairRunAna::Instance();
  //   FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  //
}

InitStatus PndSciTAnaIdeal::ReInit()
{
  InitStatus stat = kERROR;
  return stat;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndSciTAnaIdeal::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndSciTAnaIdeal::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject(fBranchName);

  if (!fPointArray) {
    std::cout << "-W- PndSciTAnaIdeal::Init: "
              << "No SciTPoint array!" << std::endl;
    return kERROR;
  }

  fmcArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fmcArray) {
    std::cout << "-W- PndSciTAnaIdeal::Init: "
              << "No SciTPoint array!" << std::endl;
    return kERROR;
  }
  // Create and register output array

  LOG(info) << " PndSciTAnaIdeal: Intialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndSciTAnaIdeal::Exec(Option_t *)
{

  ++evt;
  fTrigArray->Delete();
  fHitOutputArray->Delete();
  if (fMicroCandidates->GetEntriesFast() != 0)
    fMicroCandidates->Delete();

  std::cout << "**** PndSciTAnaIdeal::Exec  *****" << evt << std::endl;
  Double_t gam;

  TVector3 vecs, pos, vecsV, vecsc, posV, momV;
  int mcpdg = -1;
  int scpdg = -1;
  int Idtof, Idsci;
  Int_t event;
  event = -1;

  Double_t mult, timeSciT, timeFib, time, Ssci, Stof, length, bet;
  Double_t R;

  Double_t sigt, sigL, sigt1, sigL1, x, y, z, sigx, sigy, sigz;

  timeSciT = 0.;
  timeFib = 0.;
  x = 0.;
  gam = 0.;
  Stof = 0.;
  Ssci = 0.;
  R = 0.;
  sigx = sigt = sigL = sigt1 = sigL1 = 0.;

  Int_t iStrip = 0;

  std::vector<unsigned int> tpchits;
  std::vector<unsigned int> scifhits;
  std::vector<unsigned int> tofhits;
  std::vector<Double_t> reltime;

  Int_t ntrack = fTrackCandArray->GetEntriesFast();
  Int_t nhit = fSTPointArray->GetEntriesFast();
  Int_t nhyp = fHPointArray->GetEntriesFast();

  unsigned int detId, sfdetId, fhitId, lhitId, n, det, hit, tpdetid, tphitid;
  Int_t MotherId;

  detId = fhitId = lhitId = n = sfdetId = det = hit = -1;
  Int_t kaon;

  kaon = 0;
  PndSciTMassTrig *trig = new PndSciTMassTrig();
  Double_t Phi;
  Phi = 0.;

  Double_t Lk, Lpi, Lp, Lmu, Le;
  Double_t tk, tp, tpi, te, tmu;

  for (Int_t iPoint = 0; iPoint < ntrack; iPoint++) {
    GFTrackCand *track = (GFTrackCand *)fTrackCandArray->At(iPoint);
    // std::cout << " Ideal Hit Producer -Point-: " << nPoints << std::endl;
    if (track == 0)
      continue;

    PndSciTPoint *Hit;

    Hit = (PndSciTPoint *)(fPointArray->At(lhitId));

    if (Hit == 0) {
      // std::cout<<" to ze "<<std::endl;
      continue;
    }

    PndMCTrack *mctruth = (PndMCTrack *)fmcArray->At(Hit->GetTrackID());
    if (mctruth == 0)
      continue;
    // AddAndExpand(point->GetTrackID(),1,iPoint);

    MotherId = mctruth->GetMotherID();

    // if(MotherId!=-1)continue;

    mcpdg = mctruth->GetPdgCode();

    double q;
    double charg;
    double pfq;

    if (mcpdg > 5000) {

      q = GetChargeIon(mcpdg);
      // std::cout<<" particle "<<q<<std::endl;
    } else {
      q = TDatabasePDG::Instance()->GetParticle(mcpdg)->Charge() / 3.;
      // std::cout<<" particle "<<q<<std::endl;
    }
    fQ = q;
    TVector3 pos1;
    sfHit->Position(pos1);
    TVector3 pos2;
    Hit->Position(pos2);
    PndRiemannTrack *RT;

    double x0;
    double y0;
    Double_t S;
    S = 0;
    Double_t sinth;

    if (friemann) {

      RT = GetRiemannTrack(track);
      TVectorT<double> dVec = RT->orig();
      x0 = dVec[0];
      y0 = dVec[1];

      Double_t param[3] = {x0, y0, RT->r()};

      // tpcHit->Position(pos1);

      Phi = GetDPhi(param, pos1, pos2);
      sinth = RT->dip(); // sintheta=cosLambda
      Double_t tanL = sqrt(1. - sinth * sinth) / sinth;
      //(RT->sign()>0 ? pfq = -1.: pfq = 1.);//tanL

      S = RT->r() * Phi / sinth;

      TVector2 centre(x0, y0);
      Double_t fi0 = centre.Phi(); // 0 - 2Pi
      Double_t d0 = fQ * (TMath::Sqrt(x0 * x0 + y0 * y0) - param[2]);
      TVector2 pivot(-x0, -y0);
      TVector2 pnt2(pos2.X() - x0, pos2.Y() - y0);
      Double_t phi2 = pnt2.DeltaPhi(pivot);
      Double_t pT = fQ * 0.3 * RT->r() * 0.01;

      fvecp.SetXYZ(-pT * sin(phi2 + fi0), pT * cos(phi2 + fi0), pT * tanL);
    }

    // std::cout << "Riemann Fit r: " << RT.r() << " offset: "
    // 		  << x<< " " << y << " tandip "
    // 		  << RT.m()<<" z0 "<<RT.t()<<" phi "<<fabs(S)<<std::endl;
    // 	std::cout << " Pt "<<0.3*RT.r()*0.01<<" P "<<(0.3*RT.r()*0.01)/sinth<<std::endl;
    // 	std::cout << " sinth "<<sinth<<" sinth v "<<momV.Perp()/momV.Mag()<<std::endl;

    momV = mctruth->GetMomentum();
    // Hit->Momentum(momV);

    if (friemann)
      x = (0.3 * RT->r() * 0.01) / sinth; // helix riemanntrack
    else
      x = momV.Mag(); // y = momV.y();z = momV.z();

    if (x > 2.)
      continue;

    R = fabs(momV.Perp() / 0.3 * q);
    // if(2*R<0.4335)continue;

    sigx = r->Gaus(0, 0.01 * momV.Mag());

    x += sigx;
    sigt = sigt1 = 0.;
    te = tk = tp = tmu = tpi = 0;
    Le = Lp = Lmu = Lpi = Lk = 0;

    event = Hit->GetEventID();
    // sigt = 0.08;//100ps
    if (fScFtime == 1)
      sigt = 0.;
    else if (fScFtime >= 0.08)
      sigt = gRandom->Gaus(0, 0.08);
    timeSciT = Hit->GetTime();
    timeSciT += sigt;

    Stof = Hit->GetLength();
    // sigL = 0.01*Stof;//1%
    sigL = r->Gaus(0, 0.01 * Stof);

    Stof += sigL;

    // kaon selector
    // sigma =200ps/80ps;->sigkaon=0.150GeV,meanKaon=0.489Gev
    // Cut on low mass range=0.355 Gev

    // **** first fiber ring *****

    Ssci = sfHit->GetLength();
    // sigL1 = 0.01*Ssci;//1%
    sigL1 = r->Gaus(0, 0.01 * Ssci);
    Ssci += sigL1;
    // 	  double
    timeFib = sfHit->GetTime();
    // 	//sigt1 = 0.200;//0.450ns//1ns
    if (fScFtime == 1)
      sigt1 = 0.;
    else
      sigt1 = gRandom->Gaus(0, fScFtime);
    timeFib += sigt1;

    time = fabs(timeSciT - timeFib);
    // time=fabs(timeSciT);

    length = fabs(Stof - Ssci);

    // std::cout<<"length "<<length*momV.Perp()/momV.Mag()<<std::endl;

    bet = (length * 0.01) / (0.3 * time); // c=0.3 m/ns;bet=v/c
    // bet = (fabs(S)*0.01)/(0.3*time);
    double m;
    if (friemann)
      m = x * sqrt(((0.09 * time * time) / (fabs(S) * 0.01 * fabs(S) * 0.01)) - 1.);
    else
      m = x * sqrt(((0.09 * time * time) / (length * 0.01 * length * 0.01)) - 1.);

    // if(friemann)std::cout<<" charge "<<fQ<<" prefitter "
    //<<double(TMath::Sign(1.,pfq))<<" dip "<<RT->sign()<<std::endl;

    // if(mcpdg==321)++kaon;
    // std::cout<<" pdg "<<mcpdg<<std::endl;
    if (q > 0)
      charg = 1.;
    if (q < 0)
      charg = -1.;
    Double_t qp = charg * x;
    TLorentzVector lv;

    if (friemann)
      lv.SetXYZT(fvecp.x(), fvecp.y(), fvecp.z(), m);
    else
      lv.SetXYZT(momV.x(), momV.y(), momV.z(), m);

    tk = ((length * 0.01) / 0.3) * sqrt(((0.490 * 0.490) / (x * x)) + 1.);
    tpi = ((length * 0.01) / 0.3) * sqrt(((0.139 * 0.139) / (x * x)) + 1.);
    tp = ((length * 0.01) / 0.3) * sqrt(((0.938 * 0.938) / (x * x)) + 1.);
    tmu = ((length * 0.01) / 0.3) * sqrt(((0.105 * 0.105) / (x * x)) + 1.);
    te = ((length * 0.01) / 0.3) * sqrt(((0.0005 * 0.0005) / (x * x)) + 1.);

    Lk = TMath::Gaus(tk, time, fScFtime) / (TMath::Gaus(tk, time, fScFtime) + TMath::Gaus(tpi, time, fScFtime) + TMath::Gaus(tp, time, fScFtime) +
                                            TMath::Gaus(tmu, time, fScFtime) + TMath::Gaus(te, time, fScFtime));

    Lp = TMath::Gaus(tp, time, fScFtime) / (TMath::Gaus(tk, time, fScFtime) + TMath::Gaus(tpi, time, fScFtime) + TMath::Gaus(tp, time, fScFtime) +
                                            TMath::Gaus(tmu, time, fScFtime) + TMath::Gaus(te, time, fScFtime));

    Lpi = TMath::Gaus(tpi, time, fScFtime) / (TMath::Gaus(tk, time, fScFtime) + TMath::Gaus(tpi, time, fScFtime) + TMath::Gaus(tp, time, fScFtime) +
                                              TMath::Gaus(tmu, time, fScFtime) + TMath::Gaus(te, time, fScFtime));

    Lmu = TMath::Gaus(tmu, time, fScFtime) / (TMath::Gaus(tk, time, fScFtime) + TMath::Gaus(tpi, time, fScFtime) + TMath::Gaus(tp, time, fScFtime) +
                                              TMath::Gaus(tmu, time, fScFtime) + TMath::Gaus(te, time, fScFtime));

    Le = TMath::Gaus(te, time, fScFtime) / (TMath::Gaus(tk, time, fScFtime) + TMath::Gaus(tpi, time, fScFtime) + TMath::Gaus(tp, time, fScFtime) +
                                            TMath::Gaus(tmu, time, fScFtime) + TMath::Gaus(te, time, fScFtime));

    // std::cout << " Pt v "<<momV.Perp()<<" P v "<<momV.Mag()<<std::endl;
    // if (bet<1.&&bet>0.&&time<100&&time>1&&length<2000&&length>0&&q!=0)
    // if (bet<1.&&bet>0.&&q!=0)
    //{
    if (bet == 1.) {
      gam = 99999;
    } else
      gam = 1. / sqrt(1. - (bet * bet));

    if (fLHcut) {
      if (Lk > 0.4)
        ++kaon;
    } else {
      if (mcpdg == 321)
        ++kaon;
    }

    Dpi2->Fill(time);
    histo->Fill(charg * x, m / x); // 1/(gam*bet));
    if (q < 0)
      histp->Fill(x, m / x); // 1/(gam*bet));
    if (q > 0)
      histpi->Fill(x, m / x); // 1/(gam*bet));

    // if(q>0)
    histk->Fill(x, m * m);
    // if(mcpdg==321||mcpdg==211)
    if (q < 0)
      Dpi1->Fill(m); // x/(gam*bet));
    if (q > 0)
      Dp2->Fill(m); //(x/(gam*bet)));

    // Int_t count = fTrigArray->GetEntriesFast();
    // 	    trig->SetEvt(event);
    // 	    trig = new ((*fTrigArray)[count])PndSciTMassTrig(Hit->GetTrackID(),iPoint,bet,charg*x,time,length);
    //}

    /*
      Int_t count = fTrigArray->GetEntriesFast();

      trig = new ((*fTrigArray)[count])PndSciTMassTrig(Hit->GetTrackID(),iPoint,
      bet,qp ,time,length);
      trig->SetEvt(event);
      trig->SetP(momV);*/

    Int_t micsize = fMicroCandidates->GetEntriesFast();
    PndPidCandidate *micro = new ((*fMicroCandidates)[micsize]) PndPidCandidate((Int_t)fQ, pos1, lv);
    micro->SetFirstHit(pos1);
    micro->SetLastHit(pos2);
    if (friemann)
      micro->SetTrackLength(fabs(S));
    else
      micro->SetTrackLength(length);
    micro->SetMcIndex(Hit->GetTrackID());
    micro->SetTrackIndex(iPoint);
    micro->SetSciTStopTime(time);
    micro->SetSciTM2(m * m);
    micro->SetPionPidLH(Lpi);
    micro->SetKaonPidLH(Lk);
    micro->SetProtonPidLH(Lp);
    micro->SetMuonPidLH(Lmu);
    micro->SetElectronPidLH(Le);
  }

  Int_t count = fTrigArray->GetEntriesFast();
  new ((*fTrigArray)[count]) PndSciTMassTrig(event, kaon);

  std::cout << "evt " << evt << " k " << kaon << std::endl;

  std::cout << " entries of PndSciTMassTrig " << fTrigArray->GetEntriesFast() << std::endl;
  std::cout << " entries of PndSciTMicro " << fMicroCandidates->GetEntriesFast() << std::endl;

  // *** Appl. kaon multiplicity Trigger to Stopped Xi- events
  // *** cheking the efficiency of tof system.
  // ** trigger on Silicon tracker

  PndHypPoint *stg = (PndHypPoint *)(fHPointArray->At(0));
  if (stg != 0) {

    if ((stg->GetEventID() == event) && kaon > 0) {
      std::cout << " tof trig -> si " << event << " " << stg->GetEventID() << std::endl;

      for (Int_t iHypHit = 0; iHypHit < nhyp; ++iHypHit) {
        fCurrentHypPoint = (PndHypPoint *)fHPointArray->At(iHypHit);

        int size = fHitOutputArray->GetEntriesFast();
        InitTransMat();
        fCurrentHypPoint->PositionIn(posV);
        smearLocal(posV);
        TVector3 dposLocal(0.01, 0.01, 0.05);

        new ((*fHitOutputArray)[size])
          PndHypHit(fCurrentHypPoint->GetVolumeID(), (fCurrentHypPoint->GetDetName()).Data(), posV, dposLocal, iHypHit, fCurrentHypPoint->GetEnergyLoss(), 1);
      }
    }
  }

  if (fFileName != "")
    SetSciTEventCorr(event, kaon);

  // Event summary

  fsciFPointArray->Delete();
  fSTPointArray->Delete();
  fHPointArray->Delete();
  fPointArray->Delete();
  fmcArray->Delete();
  fTrackCandArray->Delete();
  fTpcArray->Delete();
}
// -------------------------------------------------------------------------
void PndSciTAnaIdeal::Finish()
{
  if (fFileName != "") {

    fFile->Write();
    fFile->Close();

    delete fEvt;
    delete fFile;
  }

  cout << " -I PndHyp::FinishRun():closing and deleting fFile fEvt " << endl;
}
void PndSciTAnaIdeal::SetSciTEventCorr(Int_t ev, Int_t mult)
{
  TVector3 vecs, pos;
  Int_t nhit = fSTPointArray->GetEntriesFast();
  PndHypPoint *stg = (PndHypPoint *)(fSTPointArray->At(0));

  if (stg != 0) {
    if (ev == stg->GetEventID() && (mult > 0)) {
      for (Int_t ihit = 0; ihit < nhit; ihit++) {
        PndHypPoint *Hit = (PndHypPoint *)(fSTPointArray->At(ihit));
        if (Hit == 0) {
          // std::cout<<" to ze "<<std::endl;
          continue;
        }
        std::cout << " it " << Hit->GetEventID() << " ev " << ev << endl;
        // if(mult==0) continue;

        fEvt->Clear();
        Int_t cnt = 0;

        fread->GetFragment(ev);

        TLorentzVector P, V;
        Int_t ntr = fread->GetNtr();
        Int_t pid, AI, ZI, LI;
        Double_t mf;
        TVector3 pv;
        Hit->PositionOut(pos);
        Hit->MomentumIn(vecs);

        Dpix->Fill(vecs.Mag());

        for (Int_t i = 0; i < ntr; i++) {

          fread->GetData(i, pv, pid, mf);
          Int_t tt;

          Double_t Energy = 0.;
          Energy = sqrt(pv.Mag2() + (mf * mf));

          // cout<<" track "<<i<<" "<<pid<<" "<<mf<<endl;
          // cout<<" track "<<pv.x()<<" "<<pv.y()<<" "<<pv.z()<<endl;

          P.SetPxPyPzE(pv.x(), pv.y(), pv.z(), Energy);
          V.SetX(pos.x());
          V.SetY(pos.y());
          V.SetZ(pos.z());

          fread->GetAZH(pid, AI, ZI, LI);
          // cout<<" track "<<i<<" "<<pid<<" A "<<AI<<" Z "<<ZI<<" L "<<LI<<endl;

          THParticle fparticle(pid, 1, ev, 0, mf, AI, ZI, LI, P, V);
          new ((*fEvt)[cnt++]) THParticle(fparticle);
        }

        activeCnt = cnt;
        t->Fill();
      }
    }
  }
}

void PndSciTAnaIdeal::InitTransMat()
{
  gGeoManager->cd(fGeoH->GetPath(fCurrentHypPoint->GetDetName()).Data());
  fCurrentTransMat = gGeoManager->GetCurrentMatrix();
}

void PndSciTAnaIdeal::smear(TVector3 &pos)
{
  /// smear a 3d vector

  double sigx = gRandom->Gaus(0, 0.01);
  double sigy = gRandom->Gaus(0, 0.01);
  double sigz = gRandom->Gaus(0, 0.005);

  double x = pos.x() + sigx;
  double y = pos.y() + sigy;
  double z = pos.z() + sigz;

  pos.SetXYZ(x, y, z);

  return;
}

void PndSciTAnaIdeal::smearLocal(TVector3 &pos)
{
  /// smear a 3d vector in the local sensor plane

  Double_t posLab[3], posSens[3];

  posLab[0] = pos.x();
  posLab[1] = pos.y();
  posLab[2] = pos.z();
  fCurrentTransMat->MasterToLocal(posLab, posSens);

  pos.SetXYZ(posSens[0], posSens[1], posSens[2]);

  smear(pos); // apply a gaussian

  posSens[0] = pos.x();
  posSens[1] = pos.y();
  posSens[2] = pos.z();
  fCurrentTransMat->LocalToMaster(posSens, posLab);
  pos.SetXYZ(posLab[0], posLab[1], posLab[2]);

  return;
}
PndRiemannTrack *PndSciTAnaIdeal::GetRiemannTrack(GFTrackCand *cand)
{
  PndRiemannTrack *result = new PndRiemannTrack();
  // result->SetVerbose(fVerbose);
  for (int i = 0; i < cand->getNHits(); i++) {
    unsigned int detId, hitId;
    cand->getHit(i, detId, hitId);
    TVector3 pos(0., 0., 0.);
    TVector3 dpos(0., 0., 0.);

    FairMCPoint *myHit;

    if (detId == 1) {
      myHit = (FairMCPoint *)fsciFPointArray->At(hitId);
    } else if (detId == 2) {
      myHit = (FairMCPoint *)fTpcArray->At(hitId);
    } else if (detId == 3) {
      myHit = (FairMCPoint *)fPointArray->At(hitId);
    }

    if (myHit != 0) {

      PndRiemannHit hit;
      hit.setXYZ(myHit->GetX(), myHit->GetY(), myHit->GetZ());
      hit.setDXYZ(0.01, 0.01, 0.05);
      result->addHit(hit);
    }
  }

  result->refit();
  result->szFit();

  return result;
}

Double_t PndSciTAnaIdeal::GetDPhi(Double_t *par, TVector3 &v1, TVector3 &v2)
{
  Double_t DPhi;
  DPhi = 0.;
  Double_t r, Q, x0, y0;
  x0 = par[0];
  y0 = par[1];
  r = par[2];
  (fQ > 0 ? Q = 1. : Q = -1.);

  TVector2 centre(x0, y0);
  Double_t fi0 = centre.Phi(); // 0 - 2Pi
  Double_t d0 = Q * (TMath::Sqrt(x0 * x0 + y0 * y0) - r);
  TVector3 pivot(-x0, -y0, 0.);

  TVector3 pnt1(v1.X() - x0, v1.Y() - y0, 0.);
  Double_t phi1; // = pnt1.DeltaPhi(pivot);
  TVector3 pnt2(v2.X() - x0, v2.Y() - y0, 0.);
  Double_t phi2; //= pnt2.DeltaPhi(pivot);
  Double_t cosPhi1 = (pnt1 * pivot) / (pnt1.Mag() * pivot.Mag());
  Double_t cosPhi2 = (pnt2 * pivot) / (pnt2.Mag() * pivot.Mag());

  // std::cout<<" phi1 "<<cosPhi1<<" "<<cosPhi2<<std::endl;

  if (cosPhi1 >= 0.)
    phi1 = acos(cosPhi1);
  else
    phi1 = TMath::Pi() + acos(cosPhi1);
  if (cosPhi2 >= 0.)
    phi2 = acos(cosPhi2);
  else
    phi2 = TMath::Pi() + acos(cosPhi2);
  // pnt2.DeltaPhi(pnt1);//phi2-phi1;
  if (phi2 - phi1 > 0)
    DPhi = (phi2 - phi1);
  if (phi2 - phi1 < 0)
    DPhi = (phi2 - phi1);

  // std::cout<<" phi1 "<<acos(cosPhi1)<<" "<<acos(cosPhi2)<<std::endl;

  return DPhi;
}

// ___________________________________________________________________________

Int_t PndSciTAnaIdeal::GetChargeIon(Int_t ion)
{
  Int_t A, Z, L;

  if (ion > 1000000000 && (ion < 1010000000)) {
    ion -= 1000000000;
    Z = ion / 10000;
    ion -= 10000 * Z;
    A = ion / 10;
    // cout<<" ion charge "<<Z<<endl;

    return Z;
  }
  if ((ion > 1010000000 || ion > 1020000000)) {
    ion -= 1000000000;
    L = ion / 10000000;
    ion -= 10000000 * L;
    Z = ion / 10000;
    ion -= 10000 * Z;
    A = ion / 10;
    // cout<<L<<" hypernuclei charge "<<Z<<endl;

    return Z;
  }
}
void PndSciTAnaIdeal::WriteHistograms()
{

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();
    outfile->mkdir("tof_trig");
    outfile->cd("tof_trig");

    outfile->WriteTObject(histo);
    delete (histo);
    histo = nullptr;

    outfile->WriteTObject(histpi);
    delete (histpi);
    histpi = nullptr;

    outfile->WriteTObject(histk);
    delete (histk);
    histk = nullptr;

    outfile->WriteTObject(histp);
    delete (histp);
    histp = nullptr;

    outfile->WriteTObject(map1);
    delete (map1);
    map1 = nullptr;

    outfile->WriteTObject(map2);
    delete (map2);
    map2 = nullptr;

    outfile->WriteTObject(map3);
    delete (map3);
    map3 = nullptr;

    outfile->WriteTObject(map4);
    delete (map4);
    map4 = nullptr;

    outfile->WriteTObject(Dpi1);
    delete (Dpi1);
    Dpi1 = nullptr;

    outfile->WriteTObject(Dpi2);
    delete (Dpi2);
    Dpi2 = nullptr;

    outfile->WriteTObject(Dp2);
    delete (Dp2);
    Dp2 = nullptr;

    outfile->WriteTObject(Dpix);
    delete (Dpix);
    Dpix = nullptr;

    outfile->cd("..");

    outfile->Close();
    delete (outfile);
  }

  //  TFile *file = FairRootManager::Instance()->GetOutFile();
  //  file->cd();
  //  file->mkdir("tof_trig");
  //  file->cd("tof_trig");
  //
  //  histo->Write();
  //  delete histo;
  //  histo = nullptr;
  //
  //  histpi->Write();
  //  delete histpi;
  //  histpi = nullptr;
  //
  //  histk->Write();
  //  delete histk;
  //  histk = nullptr;
  //
  //  histp->Write();
  //  delete histp;
  //  histp = nullptr;
  //
  //  map1->Write();
  //  delete map1;
  //  map1 = nullptr;
  //  map2->Write();
  //  delete map2;
  //  map2 = nullptr;
  //  map3->Write();
  //  delete map3;
  //  map3 = nullptr;
  //  map4->Write();
  //  delete map4;
  //  map4 = nullptr;
  //
  //  Dpi1->Write();
  //  delete Dpi1;
  //  Dpi1 = nullptr;
  //
  //  Dpi2->Write();
  //  delete Dpi2;
  //  Dpi2 = nullptr;
  //
  //  Dp2->Write();
  //  delete Dp2;
  //  Dp2 = nullptr;
  //
  //  Dpix->Write();
  //  delete Dpix;
  //  Dpix = nullptr;
}

ClassImp(PndSciTAnaIdeal)
