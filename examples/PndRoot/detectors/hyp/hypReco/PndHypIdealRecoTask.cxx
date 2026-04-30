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
// -----                MvdIdealRecoTask source file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -----              modified for Hyp detector by A.Sanchez     ------
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
#include "PndHypIdealRecoTask.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"
#include "FairHit.h"
#include "FairLogger.h"
// Hyp includes
#include "PndHypHit.h"
#include "PndHypPoint.h"

// -----   Default constructor   -------------------------------------------
PndHypIdealRecoTask::PndHypIdealRecoTask() : FairTask("Ideal reconstruction task for PANDA Hyp"), fHitCovMatrix(3, 3)
{
  fSigmaX = 0.;
  fSigmaY = 0.;
  fSigmaZ = 0.;
  fBranchName = "HypPoint";
}
// -------------------------------------------------------------------------

// -----   Constructor   ---------------------------------------------------
PndHypIdealRecoTask::PndHypIdealRecoTask(Double_t sx, Double_t sy, Double_t sz) : FairTask("Ideal reconstruction task for PANDA Hyp"), fHitCovMatrix(3, 3)
{
  fSigmaX = sx;
  fSigmaY = sy;
  fSigmaZ = sz;
  fBranchName = "HypPoint";
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHypIdealRecoTask::~PndHypIdealRecoTask()
{
  delete fGeoH;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndHypIdealRecoTask::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndHypIdealRecoTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject(fBranchName);
  if (!fPointArray) {
    std::cout << "-W- PndHypIdealRecoTask::Init: "
              << "No " << fBranchName << " array!" << std::endl;
    return kERROR;
  }

  // Get MCTruth collection
  fMctruthArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (fMctruthArray == nullptr) {
    LOG(warn) << " PndHypIdealRecoTask::Init: No McTruth array!";
    return kERROR;
  }

  // Create and register output array
  fHitOutputArray = new TClonesArray("PndHypHit");
  ioman->Register("HypHit", "Hyp ideal Clusters", fHitOutputArray, kTRUE);

  LOG(info) << " gGeoManager = " << gGeoManager;

  fGeoH = new PndHypGeoHandling(gGeoManager);

  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndHypIdealRecoTask::SetParContainers()
{
  // Get Base Container
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  LOG(info) << " rtdb = " << rtdb;
}

// -----   Public method Exec   --------------------------------------------
void PndHypIdealRecoTask::Exec(Option_t *)
{
  // Fills HypClusters with the MC Truth
  // TODO filling of RecoHits, together with the sensor plane

  if (!fHitOutputArray)
    Fatal("Exec", "No fClusterOutputArray");
  fHitOutputArray->Delete();

  // std::map<int, PndHypHit*> clusterMap;
  LOG(info) << " fPointArray Exec = " << fPointArray;
  Int_t nHypHits = fPointArray->GetEntries();
  std::cout << " entries " << nHypHits << std::endl;

  for (Int_t iHypHit = 0; iHypHit < nHypHits; ++iHypHit) {
    fCurrentHypPoint = (PndHypPoint *)fPointArray->At(iHypHit);
    // int trackid=fCurrentHypPoint->GetTrackID(); //[R.K. 01/2017] unused variable
    int size = fHitOutputArray->GetEntriesFast();
    InitTransMat();

    // cut on secondaries (deltas) etc
    // if(trackid<0)continue;

    cout << fCurrentHypPoint->GetVolumeID() << endl;
    // set the plane definition inside the local frame
    // sensor origin in the middle, u^ and v^ are xy plane
    TVector3 o(0., 0., 0.), u(1., 0., 0.), v(0., 1., 0.);
    CalcDetPlane(o, u, v);
    TVector3 pos;
    // fCurrentHypPoint->Position(pos);
    cout << fCurrentHypPoint->GetXin() << endl;
    pos.SetXYZ(fCurrentHypPoint->GetXin(), fCurrentHypPoint->GetYin(), fCurrentHypPoint->GetZin());
    cout << pos.x() << " " << pos.y() << " " << pos.z() << endl;

    smearLocal(pos);
    TVector3 dposLocal(fSigmaX, fSigmaY, fSigmaZ);

    // TODO here we shall distinguish between strip and pixel sensors
    // Now the 3D Info is smared inside the FairHit part of HypCluster
    new ((*fHitOutputArray)[size])
      PndHypHit(fCurrentHypPoint->GetVolumeID(), (fCurrentHypPoint->GetDetName()).Data(), pos, dposLocal, iHypHit, fCurrentHypPoint->GetEnergyLoss(), 1);

  } // end for HypiHypHit

  if (fVerbose > 0) {
    std::cout << fHitOutputArray->GetEntriesFast() << " hitss created out of " << fPointArray->GetEntriesFast() << " Points" << std::endl;
  }
  fPointArray->Delete();
}
// -------------------------------------------------------------------------
void PndHypIdealRecoTask::InitTransMat()
{
  gGeoManager->cd(fGeoH->GetPath(fCurrentHypPoint->GetDetName()).Data());
  fCurrentTransMat = gGeoManager->GetCurrentMatrix();
  if (fVerbose > 1) {
    fCurrentTransMat->Print("");
  }
}

void PndHypIdealRecoTask::smear(TVector3 &pos)
{
  /// smear a 3d vector

  double sigx = gRandom->Gaus(0, fSigmaX);
  double sigy = gRandom->Gaus(0, fSigmaY);
  double sigz = gRandom->Gaus(0, fSigmaZ);

  double x = pos.x() + sigx;
  double y = pos.y() + sigy;
  double z = pos.z() + sigz;

  if (fVerbose > 1) {
    std::cout << "PndHypIdealRecoTask::smear Point (x,y,z)=(" << pos.x() << "," << pos.y() << "," << pos.z() << ") by (" << fSigmaX << "," << fSigmaY << "," << fSigmaZ << ") to ";
  }
  pos.SetXYZ(x, y, z);
  if (fVerbose > 1) {
    std::cout << "(" << pos.x() << "," << pos.y() << "," << pos.z() << ")" << std::endl;
  }
  return;
}

void PndHypIdealRecoTask::smearLocal(TVector3 &pos)
{
  /// smear a 3d vector in the local sensor plane
  if (fVerbose > 1) {
    std::cout << "PndHypIdealRecoTask::smearLocal" << std::endl;
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

  //   TMatrixT<double> cov(3,3);
  //   cov[0][0]=fSigmaX; cov[0][1]=0.;      cov[0][2]=0.;
  //   cov[1][0]=0.;      cov[1][1]=fSigmaY; cov[1][2]=0.;
  //   cov[2][0]=0.;      cov[2][1]=0.;      cov[2][2]=fSigmaZ;

  //   TMatrixT<double> rot(3,3);
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

void PndHypIdealRecoTask::CalcDetPlane(TVector3 &oVect, TVector3 &uVect, TVector3 &vVect)
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
    std::cout << "PndHypIdealRecoTask::CalcDetPlane from Detector " << fCurrentHypPoint->GetDetName() << std::endl;
  }
  // make transformation
  fCurrentTransMat->LocalToMaster(O, o);
  fCurrentTransMat->LocalToMaster(U, u);
  fCurrentTransMat->LocalToMaster(V, v);
  oVect.SetXYZ(o[0], o[1], o[2]);
  uVect.SetXYZ(u[0], u[1], u[2]);
  vVect.SetXYZ(v[0], v[1], v[2]);
}

ClassImp(PndHypIdealRecoTask)
