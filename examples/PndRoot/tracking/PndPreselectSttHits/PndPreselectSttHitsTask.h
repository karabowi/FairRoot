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

/////////////////////////////////////////////////////////////
//  PndPreselectSttHitsTask
//  Finds Track
/////////////////////////////////////////////////////////////////

/** PndPreselectSttHitsTask
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 14.09.2021
 *@version 1.0
 **
 ** PANDA task class for finding tracks based on the triplet finder and the apollonius calculation
 ** Task level RECO
 **/

#pragma once

#include "FairTask.h"
#include <vector>

#include "PndPreselectSttHits.h"
//#include "PndSttStrawMap.h"
#include "PndGeoSttPar.h"

struct TrackSolution;

class PndPreselectSttHitsTask : public FairTask {
 public:
  /** Constructor **/
  PndPreselectSttHitsTask();

  /** Destructor **/
  virtual ~PndPreselectSttHitsTask();

  /** @brief Initializes and loads the data for the PndPreselectSttHitsTask */
  virtual InitStatus Init();

  void SetParContainers();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void Finish();

  void SetPreselector(PndPreselectSttHits *preselector) { fPreselector = preselector; };

  virtual void AddBranchName(TString name)
  {
    fAdditionalBranchNames.push_back(name);
  } ///< Search for tracks only in given branches. If no BranchName is given all tracking detectors are taken

 protected:

 private:
  PndGeoSttPar *fSttParameters = nullptr; // needed for SttStrawMap
  //TClonesArray *fTubeArray = nullptr;     // needed for SttStrawMap

  TClonesArray *fTrackCandArray = nullptr;
  TClonesArray *fSTTHits = nullptr;

  PndPreselectSttHits *fPreselector = nullptr;

  std::vector<TString> fAdditionalBranchNames;

  ClassDef(PndPreselectSttHitsTask, 1);
};
