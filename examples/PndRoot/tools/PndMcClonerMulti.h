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
 * @class PndMCClonerMulti
 * @brief Task to copy used MCTracks from the sim file to the pid file
 *
 * @details
 * This task copies the MC tracks from the simulation root file
 * into the pid file.
 * If the option `SetCleanMc()` is set to true only those MC tracks
 * which are seen in the reconstruction (or are mother particles
 * of seen particles) are copied and the MC truth index is corrected.
 * This class is identical to PndMCCloner and is used for pid files
 * with several pid particle options.
 *
 * @author Stefano Spataro
 */

#include "FairTask.h"
#include "PndMCTrack.h"
#include <map>

using namespace std;

class TClonesArray;

class PndMcClonerMulti : public FairTask {

 public:
  /** Default constructor **/
  PndMcClonerMulti();

  /** Destructor **/
  ~PndMcClonerMulti();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *);

  void SetCleanMc(Bool_t opt = kTRUE) { fCleanMC = opt; }
  void SetOutputBranch(TString branch) { fTrackBranchNamePidHypo = branch; }

 protected:
  void FindUsedMCIndices();
  void CloneMCTracks();
  void CloneAndCleanMCTrack();
  void CorrectMotherIndices();
  void CorrectPidIndices();
  void CloneMcTrack(Int_t mcidx = -1);
  void MarkMcTree(Int_t mcindex = -1);

  /** Input array of PndMCTrack **/
  TClonesArray *fInputArray;

  /** Input array of PidChargedCand **/
  TClonesArray *fPidChargedArray[5];

  /** Input array of PidNeutralCand **/
  TClonesArray *fPidNeutralArray;

  /** Output array of PndMCTrack **/
  TClonesArray *fOutputArray;

  map<Int_t, Int_t> fMmapMCIndex; // Map <old mc index, new mc index>

  Bool_t fCleanMC; // Flag to clean the MCTrack from unused indices

  TString fPidHypoStr[5];

  TString fTrackBranchNamePidHypo;

  ClassDef(PndMcClonerMulti, 2);
};

