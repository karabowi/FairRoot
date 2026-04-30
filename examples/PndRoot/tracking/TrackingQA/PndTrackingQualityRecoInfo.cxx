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

//
// PndTrackingQualityRecoInfo.cxx
//
//
//
//
// authors: Lia Lavezzi - University of Torino (2015)
//

#include "PndTrackingQualityRecoInfo.h"
#include "FairLink.h"
#include <iostream>

using namespace std;

PndTrackingQualityRecoInfo::PndTrackingQualityRecoInfo()
  : fNofMvdPixelTrueHits(0), fNofMvdStripTrueHits(0), fNofSttTrueHits(0), fNofGemTrueHits(0), fNofFtsTrueHits(0), fNofMvdPixelFakeHits(0), fNofMvdStripFakeHits(0),
    fNofSttFakeHits(0), fNofGemFakeHits(0), fNofFtsFakeHits(0), fNofMvdPixelMissingHits(0), fNofMvdStripMissingHits(0), fNofSttMissingHits(0), fNofGemMissingHits(0),
    fNofFtsMissingHits(0), fRecoTrackID(-1), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)), fPosLast(TVector3(0., 0., 0.)), fMomLast(TVector3(0., 0., 0.)),
    fMCTrackInfo(PndTrackingQualityMCInfo()), fFlag(-1), fNofMCTracks(0), fIdealTrackId(-1), fMCMomFirst(TVector3(0., 0., 0.)), fMCMomLast(TVector3(0., 0., 0.)),
    fMCVertex(TVector3(0., 0., 0.)), fMCMomentum(TVector3(0., 0., 0.)), fMCCharge(-100), fMCPdg(0), fIsPrimary(kFALSE), fQuality(-100), fMCQuality(-100), fCharge(-100),
    fMCTrackID(-1), fTrue(kFALSE)
{
}

PndTrackingQualityRecoInfo::PndTrackingQualityRecoInfo(int recotrackid)
  : fNofMvdPixelTrueHits(0), fNofMvdStripTrueHits(0), fNofSttTrueHits(0), fNofGemTrueHits(0), fNofFtsTrueHits(0), fNofMvdPixelFakeHits(0), fNofMvdStripFakeHits(0),
    fNofSttFakeHits(0), fNofGemFakeHits(0), fNofFtsFakeHits(0), fNofMvdPixelMissingHits(0), fNofMvdStripMissingHits(0), fNofSttMissingHits(0), fNofGemMissingHits(0),
    fNofFtsMissingHits(0), fRecoTrackID(recotrackid), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)), fPosLast(TVector3(0., 0., 0.)),
    fMomLast(TVector3(0., 0., 0.)), fMCTrackInfo(PndTrackingQualityMCInfo()), fFlag(-1), fNofMCTracks(0), fIdealTrackId(-1), fMCMomLast(TVector3(0., 0., 0.)),
    fMCVertex(TVector3(0., 0., 0.)), fMCMomentum(TVector3(0., 0., 0.)), fMCCharge(-100), fMCPdg(0), fIsPrimary(kFALSE), fQuality(-100), fMCQuality(-100), fCharge(-100),
    fMCTrackID(-1), fTrue(kFALSE)
{
}

PndTrackingQualityRecoInfo::~PndTrackingQualityRecoInfo() {}

double PndTrackingQualityRecoInfo::GetEfficiency()
{
  if (fMCTrackInfo.GetNofMCPoints() == 0)
    return -1; // CHECK
  return (double)GetNofRecoTrueHits() / fMCTrackInfo.GetNofMCPoints();
}

double PndTrackingQualityRecoInfo::GetMvdPixelEfficiency()
{
  if (fMCTrackInfo.GetNofMvdPixelPoints() == 0)
    return -1; // CHECK
  return (double)GetNofMvdPixelTrueHits() / fMCTrackInfo.GetNofMvdPixelPoints();
}

double PndTrackingQualityRecoInfo::GetMvdStripEfficiency()
{
  if (fMCTrackInfo.GetNofMvdStripPoints() == 0)
    return -1; // CHECK
  return (double)GetNofMvdStripTrueHits() / fMCTrackInfo.GetNofMvdStripPoints();
}

double PndTrackingQualityRecoInfo::GetMvdEfficiency()
{
  if (fMCTrackInfo.GetNofMvdPoints() == 0)
    return -1; // CHECK
  return (double)GetNofMvdTrueHits() / fMCTrackInfo.GetNofMvdPoints();
}

// double PndTrackingQualityRecoInfo::GetSttParalEfficiency() {
//   if(fMCTrackInfo.GetNofSttParalPoints() == 0) return -1;  // CHECK
//   return (double) GetNofSttParalTrueHits()/fMCTrackInfo.GetNofSttParalPoints();
// }

// double PndTrackingQualityRecoInfo::GetSttSkewEfficiency() {
//   if(fMCTrackInfo.GetNofSttSkewPoints() == 0) return -1;  // CHECK
//   return (double) GetNofSttSkewTrueHits()/fMCTrackInfo.GetNofSttSkewPoints();
// }

double PndTrackingQualityRecoInfo::GetSttEfficiency()
{
  if (fMCTrackInfo.GetNofSttPoints() == 0)
    return -1; // CHECK
  return (double)GetNofSttTrueHits() / fMCTrackInfo.GetNofSttPoints();
}

double PndTrackingQualityRecoInfo::GetGemEfficiency()
{
  if (fMCTrackInfo.GetNofGemPoints() == 0)
    return -1; // CHECK
  return (double)GetNofGemTrueHits() / fMCTrackInfo.GetNofGemPoints();
}

double PndTrackingQualityRecoInfo::GetFtsEfficiency()
{
  if (fMCTrackInfo.GetNofFtsPoints() == 0)
    return -1; // CHECK
  return (double)GetNofFtsTrueHits() / fMCTrackInfo.GetNofFtsPoints();
}

// double PndTrackingQualityRecoInfo::GetSciTilEfficiency() {
//   if(fMCTrackInfo.GetNofSciTilPoints() == 0) return -1;  // CHECK
//   return (double) GetNofSciTilTrueHits()/fMCTrackInfo.GetNofSciTilPoints();
// }

double PndTrackingQualityRecoInfo::GetInefficiency()
{
  if (fMCTrackInfo.GetNofMCPoints() == 0)
    return -1; // CHECK
  return (double)GetNofMissingHits() / fMCTrackInfo.GetNofMCPoints();
}

double PndTrackingQualityRecoInfo::GetMvdPixelInefficiency()
{
  if (fMCTrackInfo.GetNofMvdPixelPoints() == 0)
    return -1; // CHECK
  return (double)GetNofMvdPixelMissingHits() / fMCTrackInfo.GetNofMvdPixelPoints();
}

double PndTrackingQualityRecoInfo::GetMvdStripInefficiency()
{
  if (fMCTrackInfo.GetNofMvdStripPoints() == 0)
    return -1; // CHECK
  return (double)GetNofMvdStripMissingHits() / fMCTrackInfo.GetNofMvdStripPoints();
}

double PndTrackingQualityRecoInfo::GetMvdInefficiency()
{
  if (fMCTrackInfo.GetNofMvdPoints() == 0)
    return -1; // CHECK
  return (double)GetNofMvdMissingHits() / fMCTrackInfo.GetNofMvdPoints();
}

// double PndTrackingQualityRecoInfo::GetSttParalInefficiency() {
//   if(fMCTrackInfo.GetNofSttParalPoints() == 0) return -1;  // CHECK
//   return (double) GetNofSttParalMissingHits()/fMCTrackInfo.GetNofSttParalPoints();
// }

// double PndTrackingQualityRecoInfo::GetSttSkewInefficiency() {
//   if(fMCTrackInfo.GetNofSttSkewPoints() == 0) return -1;  // CHECK
//   return (double) GetNofSttSkewMissingHits()/fMCTrackInfo.GetNofSttSkewPoints();
// }

double PndTrackingQualityRecoInfo::GetSttInefficiency()
{
  if (fMCTrackInfo.GetNofSttPoints() == 0)
    return -1; // CHECK
  return (double)GetNofSttMissingHits() / fMCTrackInfo.GetNofSttPoints();
}

double PndTrackingQualityRecoInfo::GetGemInefficiency()
{
  if (fMCTrackInfo.GetNofGemPoints() == 0)
    return -1; // CHECK
  return (double)GetNofGemMissingHits() / fMCTrackInfo.GetNofGemPoints();
}

double PndTrackingQualityRecoInfo::GetFtsInefficiency()
{
  if (fMCTrackInfo.GetNofFtsPoints() == 0)
    return -1; // CHECK
  return (double)GetNofFtsMissingHits() / fMCTrackInfo.GetNofFtsPoints();
}

// double PndTrackingQualityRecoInfo::GetSciTilInefficiency() {
//   if(fMCTrackInfo.GetNofSciTilPoints() == 0) return -1;  // CHECK
//   return (double) GetNofSciTilMissingHits()/fMCTrackInfo.GetNofSciTilPoints();
// }

double PndTrackingQualityRecoInfo::GetPurity()
{
  if (GetNofRecoHits() == 0)
    return -1; // CHECK
  return (double)GetNofRecoTrueHits() / GetNofRecoHits();
}
// double PndTrackingQualityRecoInfo::GetSttParallelPurity() {
//   if(GetNofSttParallelHits() == 0) return -1;  // CHECK
//   return (double) GetNofSttParallelTrueHits()/GetNofSttParallelHits();
// }
// double PndTrackingQualityRecoInfo::GetSttSkewPurity() {
//   if(GetNofSttSkewHits() == 0) return -1;  // CHECK
//   return (double) GetNofSttSkewTrueHits()/GetNofSttSkewHits();
// }
double PndTrackingQualityRecoInfo::GetSttPurity()
{
  if (GetNofSttHits() == 0)
    return -1; // CHECK
  return (double)GetNofSttTrueHits() / GetNofSttHits();
}
double PndTrackingQualityRecoInfo::GetMvdPixelPurity()
{
  if (GetNofMvdPixelHits() == 0)
    return -1; // CHECK
  return (double)GetNofMvdPixelTrueHits() / GetNofMvdPixelHits();
}
double PndTrackingQualityRecoInfo::GetMvdStripPurity()
{
  if (GetNofMvdStripHits() == 0)
    return -1; // CHECK
  return (double)GetNofMvdStripTrueHits() / GetNofMvdStripHits();
}
double PndTrackingQualityRecoInfo::GetMvdPurity()
{
  if (GetNofMvdHits() == 0)
    return -1; // CHECK
  return (double)GetNofMvdTrueHits() / GetNofMvdHits();
}
double PndTrackingQualityRecoInfo::GetGemPurity()
{
  if (GetNofGemHits() == 0)
    return -1; // CHECK
  return (double)GetNofGemTrueHits() / GetNofGemHits();
}

double PndTrackingQualityRecoInfo::GetFtsPurity()
{
  if (GetNofFtsHits() == 0)
    return -1; // CHECK
  return (double)GetNofFtsTrueHits() / GetNofFtsHits();
}
// double PndTrackingQualityRecoInfo::GetSciTilPurity() {
//   if(GetNofSciTilHits() == 0) return -1;  // CHECK
//   return (double) GetNofSciTilTrueHits()/GetNofSciTilHits();
// }

double PndTrackingQualityRecoInfo::GetContamination()
{
  if (GetNofRecoHits() == 0)
    return -1; // CHECK
  return (double)GetNofRecoFakeHits() / GetNofRecoHits();
}
// double PndTrackingQualityRecoInfo::GetSttParallelContamination() {
//   if(GetNofSttParallelHits() == 0) return -1;  // CHECK
//   return (double) GetNofSttParallelFakeHits()/GetNofSttParallelHits();
// }
// double PndTrackingQualityRecoInfo::GetSttSkewContamination() {
//   if(GetNofSttSkewHits() == 0) return -1;  // CHECK
//   return (double) GetNofSttSkewFakeHits()/GetNofSttSkewHits();
// }
double PndTrackingQualityRecoInfo::GetSttContamination()
{
  if (GetNofSttHits() == 0)
    return -1; // CHECK
  return (double)GetNofSttFakeHits() / GetNofSttHits();
}
double PndTrackingQualityRecoInfo::GetMvdPixelContamination()
{
  if (GetNofMvdPixelHits() == 0)
    return -1; // CHECK
  return (double)GetNofMvdPixelFakeHits() / GetNofMvdPixelHits();
}
double PndTrackingQualityRecoInfo::GetMvdStripContamination()
{
  if (GetNofMvdStripHits() == 0)
    return -1; // CHECK
  return (double)GetNofMvdStripFakeHits() / GetNofMvdStripHits();
}
double PndTrackingQualityRecoInfo::GetMvdContamination()
{
  if (GetNofMvdHits() == 0)
    return -1; // CHECK
  return (double)GetNofMvdFakeHits() / GetNofMvdHits();
}
double PndTrackingQualityRecoInfo::GetGemContamination()
{
  if (GetNofGemHits() == 0)
    return -1; // CHECK
  return (double)GetNofGemFakeHits() / GetNofGemHits();
}
// double PndTrackingQualityRecoInfo::GetSciTilContamination() {
//   if(GetNofSciTilHits() == 0) return -1;  // CHECK
//   return (double) GetNofSciTilFakeHits()/GetNofSciTilHits();
// }
ClassImp(PndTrackingQualityRecoInfo)
