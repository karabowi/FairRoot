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
 @class PndFtsHoughTrackFinder

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief Implementation of the Hough transform based FTS PR. Creates Hough spaces, finds peaks (=tracklets) and combines them to track candidates.

 This is a class version of the HoughTest.C macro PR test implementation
 minus all the plotting stuff.
 Take a look at the notes of the macro version for further details.

 Recent Changes
 Major code cleanup and deletion of test code / unneeded code
 use PndFtsHoughTrackCand to store information about track candidates and Hough transforms
 Find all peaks with a minimum height, analysing peak shapes.
 Moved all Hough space related code to PndFtsHoughSpace -> Major code simplification, better maintainability
 Fill PndTrackCands and PndTrack for output

 This class is loosely modeled after the
 mvd/MvdTracking/PndRiemannTrackFinder
 sttmvdtracking/PndMvdSttGemRiemannTrackFinder
 classes


 TODO
 Match straight line for stations 5+6 to parabola
 Add skewed hits
 Add drift circles
 Adaptive Hough

 Created: 18.06.2013
 */

#ifndef PndFtsHoughTrackFinder_H
#define PndFtsHoughTrackFinder_H

#include "PndFtsHoughTrackFinderParams.h"

//#include "TClonesArray.h"
#include "Rtypes.h"     // for Double_t, Int_t, etc
#include "FairLogger.h" // for FairLogger, MESSAGE_ORIGIN

#include <cmath>
#include "TMath.h"
#include <math.h>
#include <algorithm>
#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

#include "PndFtsHit.h"
#include "PndFtsHoughTrackerTask.h"
#include "PndFtsHoughSpace.h"
#include "PndFtsHoughTracklet.h"
#include "PndFtsHoughTrackCand.h"
#include "PndTrackCand.h"
#include "PndTrack.h"

// For error throwing
#include "TString.h"
#include <stdexcept>

class PndFtsHoughTrackFinder {
 public:
  PndFtsHoughTrackFinder(PndFtsHoughTrackerTask *trackerTask); ///< @brief Set pointer to tracker task (super important as it provides an I/O interface to PandaRoot)
  virtual ~PndFtsHoughTrackFinder();                           ///< @brief Destructor

  // use this for parameter optimization
  void OverwriteTrackFinderParams(PndFtsHoughTrackFinderParams newParams) { fParams = newParams; };

  virtual void FindTracks(); ///< @brief Performs the track finding.

  // Output
  Int_t NTracks() const { return fHoughTrackCandsComplete.size(); }; ///< @brief Returns the number of found tracks
  /**@brief Returns the track cand. with index i.
   *
   * Note: Method calculates first and last parameters of the PndTrack object, but uses an empty PndTrackCand which has to be set lateron using SetTrackCandRef!
   * @param i Index of requested track cand.
   * @return Track cand. to index i.
   */
  PndTrack GetPndTrack(int i) { return fHoughTrackCandsComplete[i].getPndTrack(); };
  /**@brief Returns the track cand. with index i.
   *
   * Note: Use this to add a PndTrackCand to the corresponding PndTrack object with SetTrackCandRef!
   * @param i Index of requested track cand.
   * @return Track cand. to index i.
   */
  PndTrackCand GetPndTrackCand(int i) { return fHoughTrackCandsComplete[i].getPndTrackCand(); };
  /**@brief Returns the track cand. with index i.
   *
   * Note: For debugging only.
   * @param i Index of requested track cand.
   * @return Track cand. to index i.
   */
  PndFtsHoughTrackCand GetHoughTrack(int i) const { return fHoughTrackCandsComplete[i]; };

  // Output event statistics
  Int_t getNLinesBeforeDipoleFound() const { return fNLinesBeforeDipoleFound; };
  Int_t getNLinesBehindDipoleFound() const { return fNLinesBehindDipoleFound; };
  Int_t getNParabolasFound() const { return fNParabolasFound; };
  Int_t getNTracksFound() const { return fNTracksFound; };

 protected:
  /// @brief Task which handles PandaRoot input/output and provides settings for FTS PR.
  /// Has to be set using the constructor.
  PndFtsHoughTrackerTask *fTrackerTask;

  /** @brief For error reporting */
  void throwError(const TString s) const { throw std::runtime_error(s.Data()); };

  //	Int_t   fFtsBranchId; // needed for saving and accessing hits
  //	TClonesArray *fFtsHitArray; ///< @brief Input array of all FTS hits.

  // for Hough
  PndFtsHoughTrackFinderParams fParams;

  //-----------
  std::vector<PndFtsHoughTrackCand> fHoughTrackCandsComplete;    ///< For internal storing of complete track cands.
  std::vector<PndFtsHoughTrackCand> fHoughTrackCandsZxPlaneOnly; ///< For internal storing of track cands. (zx plane track model only)
  //	std::vector<PndTrackCand> fTrackCand; // resulting track candidates, also used for returning PndTracks

  static const Double_t fThetaRadLineBehindDipoleMatchesToParabolaIfBelow;

  ///< @brief Minimum required height for peaks in Hough spaces.
  const UInt_t fMinPeakHeightZxLineBeforeDipole; ///< zx line before dipole field
  ///< @brief Minimum required height for peaks in Hough spaces.
  const UInt_t fMinPeakHeightZxParabola; ///< zx parabola within dipole field
  ///< @brief Minimum required height for peaks in Hough spaces.
  const UInt_t fMinPeakHeightZxLineBehindDipole; ///< zx line after dipole field
  ///< @brief Minimum required height for peaks in Hough spaces.
  const UInt_t fMinPeakHeightZyLine; ///< zy line

  // event statistics
  Int_t fNLinesBeforeDipoleFound;
  Int_t fNLinesBehindDipoleFound;
  Int_t fNParabolasFound;
  Int_t fNTracksFound;

  // takes the heighest peak (according to peak finder)
  // of all peaks that share > maxSameHits
  /**@brief Filters a vector of tracklets based on the number of shared hits.
   *
   * It will only keep the heighest peaks if 2 or more peaks share more than maxAcceptableSharedHits hits. If two peaks have the same height, both are kept.
   * @param maxAcceptableSharedHits Defines how many hits two tracklets / peaks are allowed to share.
   * @param[in,out] tracklets Vector containing tracklets which are supposed to be filtered. The vector will be modified.
   * @return
   */
  Bool_t FilterTrackletsBasedOnSharedHits(UInt_t maxAcceptableSharedHits, std::vector<PndFtsHoughTracklet> &tracklets);
  // helper functions for tracking algorithm
  std::vector<PndFtsHoughTracklet> FindLinesBehindDipoleZx();
  std::vector<PndFtsHoughTracklet> FindLinesBeforeDipoleZx();
  void FindMatchingParabolaToLineBeforeDipoleZxAndAddLineBehindDipole(const std::vector<PndFtsHoughTracklet> &trackletsLineBeforeDipole,
                                                                      const std::vector<PndFtsHoughTracklet> &trackletsLineBehindDipole);
  // kTRUE iif angles of parabola and of line behind dipole match at z coordinate where I switch from parabola to line (in zx plane)
  inline Bool_t LineBehindDipoleMatchesToLinePlusParabola(const PndFtsHoughTrackCand &lineParabola, const PndFtsHoughTracklet &lineBehindDipole) const;
  void FindZyLineMatchingToLineParabolaLineInZx();

 private:
  ClassDef(PndFtsHoughTrackFinder, 1);
};

// inline
Bool_t PndFtsHoughTrackFinder::LineBehindDipoleMatchesToLinePlusParabola(const PndFtsHoughTrackCand &lineParabola, const PndFtsHoughTracklet &lineBehindDipole) const
{

  // the angles should be compared at the z coordinate where I switch from parabola to line behind dipole
  Double_t zParabolaLine = lineBehindDipole.getZRefLabSys();
  return fabs(lineParabola.getThetaZyRad(zParabolaLine) - lineBehindDipole.getThetaRadVal()) < fThetaRadLineBehindDipoleMatchesToParabolaIfBelow;
}

#endif /*PndFtsHoughTrackFinder_H*/
