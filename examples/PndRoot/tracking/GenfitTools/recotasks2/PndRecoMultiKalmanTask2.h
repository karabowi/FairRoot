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
//      modified by Elisabetta Prencipe, 19/05/2014
//-----------------------------------------------------------

#ifndef PndRecoMultiKalmanTask2_HH
#define PndRecoMultiKalmanTask2_HH

// Base Class Headers ----------------
#include "PndPersistencyTask.h"

// Collaborating Class Headers -------
#include "TString.h"
#include "PndRecoKalmanFit2.h"
#include "PndGeoSttPar.h"
#include "PndGeoFtsPar.h"

// Collaborating Class Declarations --
class TClonesArray;
class MeasurementFactory;

class PndRecoMultiKalmanTask2 : public PndPersistencyTask {
 public:
  // Constructors/Destructors ---------
  PndRecoMultiKalmanTask2(const char *name = "Genfit", Int_t iVerbose = 0, TString fithypo = "electron;muon;pion;kaon;proton");
  ~PndRecoMultiKalmanTask2();

  // Operators

  // Accessors -----------------------

  // Modifiers -----------------------
  void SetTrackInBranchName(const TString &name) { fTrackInBranchName = name; }
  void SetTrackOutBranchName(const TString &name) { fTrackOutBranchName = name; }
  void SetMvdBranchName(const TString &name) { fMvdBranchName = name; }
  void SetCentralTrackerBranchName(const TString &name) { fCentralTrackerBranchName = name; }
  void SetNumIterations(Int_t num) { fNumIt = num; }
  void SetFitHypotheses(const TString &name) { fFitWithHypo = name; }

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
  Bool_t fHypoFlag[5]; //! Flag to check which hypotheses to fit with
  int fPDGs[5];        //! PDG numbers

  PndRecoKalmanFit2 *fFitter;

  Bool_t fUseGeane;             //! Flag to use Geane
  Bool_t fIdealHyp;             //! Flag to use MC particle hypothesis
  Bool_t fSmoothing;            //! Flag to set on smoothing
  Int_t fNumIt;                 //! Number of iterations
  Int_t fBusyCut;               //! Skip too busy events with more tracks
  PndGeoSttPar *fSttParameters; //! STT params
  PndGeoFtsPar *fFtsParameters; //! FTS params

  ClassDef(PndRecoMultiKalmanTask2, 1);
};

#endif
