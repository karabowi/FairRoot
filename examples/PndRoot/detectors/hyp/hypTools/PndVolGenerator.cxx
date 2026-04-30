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
// -----            PndVolGenerator source file                        -----
// -----          Created 22/05/07  by S.Spataro                       -----
// -------------------------------------------------------------------------

#include "TRandom.h"
#include "TMath.h"
#include "PndVolGenerator.h"
#include "FairPrimaryGenerator.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"

#include "FairVolume.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "FairLogger.h"
#include "TGeoVolume.h"
//#include "TGeoVolumeAssembly.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"

// ------------------------------------------------------------------------
PndVolGenerator::PndVolGenerator()
  : fPDGType(0), fMult(0), fPDGMass(0), fPtMin(0), fPtMax(0), fPhiMin(0), fPhiMax(0), fEtaMin(0), fEtaMax(0), fYMin(0), fYMax(0), fPMin(0), fPMax(0), fThetaMin(0), fThetaMax(0),
    fX(0), fY(0), fZ(0), fX1(0), fY1(0), fX2(0), fY2(0), fEtaRangeIsSet(0), fYRangeIsSet(0), fThetaRangeIsSet(0), fCosThetaIsSet(0), fPtRangeIsSet(0), fPRangeIsSet(0),
    fInversePIsSet(0), fAbs(0), fQuad(0), fFunc(0), fExt(0)
{
  // fGeoH = PndGeoHandling::Instance();
  // Default constructor
}

// ------------------------------------------------------------------------
PndVolGenerator::PndVolGenerator(Int_t pdgid, Int_t mult)
  : fPDGType(pdgid), fMult(mult), fPDGMass(0), fEtaMin(0), fEtaMax(0), fYMin(0), fYMax(0), fPMin(0), fPMax(0), fX(0), fY(0), fZ(0), fX1(0), fY1(0), fX2(0), fY2(0),
    fEtaRangeIsSet(0), fYRangeIsSet(0), fThetaRangeIsSet(0), fCosThetaIsSet(0), fPtRangeIsSet(0), fPRangeIsSet(0), fInversePIsSet(0), fAbs(0), fQuad(0), fFunc(0), fExt(0)

{
  // Constructor. Set default kinematics limits
  SetPhiRange();
}
// ------------------------------------------------------------------------
Bool_t PndVolGenerator::Init()
{
  // Initialize generator

  if (fPhiMax - fPhiMin > 360)
    Fatal("Init()", "PndVolGenerator: phi range is too wide: %f<phi<%f", fPhiMin, fPhiMax);
  if (fPRangeIsSet && fPtRangeIsSet)
    Fatal("Init()", "PndVolGenerator: Cannot set P and Pt ranges simultaneously");
  if (fPRangeIsSet && fYRangeIsSet)
    Fatal("Init()", "PndVolGenerator: Cannot set P and Y ranges simultaneously");
  if ((fThetaRangeIsSet && fYRangeIsSet) || (fThetaRangeIsSet && fEtaRangeIsSet) || (fYRangeIsSet && fEtaRangeIsSet))
    Fatal("Init()", "PndVolGenerator: Cannot set Y, Theta or Eta ranges simultaneously");
  if (fPointVtxIsSet && fVolVtxIsSet)
    Fatal("Init()", "PndVolGenerator: Cannot set point and box vertices simultaneously");
  if (fInversePIsSet && ((fPRangeIsSet && (fPMin == 0 || fPMax == 0)) || (fPRangeIsSet && (fPtMin == 0 || fPtMax == 0))))
    Fatal("Init()", "PndVolGenerator: Cannot use P == 0 as limit for inverse momentum distribution");

  // Check for particle type
  // FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  // FairGeoInterface *geoFace = geoLoad->getGeoInterface();

  std::cout << " gGeoManager 1 " << gGeoManager << " " << std::endl;

  TDatabasePDG *pdgBase = TDatabasePDG::Instance();
  TParticlePDG *particle = pdgBase->GetParticle(fPDGType);
  if (!particle)
    Fatal("PndVolGenerator", "PDG code %d not defined.", fPDGType);
  fPDGMass = particle->Mass();
}

// ------------------------------------------------------------------------
Bool_t PndVolGenerator::ReadEvent(FairPrimaryGenerator *primGen)
{
  // Generate one event: produce primary particles emitted from one vertex.
  // Primary particles are distributed uniformly along
  // those kinematics variables which were limitted by setters.
  // if SetCosTheta() function is used, the distribution will be uniform in cos(theta)

  Double32_t pabs, phi, pt, theta = 0, eta, y, mt, px, py, pz, pinv = 0;

  // TGeoVolume *top ;
  // if(gGeoManager)top = (TGeoVolume*)gGeoManager->GetTopVolume();

  TGeoVolume *top;
  if (gGeoManager)
    top = (TGeoVolume *)gGeoManager->GetTopVolume();
  else {
    LOG(error) << "No gGeoManager";
    return kFALSE;
  }
  gGeoManager->GetCache()->BuildIdArray();
  std::cout << " gGeoManager 2 " << gGeoManager << std::endl;

  // if(gGeoManager)std::cout<<" gGeoManager 2 "<<gGeoManager<<" "<<top<<std::endl;
  // std::cout<<" first volume "<<gGeoManager->GetCurrentVolume()->GetName()<<std::endl;

  if (top->GetName() != gGeoManager->GetCurrentVolume()->GetName()) {
    gGeoManager->CdTop();
    //  std::cout<<" first volume Top "<<gGeoManager->GetCurrentVolume()->GetName()<<" top "<<top->GetName()<<std::endl;
  }
  gGeoManager->CdDown(0); // complex
  // std::cout<<"  volume 1 "<<gGeoManager->GetPath()<<" "
  //   <<gGeoManager->GetCurrentVolume()->GetNdaughters()<<std::endl;

  Int_t Daught;
  Daught = gGeoManager->GetCurrentVolume()->GetNdaughters();

  gGeoManager->CdDown(fQuad); // stgQuad0 , 1 -> stgQuad2 , CdUp(), CdDown(2)

  // std::cout<<"  volume 2 "<<gGeoManager->GetPath()<<" "
  //   <<gGeoManager->GetCurrentVolume()->GetNdaughters()<<std::endl;

  if (fExt) {
    gGeoManager->CdDown(fAbs); // stglAb0 ,0 -> stglSi0_0 ,CdUp(), CdDown(1)
  } else {
    fAbs = TMath::Nint(fFunc->GetRandom());
    std::cout << " random fabs " << fAbs << std::endl;
    gGeoManager->CdDown(fAbs);
  }

  fCurrentTransMat = gGeoManager->GetCurrentMatrix();
  Double_t posLab[3], posSens[3];

  posSens[0] = 0.;
  posSens[1] = 0.;
  posSens[2] = 0.;
  fCurrentTransMat->LocalToMaster(posSens, posLab);

  fX = posLab[0];
  fY = posLab[1];
  fZ = posLab[2];

  std::cout << " vertex " << fX << " " << fY << " " << fZ << std::endl;

  // Generate particles
  for (Int_t k = 0; k < fMult; k++) {
    phi = gRandom->Uniform(fPhiMin, fPhiMax) * TMath::DegToRad();

    if (fPRangeIsSet) {
      if (fInversePIsSet) {
        pinv = gRandom->Uniform(1. / fPMax, 1. / fPMin);
        pabs = 1. / pinv;
      } else {
        pabs = gRandom->Uniform(fPMin, fPMax);
      }
    } else if (fPtRangeIsSet) {
      if (fInversePIsSet) {
        pinv = gRandom->Uniform(1. / fPtMax, 1. / fPtMin);
        pt = 1. / pinv;
      } else {
        pt = gRandom->Uniform(fPtMin, fPtMax);
      }
    }

    if (fThetaRangeIsSet) {
      if (fCosThetaIsSet)
        theta = acos(gRandom->Uniform(cos(fThetaMin * TMath::DegToRad()), cos(fThetaMax * TMath::DegToRad())));
      else
        theta = gRandom->Uniform(fThetaMin, fThetaMax) * TMath::DegToRad();
    } else if (fEtaRangeIsSet) {
      eta = gRandom->Uniform(fEtaMin, fEtaMax);
      theta = 2 * TMath::ATan(TMath::Exp(-eta));
    } else if (fYRangeIsSet) {
      y = gRandom->Uniform(fYMin, fYMax);
      mt = TMath::Sqrt(fPDGMass * fPDGMass + pt * pt);
      pz = mt * TMath::SinH(y);
    }

    if (fThetaRangeIsSet || fEtaRangeIsSet) {
      if (fPRangeIsSet) {
        pz = pabs * TMath::Cos(theta);
        pt = pabs * TMath::Sin(theta);
      } else if (fPtRangeIsSet)
        pz = pt / TMath::Tan(theta);
    }

    px = pt * TMath::Cos(phi);
    py = pt * TMath::Sin(phi);

    if (fVolVtxIsSet) {
      fX = gRandom->Uniform(fX1, fX2);
      fY = gRandom->Uniform(fY1, fY2);
    }

    if (fDebug)
      printf("VolGen: kf=%d, p=(%.2f, %.2f, %.2f) GeV, x=(%.1f, %.1f, %.1f) cm\n", fPDGType, px, py, pz, fX, fY, fZ);

    primGen->AddTrack(fPDGType, px, py, pz, fX, fY, fZ);
  }
  return kTRUE;
}
// ------------------------------------------------------------------------

ClassImp(PndVolGenerator)
