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
 * @class PndGFRecoTask
 * @brief Task to perform kalman tracking via genfit2
 *
 * @date 19.04.2022
 * @author Tobias Stockmanns
 */

#pragma once

#include "PndPersistencyTask.h"
#include "PndTrack.h"
#include "PndGFRecoFit.h"
#include "FairLogger.h"

#include "TString.h"
#include "TClonesArray.h"
#include "TMath.h"

#include <vector>
//#include "eventDisplay/include/EventDisplay.h"

class TClonesArray;
class MeasurementFactory;
class TDatabasePDG;

struct PndGFPidHypo {

  PndGFPidHypo(){};
  PndGFPidHypo(int pdgvalue)
  {
    switch (TMath::Abs(pdgvalue)) {
    case 11:
      fPdgNumber = -11;
      fParticleName = "Electron";
      break;
    case 13:
      fPdgNumber = -13;
      fParticleName = "Muon";
      break;
    case 211:
      fPdgNumber = 211;
      fParticleName = "Pion";
      break;
    case 321:
      fPdgNumber = 321;
      fParticleName = "Kaon";
      break;
    case 2212:
      fPdgNumber = 2212;
      fParticleName = "Proton";
      break;
    default: LOG(error) << "Wrong particle ID given: " << pdgvalue; LOG(error) << "Allowed values are: 11, 13, 211, 321, 2212";
    }
  };
  int fPdgNumber = 0;
  TString fParticleName;
  TClonesArray *fArray = nullptr;
};

class PndGFRecoTask : public PndPersistencyTask {
 public:
  // Constructors/Destructors ---------
  PndGFRecoTask(const char *name = "Genfit", Int_t iVerbose = 0);
  ~PndGFRecoTask();

  /**
   * @brief Add a particle hypothesis used for fitting. If used more then once all hypothesis are used for one track (multiKalman).
   * @par pdgCode code of particle hypothesis according to PDG definition. Sign is not important.
   */

  void AddParticleHypo(int pdgCode);

  /**
   * @brief Adds a predefined set of particles as tracking hypothesis (e, mu, pi, K, p)
   */

  void SetMultiPID();

  /**
   * @brief Flag to use ideal PID from MC tracks instead of particle hypothesis
   * @par val switches ideal PID on or off
   * @par defaultPID default particle hypothesis used if no proper ideal value can be determined
   */
  void UseIdealPID(bool val = true, Int_t defaultPID = 211)
  {
    fIdealPID = val;
    fDefaultPID = defaultPID;
  }

  /**
   * @brief Set the name of the input branch
   * @detailed Set the name of the input branch with the prefit PndTracks
   */
  void SetTrackInBranchName(TString name) { fInputBranchName = name; }

  /**
   * @brief Name of the output branch containing the fitted PndTracks
   */
  void SetTrackOutBranchName(TString name) { fOutputBranchName = name; }

  /**
   * @brief Set fitter used for fitting. Fitter passed to PndGFRecoFit.
   */
  void SetFitter(genfit::AbsKalmanFitter *fitter);

  /**
   * @brief Cut to suppress events with too many tracks
   */
  void SetNBusyCut(int val) { fNBusyCut = val; }

  void UseEventDisplay(bool val = true) { fUseEventDisplay = val; }

  // Operations ----------------------
  virtual InitStatus Init();
  void SetParContainers();
  virtual void Exec(Option_t *opt);
  virtual void FinishTask() {}

 protected:
  int GetIdealPdgCode(PndTrack *track);
  void DeleteOldTracks()
  {
    for (auto &track : fPidHypothesis) {
      track.fArray->Delete();
    }
  }

 private:
  TClonesArray *fPrefitTrack = nullptr;     //< Input from track finder
  TClonesArray *fMCTrack = nullptr;         //< MC tracks needed for ideal PID
  std::vector<PndGFPidHypo> fPidHypothesis; //< Pid Hypothesis to fit with. Can be more than one.
  TString fInputBranchName = "";            //< Name of branch with prefit track data
  TString fOutputBranchName = "";           //< Name of output branch with fitted track data
  bool fIdealPID = false;                   //< Flag to use ideal PID instead of PID hypothesis
  bool fMultiPID = true;                    //< Flag
  int fDefaultPID = 211;                    //< PID assumption if no PID hypothesis is given
  TDatabasePDG *fPdg = nullptr;             //< Needed by GetIdealPdgCode
  int fNBusyCut = 50;                       //< Events with tracks above NBusyCut are ignored
  bool fUseEventDisplay = false;            //< Enables genfit2 event display for PndGFRecoFit
  PndGFRecoFit fFit;

  ClassDef(PndGFRecoTask, 2);
};
