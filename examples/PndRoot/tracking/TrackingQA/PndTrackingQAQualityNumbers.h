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

#pragma once

#include "PndTrackingQARecoInfo.h"
#include <array>

class PndTrackingQARecoInfo;

namespace TrackingQA {
struct qualityNumbers {
  static const std::array<int, 22> constexpr listOfNumbers{-14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8};
  static const int
    // Following: Default statuses.
    // Are the 'not found' tracks in the quality histogram of PndTrackingQualityTask.
    kPossibleSec = -1,       // possible: As defined through the possibleFunctor; secondary: a non-primary particle
    kPossiblePrim = -2,      // possible: As defined through the possibleFunctor; primary: coming directly from particle generator (e.g. EvtGen)
    kAtLeastThreeSec = -3,   // atLeastThree: min. 3 hit points in central tracking detectors (MVD, STT, GEM); secondary: a non-primary particle
    kAtLeastThreePrim = -4,  // atLeastThree: min. 3 hit points in central tracking detectors (MVD, STT, GEM); primary: coming directly from particle generator (e.g. EvtGen)
    kLessThanThreePrim = -5, // LessThanThree: fewer than 3 hit points in central tracking detectors (MVD, STT, GEM); primary: coming directly from particle generator (e.g. EvtGen)
    kLessThanThreeSec = -6,  // LessThanThree: fewer than 3 hit points in central tracking detectors (MVD, STT, GEM); secondary: not coming from particle generator (e.g. EvtGen)
                             // Following: MC statuses of all (found+notfound) tracks
    kMcPossibleSec = -7,     // see above
    kMcPossiblePrim = -8, kMcAtLeastThreeSec = -9, kMcAtLeastThreePrim = -10, kMcLessThanThreePrim = -11, kMcLessThanThreeSec = -12,
    kMcAllTracksWithHits = -13, // sum of -7 - -10
    kMcAllTracks = -14,         // sum of -11 and -12

    // Following: Status of reconstructed tracks (= created PndTracks)
    kPartiallyImpure = 1, // PartiallyImpure: at least 70% of hits of reco'd track come from one MC track ('mostProbableTrack')
    kPartiallyPure = 2,   // PartiallyPure: all hits of reco'd track come from one single MC track; at least 70% of hits of MC track have been found in reco'd track
    kFullyImpure = 3,     // FullyImpure: all hits of MC track have been found in reco'd track but some impurities from other tracks are allowed
    kFullyPure = 4,       // FullyPure: all hits of reco'd track come from one single MC track; all hits of MC track have been found in reco'd track

    kGhost = 5, // ghost: less than 70% of hits of reco'd track come from one MC track ('mostProbableTrack')
    kClone = 6, // clone: sum of (number of times one mc track was found -1) over all mc tracks

    kNotFound = 7, // notFound: total number of not reco'd tracks
    kFound = 8;    // found: total number of reco'd tracks; the sum of FullyPure, FullyImpure, PartiallyPure, PartiallyImpure

  static std::string QualityNumberToString(int qNumber)
  {
    switch (qNumber) {
    case kPossiblePrim: return "PossiblePrimary"; break;
    case kPossibleSec: return "PossibleSec"; break;
    case kAtLeastThreeSec: return "AtLeastThreeSec"; break;
    case kAtLeastThreePrim: return "AtLeastThreePrim"; break;
    case kLessThanThreePrim: return "LessThanThreePrim"; break;
    case kLessThanThreeSec: return "LessThanThreeSec"; break;
    case kMcPossiblePrim: return "McPossiblePrimary"; break;
    case kMcPossibleSec: return "McPossibleSec"; break;
    case kMcAtLeastThreeSec: return "McAtLeastThreeSec"; break;
    case kMcAtLeastThreePrim: return "McAtLeastThreePrim"; break;
    case kMcLessThanThreePrim: return "McLessThanThreePrim"; break;
    case kMcLessThanThreeSec: return "McLessThanThreeSec"; break;
    case kMcAllTracksWithHits: return "McAllTracksWithHits"; break;
    case kMcAllTracks: return "McAllTracks"; break;
    case kPartiallyImpure: return "PartiallyImpure"; break;
    case kPartiallyPure: return "PartiallyPure"; break;
    case kFullyPure: return "FullyPure"; break;
    case kFullyImpure: return "FullyImpure"; break;
    case kGhost: return "Ghost"; break;
    case kClone: return "Clone"; break;
    case kNotFound: return "NotFound"; break;
    case kFound: return "Found"; break;
    default: {
      return std::to_string(qNumber);
    }
    }
  };

  static std::string QualityNumberToDetailedString(int qNumber)
  {
    switch (qNumber) {

    case kFullyPure: return "Fully Purely found"; break;
    case kFullyImpure: return "Fully Impurely found"; break;
    case kPartiallyPure: return "Partially Purely found"; break;
    case kPartiallyImpure: return "Partially Impurely found"; break;
    case kGhost: return "Ghosts"; break;
    case kClone: return "Clones"; break;
    case kNotFound: return "Total not found"; break;
    case kFound: return "Total found"; break;
    case kPossibleSec: return "Possible, Sec."; break;
    case kPossiblePrim: return "Possible, Prim."; break;
    case kAtLeastThreeSec: return ">= 3 Hits, Sec."; break;
    case kAtLeastThreePrim: return ">= 3 Hits, Prim."; break;
    case kLessThanThreePrim: return "< 3 Hits, Prim."; break;
    case kLessThanThreeSec: return "< 3 Hits, Sec."; break;
    case kMcPossibleSec: return "MC: Possible, Sec."; break;
    case kMcPossiblePrim: return "MC: Possible, Prim."; break;
    case kMcAtLeastThreeSec: return "MC: >= 3 Hits, Sec."; break;
    case kMcAtLeastThreePrim: return "MC: >= 3 Hits, Prim."; break;
    case kMcLessThanThreePrim: return "MC: < 3 Hits, Prim."; break;
    case kMcLessThanThreeSec: return "MC: < 3 Hits, Sec."; break;
    case kMcAllTracksWithHits: return "All tracks with MC hits"; break;
    case kMcAllTracks: return "All tracks"; break;
    default: {
      return std::to_string(qNumber);
    }
    }
  };
};

bool RecoTrackFound(PndTrackingQARecoInfo *recoInfo, double purityThreshold = 0.7, double efficiencyThreshold = 0.);

int GetRecoQuality(PndTrackingQARecoInfo *recoInfo, double purityThreshold = 0.7, double efficiencyThreshold = 0.);
}; // namespace TrackingQA
