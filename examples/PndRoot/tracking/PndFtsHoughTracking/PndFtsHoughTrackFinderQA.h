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
 @class PndFtsHoughTrackFinderQA

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief Implementation of the QA for the Hough transform based FTS PR. Use this for parameter optimization.

 Created: 09.02.2014
 */

#ifndef PndFtsHoughTrackFinderQA_H
#define PndFtsHoughTrackFinderQA_H

#include "PndFtsHoughTrackFinder.h"

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

class PndFtsHoughTrackFinderQA : public PndFtsHoughTrackFinder {
 public:
  PndFtsHoughTrackFinderQA(PndFtsHoughTrackerTask *trackerTask); ///< @brief Set pointer to tracker task (super important as it provides an I/O interface to PandaRoot)
  virtual ~PndFtsHoughTrackFinderQA();                           ///< @brief Destructor

  virtual void FindTracks(); ///< @brief Performs the track finding.

 private:
  // for debugging

 private:
  ClassDef(PndFtsHoughTrackFinderQA, 1);
};

#endif /*PndFtsHoughTrackFinderQA_H*/
