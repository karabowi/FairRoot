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
//  PndHoughTrackFinderTask
//  Finds Track for one event
/////////////////////////////////////////////////////////////////

/** PndHoughTrackFinderTask
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 01.06.2020
 *@updated 02.02.2021
 *@version 2.0
 **
 ** PANDA task class for finding tracks based on the HoughTrackFinder
 ** Task level RECO
 **/

#ifndef PndHoughTrackFinderTask_H_
#define PndHoughTrackFinderTask_H_

#include "FairTask.h"
#include "PndGeoSttPar.h"
#include <vector>
#include "PndHoughTrackFinder.h"

class PndHoughTrackFinderTask : public FairTask {
 public:

  /** Constructor **/
  PndHoughTrackFinderTask();

  /** Destructor **/
  virtual ~PndHoughTrackFinderTask();

  /** Virtual method Init **/
  virtual void SetParContainers();

  /** @brief Initializes and loads the data for the HoughTrackFinder */
  virtual InitStatus Init();

  virtual void AddBranchName(TString name)
  {
    fBranchNames.push_back(name);
  } ///< Search for tracks only in given branches. If no BranchName is given all tracking detectors are taken
  virtual void AddBranchName(TString name, TString mcName)
  {
    fBranchNames.push_back(name);
  } ///< Search for tracks only in given branches. If no BranchName is given all tracking detectors are taken

  /** @brief Sets the number of bins used for the x direction of the Hough space. */
  virtual void SetBinningX(double bin) { fNbins1 = bin; };
  /** @brief Sets the number of bins used for the y direction of the Hough space. */
  virtual void SetBinningY(double bin) { fNbins2 = bin; };
  /** @brief Sets the number of bins used for the segmentation preselection algorithm. */
  virtual void SetNBinsSeg(Int_t n) { fNBinsSeg = n; };
  /** @brief Sets the cut value for the distance of two maxima in the Hough space which have to be merged. */
  virtual void SetCutMergeByHoughSpace(float cut) { fCutMergeByHoughSpace = cut; }
  /** @brief Sets a bool for deciding to use cuda. */
  virtual void SetWithCuda(bool cuda) { fWithCuda = cuda; }
  /** @brief Sets a bool for deciding to use cuda. */
  virtual void SetPreselectedTrackCandName(TString name) { fPreselectedTrackCandName = name; }

  /** @brief Sets a bool for deciding to use a ghost reduction.
   * Per default a ghost reduction is performed.
   * Without Ghost reduction: track finding rate for possible primaries ~ 85 %, ghost ratio ~ 10 %
   * With Ghost reduction:    track finding rate for possible primaries ~ 81 %, ghost ratio ~  4 %
   */
  virtual void SetWithGhostReduction(bool ghostred) { fWithGhostReduction = ghostred; }

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  bool CheckZInfo(PndTrackCand &cand);

  virtual void Finish();

 private:
  FairRootManager *ioman = nullptr;

  float fCutMergeByHoughSpace;
  Int_t fNbins1;
  Int_t fNbins2;
  Int_t fNBinsSeg;
  bool fWithCuda;
  bool fWithGhostReduction;
  TString fPreselectedTrackCandName;
  std::vector<TString> fBranchNames;
  std::map<TString, TClonesArray *> fBranchMap;

  PndGeoSttPar *fSttParameters = nullptr; // needed for SttStrawMap
  //TClonesArray *fSttTubeArray = nullptr;

  PndHoughTrackFinder *fPndHoughTrackFinder = nullptr;

  /** Output array of PndTrackCands **/
  TClonesArray *fPreselectedTrackArrayCand = nullptr;
  TClonesArray *fApolloniusTrackArrayMerged = nullptr;
  TClonesArray *fApolloniusTrackArrayMergedCand = nullptr;
  TClonesArray *fCombinedSkewed = nullptr;

  ClassDef(PndHoughTrackFinderTask, 1);
};

#endif /*PndHoughTrackFinderTask_H_*/
