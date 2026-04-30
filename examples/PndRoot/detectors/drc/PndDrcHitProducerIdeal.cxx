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
// -----                   PndDrcHitProducerIdeal source file               -----
// -----               Created 11/10/06  by Annalisa Cecchi            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include "stdio.h"

#include "PndGeoDrc.h"
#include "PndDrcHitProducerIdeal.h"
#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "PndDrcBarPoint.h"
#include "PndDrcHit.h"
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
#include "PndGeoDrcPar.h"
#include "TFormula.h"
#include "TMath.h"
#include "TGeoMatrix.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TPDGCode.h"
#include "TGeoManager.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrcHitProducerIdeal::PndDrcHitProducerIdeal() : PndPersistencyTask("PndDrcHitProducerIdeal"), fGeo(new PndGeoDrc())
{
  fVerbose = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor with verbosity level  -------------------------------------------

PndDrcHitProducerIdeal::PndDrcHitProducerIdeal(Int_t verbose) : PndPersistencyTask("PndDrcHitProducerIdeal"), fGeo(new PndGeoDrc())
{
  fVerbose = verbose;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndDrcHitProducerIdeal::~PndDrcHitProducerIdeal()
{
  if (fGeo)
    delete fGeo;
}
// -------------------------------------------------------------------------

// -----   Initialization   -----------------------------------------------
// -------------------------------------------------------------------------
InitStatus PndDrcHitProducerIdeal::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndDrcHitProducerIdeal::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fBarPointArray = (TClonesArray *)ioman->GetObject("DrcBarPoint");
  if (!fBarPointArray) {
    cout << "-W- PndDrcHitProducerIdeal::Init: "
         << "No DrcBarPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndDrcHit");
  ioman->Register("DrcHit", "Drc", fHitArray, GetPersistency());

  LOG(info) << " PndDrcHitProducerIdeal: Intialization successfull";

  return kSUCCESS;
}

// -----   Execution of Task   ---------------------------------------------
// -------------------------------------------------------------------------
void PndDrcHitProducerIdeal::Exec(Option_t *) // ion //[R.K.03/2017] unused variable(s)
{
  if (!fHitArray)
    Fatal("Exec", "No HitArray");
  fHitArray->Clear();

  PndDrcBarPoint *pt = nullptr;
  nevents++;

  if (fVerbose > 0) {
    cout << " ----------------- DRC Hit Producer --------------------" << endl;
    cout << " Number of input MC points in the bar: " << fBarPointArray->GetEntries() << endl;
  }

  // fNHits = 0;

  // Loop over PndDrcPoints
  for (Int_t j = 0; j < fBarPointArray->GetEntriesFast(); j++) {
    if (fVerbose > 1)
      printf("\n\n=====> Event No. %d\n", nevents);

    pt = (PndDrcBarPoint *)fBarPointArray->At(j);

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
      if (fVerbose > 0)
        cout << "Beta not calculated " << endl;
    }

    if (pt->GetThetaC() != -1. && beta > 1 / 1.47) {
      fDetectorID = pt->GetBoxId() * 10 + pt->GetBarId();

      // TGeoNode *dircNode = (TGeoNode*)
      gGeoManager->FindNode(pt->GetX(), pt->GetY(), pt->GetZ());
      TGeoMatrix *dircMat = (TGeoMatrix *)gGeoManager->GetCurrentMatrix();
      const Double_t *dircPos = dircMat->GetTranslation();
      fPosHit.SetXYZ(dircPos[0], dircPos[1], dircPos[2]);

      Double_t fDPosXHit = 0.5; // mm
      Double_t fDPosYHit = 0.5;
      Double_t fDPosZHit = 0.;
      fDPosHit.SetXYZ(fDPosXHit, fDPosYHit, fDPosZHit);

      fThetaC = gRandom->Gaus(pt->GetThetaC(), 0.003);
      fErrThetaC = 0.; // rad

      fRefIndex = j;

      // PndMCTrack* tr = nullptr;

      AddHit(fDetectorID, fDetectorID, fPosHit, fDPosHit, fThetaC, fErrThetaC, fRefIndex);
    }
  }
}

// -----   Add Hit to HitCollection   --------------------------------------
PndDrcHit *PndDrcHitProducerIdeal::AddHit(Int_t detID,
                                          Int_t, // sensorID //[R.K.03/2017] unused variable(s)
                                          TVector3 posHit, TVector3 dPosHit, Double_t thetaC, Double_t errThetaC, Int_t index)
{
  TClonesArray &clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndDrcHit(detID, detID, posHit, dPosHit, thetaC, errThetaC, index);
}

// -------------------------------------------------------------------------

// -----   Finish Task   ---------------------------------------------------
void PndDrcHitProducerIdeal::Finish()
{

  LOG(info) << " PndDrcHitProducerIdeal: Finish";
}
// -------------------------------------------------------------------------

ClassImp(PndDrcHitProducerIdeal)
