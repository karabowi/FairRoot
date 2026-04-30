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
 @class PndFtsHoughTrackerTaskQA

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief For automatic parameter optimization. This class tries numerous parameters and prints out good combinations.

 Created: 09.02.2014
 */

#ifndef PndFtsHoughTrackerTaskQA_H
#define PndFtsHoughTrackerTaskQA_H

#include "PndFtsHoughTrackerTask.h"
#include "PndFtsHoughTrackFinderParams.h"
#include "FairTask.h"
#include <cmath>
#include <vector>
#include "Rtypes.h" // for Double_t, Int_t, etc
#include <iostream>

#include "TVector3.h"
#include "PndFtsHit.h"
#include "TClonesArray.h"
#include "PndFtsTube.h"

class PndFtsHoughTrackCand;
class PndFtsHoughSpace;
class PndGeoFtsPar;
class FairField;

// For error reporting
#include "TString.h"
#include <stdexcept>
// for debugging
#include "FairMCPoint.h"

class PndFtsHoughTrackerTaskQA : public PndFtsHoughTrackerTask {
 public:
  /** @brief Constructor with flags. Can also be used as standard constructor.
   * @param[in] verbose Verbosity level: 0 least, higher -> more output.
   * @param[in] persistence kFALSE does not write track candidates from PR to output root file.
   * @param[in] saveDebugInfo kTRUE will write internal representation of track(let) candidates to output root file.
   */
  PndFtsHoughTrackerTaskQA(Int_t verbose = 0);
  /** @brief Destructor.	 */
  ~PndFtsHoughTrackerTaskQA();

  /** @brief Initialization of task at the beginning of a run. */
  virtual InitStatus Init();
  /** @brief ReInitiliazation of task when the runID changes. */
  virtual InitStatus ReInit();

  /** @brief Executed for each event.
   * @param opt Not used.
   */
  virtual void Exec(Option_t *opt);
  /** @brief When is this executed? After each event? */
  virtual void FinishEvent();

  /** @brief Writes output to root file, I guess. Called at the end of the run. */
  virtual void Finish();

  Int_t getNEvtsWithEnoughParabolas() const { return fNEvtsWithEnoughParabolas; };
  void setNParabolasToFind(Int_t nParabolasToFind) { fNParabolasToFind = nParabolasToFind; };
  // use this for parameter optimization
  void OverwriteTrackFinderParams(PndFtsHoughTrackFinderParams newParams) { fTrackFinderParams = newParams; };

 private:
  PndFtsHoughTrackFinderParams fTrackFinderParams;
  Int_t fNParabolasToFind;         // defines how many parabolas I should find in the event
  Int_t fNEvtsWithEnoughParabolas; // counts in how many events I have found >= fNParabolasToFind

  // TODO: I don't think I need the copy constructor and the operator=
  PndFtsHoughTrackerTaskQA(const PndFtsHoughTrackerTaskQA &);
  PndFtsHoughTrackerTaskQA operator=(const PndFtsHoughTrackerTaskQA &);

  ClassDef(PndFtsHoughTrackerTaskQA, 1);
};

#endif
