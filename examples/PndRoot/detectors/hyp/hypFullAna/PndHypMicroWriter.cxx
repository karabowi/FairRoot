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
Class PndHypMicroWriter

Collects Micro infromation from Reconstruction and
writes out PndPidCandidates

Author: K.Goetzen, GSI, 06/2008
modified by A. Sanchez for hyp purpose

*******************************************************/

#include "PndHypMicroWriter.h"

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
//#include "TParticle.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "PndStack.h"
#include "PndMCTrack.h"

#include "GFTrack.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "PndHypHit.h"

#include "TVector3.h"
#include "TVectorD.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include <string>
#include <iostream>

#include "RhoCandidate.h"
#include "PndPidCandidate.h"
//#include "RhoEventShape.h"
#include "RhoCandList.h"
//#include "PndEventInfo.h"
#include "RhoFactory.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndHypMicroWriter::PndHypMicroWriter() : FairTask("FastSim Dump") {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHypMicroWriter::~PndHypMicroWriter()
{
  if (fChargedCandidates) {
    fChargedCandidates->Delete();
    delete fChargedCandidates;
  }
  if (fNeutralCandidates) {
    fNeutralCandidates->Delete();
    delete fNeutralCandidates;
  }
  if (fMcCandidates) {
    fMcCandidates->Delete();
    delete fMcCandidates;
  }
  if (fMicroCandidates) {
    fMicroCandidates->Delete();
    delete fMicroCandidates;
  }
  if (fEventInfo) {
    fEventInfo->Delete();
    delete fEventInfo;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndHypMicroWriter::Init()
{

  fStoreNeutral = true;
  fStoreCharged = true;
  fStoreMC = true;

  // TDatabasePDG *dbpdg=TDatabasePDG::Instance();

  // TRho::Instance()->SetPDG(dbpdg);

  cout << " Inside the Init function****" << endl;

  // FairDetector::Initialize();
  // FairRun* sim = FairRun::Instance();
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb();

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndHypMicroWriter::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  // Get input array
  fTrArray = (TClonesArray *)ioman->GetObject("Track");
  if (!fTrArray) {
    cout << "-W- PndHypMicroWriter::Init: "
         << "No TpcTrack array!" << endl;
    fTrArray = new TClonesArray("GFTrack");
    fStoreCharged = false;
    // return kERROR;
  }

  fHitArray = (TClonesArray *)ioman->GetObject("HypHit");
  if (!fHitArray) {
    cout << "-W- PndHypMicroWriter::Init: "
         << "No Hit array!" << endl;
    fHitArray = new TClonesArray("HypHit");
    fStoreNeutral = false;
    // return kERROR;
  }

  /*
  fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrack) {
    cout << "-W- PndHypMicroWriter::Init: "
   << "No MCTrack array!" << endl;
   fMCTrack = new TClonesArray("MCTrack");
    fStoreMC=false;
    //return kERROR;
  }
*/

  // fChargedCandidates = new TClonesArray("RhoCandidate");
  //   FairRootManager::Instance()->Register("PndChargedCandidates","FullSim", fChargedCandidates, kTRUE);

  //   fNeutralCandidates = new TClonesArray("RhoCandidate");
  //   FairRootManager::Instance()->Register("PndNeutralCandidates","FullSim", fNeutralCandidates, kTRUE);

  //   fMcCandidates = new TClonesArray("RhoCandidate");
  //   FairRootManager::Instance()->Register("PndMcTracks","FullSim", fMcCandidates, kTRUE);

  fMicroCandidates = new TClonesArray("PndPidCandidate");
  FairRootManager::Instance()->Register("PndPidCandidates", "FullSim", fMicroCandidates, kTRUE);

  //   fEventInfo = new TClonesArray("PndEventInfo");
  //   FairRootManager::Instance()->Register("PndEventSummary","FullSim", fEventInfo, kTRUE);

  // Create and register output array
  LOG(info) << " PndHypMicroWriter: Intialization successfull";

  // fInvMass = new TH1D("invmass","",100,0.0,4.0);

  evtcnt = 0;

  return kSUCCESS;
}

void PndHypMicroWriter::SetParContainers()
{

  // Get Base Container
  FairRunAna *ana = FairRunAna::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();

  // Get run and runtime database
  //  FairRunAna* run = FairRunAna::Instance();
  //   if ( ! run ) LOG(fatal) << "SetParContainers: No analysis run";

  //  FairRuntimeDb* db = run->GetRuntimeDb();
  //   if ( ! db ) LOG(fatal) << "SetParContainers: No runtime database";
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndHypMicroWriter::Exec(Option_t *)
{
  if ((++evtcnt) % 100 == 0)
    cout << "evt: " << evtcnt << endl;

  // find # in input array
  Int_t nTracks = 0;
  if (fStoreCharged)
    nTracks = fTrArray->GetEntriesFast();

  Int_t nCluster = 0;
  // if (fStoreNeutral) nCluster=fEmcArray->GetEntriesFast();

  int nMCTrack = 0;
  // if (fStoreMC) nMCTrack=fMCTrack->GetEntriesFast();

  // PndStack *fStack=(PndStack*)gMC->GetStack();
  // int nMCTracks=fStack->GetNtrack();

  // Reset output array
  // if (fChargedCandidates->GetEntriesFast() != 0)  fChargedCandidates->Clear("C");
  //  if (fNeutralCandidates->GetEntriesFast() != 0)  fNeutralCandidates->Clear("C");
  //   if (fMcCandidates->GetEntriesFast() != 0)  fMcCandidates->Clear("C");
  if (fMicroCandidates->GetEntriesFast() != 0)
    fMicroCandidates->Clear("C");
  // if (fEventInfo->GetEntriesFast() != 0) fEventInfo->Clear("C");

  // TClonesArray &chrgCandidates  = *fChargedCandidates;
  //  TClonesArray &neutCandidates  = *fNeutralCandidates;
  //   TClonesArray &mctracks 	    = *fMcCandidates;
  TClonesArray &microCandidates = *fMicroCandidates;

  // RhoCandList l;

  TLorentzVector McSumP4(0, 0, 0, 0);
  TVector3 McAvgVtx(0, 0, 0);

  int nPrimary = 0;

  GFTrack *tr1;

  // *************************
  // Loop over the charged tracks
  // ************************
  LSLTrackRep *grep = 0;
  GFAbsTrackRep *rep = 0;

  for (Int_t i = 0; i < nTracks; i++) {

    // Int_t chcandsize = chrgCandidates.GetEntriesFast();
    Int_t micsize = microCandidates.GetEntriesFast();

    // Get the Tpc Track information

    tr1 = (GFTrack *)fTrArray->At(i);

    // check that track has been fitted
    if (tr1->getCardinalRep()->getStatusFlag() != 0) {
      std::cout << "Discarding track. Status flag !=0" << std::endl;
      continue;
    }

    // LSLTrackRep* myrep=dynamic_cast<LSLTrackRep*>(tr1->getTrackRep(0));//tr1->getCardinalRep());
    GeaneTrackRep *myrep = dynamic_cast<GeaneTrackRep *>(tr1->getTrackRep(0));

    TVectorD d(6);
    // TVector3 vtx;

    // d = myrep->getGlobal();
    // TVector3 pos =tr1->getPos();
    TVector3 pos = tr1->getTrackRep(0)->getPos();
    TVector3 mom = tr1->getTrackRep(0)->getMom();

    //  std::cout<< "pos1 "<<pos.x()<<" "<<pos.x()<<" "<<pos.z()<<std::endl;
    //     std::cout<< "pos2 "<<pos2.x()<<" "<<pos2.x()<<" "<<pos2.z()<<std::endl;
    //     std::cout<< "mom "<<mom.x()<<" "<<mom.x()<<" "<<mom.z()<<std::endl;

    TVector3 vtx(pos.x(), pos.y(), pos.z()); // d[0],d[1],d[2]);

    TLorentzVector lv;
    lv.SetXYZM(mom.x(), mom.y(), mom.z(), 0.13957); // d[3],d[4],d[5],0.13957);

    // std::cout<< "pos glob "<<d[0]<<" "<<d[1]<<" "<<d[2]<<std::endl;
    //     std::cout<< "mom glob "<<d[3]<<" "<<d[4]<<" "<<d[5]<<std::endl;

    // propagate(lv,vtx,myrep->getCharge());

    // create the RhoCandidate (keep this for the time being)
    // RhoCandidate *tcand=new (chrgCandidates[chcandsize]) RhoCandidate(lv,myrep->getCharge());

    // tcand->SetPos(vtx);

    // set the convariance matrix of tcand

    /*  TMatrixD globalCov = myrep->getGlobalCov();*/
    TMatrixD mat(7, 7);
    int ii, jj;

    unsigned int detId, hitId;
    unsigned int numhits = 0, mvdhits = 0, stthits = 0, tpchits = 0;

    numhits = tr1->getCand().getNHits();
    // cout<<" numhits "<<numhits<<endl;
    // create the PndPidCandidate

    PndPidCandidate *micro = new (microCandidates[micsize]) PndPidCandidate((Int_t)myrep->getCharge(), vtx, lv, mat); // myrep->getCharge()

    for (ii = 0; ii < numhits; ++ii) {
      tr1->getCand().getHit(ii, detId, hitId);
      // cout<<" numhits loop "<<ii<<" "<<detId<<" "<<hitId<<endl;
      switch (detId) {
      case 2:
        mvd_hitidx[mvdhits] = hitId;
        if (mvdhits < 1000)
          mvdhits++;
        break;
        // case 1:  stt_hitidx[stthits]=hitId; if(stthits<1000) stthits++;break;
      default:
        tpc_hitidx[tpchits] = hitId;
        if (tpchits < 1000)
          tpchits++;
        break;
      }
    }

    micro->SetMvdHits(numhits);

    // micro->SetSttHitIndexArray(stthits, stt_hitidx);
    // micro->SetTpcHitIndexArray(tpchits, tpc_hitidx);

    ///****** Extrapolation to primary vertex ******

    // rep=tr1->getTrackRep(0)->clone();
    unsigned int hid;
    hid = -1;

    tr1->getCand().getHit(0, detId, hid);
    // std::cout<<" entries "<<fHitArray->GetEntriesFast()<<" hid "<<hid<<std::endl;;
    PndHypHit *hit = (PndHypHit *)fHitArray->At(hid);
    if (hit == 0)
      continue;

    // std::cout<<" hit "<<hit->GetX()<<std::endl;
    TVector3 init = hit->GetPosition();

    GFDetPlane pinit(init, TVector3(1, 0, 0), TVector3(0, 1, 0));

    // myrep->extrapolate(pinit);
    // myrep->setReferencePlane(pinit);

    /* TVector3 pos(0,0,-76.5);
    GFDetPlane pfin(pos,TVector3(1,0,0),TVector3(0,1,0));

    TVector3 dist,fin;

     TMatrixT<double> state(5,1);
     TMatrixT<double> cov(5,5);
     GFDetPlane p;
    */
    // dist=myrep->getPos(pfin) ;
    // micro->SetPosition(dist);

    // std::cout<< (micro->GetPosition()).x()<<std::endl;
    // dist = myrep->extrapolateToPoca(pos,state,cov,p);
    // vtx.SetXYZ(dist.x(),dist.y(),dist.z());

    // std::cout<<dist.x()<<" "<<dist.y()<<" "<<dist.z()<<std::endl;
    // std::cout<<fin.x()<<" "<<fin.y()<<" "<<fin.z()<<std::endl;
    // grep=dynamic_cast<LSLTrackRep*>(rep);

    //     if(grep!=0){

    //       //grep->setPropDir(-1);
    //       TMatrixT<double> state(5,1);
    //       TMatrixT<double> cov(5,5);
    //       DetPlane p;
    //       //dist = grep->extrapolateToPoca(pos,state,cov,p);
    //       //dist=trk->getPos();
    //       //grep->Print();

    //       vtx.SetXYZ(dist.x(),dist.y(),dist.z());

    //       //double length=grep->extrapolate(pmed,state);
    //       //std::cout<<"length "<<length<<std::endl;
    //       //std::cout<<" x poca "<<grep->getChiSqu()<<" "<<dist.x()<<" "<<dist.y()<<" "<<dist.z()<<std::endl;
    //       //std::cout<<"error poca "<<sqrt(grep->getCovElem(0, 0))<<" erry "<<sqrt(grep->getCovElem(1, 1))<<std::endl;

    //     }
  }

  // *************************
  // Loop over the neutral clusters
  // ************************

  double calFactor = 1.035;

  Finish();
}
// -------------------------------------------------------------------------

void PndHypMicroWriter::Finish()
{
  // fChargedCandidates->Delete();
  //   //  fNeutralCandidates->Delete();
  // //    fMcCandidates->Delete();
  fTrArray->Delete();
  fHitArray->Delete();
  // fMicroCandidates->Delete();
  //    //fEventInfo->Delete();
  //  //fInvMass->Write();
}

// -------------------------------------------------------------------------

ClassImp(PndHypMicroWriter)
