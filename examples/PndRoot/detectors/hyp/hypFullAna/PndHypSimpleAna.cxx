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

/******************************************************

Analysis Task created by A.Sanchez
Read MicroCandidates and do analysis
for Hypernuclei.
*******************************************************/

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
//#include "PndTpcLheGFTrack.h"

//#include "PndTpcLheGFTrack.h"
#include "PndHypHit.h"
#include "PndHypPoint.h"
#include "PndHypGePoint.h"
#include "PndStack.h"
#include "PndMCTrack.h"

#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndHypSimpleAna.h"
#include <string>
#include <iostream>
#include "FairLogger.h"

#include "GFTrack.h"
#include "LSLTrackRep.h"

// RHO stuff
#include "RhoCandidate.h"
#include "PndPidCandidate.h"
#include "PndPidCandidate.h"
#include "RhoCandList.h"
#include "RhoCandListIterator.h"
#include "RhoFactory.h"
#include "RhoMassParticleSelector.h"
#include "RhoPlusParticleSelector.h"
#include "RhoMinusParticleSelector.h"
#include "RhoSimpleElectronSelector.h"
#include "RhoSimpleKaonSelector.h"
#include "RhoSimpleMuonSelector.h"
#include "RhoSimplePionSelector.h"
#include "RhoSimpleProtonSelector.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndHypSimpleAna::PndHypSimpleAna() : FairTask("Panda HypSimpleAna	 Task") {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHypSimpleAna::~PndHypSimpleAna() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndHypSimpleAna::Init()
{

  // cout << " Inside the Init function****" << endl;

  // FairDetector::Initialize();
  // FairRun* sim = FairRun::Instance();
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb();

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcHitProducer::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array

  fMcTr = (TClonesArray *)ioman->GetObject("MCTrack");
  fMcCands = (TClonesArray *)ioman->GetObject("HypHit");
  fMc = (TClonesArray *)ioman->GetObject("HypPoint");
  // fGe = (TClonesArray*) ioman->GetObject("HypGePoint");

  // fChargedArray = (TClonesArray*) ioman->GetObject("PndChargedCandidates");
  fMicroArray = (TClonesArray *)ioman->GetObject("PndPidCandidates");

  if (!fMicroArray) {
    cout << "-W- PndHypSimpleAna	::Init: "
         << "No PndChargedCandidates && PndNeutralCandidates array!" << endl;
    return kERROR;
  }

  // Create and register output array
  LOG(info) << " PndHypSimpleAna  : Intialization successfull";

  ppi2mass = new TH1F("ppimass", "p pi cands", 200, 0.05, 0.3);
  ppi2 = new TH1F("ppion", "p pion cands", 200, 0.05, 1.7);
  e = new TH1F("evenet", "p event", 200, 10000, 50000);

  pid = new TH2F("pid", "cands", 200, 0, 16, 200, 0, 16);
  pidh = new TH2F("pidh", "candsh", 200, 0, 16, 200, 0, 16);
  ximass = new TH1F("ximass", "xi cands", 200, 0.05, 2);
  Lamb = new TH1F("lamb mass", "lamb cands", 200, 0.05, 2);

  // itTop = new TH1F("p","cands",10000000,1010000000,1020000000);
  /* spectra[0] = new TH1F("spectra01","cluster 1",200,0.0,0.01);
  spectra[1] = new TH1F("spectra02","cluster 2",200,0.0,0.01);
  spectra[2] = new TH1F("spectra03","cluster 3",200,0.0,0.01);
  spectra[3] = new TH1F("spectra04","cluster 4",200,0.0,0.01);
  spectra[4] = new TH1F("spectra05","cluster 5",200,0.0,0.01);
  spectra[5] = new TH1F("spectra06","cluster 6",200,0.0,0.01);
 spectra[6] = new TH1F("spectra07","cluster 7",200,0.0,0.01);
 spectra[7] = new TH1F("spectra08","cluster 8",200,0.0,0.01);
 spectra[8] = new TH1F("spectra09","cluster 9",200,0.0,0.01);
 spectra[9] = new TH1F("spectra10","cluster 9",200,0.0,0.01);

  */
  // hvtx2[0]=new TH2F("hvtx201","vertex positions (x,z)",200,0.03,1.,200,0.03,1.);
  hvtx2[0] = new TH2F("hvtx201", "vertex positions (x,z)", 100, 0.0, 0.15, 100, 0.0, 0.15);
  hvtx2[1] = new TH2F("hvtx202", "vertex positions (x,z)", 100, 0.0, 0.15, 100, 0.0, 0.15);
  hvtx2[2] = new TH2F("hvtx203", "vertex positions (x,z)", 100, 0.0, 0.15, 100, 0.0, 0.15);
  hvtx2[3] = new TH2F("hvtx204", "vertex positions (x,z)", 100, 0.0, 0.15, 100, 0.0, 0.15);
  hvtx2[4] = new TH2F("hvtx205", "vertex positions (x,z)", 100, 0.03, 0.15, 100, 0.03, 0.15);
  hvtx2[5] = new TH2F("hvtx206", "vertex positions (x,z)", 100, 0.03, 0.15, 100, 0.03, 0.15);
  hvtx2[6] = new TH2F("hvtx207", "vertex positions (x,z)", 100, 0.03, 0.15, 100, 0.03, 0.15);
  hvtx2[7] = new TH2F("hvtx208", "vertex positions (x,z)", 100, 0.03, 0.15, 100, 0.03, 0.15);
  hvtx2[8] = new TH2F("hvtx209", "vertex positions (x,z)", 100, 0.03, 0.15, 100, 0.03, 0.15);
  hvtx2[9] = new TH2F("hvtx210", "vertex positions (x,z)", 100, 0.03, 0.15, 100, 0.03, 0.15);

  // **** create and configure the selectors/filters we'd like to use later
  //
  // chargedSel = new RhoChargedParticleSelector;
  // neutralSel = new RhoNeutralParticleSelector;
  plusSel = new RhoPlusParticleSelector;
  minusSel = new RhoMinusParticleSelector;

  // **** mass selectors for the resonances/composites
  //

  piSel = new RhoSimplePionSelector();
  piSel->SetCriterion("veryLoose");
  /* pSel    = new RhoSimpleProtonSelector();
     pSel->SetCriterion("veryLoose");*/

  LambMSel = new RhoMassParticleSelector("LambSelector", 1.115, 0.04);

  evcount = 0;

  return kSUCCESS;
}

void PndHypSimpleAna::SetParContainers()
{

  // Get run and runtime database
  FairRunAna *run = FairRunAna::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  // FairRuntimeDb* db = run->GetRuntimeDb();
  // if ( ! db ) LOG(fatal) << "SetParContainers: No runtime database";
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndHypSimpleAna::Exec(Option_t *)
{

  RhoFactory::Instance()->Reset();

  if (!(++evcount % 100))
    cout << "evt " << evcount << endl;
  // cout <<"evt "<<evcount<<endl;++evcount;

  // **** create all the particle lists we'll need for rebuilding the decay tree
  //
  RhoCandList neutralCands, chargedCands, plusCands, minusCands;

  RhoCandList kpCands, kmCands, piCands, ppiCands;

  RhoCandList xiCands, nonOvCands, dsCands, ds0Cands, ppCands;
  std::map<Int_t, Float_t> mapp;

  // RhoCandidate *tc;

  // **** loop over all Candidates and add them to the list allCands
  //
  chargedCands.Cleanup();
  // neutralCands.Cleanup();

  for (Int_t i1 = 0; i1 < fMicroArray->GetEntriesFast(); i1++) {
    PndPidCandidate *mic = (PndPidCandidate *)fMicroArray->At(i1);
    RhoCandidate tc(*mic, i1);
    TLorentzVector l = tc.P4();
    TVector3 p = tc.Pos();
    // cout<<" micro to tcaaand "<<tc.GetCharge()<<endl;

    chargedCands.Add(&tc);
  }

  // cout <<"c:"<<chargedCands.GetLength()<<" n:"<<neutralCands.GetLength()<<endl;

  // **** select all the basic lists
  //

  plusCands.Select(chargedCands, plusSel);
  minusCands.Select(chargedCands, minusSel);

  // **** pid selection
  //

  piCands.Select(minusCands, piSel);
  // ppCands.Select(plusCands ,pSel);

  // cout <<"pi-:"<<piCands.GetLength()<<endl;

  // **** now start combining all composits; inbetween plot masses
  //      before using the mass selectors
  //

  RhoCandidate *t2;

  RhoCandListIterator iterP(piCands);
  while (t2 = iterP.Next()) {
    // ppimass->Fill(l.P());//tc->Mass());

    TLorentzVector l = t2->P4();
    // ppimass->Fill(l.M());//tc->Mass());
    TVector3 pim = l.Vect();

    // cout <<"fitted = "<<pim.Mag()<<" charge "<<t2->GetCharge()<<endl;
    ppi2mass->Fill(pim.Mag());
  }

  int ii, jj;

  int npi = piCands.GetLength();

  // int np=pplusCands.GetLength();
  PndHypHit *hit;
  PndHypPoint *po;
  int Motherpdg, MotherId;

  // Calculate total energy SPectra
  //	SetTotESpectra(9);

  for (int k = 0; k < npi; k++) {
    RhoCandidate *pion = piCands.Get(k);
    PndPidCandidate *pionpid = (PndPidCandidate *)pion->GetRecoCandidate();

    PndHypHit *hp = (PndHypHit *)fMcCands->At((pionpid->GetMvdHits()) - 1);
    if (hp == 0)
      continue;
    PndHypPoint *pop = (PndHypPoint *)fMc->At(hp->GetRefIndex());
    if (pop == 0)
      continue;

    PndMCTrack *moc = (PndMCTrack *)fMcTr->At(pop->GetTrackID());
    if (moc == 0)
      continue;
    if (moc->GetPdgCode() == -211)
      dsCands.Add(pion);
    // cout<<"  number real "<<pop->GetEventID()<<" pion number real "<<dsCands.GetLength()<<endl;
  }
  // cout<<" dsCands.GetLength() "<<dsCands.GetLength()<<endl;

  int dsi = dsCands.GetLength();
  RhoCandidate *t3;
  RhoCandListIterator itP(dsCands);
  while (t3 = itP.Next()) {
    // ppimass->Fill(l.P());//tc->Mass());

    TLorentzVector v4 = t3->P4();
    // ppimass->Fill(l.M());//tc->Mass());
    TVector3 v3 = v4.Vect();

    // cout <<"fitted = "<<pim.Mag()<<" charge "<<t2->GetCharge()<<endl;
    ppi2->Fill(v3.Mag());
  }

  if (dsi == 1) {
    // if(npi==1){
    // RhoCandidate pi=piCands[0];
    RhoCandidate *pi = dsCands.Get(0);
    // RhoCandidate pp=piminusCands[jj];
    TLorentzVector vpim = pi->P4();
    TVector3 pi3v = vpim.Vect();
    // cout<<ii<<" "<<pi3v.Mag()<<endl;

    if (pi3v.Mag() < 0.09) {
      // hvtx2[0]->Fill(0.,pi3v.Mag());
      hvtx2[2]->Fill(0., pi3v.Mag());
    }
    if (pi3v.Mag() > 0.09) {
      // hvtx2[0]->Fill(pi3v.Mag(),0.);
      hvtx2[3]->Fill(pi3v.Mag(), 0.);
    }
  }

  if (dsi > 1) {
    for (ii = 0; ii < dsi - 1; ii++) {
      // RhoCandidate pi=piCands[ii];
      RhoCandidate *pi = dsCands.Get(ii);
      // cout<<" charge pion "<<pi.GetCharge()<<endl;
      PndPidCandidate *pipid = (PndPidCandidate *)pi->GetRecoCandidate();
      TLorentzVector vpim = pi->P4();
      TVector3 pi3v = vpim.Vect();

      for (jj = ii + 1; jj < dsi; jj++) {
        // RhoCandidate pp=piCands[jj];
        RhoCandidate *pp = dsCands.Get(jj);

        TLorentzVector vpp = pp->P4();
        TVector3 pp3v = vpp.Vect();
        // VAbsMicroCandidate cm;
        // cm = pi.GetMicroCandidate();

        hit = (PndHypHit *)fMcCands->At((pipid->GetMvdHits()) - 1);
        po = (PndHypPoint *)fMc->At(hit->GetRefIndex());
        if (po == 0)
          continue;

        PndMCTrack *mc = (PndMCTrack *)fMcTr->At(po->GetTrackID());
        if (mc == 0)
          continue;
        MotherId = mc->GetMotherID();
        if (MotherId == -1)
          Motherpdg = mc->GetPdgCode();
        else {
          PndMCTrack *mother = (PndMCTrack *)fMcTr->At(MotherId);
          Motherpdg = mother->GetPdgCode();
        }
        //****cut on PCA to primary vertex has to be added ****
        TVector3 vertex = mc->GetStartVertex();

        // if(mc->GetPdgCode()==3112) cout<<" Motherpdg mala "<<Motherpdg<<endl;
        // cout<<" event "<<po->GetEventID()<<" "<<endl;
        //****PID has to be added ****
        // if(mc->GetPdgCode()!=-211)continue;

        // if(Motherpdg==-211||Motherpdg==310||
        // if(Motherpdg==3312)continue;

        // if(vertex.x()==0&&vertex.y()==0&&vertex.z()==-76.5)continue;
        cout << " Motherpdg " << Motherpdg << " " << mc->GetPdgCode() << endl;
        // if(pi3v.Mag()<0.2&&pp3v.Mag()<0.2)continue;

        // cout<<" pion selected for clu0 "<<mc->GetPdgCode()<<" mother "<<Motherpdg<<endl;
        // cout<<" hits on hyp track "<<pi.GetMicroCandidate().GetMvdHits()<<" ev "<<po->GetEventID()<<endl;
        // cout<<ii<<" "<<pi3v.Mag()<<" "<<jj<<" "<<pp3v.Mag()<<endl;

        // if(pi3v.Mag()>pp3v.Mag())cout<<" pi3v >pp3v "<<endl;
        // if(pi3v.Mag()<pp3v.Mag())cout<<" pi3v <pp3v "<<endl;

        // if( pp.Overlaps(pim)) continue;

        // TMatrixD pcov=pp.Cov7();
        // TMatrixD picov=pim.Cov7();
        if (pi3v.Mag() > pp3v.Mag()) {

          hvtx2[0]->Fill(pi3v.Mag(), pp3v.Mag());

          if ((pi3v.Mag() > 0.12 && pi3v.Mag() < 0.14) && (pp3v.Mag() > 0.065 && pp3v.Mag() < 0.08)) {
            // if(Motherpdg==1020040110||Motherpdg==1010050110){

            // cout<<" Be11LL "<<Motherpdg<<" "<<mc->GetPdgCode()<<endl;
            // cout<<" pi3v >pp3v "<<po->GetEventID()<<endl;
            hvtx2[1]->Fill(pi3v.Mag(), pp3v.Mag());
            // if((pi3v.Mag()>0.12&&pi3v.Mag()<0.14)&&(pp3v.Mag()>0.065&&pp3v.Mag()<0.08)) {

            if (mapp[po->GetEventID()] == 0) {
              mapp[po->GetEventID()] = Motherpdg;
              // cout<<" Motherpdg "<<Motherpdg<<endl;

              //
              //  SetEnergySpectra(po->GetEventID(),0);
            }
          }
          if (Motherpdg == 1020040110 || Motherpdg == 1010050110) {
            /// hvtx2[1]->Fill(pi3v.Mag(),pp3v.Mag());
          }

          if (Motherpdg == 1020030090 || (Motherpdg == 1010040090 && MotherId != -1)) {

            // cout<<" pi3v >pp3v "<<po->GetEventID()<<endl;
            // hvtx2[2]->Fill(pi3v.Mag(),pp3v.Mag());
            // if((pi3v.Mag()>0.12&&pi3v.Mag()<0.14)&&(pp3v.Mag()>0.065&&pp3v.Mag()<0.08)) {
          }

          // cout<<" Motherpdg Li9LL "<<Motherpdg<<endl;

          //
          if ((pi3v.Mag() > 0.112 && pi3v.Mag() < 0.126) && (pp3v.Mag() > 0.09 && pp3v.Mag() < 0.103)) {
            // hvtx2[2]->Fill(pi3v.Mag(),pp3v.Mag());
            if (mapp[po->GetEventID()] == 0) {
              mapp[po->GetEventID()] = Motherpdg;
              //   SetEnergySpectra(po->GetEventID(),1);//cout<<" Motherpdg Li9LL cl1 "<<Motherpdg<<endl;
            }
          }

          if ((pi3v.Mag() > 0.128 && pi3v.Mag() < 0.147) && (pp3v.Mag() > 0.0898 && pp3v.Mag() < 0.109)) {
            // hvtx2[9]->Fill(pi3v.Mag(),pp3v.Mag());

            // cout<<" Motherpdg Li9LL cl2 "<<Motherpdg<<" "<<mc->GetPdgCode()<<endl;
            if (mapp[po->GetEventID()] == 0) {
              mapp[po->GetEventID()] = Motherpdg;
              // SetEnergySpectra(po->GetEventID(),4);
              // cout<<" Motherpdg Li9LL cl2 "<<Motherpdg<<endl;
            }
          }

          //}

          if (Motherpdg == 1020040100 || Motherpdg == 1010050100) {

            // cout<<" pi3v >pp3v "<<po->GetEventID()<<endl;
            // hvtx2[3]->Fill(pi3v.Mag(),pp3v.Mag());
          }

          if ((pi3v.Mag() > 0.097 && pi3v.Mag() < 0.106) && (pp3v.Mag() > 0.094 && pp3v.Mag() < 0.103)) {
            //	if(Motherpdg==1020040100||Motherpdg==1010050100){

            // cout<<" pi3v >pp3v "<<po->GetEventID()<<endl;
            // hvtx2[3]->Fill(pi3v.Mag(),pp3v.Mag());
            // if((pi3v.Mag()>0.12&&pi3v.Mag()<0.14)&&(pp3v.Mag()>0.065&&pp3v.Mag()<0.08)) {
            // cout<<" Be10LL "<<Motherpdg<<" "<<mc->GetPdgCode()<<endl;

            if (mapp[po->GetEventID()] == 0) {
              mapp[po->GetEventID()] = Motherpdg;
              // cout<<" Motherpdg "<<Motherpdg<<endl;

              //
              // SetEnergySpectra(po->GetEventID(),2);
            }
          }

          if (Motherpdg == 1020040120 || Motherpdg == 1010050120) {

            // cout<<" pi3v >pp3v "<<po->GetEventID()<<endl;

            // hvtx2[4]->Fill(pi3v.Mag(),pp3v.Mag());
          }

          if ((pi3v.Mag() > 0.128 && pi3v.Mag() < 0.147) && (pp3v.Mag() > 0.110 && pp3v.Mag() < 0.124)) {
            // if(Motherpdg==1020040120||Motherpdg==1010050120){

            // cout<<" pi3v >pp3v "<<po->GetEventID()<<endl;

            // hvtx2[4]->Fill(pi3v.Mag(),pp3v.Mag());
            // cout<<" Middle "<<Motherpdg<<" "<<mc->GetPdgCode()<<endl;
            if (mapp[po->GetEventID()] == 0) {
              mapp[po->GetEventID()] = Motherpdg;
              // cout<<" Mot be12LL "<<Motherpdg<<endl;

              //
              // SetEnergySpectra(po->GetEventID(),3);
            }
          }

          if (Motherpdg == 1020020060 || Motherpdg == 1010030060) {

            // cout<<" pi3v >pp3v "<<po->GetEventID()<<endl;

            // hvtx2[5]->Fill(pi3v.Mag(),pp3v.Mag());
          }

          if ((pi3v.Mag() > 0.128 && pi3v.Mag() < 0.147) && (pp3v.Mag() > 0.124 && pp3v.Mag() < 0.143)) {
            // if(Motherpdg==1020020060||Motherpdg==1010030060){

            // cout<<" pi3v >pp3v "<<po->GetEventID()<<endl;

            //  hvtx2[5]->Fill(pi3v.Mag(),pp3v.Mag());

            // cout<<" Top "<<Motherpdg<<" "<<mc->GetPdgCode()<<endl;
            if (mapp[po->GetEventID()] == 0) {
              mapp[po->GetEventID()] = Motherpdg;
              // cout<<" Mot He6LL "<<Motherpdg<<endl;

              //
              // SetEnergySpectra(po->GetEventID(),5);
            }
          }
        }

        if (pi3v.Mag() < pp3v.Mag()) {
          // cout<<" pi3v <pp3v "<<po->GetEventID()<<endl;

          hvtx2[0]->Fill(pp3v.Mag(), pi3v.Mag());

          if (Motherpdg == 1020040110 || Motherpdg == 1010050110) {

            // hvtx2[1]->Fill(pp3v.Mag(),pi3v.Mag());
          }

          if ((pp3v.Mag() > 0.12 && pp3v.Mag() < 0.14) && (pi3v.Mag() > 0.065 && pi3v.Mag() < 0.08)) {
            // if(Motherpdg==1020040110||Motherpdg==1010050110){

            hvtx2[1]->Fill(pp3v.Mag(), pi3v.Mag());
            //
            if (mapp[po->GetEventID()] == 0) {
              mapp[po->GetEventID()] = Motherpdg;
              // cout<<" Motherpdg "<<Motherpdg<<endl;
              //
              // SetEnergySpectra(po->GetEventID(),0);
            }
          }

          if (Motherpdg == 1020030090 || (Motherpdg == 1010040090 && MotherId != -1)) {

            // hvtx2[2]->Fill(pp3v.Mag(),pi3v.Mag());
          }

          if ((pp3v.Mag() > 0.112 && pp3v.Mag() < 0.126) && (pi3v.Mag() > 0.09 && pi3v.Mag() < 0.103)) {
            // if(Motherpdg==1020030090||(Motherpdg==1010040090&& MotherId!=-1)){

            //  hvtx2[2]->Fill(pp3v.Mag(),pi3v.Mag());
            // if((pp3v.Mag()>0.12&&pp3v.Mag()<0.14)&&(pi3v.Mag()>0.065&&pi3v.Mag()<0.08)) {
            if (mapp[po->GetEventID()] == 0) {
              mapp[po->GetEventID()] = Motherpdg;
              // cout<<" Motherpdg "<<Motherpdg<<endl;
              //   SetEnergySpectra(po->GetEventID(),1);//cout<<" Motherpdg Li9LL cl1 "<<Motherpdg<<endl;
            }
          }

          if ((pp3v.Mag() > 0.128 && pp3v.Mag() < 0.147) && (pi3v.Mag() > 0.0898 && pi3v.Mag() < 0.109)) {
            hvtx2[9]->Fill(pp3v.Mag(), pi3v.Mag());
            // cout<<" Motherpdg Li9LL cl2 "<<Motherpdg<<" "<<mc->GetPdgCode()<<endl;
            if (mapp[po->GetEventID()] == 0) {
              mapp[po->GetEventID()] = Motherpdg;
              //  SetEnergySpectra(po->GetEventID(),4);//
            }
          }

          //}

          if (Motherpdg == 1020040100 || Motherpdg == 1010050100) {

            // hvtx2[3]->Fill(pp3v.Mag(),pi3v.Mag());
          }

          if ((pp3v.Mag() > 0.097 && pp3v.Mag() < 0.106) && (pi3v.Mag() > 0.094 && pi3v.Mag() < 0.103)) {
            // if(Motherpdg==1020040100||Motherpdg==1010050100){

            //	 hvtx2[3]->Fill(pp3v.Mag(),pi3v.Mag());
            //
            if (mapp[po->GetEventID()] == 0) {
              mapp[po->GetEventID()] = Motherpdg;
              // cout<<" Motherpdg "<<Motherpdg<<endl;
              //
              // SetEnergySpectra(po->GetEventID(),2);
            }
          }

          if (Motherpdg == 1020040120 || Motherpdg == 1010050120) {

            // hvtx2[4]->Fill(pp3v.Mag(),pi3v.Mag());
          }

          if ((pp3v.Mag() > 0.128 && pp3v.Mag() < 0.147) && (pi3v.Mag() > 0.110 && pi3v.Mag() < 0.124)) {
            // if(Motherpdg==1020040120||Motherpdg==1010050120){

            //	 hvtx2[4]->Fill(pp3v.Mag(),pi3v.Mag());
            // cout<<" Middle "<<Motherpdg<<" "<<mc->GetPdgCode()<<endl;
            //
            if (mapp[po->GetEventID()] == 0) {
              mapp[po->GetEventID()] = Motherpdg;
              // cout<<" Motherpdg "<<Motherpdg<<endl;
              //
              // SetEnergySpectra(po->GetEventID(),3);
            }
          }

          if (Motherpdg == 1020020060 || Motherpdg == 1010030060) {

            // cout<<" pi3v >pp3v "<<po->GetEventID()<<endl;

            // hvtx2[5]->Fill(pp3v.Mag(),pi3v.Mag());
          }

          if ((pp3v.Mag() > 0.128 && pp3v.Mag() < 0.147) && (pi3v.Mag() > 0.124 && pi3v.Mag() < 0.143)) {
            // if(Motherpdg==1020020060||Motherpdg==1010030060){

            // cout<<" pi3v >pp3v "<<po->GetEventID()<<endl;

            //  hvtx2[5]->Fill(pp3v.Mag(),pi3v.Mag());
            // cout<<" top "<<Motherpdg<<" "<<mc->GetPdgCode()<<endl;
            // SetEnergySpectra(po->GetEventID(),5);
            if (mapp[po->GetEventID()] == 0) {
              mapp[po->GetEventID()] = Motherpdg;
              // 			    // cout<<" Mot He6LL "<<Motherpdg<<endl;

              // SetEnergySpectra(po->GetEventID(),5);
            }
          }
        }

        // ppimass->Fill(compo->Mass());
      }
    }
  }

  // cout<<" pdg mother "<<mapp.size()<<endl;

  // for( std::map<Int_t, TH1F >::const_iterator ci=mapp.begin();
  // 	     ci != mapp.end();ci++){
  // 	  TH1F *h = itTop = ci->second;
  // 	  cout<<" pdg mother "<<ci->first<<" length "<<itTop.size()<<endl;

  // 	}
}
// -------------------------------------------------------------------------

Int_t PndHypSimpleAna::GetIonCharge(Int_t ion, Int_t &mass, Int_t &str)
{
  Int_t A, Z, L;

  if (ion > 1000000000 && (ion < 1010000000)) {
    ion -= 1000000000;
    Z = ion / 10000;
    ion -= 10000 * Z;
    A = ion / 10;
    // cout<<" ion charge "<<Z<<endl;
    mass = A;
    str = 0;

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
    mass = A;
    str = L;
    return Z;
  }
}
void PndHypSimpleAna::Finish(TString cat)
{

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();

    outfile->WriteTObject(ppi2mass);
    delete ppi2mass;
    ppi2mass = nullptr;

    outfile->WriteTObject(ximass);
    delete ximass;
    ximass = nullptr;

    outfile->WriteTObject(Lamb);
    delete Lamb;
    Lamb = nullptr;

    outfile->WriteTObject(ppi2);
    delete ppi2;
    ppi2 = nullptr;

    outfile->WriteTObject(e);
    delete e;
    e = nullptr;

    outfile->WriteTObject(pid);
    delete pid;
    pid = nullptr;

    outfile->WriteTObject(pidh);
    delete pidh;
    pidh = nullptr;

    for (int i = 0; i < 10; i++) {
      outfile->WriteTObject(hvtx2[i]);
      delete hvtx2[i];
      hvtx2[i] = nullptr;
    }
  }

  //  TFile *file = FairRootManager::Instance()->GetOutFile();
  //  file->cd();
  //  // file->mkdir(cat.Data());//"HypHitAnaF");
  //  // file->cd(cat.Data());//"HypHitAnaF");
  //
  //  ppi2mass->Write();
  //  delete ppi2mass;
  //  ppi2mass = nullptr;
  //
  //  ximass->Write();
  //  delete ximass;
  //  ximass = nullptr;
  //
  //  Lamb->Write();
  //  delete Lamb;
  //  Lamb = nullptr;
  //
  //  ppi2->Write();
  //  delete ppi2;
  //  ppi2 = nullptr;
  //
  //  e->Write();
  //  delete e;
  //  e = nullptr;
  //
  //  pid->Write();
  //  delete pid;
  //  pid = nullptr;
  //  pidh->Write();
  //  delete pidh;
  //  pidh = nullptr;
  //
  //  for (int i = 0; i < 10; i++) {
  //    hvtx2[i]->Write();
  //    delete hvtx2[i];
  //    hvtx2[i] = nullptr;
  //
  //    /*  spectra[i]->Write();
  //    delete  spectra[i];
  //    spectra[i]=nullptr;
  //    */
  //  }

  /*
    hvtx2[0]->Write();
    hvtx2[1]->Write();
    hvtx2[2]->Write();
    hvtx2[3]->Write();
    hvtx2[4]->Write();
    hvtx2[5]->Write(); hvtx2[6]->Write(); hvtx2[7]->Write();hvtx2[8]->Write();
    // hvtx2[9]->Write();

    ppi2mass->Write();
    spectra[0]->Write();
    spectra[1]->Write();
    spectra[2]->Write();
    spectra[3]->Write();
    spectra[4]->Write();
    spectra[5]->Write();spectra[6]->Write();spectra[7]->Write();
    spectra[8]->Write(); //spectra[9]->Write();
  */
}

ClassImp(PndHypSimpleAna)
