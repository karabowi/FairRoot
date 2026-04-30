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
//      Kalman Filter Task
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Stefano Spataro, UNI Torino
//      modified by Elisabetta Prencipe 19/05/2014
//-----------------------------------------------------------

#ifndef PNDRECOKALMANTASK2_HH
#define PNDRECOKALMANTASK2_HH

// Base Class Headers ----------------
#include "PndPersistencyTask.h"

// Collaborating Class Headers -------
#include "TString.h"
#include "PndRecoKalmanFit2.h"
#include "PndRecoDafFit2.h"

// Collaborating Class Declarations --
class TClonesArray;
class MeasurementFactory;

class PndRecoKalmanTask2 : public PndPersistencyTask {
 public:
  // Constructors/Destructors ---------
  PndRecoKalmanTask2(const char *name = "Genfit", Int_t iVerbose = 0);
  ~PndRecoKalmanTask2();

  // Operators

  // Accessors -----------------------

  // Modifiers -----------------------
  void SetTrackInBranchName(const TString &name) { fTrackInBranchName = name; }
  //  void SetTrackInIDBranchName(const TString& name) { fTrackInIDBranchName = name;}
  void SetTrackOutBranchName(const TString &name) { fTrackOutBranchName = name; }
  void SetMvdBranchName(const TString &name) { fMvdBranchName = name; }
  void SetCentralTrackerBranchName(const TString &name) { fCentralTrackerBranchName = name; }
  void SetGeane(Bool_t opt = kTRUE) { fUseGeane = opt; }
  void SetIdealHyp(Bool_t opt = kTRUE) { fIdealHyp = opt; }
  void SetDaf(Bool_t opt = kTRUE) { fDaf = opt; }
  void SetPropagateToIP(Bool_t opt = kTRUE) { fPropagateToIP = opt; }
  void SetPropagateDistance(Float_t opt = -1.) { fPropagateDistance = opt; }
  void SetPerpPlane(Bool_t opt = kTRUE) { fPerpPlane = opt; }
  void SetNumIterations(Int_t num) { fNumIt = num; }
  void SetParticleHypo(TString s);
  void SetParticleHypo(Int_t h);
  void SetBusyCut(Int_t b) { fBusyCut = b; }

  // Operations ----------------------
  virtual InitStatus Init();
  void SetParContainers();
  virtual void Exec(Option_t *opt);

 protected:
  // Private Data Members ------------
  TClonesArray *fTrackArray; //! Input TCA for PndTrack
  //  TClonesArray* fTrackIDArray;    //! Input TCA for PndTrackID
  TClonesArray *fMCTrackArray;  //! Input TCA for PndMCTrack
  TClonesArray *fFitTrackArray; //! Output TCA for track

  TString fTrackInBranchName; //! Name of the input TCA
  //  TString fTrackInIDBranchName;    //! Name of the input TCA
  TString fTrackOutBranchName; //! Name of the output TCA

  TString fMvdBranchName;            //! Name of the TCA for MVD
  TString fCentralTrackerBranchName; //! Name of the TCA for central tracker

  PndRecoKalmanFit2 *fFitter; //! Standard Kalman Filter class
  PndRecoDafFit2 *fDafFitter; //! Deterministic Annealing class
  TDatabasePDG *pdg;          //! Particle DB

  Bool_t fUseGeane;           //! Flag to use Geane
  Bool_t fSmoothing;          //! Flag to set on smoothing (not used)
  Bool_t fIdealHyp;           //! Flag to use MC particle hypothesis
  Bool_t fDaf;                //! Flag to use Deterministic Annealing
  Bool_t fPropagateToIP;      //! Flag to propagate the parameters to the interaction point (kTRUE)
  Float_t fPropagateDistance; //! Distance in [cm] to back-propagate the parameters, negative number means no backpropagation
  Bool_t fPerpPlane;          //! Flag to use as initial plane the one perpendicular to the track (kFALSE)
  Int_t fNumIt;               //! Number of iterations
  Int_t fPDGHyp;              //! Hypothesis
  Int_t fBusyCut;             //! Skip too busy events with more tracks

  ClassDef(PndRecoKalmanTask2, 1);
};

#endif
