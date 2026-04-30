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
 * @class PndMCCloner
 * @brief Task to copy used MCTracks from the sim file to the pid file
 *
 * @details
 * This task copies the MC tracks from the simulation root file
 * into the pid file.
 * If the option `SetCleanMc()` is set to true only those MC tracks
 * which are seen in the reconstruction (or are mother particles
 * of seen particles) are copied and the MC truth index is corrected.
 *
 * @author Stefano Spataro
 */

#include "FairTask.h"
#include "PndMCTrack.h"
#include <map>

using namespace std;

class TClonesArray;

class PndMcCloner : public FairTask {

 public:
  /** Default constructor **/
  PndMcCloner();

  /** Destructor **/
  ~PndMcCloner();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *);

  void SetCleanMc(Bool_t opt = kTRUE) { fCleanMC = opt; }

 protected:
  void FindUsedMCIndices();
  void CloneMCTrack();
  void CloneAndCleanMCTrack();
  void CorrectMotherIndices();
  void CorrectPidIndices();

  /** Input array of PndMCTrack **/
  TClonesArray *fInputArray = nullptr;

  /** Input array of PidChargedCand **/
  TClonesArray *fPidChargedArray = nullptr;

  /** Input array of PidNeutralCand **/
  TClonesArray *fPidNeutralArray = nullptr;

  /** Output array of PndMCTrack **/
  TClonesArray *fOutputArray = nullptr;

  map<Int_t, Int_t> mapMCIndex; // Map <old mc index, new mc index>

  Bool_t fCleanMC = false; // Flag to clean the MCTrack from unused indices

  ClassDef(PndMcCloner, 2);
};

