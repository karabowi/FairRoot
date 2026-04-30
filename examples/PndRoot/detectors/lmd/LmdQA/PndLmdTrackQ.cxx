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
// -----               PndTrackLmdQ source file                  -----
// -----       Created 18/06/2013 by A.Karavdina           -----
// -------------------------------------------------------------------------

#include "PndLmdTrackQ.h"
// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndLinTrack.h"
#include "PndTrackCand.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "PndTrackCandHit.h"
#include "PndTrack.h"
#include "PndSdsClusterPixel.h"
#include "PndSdsDigiPixel.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "FairTrackParH.h"
#include "PndMCTrack.h"
#include <TMath.h>
#include <TVector3.h>
#include <TRandom.h>
//#include <TStyle.h>
//#include <TCanvas.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include "TH2D.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"
using namespace ROOT::Math;
using namespace std;

PndLmdTrackQ::PndLmdTrackQ()
  : fXrecLMD(0), fYrecLMD(0), fZrecLMD(0), fThetarecLMD(0), fPhirecLMD(0), fXrec(0), fYrec(0), fZrec(0), fThetarec(0), fPhirec(0), fMomrec(0), fXmc(0), fYmc(0), fZmc(0),
    fThetamc(0), fPhimc(0), fMommc(0), fXmcLMD(0), fYmcLMD(0), fZmcLMD(0), fThetamcLMD(0), fPhimcLMD(0), fMommcLMD(0), fchi2LMD(0), fTrkRecStatus(0), fSecondary(0), fPDGcode(0),
    fnumMChits(0), fnumMCdoublehits(0)
{
}

PndLmdTrackQ::PndLmdTrackQ(double Plab)
  : fXrecLMD(0), fYrecLMD(0), fZrecLMD(0), fThetarecLMD(0), fPhirecLMD(0), fXrec(0), fYrec(0), fZrec(0), fThetarec(0), fPhirec(0), fMomrec(0), fXmc(0), fYmc(0), fZmc(0),
    fThetamc(0), fPhimc(0), fXmcLMD(0), fYmcLMD(0), fZmcLMD(0), fThetamcLMD(0), fPhimcLMD(0), fMommcLMD(0), fchi2LMD(0), fTrkRecStatus(0), fSecondary(0), fPDGcode(0),
    fnumMChits(0), fnumMCdoublehits(0)
{
  fMommc = Plab;
}

PndLmdTrackQ::~PndLmdTrackQ() {}
