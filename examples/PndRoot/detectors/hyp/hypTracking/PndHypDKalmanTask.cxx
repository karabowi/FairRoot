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

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class DemoKalmanTask
//      see DemoKalmanTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndHypDKalmanTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"
#include "GFTrackCand.h"
//#include "PndTpcPoint.h"
//#include "DemoRecoHit.h"
//#include "DemoSPHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsHit.h"
#include "PndSdsRecoHit.h"
#include "PndGeoHandling.h"
#include "PndHypRecoSPHit.h"
#include "PndHypRecoHit.h"
#include "FairMCPoint.h"
#include "FairHit.h"
#include "FairLogger.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"

// Class Member definitions -----------

PndHypDKalmanTask::PndHypDKalmanTask() : FairTask("Kalman Filter"), fPersistence(kFALSE), fSmooth(kFALSE), fUseMVD(false), fEvt(0)
{
  fTrackBranchName = "Track";
  PndGeoHandling::Instance();
}

PndHypDKalmanTask::~PndHypDKalmanTask()
{
  if (fPH != nullptr)
    delete fPH;
  if (fChi2H != nullptr)
    delete fChi2H;
}

InitStatus PndHypDKalmanTask::Init()
{
  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();

  if (ioman == 0) {
    Error("PndHypDKalmanTask::Init", "RootManager not instantiated!");
    return kERROR;
  }

  // Get input collection
  fTrackArray = (TClonesArray *)ioman->GetObject(fTrackBranchName);

  // if(_trackArray==0)
  //{
  //   Error("PndHypDKalmanTask::Init","track-array not found!");
  // return kERROR;
  //}

  // create and register output array
  //_trackArray = new TClonesArray("GFTrack");
  // ioman->Register("TrackPreFit","GenFit",_trackArray,_persistence);

  // Build hit factory -----------------------------
  fTheRecoHitFactory = new GFRecoHitFactory();

  std::map<unsigned int, TString>::iterator iter = fHitBranchMap.begin();
  while (iter != fHitBranchMap.end()) {

    TClonesArray *har = (TClonesArray *)ioman->GetObject("HypHit");

    // cout<<" "<<iter->second<<endl;

    if (har == 0) {

      Error("PndHypKalmanTask::Init", "Hit array not found");
    } else {

      if (iter->first == 2)
        fTheRecoHitFactory->addProducer(iter->first, new GFRecoHitProducer<PndHypHit, PndHypRecoSPHit>(har));
    }

    if (fUseMVD == true) {
      TClonesArray *sar = (TClonesArray *)ioman->GetObject("MVDHit");
      if (sar == 0) {

        Error("PndHypKalmanTask::Init", "Hit array not found");
      } else {

        if (iter->first == 3)
          fTheRecoHitFactory->addProducer(iter->first, new GFRecoHitProducer<PndSdsHit, PndSdsRecoHit>(sar));
      }
    }

    // TClonesArray* ar=(TClonesArray*) ioman->GetObject(iter->second);
    // if(ar==0){
    //   Error("PndHypDKalmanTask::Init","point-array %s not found!",iter->second.Data());
    // }
    // else{
    //   // the next lines is not general because it will work only for CmMCPoints!
    //   fTheRecoHitFactory->addProducer(iter->first,new GFRecoHitProducer<PndHypHit,PndHypRecoSPHit>(ar));
    //   //FairHit,PndHypDSPHit>(ar));PndHypPoint,PndHypRecoHit
    // }

    ++iter;
  } // end loops over hit types

  // setup histograms
  fPH = new TH1D("pH", "p", 500, 0.02, 0.7);
  fChi2H = new TH1D("chi2H", "chi2", 100, 0, 20);
  fXresH = new TH1D("xres", "xres", 100, -5, 5);
  fYresH = new TH1D("yres", "yres", 100, -5, 5);
  fXresFitH = new TH1D("xresfit", "xres after fit", 100, -5, 5);
  fYresFitH = new TH1D("yresfit", "yres after fit", 100, -5, 5);
  fPEnd = new TH1D("pPre", "Endpoint", 500, 0.02, 0.7);
  fPull = new TH1D("pPull", "Pull", 500, -0.3, 0.3);
  LOG(info) << " gGeoManager = " << gGeoManager;

  return kSUCCESS;
}

void PndHypDKalmanTask::Exec(Option_t *)
{
  std::cout << "PndHypDKalmanTask::Exec Event " << fEvt++ << std::endl;
  // Reset output Array
  // if(_trackArray==0) Fatal("PndHypDKalman::Exec)","No TrackArray");
  // _trackArray->Delete();

  Int_t ntracks = fTrackArray->GetEntriesFast();

  // Fitting ---------------- can go to another task!
  GFKalman fitter;
  // fitter.setLazy(1); // tell the fitter to skip hits if error occurs
  fitter.setNumIterations(1);
  for (Int_t itr = 0; itr < ntracks; ++itr) {
    GFTrack *trk = (GFTrack *)fTrackArray->At(itr);
    GFTrackCand trcnd = trk->getCand();

    // Load RecoHits
    try {
      trk->addHitVector(fTheRecoHitFactory->createMany(trk->getCand()));
      std::cout << trk->getNumHits() << " hits in track " << itr << std::endl;
    }

    catch (GFException &e) {
      std::cout << e.what() << std::endl;
      throw e;
    }

    /*
    // fill tpc residuals
    std::vector<double> res;
    trk->getResiduals(2,0,0,res);
    for(int i=0;i<res.size();++i){
      _xresH->Fill(res[i]);
    }
    res.clear();
    trk->getResiduals(2,1,0,res);
    for(int i=0;i<res.size();++i){
      _yresH->Fill(res[i]);
    }
    res.clear();
    */
    // Start Fitter
    try {
      fitter.processTrack(trk);

    } catch (GFException e) {
      std::cout << "*** FITTER EXCEPTION ***" << std::endl;
      std::cout << e.what() << std::endl;
    }

    std::cout << "SUCESSFULL FIT!" << std::endl;

    // Fill some histos after fit
    if (trk->getTrackRep(0)->getStatusFlag() == 0) {
      // propagate backwards
      GeaneTrackRep *gtrk = dynamic_cast<GeaneTrackRep *>(trk->getTrackRep(0));
      if (gtrk != nullptr)
        gtrk->setPropDir(-1);

      trk->getCardinalRep()->Print();

      // ------- Propagation to prim vertex ---------

      GFDetPlane pl(TVector3(0, 0, -55.0), TVector3(1, 0, 0), TVector3(0, 1, 0));

      // TVector3 p3=trk->getTrackRep(0)->getMom(pl);
      //--------------------------------------------

      double p = trk->getTrackRep(0)->getMom().Mag();

      std::cout << " momentum "
                << " " << trk->getMom().Mag() << std::endl;
      std::cout << " "
                << " N reps " << trk->getNumReps() << " chi2red " << trk->getRedChiSqu() << std::endl;

      // INFO: Changing the EPSIL param to
      // 0.05 (HYPsilicon, HYPdiamond, HYPcarbon--> materials in geo  file)
      // propagation to prim vertex works smoothly for 500 ev.
      // no dedx modification from media file is needed

      fPH->Fill(p);

      fPEnd->Fill(-1 / (trcnd.getQoverPseed()));

      fPull->Fill((-1 / (trcnd.getQoverPseed())) - p);

      TVector3 pos = trk->getPos();
      // fPEnd->Fill(pos.X(),pos.Z());

      double chi2 = trk->getChiSqu();
      fChi2H->Fill(chi2);
      ++fTrackcount;

      //  // fill tpc residuals
      //       trk->getResiduals(2,0,0,res);
      //       for(int i=0;i<res.size();++i){
      // 	_xresFitH->Fill(res[i]);
      //       }
      //       res.clear();
      //       trk->getResiduals(2,1,0,res);
      //       for(int i=0;i<res.size();++i){
      // 	_yresFitH->Fill(res[i]);
      //       }
      //       res.clear();
    }
  }

  return;
}

void PndHypDKalmanTask::WriteHistograms(const TString &filename)
{

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();
    outfile->mkdir("PndHypDKalman");
    outfile->cd("PndHypDKalman");

    outfile->WriteTObject(fPH);
    delete fPH;
    fPH = nullptr;

    outfile->WriteTObject(fChi2H);
    delete fChi2H;
    fChi2H = nullptr;

    outfile->WriteTObject(fXresH);
    delete fXresH;
    fXresH = nullptr;

    outfile->WriteTObject(fYresH);
    delete fYresH;
    fYresH = nullptr;

    outfile->WriteTObject(fXresFitH);
    delete fXresFitH;
    fXresFitH = nullptr;

    outfile->WriteTObject(fYresFitH);
    delete fYresFitH;
    fYresFitH = nullptr;

    outfile->WriteTObject(fPEnd);
    delete fPEnd;
    fPEnd = nullptr;

    outfile->WriteTObject(fPull);
    delete fPull;
    fPull = nullptr;

    outfile->cd("..");

    outfile->Close();
    delete (outfile);
  }

  //  TFile *file = FairRootManager::Instance()->GetOutFile();
  //  file->cd();
  //  file->mkdir("PndHypDKalman");
  //  file->cd("PndHypDKalman");
  //
  //  // TFile* file = new TFile(filename,"UPDATE");
  //  // if(file->cd("Kalman")) file->Delete("Kalman;*");
  //  // file->mkdir("Kalman");
  //  // file->cd("Kalman");
  //
  //  fPH->Write();
  //  delete fPH;
  //  fPH = nullptr;
  //
  //  fChi2H->Write();
  //  delete fChi2H;
  //  fChi2H = nullptr;
  //
  //  fXresH->Write();
  //  delete fXresH;
  //  fXresH = nullptr;
  //
  //  fYresH->Write();
  //  delete fYresH;
  //  fYresH = nullptr;
  //
  //  fXresFitH->Write();
  //  delete fXresFitH;
  //  fXresFitH = nullptr;
  //
  //  fYresFitH->Write();
  //  delete fYresFitH;
  //  fYresFitH = nullptr;
  //
  //  fPEnd->Write();
  //  delete fPEnd;
  //  fPEnd = nullptr;
  //
  //  fPull->Write();
  //  delete fPull;
  //  fPull = nullptr;
  //
  //  file->Close();
  //  delete file;
}

ClassImp(PndHypDKalmanTask)
