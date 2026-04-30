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

#include "PndSTETools.h"
#include "FairLogger.h"
#include <vector>
#include <numeric>
#include <functional>
#include <utility>

using std::cout;
using std::endl;
using std::unique_ptr;

double PndSTETools::GetDistanceToTrack(PndRiemannTrack mvdRiemannTrack, PndSdsHit *mvdHit)
{

  LOG(debug) << "------ PndSTETools::GetDistanceToTrack() ------" << endl;

  TVector3 pos = mvdHit->GetPosition();
  TVector2 diff;
  TVectorD orig(2);
  double r = mvdRiemannTrack.r();
  orig = mvdRiemannTrack.orig();
  diff.Set(pos.x() - orig[0], pos.y() - orig[1]);
  double distance = TMath::Abs(diff.Mod() - r);

  return distance;
}

double PndSTETools::GetDistanceToTrackRiemannHit(PndRiemannTrack mvdRiemannTrack, PndRiemannHit hit)
{

  LOG(debug) << "------ PndSTETools::GetDistanceToTrackRiemannHit() ------" << endl;

  TVector3 pos = hit.x();
  TVector2 diff;
  TVectorD orig(2);
  double r = mvdRiemannTrack.r();
  orig = mvdRiemannTrack.orig();
  diff.Set(pos.x() - orig[0], pos.y() - orig[1]);
  double distance = TMath::Abs(diff.Mod() - r);

  return distance;
}

PndRiemannTrack PndSTETools::CompareError(PndRiemannTrack oldRiemannTrack, PndRiemannTrack newRiemannTrack)
{

  LOG(debug) << "------ PndSTETools::CompareError() ------" << endl;

  if (oldRiemannTrack.dR() < newRiemannTrack.dR()) {
    return oldRiemannTrack;

    fNewTrack = false;
  } else {
    return newRiemannTrack;
    fNewTrack = true;
  }
}

bool PndSTETools::IsHitInTrack(PndSdsHit *mvdHit, PndTrackCand trackCand)
{

  LOG(debug) << "------ PndSTETools::IsHitInTrack() ------" << endl;

  FairMultiLinkedData linksPixel = trackCand.GetLinksWithType(FairRootManager::Instance()->GetBranchId("MVDHitsPixel"));
  FairMultiLinkedData linksStrip = trackCand.GetLinksWithType(FairRootManager::Instance()->GetBranchId("MVDHitsStrip"));

  double MVDtime = mvdHit->GetTimeStamp();
  double MVDtime_new = 0.0;

  LOG(debug2) << "MVDtime: " << MVDtime << endl;

  if (linksPixel.GetNLinks() > 0) {
    for (int j = 0; j < (int)linksPixel.GetNLinks(); j++) {

      PndSdsHit *mvdPixel = static_cast<PndSdsHit *>(FairRootManager::Instance()->GetCloneOfLinkData(linksPixel.GetLink(j)));
      if (mvdPixel != nullptr) {
        MVDtime_new = mvdPixel->GetTimeStamp();

        if (MVDtime == MVDtime_new) {
          return true;
        }
      }
    }
  }

  if (linksStrip.GetNLinks() > 0) {
    for (int j = 0; j < (int)linksStrip.GetNLinks(); j++) {

      PndSdsHit *mvdStrip = static_cast<PndSdsHit *>(FairRootManager::Instance()->GetCloneOfLinkData(linksStrip.GetLink(j)));
      if (mvdStrip != nullptr) {
        MVDtime_new = mvdStrip->GetTimeStamp();

        LOG(debug2) << "MVDtime new: " << MVDtime_new << endl;

        if (MVDtime == MVDtime_new) {
          return true;
        }
      }
    }
  }

  return false;
}

void PndSTETools::CalcSlope(PndTrack *myTrack)
{

  LOG(debug) << "------ PndSTETools::CalcSlope() ------" << endl;

  // Center of gravity position
  int xc = 0;
  int yc = 0;

  int n_Hits = 0; // Instead of just using GetNLinks as the number of hits a counter is defined and is counted up when hit is non-nullptr
  double lineSlope = 0.0;

  FairMultiLinkedData linksSTTHit = myTrack->GetLinksWithType(FairRootManager::Instance()->GetBranchId("STTHit"));

  for (int i_hit = 0; i_hit < (int)linksSTTHit.GetNLinks(); i_hit++) {

    unique_ptr<PndSttHit> sttHit(static_cast<PndSttHit *> (FairRootManager::Instance()->GetCloneOfLinkData(linksSTTHit.GetLink(i_hit))));
   
    if (sttHit != nullptr) {

      xc = xc + (sttHit->GetX()) / ((sttHit->GetDx()) * (sttHit->GetDx()));
      yc = yc + (sttHit->GetY()) / ((sttHit->GetDy()) * (sttHit->GetDy()));
      n_Hits++;

    }
  }

  if (n_Hits > 0) {
    xc = xc / n_Hits;
    fxc = xc;
    yc = yc / n_Hits;
  }
  if (yc != 0) {
    lineSlope = -xc / yc;
    fLineSlope = lineSlope;
  } else {
    lineSlope = 0;
    fLineSlope = 0;
  }
}

std::vector<PndSdsHit *> PndSTETools::RefitRiemannTrack(PndRiemannTrack riemann, std::vector<PndSdsHit *> mvdHits)
{

  LOG(debug) << "------ PndSTETools::RefitRiemannTrack() ------" << endl;

  std::vector<PndSdsHit *> mvdHitsNewRefit;
  mvdHitsNewRefit.clear();

  for (int i_mvdHit = 0; i_mvdHit < (int)mvdHits.size(); i_mvdHit++) {

    PndRiemannHit hitToAdd(mvdHits[i_mvdHit]);
    riemann.addHit(hitToAdd);
  }

  riemann.sortHits();
  riemann.refit(true);

  // The block of code below draws the Riemann track if the option is chosen
  if (fDrawTrack == true) {

    PndSTEDraw *DrawObj = new PndSTEDraw();
    DrawObj->DrawTrack(riemann);
  }

  LOG(debug) << "Chi2 Riemann circle fit: " << riemann.ChiSquareDistCircle() / (riemann.getNumHits() - 3) << endl;

  LOG(debug1) << "Radius : " << riemann.r() << endl;
  LOG(debug1) << "Center : " << riemann.orig()[0] << " : " << riemann.orig()[1] << " : " << riemann.orig()[2] << endl;
  LOG(debug1) << "Hit positions s,z: " << endl;

  for (int riemann_hits = 0; riemann_hits < (int)riemann.getNumHits(); riemann_hits++) {

    PndRiemannHit *hit = riemann.getHit(riemann_hits);

    LOG(debug1) << hit->s() << " : " << hit->z() << endl;
  }

  CalcChi2(riemann, mvdHits);

  sort(fChi2OneHit.begin(), fChi2OneHit.end());

  for (int i_mvdHit = 0; i_mvdHit < (int)fChi2OneHit.size() - 1; i_mvdHit++) {

    mvdHitsNewRefit.push_back(fMapChi2OneHitMvdHit[fChi2OneHit[i_mvdHit]]);
  }

  return mvdHitsNewRefit;
}

void PndSTETools::CalcChi2(PndRiemannTrack track, std::vector<PndSdsHit *> sttHitVec)
{

  LOG(debug) << "------ PndSTETools::CalcChi2() ------" << endl;

  fChi2OneHit.clear();
  fMapChi2OneHitMvdHit.clear();

  track.refit(true);
  LOG(debug1) << "Errors in track parameters: " << track.mError() << " " << track.tError() << endl;

  for (int i = 0; i < (int)sttHitVec.size(); ++i) {

    PndSdsHit *mvdHitToTest = static_cast<PndSdsHit *>(sttHitVec[i]);
    PndRiemannHit riemannHit(mvdHitToTest);

    double distForChi2 = track.dist((PndRiemannHit *)&riemannHit);
    double distErrorForCHi2 = track.distError((PndRiemannHit *)&riemannHit);

    fChi2OneHit.push_back((distForChi2 * distForChi2) / (distErrorForCHi2 * distErrorForCHi2)); // (mvdHitToTest->GetDx()*mvdHitToTest->GetDx()));
    fMapChi2OneHitMvdHit[(distForChi2 * distForChi2) / (distErrorForCHi2 * distErrorForCHi2)] = mvdHitToTest;
  }

  fSumChi2 = accumulate(fChi2OneHit.begin(), fChi2OneHit.end(), 0.0);

  // Reduced chi2: the chi2 over the DOF. DOF=number of points to fit line to - number of parameters to fit
  // Four parameters are fitted in this case but only three are independent

  fReducedChi2 = track.ChiSquareDistCircle() / (track.getNumHits() + sttHitVec.size() - 3);

  sort(fChi2OneHit.begin(), fChi2OneHit.end());

  fSumChi2 = sqrt(fSumChi2);

  LOG(debug2) << "====================  SORTED HITS IN TRACK CHI2 =================================" << endl;

  for (int i = 0; i < (int)fChi2OneHit.size(); ++i) {

    LOG(debug2) << fChi2OneHit[i] << " : " << fMapChi2OneHitMvdHit[fChi2OneHit[i]]->GetTimeStamp() << endl;
  }
}

void PndSTETools::CalcResidual(PndRiemannTrack track, std::vector<PndSdsHit *> sttHitVec)
{

  LOG(debug) << "------ PndSTETools::CalcResidual() ------" << endl;

  std::vector<double> residuals;
  residuals.clear();

  std::map<double, PndSdsHit *> mapResidualHit;

  track.szFit(true);

  for (int i = 0; i < (int)sttHitVec.size(); ++i) {

    PndSdsHit *mvdHitToTest = static_cast<PndSdsHit *>(sttHitVec[i]);

    PndRiemannHit riemannHit(mvdHitToTest);

    double szDist = track.szDist((PndRiemannHit *)&riemannHit);
    residuals.push_back(szDist);

    LOG(debug) << szDist << endl;
  }
}

void PndSTETools::CalcChi2SZ(PndRiemannTrack track, std::vector<PndSdsHit *> sttHitVec)
{

  LOG(debug) << "------ PndSTETools::CalcChi2SZ() ------" << endl;

  fChi2OneHitSZ.clear();
  fMapChi2OneHitMvdHitSZ.clear();

  track.szFit(true);

  LOG(debug2) << "Errors in track parameters: " << track.mError() << " " << track.tError() << endl;

  for (int i = 0; i < (int)sttHitVec.size(); ++i) {

    PndSdsHit *mvdHitToTest = static_cast<PndSdsHit *>(sttHitVec[i]);

    PndRiemannHit riemannHit(mvdHitToTest);

    double szDist = track.szDist((PndRiemannHit *)&riemannHit);
    fChi2OneHitSZ.push_back((szDist * szDist) / (mvdHitToTest->GetDz() * mvdHitToTest->GetDz()));
    fMapChi2OneHitMvdHitSZ[(szDist * szDist) / (mvdHitToTest->GetDz() * mvdHitToTest->GetDz())] = mvdHitToTest;
    LOG(debug) << fChi2OneHitSZ[i] << " : " << szDist << " : " << mvdHitToTest->GetDz() << " Hit Pos : " << mvdHitToTest->GetX() << " : " << mvdHitToTest->GetY() << " : "
               << mvdHitToTest->GetZ() << " : " << endl;
  }

  fSumChi2SZ = accumulate(fChi2OneHitSZ.begin(), fChi2OneHitSZ.end(), 0.0);

  // Reduced chi2: the chi2 over the DOF. DOF=number of points to fit line to - number of parameters to fit
  // Four parameters are fitted in this case but only three are independent
  fReducedChi2SZ = fSumChi2SZ / (track.getNumHits() + sttHitVec.size() - 3);

  sort(fChi2OneHitSZ.begin(), fChi2OneHitSZ.end());

  LOG(debug1) << fSumChi2 << endl;
}

bool PndSTETools::IsHitInCorrectHemisphere(PndSdsHit *myHit, double xc, double lineSlope)
{

  LOG(debug) << "------ PndSTETools::IsHitInCorrectHemisphere() ------" << endl;

  bool hitOnCorrectHemisphere = true;

  double x_hit = myHit->GetX();
  double y_hit = myHit->GetY();

  double x_line = y_hit / lineSlope;

  if (x_hit > x_line && xc < x_line) {

    hitOnCorrectHemisphere = false;
  }
  if (x_hit < x_line && xc > x_line) {

    hitOnCorrectHemisphere = false;
  }

  return hitOnCorrectHemisphere;
}