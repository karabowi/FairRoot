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
 *@class PndFtsTrackFinderTask
 *@brief PandaRoot task providing PndFtsTrackFinder functionality.
 *@author Bartosz Sobol
 *@date: 18.11.2021
 */

#pragma once

#include "TClonesArray.h"

#include "FairTask.h"

#include "PndTrackCand.h"
#include "PndFtsContext.h"
#include "PndFtsReconRunner.h"
#include "FairTask.h"
#include "FairRootManager.h"
#include "PndPersistencyTask.h"

#include "PndFtsTrackFinderPndTrackProducer.h"

#include "tool/PndFtsSimpleHit.h"

class PndFtsTrackFinderTask : public PndPersistencyTask {
 public:
  /**
   * Primary and only constructor.
   * @param name Task name.
   */
  explicit PndFtsTrackFinderTask(const TString &name = "PndFtsTrackFinder");

  PndFtsTrackFinderTask(const PndFtsTrackFinderTask &) = delete;

  PndFtsTrackFinderTask(const PndFtsTrackFinderTask &&) = delete;

  PndFtsTrackFinderTask operator=(const PndFtsTrackFinderTask &) = delete;

  PndFtsTrackFinderTask operator=(const PndFtsTrackFinderTask &&) = delete;

  ~PndFtsTrackFinderTask() override;

  /**
   * Main execution method, called once for each event.
   * @param option Not used.
   */
  void Exec(Option_t *option) override;

  /**
   * Sets the name of the output branch containing generated PndTracks.
   * If never invoked, the default "FtsTrack" name is used.
   * Name of the branch containig PndTrackCands will be set to `name + "Cand"`
   * @param name Name of the branch with PndTracks in output ROOT file.
   */
  void SetOutputBranchName(const TString &name);

  /**
   * Sets moemntum estimation method.
   * If never invoked, the default "TANGENT" method is used.
   * @param momEstMethod Momentum estimatrion method.
   */
  void SetMomentumEstimationMethod(PndFtsTrackFinder::PndFtsMomEstMethod momEstMethod);

 protected:
  /**
   * Initialisation of the task job in FairRoot and PandaRoot environment. Called once.
   * @return Initialization status code.
   */
  InitStatus Init() override;

  /**
   * Method called once after all events are processed.
   */
  void Finish() override;

 private:
  void PrintTmpTracks() const;

  [[nodiscard]] static PndFtsTrackFinder::PndFtsGeom ReadRootGeom(std::string_view filename);

  FairRootManager *ioman; //!< Pointer to FairRootManager singleton.

  int32_t fInBranchId;                                 //!< ID of the ROOT branch containing Hits to process. Also a detector Id for PndTrackCand.
  TString fInBranchName;                               //!< Name of the ROOT branch containing Hits to process.
  TString fOutBranchName;                              //!< Base name of the ROOT branch for found FTS tracks.
  TString fOutCandBranchName;                          //!< Base name of the ROOT branch for found FTS track cands.
  TString fOutAnalyticBranchName;                      //!< Base name of the ROOT branch for found FTS analytic tracks.
  PndFtsTrackFinder::PndFtsMomEstMethod fMomEstMethod; //!< Method to be used for momentum estimation.

  TClonesArray *fInHits; //!< Pointer to ROOT branch containing input hits.

  TClonesArray fOutTrackCands;     //!< Output destination (ROOT branch) for found FTS track candidates.
  TClonesArray fOutTracks;         //!< Output destination (ROOT branch) for found FTS tracks.
  TClonesArray fOutAnalyticTracks; //!< Output destination (ROOT branch) for found FTS  analytic tracks.

  std::vector<PndFtsTrackFinder::PndFtsSimpleHit> fTmpHits;   //!< Temporarily stores simplified hits (with minimal data needed by track recon algorithm).
  std::vector<PndFtsTrackFinder::PndFtsFullTrack> fTmpTracks; //!< Temporarily stores reconstructed tracks in algorithms' inner format

  // Those are unique_ptr for the sake of lazy initialization of the task ( Init() method ).
  std::unique_ptr<PndFtsTrackFinder::PndFtsContext> fReconContext;      //!< Context object for track reconstruction (contains constants, options, etc.)
  std::unique_ptr<PndFtsTrackFinder::PndFtsReconRunner> fReconRunner;   //!< Track reconstruction algorithm frontend class object.
  std::unique_ptr<PndFtsTrackFinderPndTrackProducer> fPndTrackProducer; //!< Converts PndFtsTrackFinder::FullTrack to PndTrack and PndTrckCand.

  ClassDef(PndFtsTrackFinderTask, 1);
};
