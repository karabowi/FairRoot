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
//      GFKalman Filter Task
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PNDHYPDKALMANTASK_HH
#define PNDHYPDKALMANTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;
class TH1D;
class TH2D;
class TGeoManager;
class FairField;

class PndHypDKalmanTask : public FairTask {
 public:
  // Constructors/Destructors ---------
  PndHypDKalmanTask();
  ~PndHypDKalmanTask();

  // Operators

  // Accessors -----------------------

  // Modifiers -----------------------
  void SetTrackBranchName(const TString &name) { fTrackBranchName = name; }
  void AddHitBranch(unsigned int detId, const TString &m) { fHitBranchMap[detId] = m; };
  void SetPersistence(Bool_t opt = kTRUE) { fPersistence = opt; }
  void SetSmooth(Bool_t opt = kTRUE) { fSmooth = opt; }
  void UseMVD(bool mvd) { fUseMVD = mvd; }

  // Operations ----------------------

  virtual InitStatus Init();

  virtual void Exec(Option_t *opt);

  void WriteHistograms(const TString &filename);

 private:
  // Private Data Members ------------
  TString fTrackBranchName;
  std::map<unsigned int, TString> fHitBranchMap;
  TClonesArray *fTrackArray;

  Bool_t fPersistence;

  GFRecoHitFactory *fTheRecoHitFactory;
  FairField *fField;

  TH1D *fPH;    // momentum histo;
  TH1D *fChi2H; // chi2 histo;
  TH1D *fXresH;
  TH1D *fYresH;
  TH1D *fXresFitH;
  TH1D *fYresFitH;
  TH1D *fPEnd;
  TH1D *fPull;

  int fTrackcount;
  Bool_t fSmooth;
  int fEvt; // event counter
  bool fUseMVD;

  // Private Methods -----------------

 public:
  ClassDef(PndHypDKalmanTask, 4)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
