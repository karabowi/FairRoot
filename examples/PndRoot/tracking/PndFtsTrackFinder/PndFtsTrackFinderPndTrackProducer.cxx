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

/**
 *@class PndFtsTrackFinderPndTrackProducer
 *@brief Class that manages conversion from PndFtsTrackFinder::FullTrack to PndTrack and PndTrckCand
 *@author Bartosz Sobol
 *@date: 11.02.2022
 */

#include <FairRunAna.h>

#include "TClonesArray.h"

#include "PndFtsTrackFinderPndTrackProducer.h"
#include "PndFtsMath.h"

PndFtsTrackFinderPndTrackProducer::PndFtsTrackFinderPndTrackProducer(const PndFtsTrackFinder::PndFtsContext &fReconContext, int32_t inBranchId, TClonesArray *inHits)
  : fReconContext(fReconContext), fInBranchId{inBranchId}, fInHits{inHits}, fField{FairRunAna::Instance()->GetField()}
{
}

void PndFtsTrackFinderPndTrackProducer::CreatePndTrack(const PndFtsTrackFinder::PndFtsFullTrack &track, TObject *&pndTrackDest, TObject *&pndTrackCandDest,
                                                       TObject *&pndAnalyticTrackDest) const
{
  new (pndAnalyticTrackDest) PndFtsTrackFinder::PndFtsAnalyticTrack(track);
  auto *pndTrackCand = new (pndTrackCandDest) PndTrackCand();

  for (const auto &hit : track.fHits) {
    pndTrackCand->AddHit(fInBranchId, hit.fPndHitId, hit.fStrawId);
  }

  const int charge = [&track]() {
    if (track.fFT34CircleZOX.fOrigin.fX > 0) {
      return -1;
    }
    if (track.fFT34CircleZOX.fOrigin.fX < 0) {
      return 1;
    }
    return 0;
  }();

  const auto momMag = GetMomMag(track);

  const TVector3 errPos(0.008, 0.08, 0.0); // TODO ?

  // First param
  const auto firstHitId = pndTrackCand->GetSortedHit(0).GetHitId();
  const auto firstHitZ = static_cast<float>(((PndFtsHit *)fInHits->At(firstHitId))->GetZ());

  const TVector3 firstPos = GetPosAtZ(firstHitZ, track);
  const TVector3 firstMomUnit = GetMomUnitAtPos(firstPos, track);
  const TVector3 firstMom = momMag * firstMomUnit;

  const TVector3 firstMomErr = firstMom * 0.05; // TODO ?

  const FairTrackParP firstPar(firstPos, firstMom, errPos, firstMomErr, charge, firstPos, TVector3(1., 0., 0.), TVector3(0., 1., 0.));

  // Last param
  const auto lastHitId = pndTrackCand->GetSortedHit(pndTrackCand->GetNHits() - 1).GetHitId();
  const auto lastHitZ = static_cast<float>(((PndFtsHit *)fInHits->At(lastHitId))->GetZ());

  const TVector3 lastPos = GetPosAtZ(lastHitZ, track);
  const TVector3 lastMomUnit = GetMomUnitAtPos(lastPos, track);
  const TVector3 lastMom = momMag * lastMomUnit;

  const TVector3 lastMomErr = lastMom * 0.05; // TODO ?

  const FairTrackParP lastPar(lastPos, lastMom, errPos, lastMomErr, charge, lastPos, TVector3(1., 0., 0.), TVector3(0., 1., 0.));

  new (pndTrackDest) PndTrack(firstPar, lastPar, *pndTrackCand);
}

TVector3 PndFtsTrackFinderPndTrackProducer::GetPosAtZ(float posZ, const PndFtsTrackFinder::PndFtsFullTrack &track) const
{
  using PndFtsTrackFinder::PndFtsMath::CircleValue;
  using PndFtsTrackFinder::PndFtsMath::LineValue;

  const auto [posX, posY] = [&]() -> std::pair<double, double> {
    if (posZ < fReconContext.fConstants.fZBStart) {
      const double x = LineValue(track.fFT12LineZOX, posZ);
      const double y = LineValue(track.fFT12LineZOY, posZ);
      return {x, y};
    } else if (posZ < fReconContext.fConstants.fZBEnd) {
      const double x = CircleValue(track.fFT34CircleZOX, posZ);
      const double y = LineValue(track.fFT34LineZOY, posZ);
      return {x, y};
    } else {
      const double x = LineValue(track.fFT56LineZOX, posZ);
      const double y = LineValue(track.fFT56LineZOY, posZ);
      return {x, y};
    }
  }();

  return {posX, posY, posZ};
}
TVector3 PndFtsTrackFinderPndTrackProducer::GetMomUnitAtPos(const TVector3 &pos, const PndFtsTrackFinder::PndFtsFullTrack &track) const
{
  const auto hitZ = pos.z();

  const auto [thetaXZ, thetaYZ] = [&]() -> std::pair<double, double> {
    if (hitZ < fReconContext.fConstants.fZBStart) {
      const double xz = std::atan(track.fFT12LineZOX.fSlope);
      const double yz = std::atan(track.fFT12LineZOY.fSlope);
      return {xz, yz};
    } else if (hitZ < fReconContext.fConstants.fZBEnd) {
      const auto z0 = track.fFT34CircleZOX.fOrigin.fZ;
      const auto sign = track.fFT34CircleZOX.fOrigin.fX > 0 ? 1 : -1;
      const auto radius = GetRadius(track);
      const auto rSq = radius * radius;
      const auto zDiff = hitZ - z0;
      const double momXSlope = -2 * zDiff * std::pow(rSq - zDiff * zDiff, -0.5);
      const double xz = std::atan(sign * momXSlope);
      const double yz = std::atan(track.fFT34LineZOY.fSlope);
      return {xz, yz};
    } else {
      const double xz = std::atan(track.fFT56LineZOX.fSlope);
      const double yz = std::atan(track.fFT56LineZOY.fSlope);
      return {xz, yz};
    }
  }();

  return {std::sin(thetaXZ), std::sin(thetaYZ), std::cos(thetaXZ)};
}

double PndFtsTrackFinderPndTrackProducer::GetMomMag(const PndFtsTrackFinder::PndFtsFullTrack &track) const
{
  const auto radius = GetRadius(track);

  const auto zBMid = (fReconContext.fConstants.fZBStart + fReconContext.fConstants.fZBEnd) / 2;
  const auto posMid = GetPosAtZ(zBMid, track);

  const auto BY = GetBYFromField(posMid);
  const double thetaPZ = std::atan(track.fFT34LineZOY.fSlope);

  // p=0.3BY*R; R converted to meters; sin(pi/2 - th) = cos(th); thetaPY = pi/2 - thetaPZ
  //  const auto momMag = static_cast<double>(track.fFT34CircleZOX.fRadius) / 100 * 0.3 * BY / std::cos(thetaPZ);
  const auto momMag = radius / 100 * 0.3 * BY / std::cos(thetaPZ);
  return momMag;
}

double PndFtsTrackFinderPndTrackProducer::GetBYFromField(const TVector3 &pos) const
{
  const double posArr[3] = {pos.x(), pos.y(), pos.z()};
  double B[3];

  fField->GetFieldValue(posArr, B);
  const double BYTesla = B[1] / 10.; // Convert kG to T

  return BYTesla;
}
double PndFtsTrackFinderPndTrackProducer::GetRadius(const PndFtsTrackFinder::PndFtsFullTrack &track) const
{
  using PndFtsTrackFinder::PndFtsMath::LineValue;

  if (fReconContext.fMomEstMethod == PndFtsTrackFinder::PndFtsMomEstMethod::CURVATURE or track.fFT56LineZOX.fSlope == 0.0) {
    return track.fFT34CircleZOX.fRadius;
  }

  const auto z_start = fReconContext.fConstants.fZBStart - fReconContext.fConstants.fZBTanOffset;
  const auto z_end = fReconContext.fConstants.fZBEnd + fReconContext.fConstants.fZBTanOffset;

  const auto xAtBendStart = LineValue(track.fFT12LineZOX, z_start);
  const auto xAtBendEnd = LineValue(track.fFT56LineZOX, z_end);

  const auto alpha_1 = -1 / track.fFT12LineZOX.fSlope;
  const auto beta_1 = xAtBendStart - alpha_1 * z_start;

  const auto alpha_2 = -1 / track.fFT56LineZOX.fSlope;
  const auto beta_2 = xAtBendEnd - alpha_2 * z_end;

  const auto z = (beta_2 - beta_1) / (alpha_1 - alpha_2);
  const auto x = alpha_1 * z + beta_1;

  const auto r = std::sqrt((z - z_start) * (z - z_start) + (x - xAtBendStart) * (x - xAtBendStart));

  return r;
}
