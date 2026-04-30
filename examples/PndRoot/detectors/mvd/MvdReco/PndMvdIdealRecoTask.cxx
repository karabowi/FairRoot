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
// -----                PndMvdIdealRecoTask source file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TRandom.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

// framework includes
#include "FairRootManager.h"
#include "PndMvdIdealRecoTask.h"
#include "FairRun.h"
#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"
#include "FairHit.h"
// PndMvd includes
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"

// -----   Default constructor   -------------------------------------------
PndMvdIdealRecoTask::PndMvdIdealRecoTask()
  : FairTask("Ideal reconstruction task for PANDA PndMvd"), fPersistance(kTRUE), fBranchName("MVDPoint"), fGeoH(PndGeoHandling::Instance()), fPointArray(nullptr),
    fMctruthArray(nullptr), fHitOutputArray(nullptr), fSigmaX(0.), fSigmaY(0.), fSigmaZ(0.), fCurrentPndMvdMCPoint(nullptr), fCurrentTransMat(nullptr), fHitCovMatrix(3, 3)
{
}
// -------------------------------------------------------------------------

// -----   Constructor   ---------------------------------------------------
PndMvdIdealRecoTask::PndMvdIdealRecoTask(Double_t sx, Double_t sy, Double_t sz)
  : FairTask("Ideal reconstruction task for PANDA PndMvd"), fPersistance(kTRUE), fBranchName("MVDPoint"), fGeoH(PndGeoHandling::Instance()), fPointArray(nullptr),
    fMctruthArray(nullptr), fHitOutputArray(nullptr), fSigmaX(sx), fSigmaY(sy), fSigmaZ(sz), fCurrentPndMvdMCPoint(nullptr), fCurrentTransMat(nullptr), fHitCovMatrix(3, 3)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdIdealRecoTask::~PndMvdIdealRecoTask() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdIdealRecoTask::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndMvdIdealRecoTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject(fBranchName);
  if (!fPointArray) {
    std::cout << "-W- PndMvdIdealRecoTask::Init: "
              << "No " << fBranchName << " array!" << std::endl;
    return kERROR;
  }

  // Get MCTruth collection
  fMctruthArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (fMctruthArray == nullptr) {
    LOG(warn) << " PndMvdIdealRecoTask::Init: No McTruth array!";
    return kERROR;
  }

  // Create and register output array
  fHitOutputArray = new TClonesArray("PndSdsHit");
  ioman->Register("MVDHit", "PndMvd ideal Hits", fHitOutputArray, fPersistance);

  LOG(info) << " gGeoManager = " << gGeoManager;
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndMvdIdealRecoTask::SetParContainers()
{
  // Get Base Container
  //  FairRun* ana = FairRun::Instance();
  //  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  //	PndSdsIdealRecoTask::SetParContainers();
  fGeoH->SetParContainers();
}

// -----   Public method Exec   --------------------------------------------
void PndMvdIdealRecoTask::Exec(Option_t *)
{
  // Fills PndMvdHits with the MC Truth
  // TODO filling of RecoHits, together with the sensor plane

  if (!fHitOutputArray)
    Fatal("Exec", "No fHitOutputArray");
  fHitOutputArray->Delete();

  std::map<Int_t, PndSdsHit *> clusterMap;

  Int_t nPndMvdHits = fPointArray->GetEntriesFast();
  for (Int_t iMvdPoint = 0; iMvdPoint < nPndMvdHits; ++iMvdPoint) {
    fCurrentPndMvdMCPoint = (PndSdsMCPoint *)fPointArray->At(iMvdPoint);
    Int_t trackid = fCurrentPndMvdMCPoint->GetTrackID();
    Int_t size = fHitOutputArray->GetEntriesFast();
    InitTransMat();

    // cut on secondaries (deltas) etc
    if (trackid < 0)
      continue;

    // set the plane definition inside the local frame
    // sensor origin in the middle, u^ and v^ are xy plane
    TVector3 o(0., 0., 0.), u(1., 0., 0.), v(0., 1., 0.);
    CalcDetPlane(o, u, v);
    TVector3 pos;
    fCurrentPndMvdMCPoint->Position(pos);
    smearLocal(pos);
    TVector3 dposLocal(fSigmaX, fSigmaY, fSigmaZ);

    // TODO here we shall distinguish between strip and pixel sensors
    // TODO How to handle the covariance matrix? OR do we really use local point
    // errors. this would avoid two conversations, myabe overload the FairHit
    // functions for the global error points.

    // Now the 3D Info is smared inside the FairHit part of PndSdsHit
    new ((*fHitOutputArray)[size])
      PndSdsHit(fCurrentPndMvdMCPoint->GetDetectorID(), fCurrentPndMvdMCPoint->GetSensorID(), pos, dposLocal, -1, fCurrentPndMvdMCPoint->GetEnergyLoss(), 1, iMvdPoint);

  } // end for PndMvdiMvdPoint

  if (fVerbose > 0) {
    std::cout << fHitOutputArray->GetEntriesFast() << " Hits created out of " << fPointArray->GetEntriesFast() << " Points" << std::endl;
  }
}
// -------------------------------------------------------------------------
void PndMvdIdealRecoTask::InitTransMat()
{
  //     std::cout<<"InitTransMat() with "<<fCurrentPndMvdMCPoint->GetDetName()<<std::endl;
  gGeoManager->cd(fGeoH->GetPath(fCurrentPndMvdMCPoint->GetSensorID()).Data());
  fCurrentTransMat = gGeoManager->GetCurrentMatrix();
  if (fVerbose > 1) {
    fCurrentTransMat->Print("");
  }
}

void PndMvdIdealRecoTask::smear(TVector3 &pos)
{
  /// smear a 3d vector

  Double_t sigx = gRandom->Gaus(0, fSigmaX);
  Double_t sigy = gRandom->Gaus(0, fSigmaY);
  Double_t sigz = gRandom->Gaus(0, fSigmaZ);

  Double_t x = pos.x() + sigx;
  Double_t y = pos.y() + sigy;
  Double_t z = pos.z() + sigz;

  if (fVerbose > 1) {
    std::cout << "PndMvdIdealRecoTask::smear Point (x,y,z)=(" << pos.x() << "," << pos.z() << "," << pos.z() << ") by (" << fSigmaX << "," << fSigmaY << "," << fSigmaZ << ") to ";
  }
  pos.SetXYZ(x, y, z);
  if (fVerbose > 1) {
    std::cout << "(" << pos.x() << "," << pos.y() << "," << pos.z() << ")" << std::endl;
  }
  return;
}

void PndMvdIdealRecoTask::smearLocal(TVector3 &pos)
{
  /// smear a 3d vector in the local sensor plane
  if (fVerbose > 1) {
    std::cout << "PndMvdIdealRecoTask::smearLocal" << std::endl;
  }
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

  //   TMatrixT<Double_t> cov(3,3);
  //   cov[0][0]=fSigmaX; cov[0][1]=0.;      cov[0][2]=0.;
  //   cov[1][0]=0.;      cov[1][1]=fSigmaY; cov[1][2]=0.;
  //   cov[2][0]=0.;      cov[2][1]=0.;      cov[2][2]=fSigmaZ;

  //   TMatrixT<Double_t> rot(3,3);
  //   rot[0][0] =  fCurrentTransMat->GetRotationMatrix()[0];
  //   rot[0][1] =  fCurrentTransMat->GetRotationMatrix()[1];
  //   rot[0][2] =  fCurrentTransMat->GetRotationMatrix()[2];
  //   rot[1][0] =  fCurrentTransMat->GetRotationMatrix()[3];
  //   rot[1][1] =  fCurrentTransMat->GetRotationMatrix()[4];
  //   rot[1][2] =  fCurrentTransMat->GetRotationMatrix()[5];
  //   rot[2][0] =  fCurrentTransMat->GetRotationMatrix()[6];
  //   rot[2][1] =  fCurrentTransMat->GetRotationMatrix()[7];
  //   rot[2][2] =  fCurrentTransMat->GetRotationMatrix()[8];

  //   fHitCovMatrix = cov;
  //   fHitCovMatrix *= rot;

  return;
}

void PndMvdIdealRecoTask::CalcDetPlane(TVector3 &oVect, TVector3 &uVect, TVector3 &vVect)
{
  Double_t O[3], U[3], V[3], o[3], u[3], v[3];
  O[0] = oVect.x();
  O[1] = oVect.y();
  O[2] = oVect.z();
  U[0] = uVect.x();
  U[1] = uVect.y();
  U[2] = uVect.z();
  V[0] = vVect.x();
  V[1] = vVect.y();
  V[2] = vVect.z();

  if (fVerbose > 1) {
    std::cout << "PndMvdIdealRecoTask::CalcDetPlane from Detector " << fCurrentPndMvdMCPoint->GetSensorID() << std::endl;
  }
  // make transformation
  fCurrentTransMat->LocalToMaster(O, o);
  fCurrentTransMat->LocalToMaster(U, u);
  fCurrentTransMat->LocalToMaster(V, v);
  oVect.SetXYZ(o[0], o[1], o[2]);
  uVect.SetXYZ(u[0], u[1], u[2]);
  vVect.SetXYZ(v[0], v[1], v[2]);
}

ClassImp(PndMvdIdealRecoTask);
