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
// PndTrackingQualityMCInfo.cxx
//
//
//
//
// authors: Lia Lavezzi - University of Torino (2015)
//

#include "PndTrackingQualityMCInfo.h"
#include <iostream>

using namespace std;

PndTrackingQualityMCInfo::PndTrackingQualityMCInfo()
  : fNofMvdPixelPoints(0), fNofMvdStripPoints(0), fNofSttParalPoints(0), fNofSttSkewPoints(0), fNofGemPoints(0), fNofFtsPoints(0), fReconstructabilityStatus(-1), fMCTrackID(-1),
    fRecoTrackIDs(), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)), fPosLast(TVector3(0., 0., 0.)), fMomLast(TVector3(0., 0., 0.)), fCharge(0),
    fIsPrimary(kFALSE), fVertex(TVector3(-10000., -10000., -10000.)), fMomentum(TVector3(-10000., -10000., -10000.)), fPDGCode(-1), fQuality(0.), fMCQuality(0),
    fAssoRecoTrackID(-1)
{
}

PndTrackingQualityMCInfo::PndTrackingQualityMCInfo(int nofmvdpix, int nofmvdstr, int nofsttparal, int nofsttskew, int nofgem, int noffts)
  : fNofMvdPixelPoints(nofmvdpix), fNofMvdStripPoints(nofmvdstr), fNofSttParalPoints(nofsttparal), fNofSttSkewPoints(nofsttskew), fNofGemPoints(nofgem), fNofFtsPoints(noffts),
    fReconstructabilityStatus(-1), fMCTrackID(-1), fRecoTrackIDs(), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)), fPosLast(TVector3(0., 0., 0.)),
    fMomLast(TVector3(0., 0., 0.)), fCharge(0), fIsPrimary(kFALSE), fVertex(TVector3(-10000., -10000., -10000.)), fMomentum(TVector3(-10000., -10000., -10000.)), fPDGCode(-1),
    fQuality(0.), fMCQuality(0), fAssoRecoTrackID(-1)
{
}

PndTrackingQualityMCInfo::PndTrackingQualityMCInfo(const PndTrackingQualityMCInfo &info)
  : TObject(info), fNofMvdPixelPoints(0), fNofMvdStripPoints(0), fNofSttParalPoints(0), fNofSttSkewPoints(0), fNofGemPoints(0), fNofFtsPoints(0), fReconstructabilityStatus(-1),
    fMCTrackID(-1), fRecoTrackIDs(), fPosFirst(TVector3(0., 0., 0.)), fMomFirst(TVector3(0., 0., 0.)), fPosLast(TVector3(0., 0., 0.)), fMomLast(TVector3(0., 0., 0.)), fCharge(0),
    fIsPrimary(kFALSE), fVertex(TVector3(-10000., -10000., -10000.)), fMomentum(TVector3(-10000., -10000., -10000.)), fPDGCode(-1), fQuality(0.), fMCQuality(0),
    fAssoRecoTrackID(-1)
{
  *this = info;
}

PndTrackingQualityMCInfo::~PndTrackingQualityMCInfo() {}

PndTrackingQualityMCInfo &PndTrackingQualityMCInfo::operator=(const PndTrackingQualityMCInfo &info)
{
  fNofMvdPixelPoints = info.fNofMvdPixelPoints;
  fNofMvdStripPoints = info.fNofMvdStripPoints;
  fNofSttParalPoints = info.fNofSttParalPoints;
  fNofSttSkewPoints = info.fNofSttSkewPoints;
  fNofGemPoints = info.fNofGemPoints;
  fNofFtsPoints = info.fNofFtsPoints;
  fReconstructabilityStatus = info.fReconstructabilityStatus;
  fMCTrackID = info.fMCTrackID;
  fRecoTrackIDs = info.fRecoTrackIDs;
  fPosFirst = info.fPosFirst;
  fMomFirst = info.fMomFirst;
  fPosLast = info.fPosLast;
  fMomLast = info.fMomLast;
  fCharge = info.fCharge;
  fIsPrimary = info.fIsPrimary;
  fVertex = info.fVertex;
  fPDGCode = info.fPDGCode;
  fQuality = info.fQuality;
  fMCQuality = info.fMCQuality;

  return *this;
}

ClassImp(PndTrackingQualityMCInfo)
