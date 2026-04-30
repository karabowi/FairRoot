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
// -----                PndDskFLGHitProducerIdeal source file             -----
// -----                 Created 23/04/10  by Yutie Liang              -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <math.h>
#include "stdio.h"

//#include "PndGeoDsk.h"
#include "PndDskFLGHitProducerIdeal.h"
#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "PndDskParticle.h"
#include "PndDskFLGHit.h"
#include "TVector3.h"
#include "TRandom.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "TString.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"
#include "FairLogger.h"
#include "TFormula.h"
#include "TMath.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TPDGCode.h"
#include "TGeoManager.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDskFLGHitProducerIdeal::PndDskFLGHitProducerIdeal() : PndPersistencyTask("PndDskFLGHitProducerIdeal")
{

  fGeo = new PndGeoDskFLG();
}
// -------------------------------------------------------------------------

// -----   Standard constructor with verbosity level  -------------------------------------------

PndDskFLGHitProducerIdeal::PndDskFLGHitProducerIdeal(Int_t verbose) : PndPersistencyTask("PndDskFLGHitProducerIdeal")
{
  fGeo = new PndGeoDskFLG();
  fVerbose = verbose;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndDskFLGHitProducerIdeal::~PndDskFLGHitProducerIdeal()
{

  if (fGeo)
    delete fGeo;
}
// -------------------------------------------------------------------------

// -----   Initialization   -----------------------------------------------
// -------------------------------------------------------------------------
InitStatus PndDskFLGHitProducerIdeal::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndDskFLGHitProducerIdeal::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fDskParticleArray = (TClonesArray *)ioman->GetObject("DskParticle");
  if (!fDskParticleArray) {
    cout << "-W- PndDskFLGHitProducerIdeal::Init: "
         << "No DskBarPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndDskFLGHit");
  ioman->Register("PndDskFLGHit", "Dsk", fHitArray, GetPersistency());

  // fVerbose = 2;
  LOG(info) << " PndDskFLGHitProducerIdeal: Intialization successfull";

  return kSUCCESS;
}

// -----   Execution of Task   ---------------------------------------------
// -------------------------------------------------------------------------
void PndDskFLGHitProducerIdeal::Exec(Option_t *)
{
  if (!fHitArray)
    Fatal("Exec", "No HitArray");
  fHitArray->Clear();

  PndDskParticle *pt = nullptr;
  nevents++;

  if (fVerbose > 1) {
    cout << " ----------------- DSK Hit Producer --------------------" << endl;
    cout << " Number of input MC points in the plate: " << fDskParticleArray->GetEntries() << endl;
  }

  // fNHits = 0;

  // Loop over PndDskPoints
  for (Int_t j = 0; j < fDskParticleArray->GetEntriesFast(); j++) {
    if (fVerbose > 1)
      printf("\n\n=====> Event No. %d\n", nevents);

    pt = (PndDskParticle *)fDskParticleArray->At(j);

    Double_t Px = pt->GetPx();
    Double_t Py = pt->GetPy();
    Double_t Pz = pt->GetPz();
    Double_t P = sqrt(Px * Px + Py * Py + Pz * Pz);
    Double_t mass = pt->GetMass();
    Double_t energy = TMath::Sqrt(P * P + mass * mass);

    Double_t beta;
    if (energy != 0) {
      beta = P / energy;
    } else {
      beta = -1.;
      if (fVerbose > 1)
        cout << "Beta not calculated " << endl;
    }

    Double_t thetaC;
    if (fabs(1. / (1.47 * beta)) > 1. || P == 0. || energy == 0.) {
      thetaC = -1.;
    } else {
      thetaC = acos(1 / (1.47 * beta));
    }

    if (thetaC != -1. && beta > 1 / 1.47) {

      // collect that need to be stored
      Int_t trackID = pt->GetTrackID();
      Int_t detectorID = 0;
      TVector3 position_store(pt->GetX(), pt->GetY(), pt->GetZ());
      TVector3 momentum_store(Px, Py, Pz);
      Double_t time = 0;
      Double_t angIn = pt->GetAngIn();
      Double_t thetaC_store = pt->GetThetaC();

      // generate Cherenkov photons in a cone

      int N_cherenko_photon = pt->GetNPhot();

      TVector3 Cone_Pos(pt->GetX() * 10, pt->GetY() * 10, pt->GetZ() * 10);
      TVector3 Cone_Axis(Px, Py, Pz);
      TVector3 Cherenkov_photon(1, 0, 0); // initialize
      Cherenkov_photon.SetTheta(Cone_Axis.Theta() + thetaC);
      Cherenkov_photon.SetPhi(Cone_Axis.Phi());
      for (int i_photon = 0; i_photon < N_cherenko_photon; i_photon++) {
        Double_t rot_angle = 2 * (TMath::Pi()) * gRandom->Rndm();
        Cherenkov_photon.Rotate(rot_angle, Cone_Axis);

        // cout<<"Cone_Axis: "<<Cone_Axis.X()<<" "<<Cone_Axis.Y()<<" "<<Cone_Axis.Z()<<endl;
        // cout<<"Cone_Pos: "<<Cone_Pos.X()<<" "<<Cone_Pos.Y()<<" "<<Cone_Pos.Z()<<endl;
        // cout<<"photon_dir: "<<Cherenkov_photon.X()<<" "<<Cherenkov_photon.Y()<<" "<<Cherenkov_photon.Z()<<"  theta: "<<Cherenkov_photon.Theta()<<"  phi:
        // "<<Cherenkov_photon.Phi()<<endl;
        if (Cherenkov_photon.Theta() < 0.8 || Cherenkov_photon.Z() < 0)
          continue;

        f_light_guide = -99;
        f_pixel = -99; // fixme
        // Propagate(Cone_Pos, Cherenkov_photon);

        if (Cherenkov_photon.Theta() > fGeo->reflect_threshold())
          fGeo->Propagate(Cone_Pos, Cherenkov_photon, f_light_guide, f_pixel);

        if (f_light_guide != -99)
          AddHit(trackID, detectorID, position_store, momentum_store, time, angIn, thetaC_store, Cherenkov_photon, f_light_guide, f_pixel);
      }
    }
  }
}

// -----   Add Hit to HitCollection   --------------------------------------
PndDskFLGHit *PndDskFLGHitProducerIdeal::AddHit(Int_t trackID, Int_t detectorID, TVector3 position_store, TVector3 momentum_store, Double_t time, Double_t angIn,
                                                Double_t thetaC_store, TVector3 Cherenkov_photon, Int_t light_guide, Int_t pixel)
{
  TClonesArray &clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndDskFLGHit(trackID, detectorID, position_store, momentum_store, time, angIn, thetaC_store, Cherenkov_photon, light_guide, pixel);
}
// -------------------------------------------------------------------------

// -----   Finish Task   ---------------------------------------------------
void PndDskFLGHitProducerIdeal::Finish()
{

  LOG(info) << " PndDskFLGHitProducerIdeal: Finish";
}
// -------------------------------------------------------------------------

ClassImp(PndDskFLGHitProducerIdeal)
