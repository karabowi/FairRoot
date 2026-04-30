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
// -----                       PndRichPhoton source file                  -----
// -----               Created 01/11/14  by Beloborodov Konstantin     -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndRichPhoton.h"

// -----   Default constructor   -------------------------------------------
PndRichPhoton::PndRichPhoton()
  : fPDHit(nullptr), fDTime(0.), fTime(0.), fHitTime(0.), fTheta(0.), fPhi(0.), fTMask(0), fMirrors(), fHitPosition(0, 0, 0), fTrackPosition(0, 0, 0), fTrackDirection(0, 0, 0),
    fTrackPositionOld(0, 0, 0), fTrackDirectionOld(0, 0, 0), fMirrRefPosition(), fLength(0.), fTrack(nullptr)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------

PndRichPhoton::PndRichPhoton(Double_t theta, Double_t phi, UInt_t tmask)
  : fPDHit(nullptr), fDTime(0.), fTime(0.), fHitTime(0.), fTheta(theta), fPhi(phi), fTMask(tmask), fMirrors(), fHitPosition(0, 0, 0), fTrackPosition(0, 0, 0),
    fTrackDirection(0, 0, 0), fTrackPositionOld(0, 0, 0), fTrackDirectionOld(0, 0, 0), fMirrRefPosition(), fLength(0.), fTrack(nullptr)
{
}

// -----   Destructor   ----------------------------------------------------
PndRichPhoton::~PndRichPhoton() {}
// -------------------------------------------------------------------------

bool PndRichPhoton::TrackCalc()
{
  // coordinate system calculation
  TVector3 axisTx = fTrack->GetAxisX();
  TVector3 axisTy = fTrack->GetAxisY();
  fTrackDirection = fTrack->GetAxisZ();
  fTrackPosition = fTrack->GetPosition0();
  fDTime = fTrack->GetTime0();
  fTrackDirectionOld = fTrackDirection;
  fTrackPositionOld = fTrackPosition;
  // single reflection
  if (fMirrors.size() == 0) {
    // fHitPosition = fPDHit->GetPosition();
    fMirrRefPosition.clear();
    fMirrRefPosition.push_back(fHitPosition);
    fLength = (fTrackPosition - fHitPosition).Mag();
    TVector3 nf = (fHitPosition - fTrackPosition).Unit();
    TVector3 nfT = (nf - fTrackDirection * (fTrackDirection * nf)).Unit();
    fTheta = std::acos(fTrackDirection * nf);
    fPhi = std::atan2(axisTy * nfT, axisTx * nfT);
    fTime = fHitTime - fLength / 30 - fDTime;
    return true;
  }
  // single reflection
  if (fMirrors.size() == 1) {
    // fHitPosition = fPDHit->GetPosition();
    Double_t t1 = (fTrackPosition - fMirrors[0]->GetPoint()) * fMirrors[0]->GetNormal();
    Double_t t2 = (fHitPosition - fMirrors[0]->GetPoint()) * fMirrors[0]->GetNormal();
    TVector3 p1 = fTrackPosition - t1 * fMirrors[0]->GetNormal();
    TVector3 p2 = fHitPosition - t2 * fMirrors[0]->GetNormal();
    TVector3 p0 = (t2 * p1 + t1 * p2) * (1.0 / (t1 + t2)); // point of reflection
    if (std::fabs(p0.X() - fMirrors[0]->GetPoint().X()) < fMirrors[0]->GetDims().X() && std::fabs(p0.Y() - fMirrors[0]->GetPoint().Y()) < fMirrors[0]->GetDims().Y() &&
        std::fabs(p0.Z() - fMirrors[0]->GetPoint().Z()) < fMirrors[0]->GetDims().Z()) {
      fMirrRefPosition.clear();
      fMirrRefPosition.push_back(p0);
      fLength = (fTrackPosition - p0).Mag() + (fHitPosition - p0).Mag();
      TVector3 nf = (p0 - fTrackPosition).Unit();
      TVector3 nfT = (nf - fTrackDirection * (fTrackDirection * nf)).Unit();
      fTheta = std::acos(fTrackDirection * nf);
      fPhi = std::atan2(axisTy * nfT, axisTx * nfT);
      fTime = fHitTime - fLength / 30 - fDTime;
      return true;
    } else
      return false;
  }
  // double reflections
  if (fMirrors.size() == 2) {
    // fHitPosition = fPDHit->GetPosition();
    Double_t t1 = (fTrackPosition - fMirrors[0]->GetPoint()) * fMirrors[0]->GetNormal();
    TVector3 p1 = fTrackPosition - 2 * fMirrors[0]->GetNormal() * t1;
    Double_t t2 = (fHitPosition - fMirrors[1]->GetPoint()) * fMirrors[1]->GetNormal();
    TVector3 p2 = fHitPosition - 2 * fMirrors[1]->GetNormal() * t2;
    TVector3 dir = (p2 - p1).Unit();
    t1 /= dir * fMirrors[0]->GetNormal();
    TVector3 q1 = p1 + dir * t1;
    t2 /= dir * fMirrors[1]->GetNormal();
    TVector3 q2 = p2 + dir * t2;
    if (std::fabs(q1.X() - fMirrors[0]->GetPoint().X()) < fMirrors[0]->GetDims().X() && std::fabs(q1.Y() - fMirrors[0]->GetPoint().Y()) < fMirrors[0]->GetDims().Y() &&
        std::fabs(q1.Z() - fMirrors[0]->GetPoint().Z()) < fMirrors[0]->GetDims().Z() && std::fabs(q2.X() - fMirrors[1]->GetPoint().X()) < fMirrors[1]->GetDims().X() &&
        std::fabs(q2.Y() - fMirrors[1]->GetPoint().Y()) < fMirrors[1]->GetDims().Y() && std::fabs(q2.Z() - fMirrors[1]->GetPoint().Z()) < fMirrors[1]->GetDims().Z()) {
      fMirrRefPosition.clear();
      fMirrRefPosition.push_back(q1);
      fMirrRefPosition.push_back(q2);
      fLength = (fTrackPosition - q1).Mag() + (q2 - q1).Mag() + (fHitPosition - q2).Mag();
      TVector3 nf = (q1 - fTrackPosition).Unit();
      TVector3 nfT = (nf - fTrackDirection * (fTrackDirection * nf)).Unit();
      fTheta = std::acos(fTrackDirection * nf);
      fPhi = std::atan2(axisTy * nfT, axisTx * nfT);
      fTime = fHitTime - fLength / 30 - fDTime;
      return true;
    }
  }
  return false;
}

Double_t PndRichPhoton::GetLength()
{
  fTrackDirection = fTrack->GetAxisZ();
  fTrackPosition = fTrack->GetPosition0();
  if ((fTrackDirection != fTrackDirectionOld) || (fTrackPosition != fTrackPositionOld))
    TrackCalc();
  return fLength;
}

Double_t PndRichPhoton::GetTime()
{
  fTrackDirection = fTrack->GetAxisZ();
  fTrackPosition = fTrack->GetPosition0();
  if ((fTrackDirection != fTrackDirectionOld) || (fTrackPosition != fTrackPositionOld))
    TrackCalc();
  return fTime;
}

Double_t PndRichPhoton::GetTheta()
{
  fTrackDirection = fTrack->GetAxisZ();
  fTrackPosition = fTrack->GetPosition0();
  if ((fTrackDirection != fTrackDirectionOld) || (fTrackPosition != fTrackPositionOld))
    TrackCalc();
  return fTheta;
}

Double_t PndRichPhoton::GetPhi()
{
  fTrackDirection = fTrack->GetAxisZ();
  fTrackPosition = fTrack->GetPosition0();
  if ((fTrackDirection != fTrackDirectionOld) || (fTrackPosition != fTrackPositionOld))
    TrackCalc();
  return fPhi;
}

// -----   Public method Print   -------------------------------------------
void PndRichPhoton::Print(const Option_t *opt) const
{
  cout << "RICH Photon hit: Time = " << fTime << " opt=" << opt << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndRichPhoton)
