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

//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemMagneticFieldVsTrackParameters source file                 -----
// -----                  Created 12/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

// Includes from GEM
#include "PndGemMagneticFieldVsTrackParameters.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoNode.h"

#include "PndGemMCPoint.h"
#include "PndMCTrack.h"
#include "PndGemDigiPar.h"

#include <iostream>
#include <iomanip>
#include <map>

using std::cerr;
using std::cout;
using std::endl;
using std::fixed;
using std::left;
using std::map;
using std::pair;
using std::right;
using std::setprecision;
using std::setw;

// -----   Default constructor   ------------------------------------------
PndGemMagneticFieldVsTrackParameters::PndGemMagneticFieldVsTrackParameters() : FairTask("GEM MagneticFieldVsTrackParametersr", 1)
{
  fDigiPar = nullptr;
  fMCPointArray = nullptr;
  fMCTrackArray = nullptr;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemMagneticFieldVsTrackParameters::PndGemMagneticFieldVsTrackParameters(Int_t iVerbose) : FairTask("GEM MagneticFieldVsTrackParametersr", iVerbose)
{
  fDigiPar = nullptr;
  fMCPointArray = nullptr;
  fMCTrackArray = nullptr;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
PndGemMagneticFieldVsTrackParameters::PndGemMagneticFieldVsTrackParameters(const char *name, Int_t iVerbose) : FairTask(name, iVerbose)
{
  fDigiPar = nullptr;
  fMCPointArray = nullptr;
  fMCTrackArray = nullptr;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemMagneticFieldVsTrackParameters::~PndGemMagneticFieldVsTrackParameters()
{
  Reset();
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndGemMagneticFieldVsTrackParameters::Exec(Option_t *)
{

  // Size of fMCTrackArray
  // cout <<"# MC Tracks: "<< fMCTrackArray->GetEntriesFast() << endl;
  // Size of fMCTrackArray
  // cout <<"# MC Points: "<< fMCPointArray->GetEntriesFast() << endl;

  Fill1StationHistograms();
  Fill2StationsHistograms();

  Reset();
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemMagneticFieldVsTrackParameters::SetParContainers()
{

  // Get run and runtime database
  FairRunAna *run = FairRunAna::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get GEM digitisation parameter container. FairRuntimeDb is responsible for deleting "PndGemDetectors" container.
  fDigiPar = (PndGemDigiPar *)(db->getContainer("PndGemDetectors"));
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus PndGemMagneticFieldVsTrackParameters::Init()
{

  // Get input array
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("Init", "No FairRootManager");

  // Get MCPoint array
  fMCPointArray = (TClonesArray *)ioman->GetObject("GEMPoint");
  if (!fMCPointArray) {
    LOG(error) << " " << GetName() << "::Init: No MCPoint array!";
    return kERROR;
  }

  // Get MCTrack array
  fMCTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTrackArray) {
    LOG(error) << " " << GetName() << "::Init: No MCTrack array!";
    return kERROR;
  }

  fTrackFinderOnHits_ParThetaA = fDigiPar->GetTrackFinderOnHits_ParThetaA();
  fTrackFinderOnHits_ParThetaB = fDigiPar->GetTrackFinderOnHits_ParThetaB();

  fTrackFinderOnHits_ParTheta0 = fDigiPar->GetTrackFinderOnHits_ParTheta0();
  fTrackFinderOnHits_ParTheta1 = fDigiPar->GetTrackFinderOnHits_ParTheta1();
  fTrackFinderOnHits_ParTheta2 = fDigiPar->GetTrackFinderOnHits_ParTheta2();
  fTrackFinderOnHits_ParTheta3 = fDigiPar->GetTrackFinderOnHits_ParTheta3();

  fTrackFinderOnHits_ParRadPhi0 = fDigiPar->GetTrackFinderOnHits_ParRadPhi0();
  fTrackFinderOnHits_ParRadPhi2 = fDigiPar->GetTrackFinderOnHits_ParRadPhi2();
  for (Int_t in = 0; in < 3; in++) {
    fTrackFinderOnHits_ParMat0[in] = fDigiPar->GetTrackFinderOnHits_ParMat0(in);
    fTrackFinderOnHits_ParMat1[in] = fDigiPar->GetTrackFinderOnHits_ParMat1(in);
  }

  CreateHistos();

  LOG(info) << " " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations.";
  LOG(info) << " " << fName.Data() << "::Init(). Initialization succesfull.";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemMagneticFieldVsTrackParameters::ReInit()
{
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void PndGemMagneticFieldVsTrackParameters::Reset()
{
  fNofEvents = 0;
}
// -------------------------------------------------------------------------

// ---- Private function to fill histograms dependent on 1 station --------------------
Int_t PndGemMagneticFieldVsTrackParameters::Fill1StationHistograms()
{
  Int_t nofGemPoints = fMCPointArray->GetEntriesFast();
  PndGemMCPoint *mcPoint;
  PndMCTrack *mcTrack;

  for (Int_t imcp = 0; imcp < nofGemPoints; imcp++) {
    mcPoint = (PndGemMCPoint *)fMCPointArray->At(imcp);

    Int_t stationNumber = fDigiPar->GetStationNr(mcPoint->GetSensorId());

    mcTrack = (PndMCTrack *)fMCTrackArray->At(mcPoint->GetTrackID());
    TVector3 mcMomVec = mcTrack->GetMomentum();

    Double_t pRadius = TMath::Sqrt(mcPoint->GetX() * mcPoint->GetX() + mcPoint->GetY() * mcPoint->GetY());
    Double_t pZ = mcPoint->GetZ();
    Double_t tMom = mcMomVec.Mag();
    Double_t tTheta = mcMomVec.Theta() * TMath::RadToDeg();

    fhThetaVsRadiusVsMomentum[stationNumber - 1]->Fill(tMom, pRadius, tTheta);
    fhThetaVsRadiusVsMomentumAll->Fill(tMom, pRadius / pZ, tTheta);
  }
  return 0;
}
// -------------------------------------------------------------------------

// ---- Private function to fill histograms dependent on 2 stations --------------------
Int_t PndGemMagneticFieldVsTrackParameters::Fill2StationsHistograms()
{
  Int_t nofGemPoints = fMCPointArray->GetEntriesFast();
  PndGemMCPoint *mcPoint1;
  PndGemMCPoint *mcPoint2;
  PndMCTrack *mcTrack;

  for (Int_t imcp1 = 0; imcp1 < nofGemPoints; imcp1++) {
    mcPoint1 = (PndGemMCPoint *)fMCPointArray->At(imcp1);

    Double_t p1Radius = TMath::Sqrt(mcPoint1->GetX() * mcPoint1->GetX() + mcPoint1->GetY() * mcPoint1->GetY());
    Double_t p1Z = mcPoint1->GetZ();
    Double_t p1PhiAng = TMath::ACos(mcPoint1->GetX() / p1Radius);
    if (mcPoint1->GetY() < 0)
      p1PhiAng = 2. * TMath::Pi() - p1PhiAng;
    p1PhiAng *= TMath::RadToDeg();

    Int_t mcTrackId = mcPoint1->GetTrackID();

    mcTrack = (PndMCTrack *)fMCTrackArray->At(mcTrackId);

    if (mcTrack->GetMotherID() != -1)
      continue;

    TVector3 mcMomVec = mcTrack->GetMomentum();
    Double_t tMom = mcMomVec.Mag();
    Double_t tPt = mcMomVec.Pt();
    Double_t pMom = TMath::RadToDeg() * mcMomVec.Phi() + (mcMomVec.Phi() >= 0 ? 0. : 360.);

    for (Int_t imcp2 = imcp1 + 1; imcp2 < nofGemPoints; imcp2++) {
      mcPoint2 = (PndGemMCPoint *)fMCPointArray->At(imcp2);
      Double_t p2Z = mcPoint2->GetZ();
      if (TMath::Abs(p2Z - p1Z) < 5.)
        continue;

      Double_t p2Radius = TMath::Sqrt(mcPoint2->GetX() * mcPoint2->GetX() + mcPoint2->GetY() * mcPoint2->GetY());

      Double_t p2PhiAng = TMath::ACos(mcPoint2->GetX() / p2Radius);
      if (mcPoint2->GetY() < 0)
        p2PhiAng = 2. * TMath::Pi() - p2PhiAng;
      p2PhiAng *= TMath::RadToDeg();

      if (p1PhiAng < 90. && p2PhiAng > 270.)
        p2PhiAng -= 360.;
      if (p1PhiAng > 270. && p2PhiAng < 90.)
        p2PhiAng += 360.;

      fhRadiusVsAngle->Fill((p2PhiAng - p1PhiAng) * p1Z / p2Z, p2Radius * p1Z / (p1Radius * p2Z));

      Int_t histNo = -1;
      Int_t ihist = 0;
      for (Int_t istat1 = 0; istat1 < fDigiPar->GetNStations(); istat1++) {
        PndGemStation *stat1 = (PndGemStation *)fDigiPar->GetStation(istat1);
        Double_t zStation1 = stat1->GetZ();
        for (Int_t istat2 = istat1 + 1; istat2 < fDigiPar->GetNStations(); istat2++) {
          PndGemStation *stat2 = (PndGemStation *)fDigiPar->GetStation(istat2);
          Double_t zStation2 = stat2->GetZ();
          if (TMath::Abs(p1Z - zStation1) < 2. && TMath::Abs(p2Z - zStation2) < 2.)
            histNo = ihist;
          ihist++;
        }
      }
      if (histNo == -1)
        continue;
      fhRadiusVsAnglePair[histNo]->Fill((p2PhiAng - p1PhiAng) * p1Z / p2Z, p2Radius * p1Z / (p1Radius * p2Z));

      //      Double_t circRad = TMath::Sqrt( (gemHit2X-gemHit1X)*(gemHit2X-gemHit1X) + (gemHit2Y-gemHit1Y)*(gemHit2Y-gemHit1Y) ) / TMath::Sin(pangle-pangle2) / 2.;
      fhMomTransVsHRadius[histNo]->Fill(TMath::Abs(TMath::Sqrt((mcPoint2->GetX() - mcPoint1->GetX()) * (mcPoint2->GetX() - mcPoint1->GetX()) +
                                                               (mcPoint2->GetY() - mcPoint1->GetY()) * (mcPoint2->GetY() - mcPoint1->GetY())) /
                                                   TMath::Sin(TMath::DegToRad() * (p2PhiAng - p1PhiAng)) / 2.),
                                        tPt);

      if (mcTrackId != mcPoint2->GetTrackID())
        continue;

      fhMomentumVsPhiDiffVsRadius[histNo]->Fill(p1Radius, TMath::Abs(p2PhiAng - p1PhiAng), tMom);

      fhMomentumVsPhiDiffAll[histNo]->Fill(TMath::Abs(p2PhiAng - p1PhiAng), tMom);
      fhMomentumVsPhiDiff[histNo][Int_t(p1Radius / 5.)]->Fill(TMath::Abs(p2PhiAng - p1PhiAng), tMom);

      fhTrackPhiVsHitPhis[histNo]->Fill(p1PhiAng, (p1PhiAng - p2PhiAng) * p1Z / (p2Z - p1Z), pMom);
      fhTrackPhiVsCalcPhi[histNo]->Fill(p1PhiAng + (p1PhiAng - p2PhiAng) * p1Z / (p2Z - p1Z), pMom);
    }
  }
  return 0;
}
// -------------------------------------------------------------------------

// -----   Private method CreateHistos   --------------------------------------------
void PndGemMagneticFieldVsTrackParameters::CreateHistos()
{
  fHistoList = new TList();
  // number of mc tracks, reco tracks, efficiency as function of MOMENTUM

  const Int_t nofMomBins = 19;
  Double_t momBins[nofMomBins + 1] = {0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85, 0.95, 1.05, 1.15, 1.25, 1.75, 2.25, 2.75, 3.25, 4.75, 7.25, 10.75};
  const Int_t nofRadiusBins = 60;
  Double_t minRadius = 0.;
  Double_t maxRadius = 60.;
  Double_t minRRadius = 0.;  // radius/zdist
  Double_t maxRRadius = 0.4; // radius/zdist
  Double_t radBins[nofRadiusBins + 1];
  for (Int_t itemp = 0; itemp < nofRadiusBins + 1; itemp++)
    radBins[itemp] = minRadius + (Double_t)itemp * (maxRadius - minRadius) / (Double_t(nofRadiusBins));
  Double_t rRadBins[nofRadiusBins + 1];
  for (Int_t itemp = 0; itemp < nofRadiusBins + 1; itemp++)
    rRadBins[itemp] = minRRadius + (Double_t)itemp * (maxRRadius - minRRadius) / (Double_t(nofRadiusBins));
  const Int_t nofThetaBins = 60;
  Double_t minTheta = 0.;
  Double_t maxTheta = 30.;
  Double_t theBins[nofThetaBins + 1];
  for (Int_t itemp = 0; itemp < nofThetaBins + 1; itemp++)
    theBins[itemp] = minTheta + (Double_t)itemp * (maxTheta - minTheta) / (Double_t(nofThetaBins));
  const Int_t nofPhiBins = 160;
  Double_t minPhi = 0.;
  Double_t maxPhi = 40.;
  Double_t phiBins[nofPhiBins + 1];
  for (Int_t itemp = 0; itemp < nofPhiBins + 1; itemp++)
    phiBins[itemp] = minPhi + (Double_t)itemp * (maxPhi - minPhi) / (Double_t(nofPhiBins));
  const Int_t nofDetPhiBins = 400;
  const Int_t nofDetMomBins = 100;
  Double_t minMom = 0.;
  Double_t maxMom = 10.;
  const Int_t nofPhi1Bins = 360;
  Double_t phi1Bins[nofPhi1Bins + 1];
  for (Int_t itemp = 0; itemp < nofPhi1Bins + 1; itemp++)
    phi1Bins[itemp] = itemp;
  const Int_t nofPhi2Bins = 120;
  Double_t minPhi2 = -60.;
  Double_t maxPhi2 = 60.;
  Double_t phi2Bins[nofPhi2Bins + 1];
  for (Int_t itemp = 0; itemp < nofPhi2Bins + 1; itemp++)
    phi2Bins[itemp] = minPhi2 + (Double_t)itemp * (maxPhi2 - minPhi2) / (Double_t(nofPhi2Bins));

  fhThetaVsRadiusVsMomentumAll = new TH3F("fhThetaVsRadiusVsMomentumAll", "Track Theta vs Point Radius/Z vs Track Momentum, all;p [GeV/c];r [cm];#theta [#circ]", nofMomBins,
                                          momBins, nofRadiusBins, rRadBins, nofThetaBins, theBins);
  for (Int_t istat = 0; istat < fDigiPar->GetNStations(); istat++) {
    fhThetaVsRadiusVsMomentum[istat] =
      new TH3F(Form("fhThetaVsRadiusVsMomentum_s%d", istat + 1), Form("Track Theta vs Point Radius vs Track Momentum on station %d;p [GeV/c];r [cm];#theta [#circ]", istat + 1),
               nofMomBins, momBins, nofRadiusBins, radBins, nofThetaBins, theBins);
    ffThetaVsRadiusVsMomentum[istat] = new TF2(Form("ffThetaVsRadiusVsMomentum_s%d", istat + 1), "([0]+1/(x+[1]*[4]+[2]))*y/[4]+[3]", 0., 20., minRRadius, maxRRadius);
    fHistoList->Add(fhThetaVsRadiusVsMomentum[istat]);
    fHistoList->Add(ffThetaVsRadiusVsMomentum[istat]);
  }
  fHistoList->Add(fhThetaVsRadiusVsMomentumAll);

  fhRadiusVsAngle =
    new TH2F("fhRadiusVsAngle", "Radius ratio/z ratio vs phi angle difference;#phi_{2}-#phi_{1} [#circ];#frac{r_{2}}{r_{1}}#frac{z_{1}}{z_{2}}", 800, -40., 40., 200, 0., 2.);
  fHistoList->Add(fhRadiusVsAngle);
  Int_t crHist = 0;
  for (Int_t istat1 = 0; istat1 < fDigiPar->GetNStations(); istat1++) {
    for (Int_t istat2 = istat1 + 1; istat2 < fDigiPar->GetNStations(); istat2++) {
      fhRadiusVsAnglePair[crHist] =
        new TH2F(Form("fhRadiusVsAnglePair_s%d_s%d", istat1 + 1, istat2 + 1),
                 Form("Radius ratio/z ratio vs phi angle difference, stat%d vs stat%d;#phi_{2} - #phi_{1} [#circ];#frac{r_{2}}{r_{1}}#frac{z_{1}}{z_{2}}", istat1 + 1, istat2 + 1),
                 800, -40., 40., 200, 0., 2.);
      fHistoList->Add(fhRadiusVsAnglePair[crHist]);

      fhMomentumVsPhiDiffVsRadius[crHist] =
        new TH3F(Form("fhMomentumVsPhiDiffVsRadius_s%d_s%d", istat1 + 1, istat2 + 1),
                 Form("Momentum vs phi angle difference vs radius, stat%d vs stat%d;r [cm];#phi_{2} - #phi_{1} [#circ];p [GeV/c]", istat1 + 1, istat2 + 1), nofRadiusBins, radBins,
                 nofPhiBins, phiBins, nofMomBins, momBins);
      fHistoList->Add(fhMomentumVsPhiDiffVsRadius[crHist]);

      ffMomentumVsPhiDiffVsRadius[crHist] = new TF2(Form("ffMomentumVsPhiDiffVsRadius_s%d_s%d", istat1 + 1, istat2 + 1), "([0]+[1]/x)/y", 0., 100., 0., 40.);
      fHistoList->Add(ffMomentumVsPhiDiffVsRadius[crHist]);

      fhMomentumVsPhiDiffAll[crHist] = new TH2F(Form("fhMomentumVsPhiDiffAll_s%d_s%d", istat1 + 1, istat2 + 1),
                                                Form("Momentum vs phi angle difference, stat%d vs stat%d;#phi_{2} - #phi_{1} [#circ];p [GeV/c]", istat1 + 1, istat2 + 1),
                                                nofDetPhiBins, minPhi, maxPhi, nofDetMomBins, minMom, maxMom);
      fHistoList->Add(fhMomentumVsPhiDiffAll[crHist]);

      ffMomentumVsPhiDiffAll[crHist] = new TF1(Form("ffMomentumVsPhiDiffAll_s%d_s%d", istat1 + 1, istat2 + 1), "[0]/x", 0., 40.);
      fHistoList->Add(ffMomentumVsPhiDiffAll[crHist]);

      for (Int_t irad = 0; irad < 20; irad++) {
        fhMomentumVsPhiDiff[crHist][irad] =
          new TH2F(Form("fhMomentumVsPhiDiff_s%d_s%d_r%d", istat1 + 1, istat2 + 1, irad),
                   Form("Momentum vs phi angle difference, stat%d vs stat%d, rad %d to %d;#phi_{2} - #phi_{1} [#circ];p [GeV/c]", istat1 + 1, istat2 + 1, irad * 5, (irad + 1) * 5),
                   nofDetPhiBins, minPhi, maxPhi, nofDetMomBins, minMom, maxMom);
        fHistoList->Add(fhMomentumVsPhiDiff[crHist][irad]);

        ffMomentumVsPhiDiff[crHist][irad] = new TF1(Form("ffMomentumVsPhiDiff_s%d_s%d_r%d", istat1 + 1, istat2 + 1, irad), "[0]/x", 0., 40.);
        fHistoList->Add(ffMomentumVsPhiDiff[crHist][irad]);
      }

      fhTrackPhiVsHitPhis[crHist] =
        new TH3F(Form("fhTrackPhiVsHitPhis_s%d_s%d", istat1 + 1, istat2 + 1),
                 Form("Track phi vs hit1 phi vs phi difference/z ratio, stat%d vs stat%d;#phi_{1} [#circ];(#phi_{2} - #phi_{1})*z__{2}/z_{1} [#circ];#phi_{T} [#circ]", istat1 + 1,
                      istat2 + 1),
                 nofPhi1Bins, phi1Bins, nofPhi2Bins, phi2Bins, nofPhi1Bins, phi1Bins);
      fhTrackPhiVsCalcPhi[crHist] = new TH2F(Form("fhTrackPhiVsCalcPhi_s%d_s%d", istat1 + 1, istat2 + 1),
                                             Form("Track MC phi vs track calculated phi, stat%d vs stat%d;#phi_{calc} [#circ];#phi_{MC} [#circ]", istat1 + 1, istat2 + 1),
                                             nofPhi1Bins, phi1Bins, nofPhi1Bins, phi1Bins);
      fHistoList->Add(fhTrackPhiVsHitPhis[crHist]);
      fHistoList->Add(fhTrackPhiVsCalcPhi[crHist]);

      fhMomTransVsHRadius[crHist] =
        new TH2F(Form("fhMomTransVsHRadius_s%d_s%d", istat1 + 1, istat2 + 1), Form("Track MC momentum trans vs track calculated helix radius;radius_{radius} [cm];pt_{MC} [GeV/c]"),
                 nofDetMomBins, 0., 800., nofDetMomBins, 0., maxMom / 3.);
      fHistoList->Add(fhMomTransVsHRadius[crHist]);

      crHist++;
    }
  }
  for (Int_t itemp = 0; itemp < 3; itemp++) {
    ffRadiusVsAngle[itemp] = new TF1(Form("ffRadiusVsAngle%d", itemp), "pol2", -40., 40.);
    fHistoList->Add(ffRadiusVsAngle[itemp]);
  }
}
// -------------------------------------------------------------------------

// -----   Private method Finish   --------------------------------------------
void PndGemMagneticFieldVsTrackParameters::Finish()
{

  for (Int_t istat = 0; istat < fDigiPar->GetNStations(); istat++) {
    PndGemStation *station = (PndGemStation *)fDigiPar->GetStation(istat);
    Double_t zStation = station->GetZ();

    ffThetaVsRadiusVsMomentum[istat]->SetParameters(fTrackFinderOnHits_ParTheta0, fTrackFinderOnHits_ParTheta1, fTrackFinderOnHits_ParTheta2, fTrackFinderOnHits_ParTheta3,
                                                    zStation);
  }
  for (Int_t itemp = 0; itemp < 3; itemp++) {
    ffRadiusVsAngle[itemp]->SetParameters(fTrackFinderOnHits_ParRadPhi0 + (-1 + itemp) * 0.05, 0., fTrackFinderOnHits_ParRadPhi2);
  }
  Int_t crHist = 0;
  for (Int_t istat1 = 0; istat1 < fDigiPar->GetNStations(); istat1++) {
    PndGemStation *stat1 = (PndGemStation *)fDigiPar->GetStation(istat1);
    Double_t zStation1 = stat1->GetZ();
    for (Int_t istat2 = istat1 + 1; istat2 < fDigiPar->GetNStations(); istat2++) {
      PndGemStation *stat2 = (PndGemStation *)fDigiPar->GetStation(istat2);
      Double_t zStation2 = stat2->GetZ();

      Double_t zCuDiff = zStation2 * zStation2 * zStation2 - zStation1 * zStation1 * zStation1;
      Double_t zSqDiff = zStation2 * zStation2 - zStation1 * zStation1;
      Double_t zDiff = zStation2 - zStation1;

      Double_t par0_mom = fTrackFinderOnHits_ParMat0[0] * zCuDiff + fTrackFinderOnHits_ParMat0[1] * zSqDiff + fTrackFinderOnHits_ParMat0[2] * zDiff;
      Double_t par1_mom = fTrackFinderOnHits_ParMat1[0] * zCuDiff + fTrackFinderOnHits_ParMat1[1] * zSqDiff + fTrackFinderOnHits_ParMat1[2] * zDiff;

      ffMomentumVsPhiDiffVsRadius[crHist]->SetParameters(par0_mom, par1_mom);
      ffMomentumVsPhiDiffAll[crHist]->SetParameter(0, par0_mom);
      for (Int_t irad = 0; irad < 20; irad++) {
        Double_t par = par0_mom + par1_mom / (Double_t(irad) * 5. + 2.5);
        ffMomentumVsPhiDiff[crHist][irad]->SetParameter(0, par);
      }

      crHist++;
    }
  }

  gDirectory->mkdir("GemMFVTP");
  gDirectory->cd("GemMFVTP");
  TIter next(fHistoList);
  while (TH1 *histo = ((TH1 *)next()))
    histo->Write();
  gDirectory->cd("..");
}
// ------------------------------------------------------------

ClassImp(PndGemMagneticFieldVsTrackParameters)
