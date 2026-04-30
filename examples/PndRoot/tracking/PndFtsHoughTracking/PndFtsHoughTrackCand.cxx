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

#include "PndFtsHoughTrackCand.h"

#include "PndFtsHoughTrackerTask.h"

#include <iostream>
#include "math.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "PndFtsHit.h"
#include "PndTrack.h"
#include "FairTrackParP.h"
#include "TVector3.h"

ClassImp(PndFtsHoughTrackCand);

PndFtsHoughTrackCand::PndFtsHoughTrackCand(PndFtsHoughTrackerTask *trackerTask)
  : fTrackerTask(trackerTask),

    fVerbose(0),

    fZxLineBeforeDipole(0., trackerTask), // TODO: It could be a problem here that I set the z reference value to 0.

    fZxParabola(0., trackerTask),

    fZxLineBehindDipole(0., trackerTask),

    fZyLine(0., trackerTask),

    fIntTrackCand(),

    fZCoordLineParabola(0.), fZCoordParabolaLine(0.)
{
  if (nullptr == fTrackerTask) {
    std::cout << "PndFtsHoughTrackCand FATAL ERROR Tracker task pointer not set.\n";
  } else {
    fVerbose = fTrackerTask->GetVerbose();
    if (3 < fVerbose)
      std::cout << "PndFtsHoughTrackCand called with tracker ptr " << fTrackerTask << '\n';
  }
}

PndFtsHoughTrackCand::~PndFtsHoughTrackCand() {}

void PndFtsHoughTrackCand::SetZxLineBeforeDipole(const PndFtsHoughTracklet zxLineParabola)
{
  if (fZxLineBeforeDipole.isSet()) {
    std::cerr << "WARNING from PndFtsHoughTrackCand: Line before dipole in zx plane is set more than once! Ignore new values! Potentially FATAL ERROR!\n";
    return;
  }
  fZxLineBeforeDipole = zxLineParabola;
  fZCoordLineParabola = fZxLineBeforeDipole.getZRefLabSys();
  addUniqueTrackletHits(fZxLineBeforeDipole);
}
void PndFtsHoughTrackCand::SetZxParabola(const PndFtsHoughTracklet zxParabola)
{
  if (fZxParabola.isSet()) {
    std::cerr << "WARNING from PndFtsHoughTrackCand: Parabola inside dipole in zx plane is set more than once! Ignore new values! Potentially FATAL ERROR!\n";
    return;
  }
  // warn if line before dipole field and parabola within are not calculated wrt the same z reference value
  if (zxParabola.getZRefLabSys() != fZCoordLineParabola) {
    std::cout << "WARNING from PndFtsHoughTrackCand: First line and parabola were not calculated wrt the same z position! Potentially FATAL ERROR!\n";
  }
  fZxParabola = zxParabola;
  addUniqueTrackletHits(fZxParabola);
}
void PndFtsHoughTrackCand::SetZxLineBehindDipole(const PndFtsHoughTracklet zxParabolaLine)
{
  if (fZxLineBehindDipole.isSet()) {
    std::cerr << "WARNING from PndFtsHoughTrackCand: Line behind dipole in zx plane is set more than once! Ignore new values! Potentially FATAL ERROR!\n";
    return;
  }
  fZxLineBehindDipole = zxParabolaLine;
  fZCoordParabolaLine = fZxLineBehindDipole.getZRefLabSys();
  addUniqueTrackletHits(fZxLineBehindDipole);
}
void PndFtsHoughTrackCand::SetZyLine(const PndFtsHoughTracklet zyLine)
{
  if (fZyLine.isSet()) {
    std::cerr << "WARNING from PndFtsHoughTrackCand: Line in zy plane is set more than once! Ignore new values! Potentially FATAL ERROR!\n";
    return;
  }
  fZyLine = zyLine;
  addUniqueTrackletHits(fZyLine);
}

void PndFtsHoughTrackCand::Print()
{
  std::cout << "=========== PndFtsHoughTrackCand::Print() ==========\n";
  fIntTrackCand.Print();
  if (kTRUE == fZxLineBeforeDipole.isSet()) {
    std::cout << "zx plane\n\n";
    std::cout << "1st line: ";
    fZxLineBeforeDipole.Print();
  }
  if (kTRUE == fZxParabola.isSet()) {
    std::cout << "parabola: ";
    fZxParabola.Print();
  }
  if (kTRUE == fZxLineBehindDipole.isSet()) {
    std::cout << "2nd line: ";
    fZxLineBehindDipole.Print();
  }
  if (kTRUE == fZyLine.isSet()) {
    std::cout << "zy plane\n\n";
    std::cout << "Line: ";
    fZyLine.Print();
  }
}

void PndFtsHoughTrackCand::addUniqueTrackletHits(PndFtsHoughTracklet inTracklet)
{
  // add all the hits from the tracklet which are not already in the hitId vector
  for (UInt_t iHit = 0; iHit < inTracklet.GetNHits(); ++iHit) {
    PndTrackCandHit inHit = inTracklet.GetSortedHit(iHit);
    const Int_t inHitId = inHit.GetHitId();
    const Int_t inDetId = inHit.GetDetId();
    // if hit is NOT in track -1 is returned by HitInTrack, otherwise the index (>=0) in the HitId vector is returned
    if (-1 == fIntTrackCand.HitInTrack(inDetId, inHitId)) {
      // add hit to track cand
      const Double_t inRho = inHit.GetRho();
      fIntTrackCand.AddHit(inDetId, inHitId, inRho);
    }
  }
}

// TODO: Check this!
Double_t PndFtsHoughTrackCand::getXLabForParabola(const Double_t &zLabSys) const
{
  // does not take option of varying B field into account
  // calculate x in lab sys for a given z position in lab sys for which the parabola assumption holds
  // theta in radian in zx plane given at z = zRefLabSys
  const Double_t thetaRad = fZxParabola.getThetaRadVal();
  const Double_t qDivPzx = getQdivPzx(); // Q/pzx
  const Double_t zRefLabSys = fZxParabola.getZRefLabSys();
  const Double_t zshifted = zLabSys - zRefLabSys;

  const Double_t interceptLine = fZxLineBeforeDipole.getSecondVal();

  const Double_t By = 1.;

  if (0 == thetaRad) {
    Double_t xParabolaSys = qDivPzx / 2. * By * zshifted;
    // go from local parabola system to the lab system
    Double_t xLabSys = xParabolaSys + interceptLine;
    return xLabSys;
  }

  const Double_t tantheta = tan(thetaRad);
  const Double_t pzstuff = 1. / qDivPzx / By / sin(thetaRad);
  const Double_t ztantheta = zshifted / tantheta;
  const Double_t a = -ztantheta + pzstuff / tantheta;

  const Double_t wurzel = sqrt(a * a - 2. * pzstuff * zshifted - ztantheta * ztantheta);
  const Double_t x1ParabolaSys = a - wurzel;
  const Double_t x2ParabolaSys = a + wurzel;

  // go from local parabola system to the lab system
  Double_t x1LabSys = x1ParabolaSys + interceptLine;
  Double_t x2LabSys = x2ParabolaSys + interceptLine;

  // there are two analytical solutions, take the one closer to 0
  if (fabs(x1LabSys) < fabs(x2LabSys)) {
    return x1LabSys;
  } else {
    return x2LabSys;
  }
}

PndTrackCand PndFtsHoughTrackCand::getPndTrackCand()
{
  return fIntTrackCand;

  //	// The following is what I used when I derived this class from PndTrackCand. Instead this class now contains a member of PndTrackCand
  //	// Maybe this is not necessary because PndFtsHoughTrackCand is derived from PndTrackCand
  //	PndTrackCand myCand;
  //	// copy all the hits from PndFtsHoughTrackCand *this to PndTrackCand myCand
  //	for (UInt_t iHit = 0; iHit < intTrackCand.GetNHits(); ++iHit)
  //	{
  //		PndTrackCandHit inHit = intTrackCand.GetSortedHit(iHit);
  //		const Int_t inHitId = inHit.GetHitId();
  //		const Int_t inDetId = inHit.GetDetId();
  //		const Double_t inRho = inHit.GetRho();
  //		myCand.AddHit(inDetId, inHitId, inRho);
  //	}
  //	return myCand;
}

// for conversion to PndTrack, compare to tools/riemannfit/PndRiemannTrack.cxx, line 1104
//--------------------------------------------------------------------------------------------
// getNumHits is called getNHits in PndTrackCand (which I use to store the hits belonging to this track candidate)
// I don't need the B field to calculate the parameters
PndTrack PndFtsHoughTrackCand::getPndTrack()
{
  // calculates first and last parameter, but uses an empty PndTrackCand which has to be set lateron using SetTrackCandRef
  FairTrackParP firstPar, lastPar;
  PndTrackCand myCand;
  if (fIntTrackCand.GetNHits() > 0) {
    firstPar = getTrackParPForHit(0);
    lastPar = getTrackParPForHit(fIntTrackCand.GetNHits() - 1);
  }
  return PndTrack(firstPar, lastPar, myCand);
}

FairTrackParP PndFtsHoughTrackCand::getTrackParPForHit(const UInt_t index)
{
  // index is the index of the hit in intTrackCand, not in the FTS hit array
  // this method will sort the hitId vector of intTrackCand and therefore cannot be used for const track candidates

  // TODO: Check if all arguments are correct

  // Warn if we do not have a complete track candidate
  if (!isComplete())
    std::cout << "getHit: You try to access hits before we have a complete track candidate.\n";

  // not necessary, I check later if I get a hit or not
  // if (index >= intTrackCand.GetNHits()) throwError("index in PndFtsHoughTrackCand::getTrackParPForHit is too large.");

  // Translate index in track candidate to hitId in FTS hit array
  UInt_t hitId = fIntTrackCand.GetSortedHit(index).GetHitId();

  // get position of hit
  const PndFtsHit *myHit = fTrackerTask->GetFtsHit(hitId);
  if (0 == myHit)
    throwError("Cannot get hit, probably the tracking has not finished or the index is too large.");

  // Take z of hit and calculate the position and momentum for that z using the results from the Hough transforms and my track model
  Double_t zLabSys = myHit->GetZ();

  // position should NOT come from hit, it should come from the pattern recognition track model
  // position error can be large (like 1* or 2* tube size) as the Kalman filter will adjust it.
  TVector3 hitPos = getPos(zLabSys);

  TVector3 hitPosError = fTrackerTask->GetFtsHitPosErrors(myHit);

  // momentum comes from the pattern recognition track model
  TVector3 mom = getP(zLabSys);
  TVector3 momError = 0.1 * mom; // TODO: add correct values here

  // set plane as detector plane in which the hit is, FTS planes are parallel to xy plane
  TVector3 origin; // set origin of plane = position of hit
  myHit->Position(origin);
  TVector3 dj(1, 0, 0); // unit vector along x // TODO: Check if that is set correctly for FTS
  TVector3 dk(0, 1, 0); // unit vector along y // TODO: Check if that is set correctly for FTS

  // -----   Constructor with track parameters in LAB -----------------------------------
  // FairTrackParP::FairTrackParP(TVector3 pos, TVector3 Mom, TVector3 posErr, TVector3 MomErr, Int_t Q, TVector3 o, TVector3 dj, TVector3 dk)
  FairTrackParP result(hitPos, mom, hitPosError, momError, getCharge(), origin, dj, dk);
  return result;
}

TVector3 PndFtsHoughTrackCand::getP(const Double_t zLabSys) const
{
  throwIfZOutOfRange(zLabSys);

  TVector3 mom;

  if (kFALSE == isComplete()) {
    std::cout << "getPforHit: Track cand. is not complete yet. Momentum will be calculated for incomplete track cand.\n";
  }

  Double_t pZLabSys;
  Double_t pXLabSys;
  std::pair<Double_t, Double_t> pZPXLabSys;

  // track model is assumed to be line+parabola+line in zx and line in zy
  if (zLabSys <= fZCoordLineParabola) {
    // use 1st line in zx plane
    pZPXLabSys = getPZPXLabLine(zLabSys, &fZxLineBeforeDipole);
  } else if (zLabSys < fZCoordParabolaLine) {
    // use "tangent" to parabola in zx plane
    pZPXLabSys = getPZPXLabParabola(zLabSys);
  } else {
    // use 2nd line in zx plane
    pZPXLabSys = getPZPXLabLine(zLabSys, &fZxLineBehindDipole);
  }

  pZLabSys = pZPXLabSys.first;
  pXLabSys = pZPXLabSys.second;
  const Double_t pYLabSys = getPYLab();
  mom.SetXYZ(pXLabSys, pYLabSys, pZLabSys);
  if (fVerbose > 10)
    std::cout << "P-Vector for z=" << zLabSys << " : " << mom.X() << " " << mom.Y() << " " << mom.Z() << '\n';
  return mom;
}

Double_t PndFtsHoughTrackCand::getThetaZxRad(const Double_t zLabSys) const
{
  // estimate angle by calculating 2 points (zLabSys1, xLabSys1) and (zLabSys2, xLabSys2) which are close together

  throwIfZOutOfRange(zLabSys);

  const Double_t epsilon = 0.5; // small positive number

  const Double_t xLabSys1 = getXLabSys(zLabSys - epsilon);
  const Double_t xLabSys2 = getXLabSys(zLabSys + epsilon);

  return atan2(xLabSys2 - xLabSys1, 2 * epsilon);
}

Double_t PndFtsHoughTrackCand::getXLabSys(const Double_t zLabSys) const
{
  // calculates the corresponding x coordinate to a given z coordinate according to the track model

  throwIfZOutOfRange(zLabSys);

  if (zLabSys <= fZCoordLineParabola) {
    // use 1st line in zx plane
    return getXOrYLabForLine(zLabSys, &fZxLineBeforeDipole);
  } else if (zLabSys < fZCoordParabolaLine) {
    // use parabola in zx plane
    return getXLabForParabola(zLabSys);
  } else {
    // use 2nd line in zx plane
    return getXOrYLabForLine(zLabSys, &fZxLineBehindDipole);
  }
  throwError("Argument was not handled!");
}

TVector3 PndFtsHoughTrackCand::getPos(const Double_t zLabSys) const
{
  // calculates the point on the track based on the results from the Hough transforms using my track model for the given z

  throwIfZOutOfRange(zLabSys);

  if (kFALSE == isComplete())
    std::cout << "getPositionForHit: Track cand. is not complete yet. Position will be calculated for incomplete track cand.\n";

  TVector3 position;
  // track model is assumed to be line+parabola+line in zx and line in zy
  const Double_t yLabSys = getXOrYLabForLine(zLabSys, &fZyLine);

  Double_t xLabSys = getXLabSys(zLabSys);

  position.SetXYZ(xLabSys, yLabSys, zLabSys);
  if (fVerbose > 10)
    std::cout << "Pos-Vector for z=" << zLabSys << " : " << position.X() << " " << position.Y() << " " << position.Z() << '\n';
  return position;
}

// end for conversion to PndTrack
/////////////////////////////////

// Double_t PndFtsHoughTrackCand::getThetaZxComplicated(const Double_t zLabSys) const{
//	throwIfZOutOfRange(zLabSys);
//
//	if ( zLabSys <= fZCoordLineParabola ){
//		// use 1st line in zx plane
//		return fZxLineBeforeDipole.getThetaRadVal();
//	} else if ( zLabSys < fZCoordParabolaLine ){
//		// use parabola to calculate angle in zx plane
//		return getThetaZxLabForParabolaUnused(zLabSys);
//	} else {
//		// use 2nd line in zx plane
//		return fZxLineBehindDipole.getThetaRadVal();
//	}
//}

//	Double_t getThetaZxLabForParabolaComplicated(const Double_t &zLabSys) const {
//		// calculate theta in radian in zx plane for parabola part of track model
//		// go into local coordinate system and use derivation of parabola to calculate angles, then add rotation of coordinate systems
//
//		// go from lab system into local parabola system
//		// which is rotated with thetaRadVor
//		// and shifted (compare to PndFtsHoughSpace)
//		const Double_t thetaZxRadVor = fZxLineBeforeDipole.getThetaRadVal();
//		const Double_t zRefPos = fZxLineBeforeDipole.getZRefLabSys();
//		const Double_t interceptZx = fZxLineBeforeDipole.getSecondVal();
//
//		const Double_t pZxInv = getQoverPzx()/getCharge();
//
//		// is used to redefine an origin for the coordinate system (so that the angle definition gives meaningful theta values)
//		//			Double_t interceptZx; // is used to shift the true x values of hits so that they hit the point (zOffset|0) in z-x-plane (value is determined by line fit on
// chambers1+2)
//		// (zreal=zOffset, xreal=interceptZx) = (zshifted = 0, xshifted = 0)
//		// zshifted = zreal - zRefPos
//		// xshifted = xreal - interceptZx
//		// zreal = zshifted + zRefPos
//		// xreal = xshifted + interceptZx
//		// For the z-x-plane parabola, a shift in x (hitshiftinx) needs to be set (which should be the result of the straight line hough transform before the dipole field)
//
//		// shifted origin between global and local coordinate system
//		const Double_t zshifted = zLabSys - zRefPos;
//		const Double_t xshifted = getXLabForParabola(zLabSys) - interceptZx;
//
//		// rotation of local coordinate system wrt (shifted) global coordiate system
//		const Double_t zShiftedRotated = zshifted * cos(thetaZxRadVor) + xshifted*sin(thetaZxRadVor);
//
//
//		// for parabola equation
//		const Double_t n = 1.;
//		const Double_t e = 1.;
//		const Double_t By = 1.;
//
//
//		// calculate rotation angle (in zx plane) of parabola in shifted and rotated system
//		const Double_t thetaZxLocal = atan(n*e*By*pZxInv*zShiftedRotated); // TODO: Maybe here I actually get 2 values
//		// add rotation of local coordinate system wrt global coordinate system
//		return thetaZxRadVor + thetaZxLocal;
//	}

// inline std::pair<Double_t, Double_t> getPZPXLabParabolaApproximation(const Double_t &zLabSys) const {
//		// theta in radian in zx plane given at z = zRefLabSys
//		const Double_t thetaRadVor = fZxLineBeforeDipole.getThetaRadVal();
//		const Double_t zRefLabVor = fZxLineBeforeDipole.getZRefLabSys();
//
//		const Double_t thetaRadNach = fZxLineBehindDipole.getThetaRadVal();
//		const Double_t zRefLabNach = fZxLineBehindDipole.getZRefLabSys();
//
//		// assume that theta is rotated linearly along z which is (probably) true for a circle, but not for a parabola // TODO: Check if this is a good assumption
//		const Double_t zDistTotal = zRefLabNach - zRefLabVor;
//		const Double_t zDistTraveled = zLabSys - zRefLabVor;
//
//		const Double_t currentThetaRad = thetaRadVor + zDistTraveled / zDistTotal * (thetaRadNach - thetaRadVor);
//
//		const Double_t qDivPzx = fZxParabola.getSecondVal(); // Q/pzx
//		const Double_t pZx = getCharge() / qDivPzx;
//
//		const Double_t pZLabSys = pZx*cos(currentThetaRad);
//		const Double_t pXLabSys = pZx*sin(currentThetaRad);
//		std::pair<Double_t, Double_t> pZPXLabSys(pZLabSys, pXLabSys);
//
//		return pZPXLabSys;
//	}
