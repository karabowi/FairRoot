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
 * @class PndTrackingQASummary
 *
 * @date 05.05.2023
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 */

#pragma once

#include "PndTrackingQA.h"
#include "PndTrackingQAQualityNumbers.h"

#include <TObject.h>

#include <map>
#include <iostream>

class PndTrackingQASummary : public TObject {
 public:
  PndTrackingQASummary(){};
  virtual ~PndTrackingQASummary(){};

  void AddResult(int quality) { fMapTrackingQAResults[quality]++; }

  void AddResult(int quality, int value) { fMapTrackingQAResults[quality] += value; }
  int GetResult(int quality) { return fMapTrackingQAResults[quality]; }

  int GetAllPossibleTracks();
  int GetAllTracksWithAtLeast3Hits();
  int GetAllTracks();
  int GetFoundPossibleTracks();
  int GetAllTracksWithHitsNotFound();

  int GetFoundPrimaryTracksLessThan3Hits() { return GetResult(TrackingQA::qualityNumbers::kMcLessThanThreePrim) - GetResult(TrackingQA::qualityNumbers::kLessThanThreePrim); }
  int GetFoundSecondaryTracksLessThan3Hits() { return GetResult(TrackingQA::qualityNumbers::kMcLessThanThreeSec) - GetResult(TrackingQA::qualityNumbers::kLessThanThreeSec); }

  int GetFoundPrimaryTracksMoreThan3Hits() { return GetResult(TrackingQA::qualityNumbers::kMcAtLeastThreePrim) - GetResult(TrackingQA::qualityNumbers::kAtLeastThreePrim); }

  int GetFoundSecondaryTracksMoreThan3Hits() { return GetResult(TrackingQA::qualityNumbers::kMcAtLeastThreeSec) - GetResult(TrackingQA::qualityNumbers::kAtLeastThreeSec); }

  int GetFoundPrimaryTracksPossible() { return GetResult(TrackingQA::qualityNumbers::kMcPossiblePrim) - GetResult(TrackingQA::qualityNumbers::kPossiblePrim); }

  int GetFoundSecondaryTracksPossible() { return GetResult(TrackingQA::qualityNumbers::kMcPossibleSec) - GetResult(TrackingQA::qualityNumbers::kPossibleSec); }

  friend std::ostream &operator<<(std::ostream &os, const PndTrackingQASummary &summary)
  {
    os << "MapResults:" << std::endl;
    for (auto val : summary.fMapTrackingQAResults) {
      os << TrackingQA::qualityNumbers::QualityNumberToString(val.first) << " : " << val.second << std::endl;
    }
    os << std::endl;
    return os;
  }

 private:
  std::map<int, int> fMapTrackingQAResults; //<qualityNumbers, counts> see PndTrackingQA.h for qualityNumbers

  ClassDef(PndTrackingQASummary, 1);
};
