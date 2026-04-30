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

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Kalman Filter Task for multiple particle hypothesis
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Stefano Spatarot, UNI Torino
//
//-----------------------------------------------------------

#ifndef PNDRECOMULTIKALMANTASK_HH
#define PNDRECOMULTIKALMANTASK_HH

// Base Class Headers ----------------
#include "PndPersistencyTask.h"

// Collaborating Class Headers -------
#include "TString.h"
#include "PndRecoKalmanFit.h"
#include "PndGeoSttPar.h"
#include "PndGeoFtsPar.h"

// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;

class PndRecoMultiKalmanTask : public PndPersistencyTask {
 public:
  // Constructors/Destructors ---------
  PndRecoMultiKalmanTask(const char *name = "Genfit", Int_t iVerbose = 0, TString fithypo = "electron;muon;pion;kaon;proton");
  ~PndRecoMultiKalmanTask();

  // Operators

  // Accessors -----------------------

  // Modifiers -----------------------
  void SetTrackInBranchName(const TString &name) { fTrackInBranchName = name; }
  void SetTrackOutBranchName(const TString &name) { fTrackOutBranchName = name; }
  void SetMvdBranchName(const TString &name) { fMvdBranchName = name; }
  void SetCentralTrackerBranchName(const TString &name) { fCentralTrackerBranchName = name; }
  void SetGeane(Bool_t opt = kTRUE) { fUseGeane = opt; }
  void SetNumIterations(Int_t num) { fNumIt = num; }
  void SetFitHypotheses(const TString &name) { fFitWithHypo = name; }
  void SetPropagateToIP(Bool_t opt = kTRUE) { fPropagateToIP = opt; }
  void SetBusyCut(Int_t b) { fBusyCut = b; }
  void SetTrackRep(Short_t num) { fTrackRep = num; }

  // Operations ----------------------
  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);

  void SetParContainers();

 private:
  // Private Data Members ------------
  TClonesArray *fTrackArray;        //! Input TCA from pattern recognition
  TClonesArray *fFitTrackArrays[5]; //! Output TCA for track

  TString fTrackInBranchName;  //! Name of the input TCA
  TString fTrackOutBranchName; //! Name of the output TCA

  TString fMvdBranchName;            //! Name of the TCA for MVD
  TString fCentralTrackerBranchName; //! Name of the TCA for central tracker

  TString fFitWithHypo;

  PndRecoKalmanFit *fFitter;

  Bool_t fUseGeane;             //! Flag to use Geane
  Bool_t fIdealHyp;             //! Flag to use MC particle hypothesis
  Bool_t fPropagateToIP;        //! Flag to propagate the parameters to the interaction point (kTRUE)
  Float_t fPropagateDistance;   //! Distance in [cm] to back-propagate the parameters, negative number means no backpropagation
  Bool_t fPerpPlane;            //! Flag to use as initial plane the one perpendicular to the track (kFALSE)
  Short_t fTrackRep;            //! (0) GeaneTrackRep, 1 RKTrackRep
  Int_t fNumIt;                 //! Number of iterations
  Int_t fBusyCut;               //! Skip too busy events with more tracks
  Bool_t fSmoothing;            //! Flag to set on smoothing
  Bool_t fHypoFlag[5];          //! Flag to check which hypotheses to fit with
  int fPDGs[5];                 //! PDG numbers
  PndGeoSttPar *fSttParameters; //! STT params
  PndGeoFtsPar *fFtsParameters; //! STT params

  TDatabasePDG *pdg; //! Particle DB

  ClassDef(PndRecoMultiKalmanTask, 1);
};

#endif
