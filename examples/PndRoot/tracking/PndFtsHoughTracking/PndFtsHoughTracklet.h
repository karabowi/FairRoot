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
 @class PndFtsHoughTracklet

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief Class for saving the result of one Hough transform for FTS PR.

 One object of this class corresponds to one peak in the Hough space or part of a track candidate.
 The tracklet can represent a line or a parabola (actually an arbitrary result of a 2d Hough transform).

 Created: 11.02.2014
*/

#ifndef PNDFTSHOUGHTRACKLET_H
#define PNDFTSHOUGHTRACKLET_H

#include "PndFtsHoughTrackerTask.h"

// Root Class Headers ----------------
#include "PndTrackCand.h"
#include "Rtypes.h"     // for Double_t, Int_t, etc
#include "FairLogger.h" // for FairLogger, MESSAGE_ORIGIN

// For error reporting
#include "TString.h"
#include <stdexcept>

class PndFtsHit;
class PndTrack;
class TClonesArray;

class PndFtsHoughTracklet : public PndTrackCand {
 public:
  // Constructors/Destructors ---------
  PndFtsHoughTracklet(Double_t zRefLabSys = 0.,
                      PndFtsHoughTrackerTask *trackerTask = nullptr); ///< @brief Set pointer to tracker task (super important as it provides an I/O interface to PandaRoot)
  ~PndFtsHoughTracklet();

  // Accessors -----------------------
  void Print() const;

  Bool_t isSet() const { return fIsSet; }; // returns kTRUE iif data from the 2d Hough transforms have been entered

  // add results from Hough transforms
  void SetHoughTransformResults(const Double_t thetaVal, const Double_t secondVal, const Double_t peakHeight, const Double_t thetaHw, const Double_t secondHw);

  // getters for Hough space peak info
  Double_t getPeakHeightFromPeakFinder() const { return fPeakHeightFromPeakFinder; };
  Double_t getThetaRadVal() const { return fThetaRadVal; }; // theta value for peak (in rad)
  Double_t getThetaRadHw() const { return fThetaRadHw; };   // hw = half width = half length of Hough space bin
  Double_t getSecondVal() const { return fSecondVal; };     // second value for peak, see below for explanation

  Double_t getSecondHw() const { return fSecondHw; };     // hw = half width = half length of Hough space bin
  Double_t getZRefLabSys() const { return fZRefLabSys; }; // in cm

  // hits
  UInt_t getNSharedHits(const PndFtsHoughTracklet &rhs); // gives the number of hits that are contained both in rhs and *this

 private:
  // Private Data Members ------------
  // for PandaRoot input/output
  PndFtsHoughTrackerTask *fTrackerTask;

  /** @brief For error reporting */
  void throwError(const TString s) const { throw std::runtime_error(s.Data()); };

  Int_t fVerbose;

  Bool_t fIsSet; // kTRUE if values have already been filled

  Double_t fPeakHeightFromPeakFinder;
  // height of peak in Hough transform
  // (as determined by peakFinder -- does not need to be == fHitId.size()
  // as the peak finder might add a weight to the hits depending on how close they have been to the actual peak)

  // saving peak position in a 2d Hough transform as well as the half width of the histogram
  Double_t fThetaRadVal; // theta angle
  Double_t fThetaRadHw;  // error on angle

  Double_t fSecondVal; // second value is x or y intercept for line in zx or zy plane and Q/pZx for parabola in zx plane
  Double_t fSecondHw;  // error on second value

  // at which z value in lab sys the values have to be interpreted
  Double_t fZRefLabSys;

  //	void addPeakHits(); // check which hits have contributed to the peak and add them to the hitId vector

  ClassDef(PndFtsHoughTracklet, 1);
};

#endif
