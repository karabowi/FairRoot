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

#ifndef PndHypIdealTrackFinderTASK_H
#define PndHypIdealTrackFinderTASK_H

#include "FairTask.h"
#include "PndMCTrack.h"
#include "PndGeoHypPar.h"
#include "PndHypHit.h"
#include "PndHypPoint.h"
//#include "PndHypPixel.h"
//#include "PndHypClusterCand.h"
#include "GFTrackCand.h"
#include "PndHypCluster.h"
#include <string>
#include <vector>

class TClonesArray;

class PndHypIdealTrackFinderTask : public FairTask {
 public:
  /** Default constructor **/
  PndHypIdealTrackFinderTask();
  /** Destructor **/
  virtual ~PndHypIdealTrackFinderTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void PrintResult();
  void SetVerbose(Int_t verbose) { fVerbose = verbose; };

 private:
  // std::vector<Int_t> GetHitPerCluster(PndHypCluster* clusterCand);
  void ClearTrackCandMap();
  void AddAndExpand(Int_t trackID, Int_t detnum, Int_t iHit);
  Double_t GetTrackDip(PndMCTrack *myTrack);
  Double_t GetTrackCurvature(PndMCTrack *myTrack);

  TString fHitBranchStrip;
  // TString fClusterBranchStrip;
  // TString fDigiBranchStrip;
  TString fMcBranch;
  TString fTrackBranch;

  /** Input array of PndHypDigis **/
  TClonesArray *fStripHitArray;
  // TClonesArray* fStripClusterArray;
  // TClonesArray* fStripDigiArray;
  TClonesArray *fMcArray;
  TClonesArray *fTrackArray;

  /** Output array of PndHypHits **/
  TClonesArray *fTrackCandArray;
  std::map<Int_t, GFTrackCand *> fTrackCandMap;

  Int_t fVerbose;

  void Register();
  void Reset();
  void ProduceHits();

  ClassDef(PndHypIdealTrackFinderTask, 2);
};

#endif /* PndHypIdealTrackFinderTASK_H */
