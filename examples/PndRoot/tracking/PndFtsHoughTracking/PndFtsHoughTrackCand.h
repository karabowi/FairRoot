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
 @class PndFtsHoughTrackCand

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief Class for saving a FTS track cand. for Hough transform based FTS PR.

 The track cand. consists of several tracklets:
 In zx-plane line + parabola + line
 In zy-plane line

 This class takes care of geometric calculations for tracklets.

 Loosely modeled after tools/riemannfit/PndRiemannTrack.h

 TODO
 Maybe it would be better not to derive from PndTrackCand, but to contain an object of PndTrackCand

 Created: 24.01.2014
 */

#ifndef PNDFTSHOUGHTRACKCAND_H
#define PNDFTSHOUGHTRACKCAND_H

class PndFtsHoughTrackerTask;

// Root Class Headers ----------------
#include "PndTrackCand.h"
#include "PndFtsHoughTracklet.h"
#include "Rtypes.h"     // for Double_t, Int_t, etc
#include "FairLogger.h" // for FairLogger, MESSAGE_ORIGIN

// For error reporting
#include "TString.h"
#include <stdexcept>

#include <cmath>

class PndFtsHit;
class PndTrack;
class FairTrackParP;
class TVector3;
class TClonesArray;

class PndFtsHoughTrackCand : public TObject {
 public:
  // Constructors/Destructors ---------
  PndFtsHoughTrackCand(PndFtsHoughTrackerTask *trackerTask = nullptr); ///< @brief Set pointer to tracker task (super important as it provides an I/O interface to PandaRoot)
  ~PndFtsHoughTrackCand();

  // operators
  // PndFtsHoughTrackCand are the same if they contain the same hits, that means if the PndTrackCand are the same, therefore no need to implement that operator here

  // Accessors -----------------------
  void Print();
  // isComplete() is kTRUE iif data from all Hough transforms have been entered
  Bool_t isComplete() const { return (fZxLineBeforeDipole.isSet() && fZxParabola.isSet() && fZxLineBehindDipole.isSet() && fZyLine.isSet()); };

  PndTrackCand getPndTrackCand(); // convert *this to a PndTrackCand, cannot be const
  PndTrack getPndTrack(); // convert *this to a PndTrack, cannot be const // calculates first and last parameter, but uses an empty PndTrackCand which has to be set lateron using
                          // SetTrackCandRef

  FairTrackParP getTrackParPForHit(const UInt_t i); // get the track parameters (needed for conversion to PndTrack) for hit with index i, cannot be const

  inline Int_t getCharge() const;                // gets charge of track candidate // TODO only charge sign is implemented
  TVector3 getP(const Double_t zLabSys) const;   // gets the momentum calculated at some z coordinate in laboratory system
  TVector3 getPos(const Double_t zLabSys) const; // gets the position calculated at some z coordinate in laboratory system
  Double_t getXLabSys(const Double_t zLabSys) const;

  Double_t getThetaZxRad(const Double_t zLabSys) const; // gets the angle to the z axis in the zx plane calculated at some z coordinate (in laboratory system)
  // gets the angle to the z axis in the zy plane calculated at some z coordinate (in laboratory system)
  Double_t getThetaZyRad(const Double_t) const { return fZyLine.getThetaRadVal(); }; // zLabSys //[R.K.03/2017] unused variable(s)

  Double_t getZLineParabola()
  {
    return fZCoordLineParabola;
  }; // gets z coordinate in laboratory system where I switch from line before dipole to parabola within dipole (in zx plane)
  Double_t getZParabolaLine()
  {
    return fZCoordParabolaLine;
  }; // gets z coordinate in laboratory system where I switch from parabola within dipole to line behind dipole (in zx plane)

  // Modifiers -----------------------
  // add results from Hough transforms
  void SetZxLineBeforeDipole(const PndFtsHoughTracklet zxLineParabola);
  void SetZxParabola(const PndFtsHoughTracklet zxParabola);
  void SetZxLineBehindDipole(const PndFtsHoughTracklet zxParabolaLine);
  void SetZyLine(const PndFtsHoughTracklet zyLine);

 private:
  // for PandaRoot input/output
  PndFtsHoughTrackerTask *fTrackerTask;

  /** @brief For error reporting */
  void throwError(const TString s) const { throw std::runtime_error(s.Data()); };
  void throwIfZOutOfRange(const Double_t &zLabSys) const
  {
    if (zLabSys <= 100)
      throwError("zLabSys is too small, track model is not valid in that region.");
    if (zLabSys >= 1000)
      throwError("zLabSys is too big, track model is not valid in that region.");
  };

  void addUniqueTrackletHits(const PndFtsHoughTracklet inTracklet);

  Double_t getQdivPzx() const { return fZxParabola.getSecondVal(); };

  inline Double_t getPYLab() const;
  inline std::pair<Double_t, Double_t> getPZPXLabLine(const Double_t &zLabSys, const PndFtsHoughTracklet *const lineTracklet) const;
  inline std::pair<Double_t, Double_t> getPZPXLabParabola(const Double_t &zLabSys) const;

  inline Double_t getXOrYLabForLine(const Double_t &zLabSys, const PndFtsHoughTracklet *const lineTracklet) const;
  Double_t getXLabForParabola(const Double_t &zLabSys) const; // TODO: Check this!

  // Private Data Members ------------
  Int_t fVerbose;

  // zx plane
  // straight line Hough transform in zx plane (stations before dipole field)
  PndFtsHoughTracklet fZxLineBeforeDipole;
  // theta in zx plane
  // vs
  // x intercept

  // parabola Hough transform in zx plane (stations inside dipole field)
  PndFtsHoughTracklet fZxParabola;
  // theta in zx plane
  // vs
  // charge divided by momentum projected into zx plane

  // straight line Hough transform in zx plane (stations after dipole field)
  PndFtsHoughTracklet fZxLineBehindDipole;
  // theta in zx plane
  // vs
  // x intercept

  // zy plane
  // straight line Hough transform in zy plane (all stations)
  PndFtsHoughTracklet fZyLine;
  // theta in zy plane
  // vs
  // y intercept

  // internal track candidate for storing of hits belonging to this track candidate
  PndTrackCand fIntTrackCand;

  // at which z value the transition in the bending zx plane is done from a line (before dipole field) to a parabola (within dipole field)
  Double_t fZCoordLineParabola;
  // at which z value the transition in the bending zx plane is done from a parabola (within dipole field) to a line (before dipole field)
  Double_t fZCoordParabolaLine;

 public:
  ClassDef(PndFtsHoughTrackCand, 1);
};

// inline functions
Int_t PndFtsHoughTrackCand::getCharge() const
{
  if (0 < getQdivPzx()) {
    return 1;
  } else {
    return -1;
  }
}

Double_t PndFtsHoughTrackCand::getXOrYLabForLine(const Double_t &zLabSys, const PndFtsHoughTracklet *const lineTracklet) const
{
  // calculate x or y in lab sys for a given z position in lab sys for which the line assumption holds
  // theta in radian in zx plane given at z = zRefLabSys
  const Double_t thetaRad = lineTracklet->getThetaRadVal();
  const Double_t intercept = lineTracklet->getSecondVal();
  const Double_t zRefLabSys = lineTracklet->getZRefLabSys();
  const Double_t zshifted = zLabSys - zRefLabSys;

  Double_t xOrYLabSys = tan(thetaRad) * zshifted + intercept;

  return xOrYLabSys;
}

std::pair<Double_t, Double_t> PndFtsHoughTrackCand::getPZPXLabParabola(const Double_t &zLabSys) const
{

  const Double_t currentThetaRad = getThetaZxRad(zLabSys);

  const Double_t qDivPzx = getQdivPzx(); // Q/pzx
  const Double_t pZx = getCharge() / qDivPzx;

  const Double_t pZLabSys = pZx * cos(currentThetaRad);
  const Double_t pXLabSys = pZx * sin(currentThetaRad);
  std::pair<Double_t, Double_t> pZPXLabSys(pZLabSys, pXLabSys);

  return pZPXLabSys;
}

std::pair<Double_t, Double_t> PndFtsHoughTrackCand::getPZPXLabLine(const Double_t &, const PndFtsHoughTracklet *const lineTracklet) const
{ // zLabSys //[R.K.03/2017] unused variable(s)
  // theta in radian in zx plane given at z = zRefLabSys
  const Double_t thetaRad = lineTracklet->getThetaRadVal();
  const Double_t qDivPzx = fZxParabola.getSecondVal(); // Q/pzx
  const Double_t pZx = getCharge() / qDivPzx;

  const Double_t pZLabSys = pZx * cos(thetaRad);
  const Double_t pXLabSys = pZx * sin(thetaRad);
  std::pair<Double_t, Double_t> pZPXLabSys(pZLabSys, pXLabSys);

  return pZPXLabSys;
}

Double_t PndFtsHoughTrackCand::getPYLab() const
{
  // theta in radian in zy plane given at z where first zx line meets the parabola
  const Double_t zRefLabSys = fZxParabola.getZRefLabSys();
  const Double_t thetaZyRad = fZyLine.getThetaRadVal();
  std::pair<Double_t, Double_t> pZPXLabSys = getPZPXLabLine(zRefLabSys, &fZxLineBeforeDipole);
  const Double_t pZLabSys = pZPXLabSys.first;
  Double_t pYLabSys = tan(thetaZyRad) * pZLabSys;
  return pYLabSys;
}

#endif
