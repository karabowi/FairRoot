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


/** PndMvdIdealTrackFinderTask.h
 *@author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Ideal cluster finding task
 */

#ifndef PNDHYPGEIDEALTRACKFINDERTASK_H
#define PNDHYPGEIDEALTRACKFINDERTASK_H

#include "FairTask.h"
#include "PndMCTrack.h"

#include "TH1.h"
#include "PndHypGePoint.h"

//#include "GFTrackCand.h"
//#include "PndHypCluster.h"
#include <string>
#include <vector>
#include <map>

class TClonesArray;

class PndHypGeIdealTrackFinderTask : public FairTask {
 public:
  /** Default constructor **/
  PndHypGeIdealTrackFinderTask();
  /** Destructor **/
  virtual ~PndHypGeIdealTrackFinderTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void WriteHistograms();

 private:
  TString fMcBranch;
  TString fTrackBranch;

  /** Input array of PndHypDigis **/

  TClonesArray *fMcArray;
  TClonesArray *fTrackArray;

  /** Output array of PndHypHits **/

  TH1F *hismom;
  int fEventNr;
  std::map<int, std::vector<int>> mcHitMap;
  Int_t fVerbose;

  // void Register();
  // void Reset();
  // void ProduceHits();
  // std::map<int, std::vector<int> > AssignHitsToTracks();

  ClassDef(PndHypGeIdealTrackFinderTask, 1);
};

#endif /* PndHypGeIdealTrackFinderTASK_H */
