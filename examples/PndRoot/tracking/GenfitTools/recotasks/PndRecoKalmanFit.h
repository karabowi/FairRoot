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
//      Kalman Filter for single tracks
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Stefano Spataro, UNI Torino      //
//-----------------------------------------------------------

#ifndef PNDRECOKALMANFIT_HH
#define PNDRECOKALMANFIT_HH

// Base Class Headers ----------------
#include "TNamed.h"
#include "TString.h"

// Collaborating Class Headers ------
#include "FairGeanePro.h"
#include "PndTrack.h"
#include "GFKalman.h"

// Collaborating Class Declarations --
class GFRecoHitFactory;
class GFKalman;

class PndRecoKalmanFit : public TNamed {
 public:
  // Constructors/Destructors ---------
  PndRecoKalmanFit();
  ~PndRecoKalmanFit();

  // Modifiers -----------------------
  void SetGeane(Bool_t opt = kTRUE) { fUseGeane = opt; }
  void SetPropagateToIP(Bool_t opt = kTRUE) { fPropagateToIP = opt; }
  void SetPropagateDistance(Float_t opt = -1.f) { fPropagateDistance = opt; }
  void SetPerpPlane(Bool_t opt = kTRUE) { fPerpPlane = opt; }
  void SetNumIterations(Int_t num) { fNumIt = num; }
  void SetTrackRep(Int_t num) { fTrackRep = num; }
  void SetVerbose(Int_t verb) { fVerbose = verb; }
  void SetMvdBranchName(const TString &name) { fMvdBranchName = name; }
  void SetCentralTrackerBranchName(const TString &name) { fCentralTrackerBranchName = name; }
  // Operations ----------------------
  Bool_t Init();
  PndTrack *Fit(PndTrack *tBefore, Int_t PDG);

  GFRecoHitFactory *GetRecoHitFactory() { return fTheRecoHitFactory; };

 private:
  // Private Data Members ------------
  GFRecoHitFactory *fTheRecoHitFactory;
  GFKalman fGenFitter;

  FairGeanePro *fPro; //! Geane Propagator

  TString fMvdBranchName;            //! Name of the TCA for MVD
  TString fCentralTrackerBranchName; //! Name of the TCA for central tracker

  Bool_t fUseGeane;           //! Flag to use Geane
  Bool_t fPropagateToIP;      //! Flag to propagate to the interaction point
  Float_t fPropagateDistance; //! Distance in [cm] to back-propagate the parameters, negative number means no backpropagation
  Bool_t fPerpPlane;          //! Flag to use as initial plane the one perpendicular to the track
  Int_t fNumIt;               //! Number of iterations
  Short_t fTrackRep;          //! (0) GeaneTrackRep, 1 RKTrackRep
  Int_t fVerbose;             //! Verbose level

  ClassDef(PndRecoKalmanFit, 0);
};

#endif
