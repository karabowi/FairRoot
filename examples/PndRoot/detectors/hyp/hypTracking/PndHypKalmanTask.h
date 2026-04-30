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
//      Ralf Kliemt, TU Dresden             (Copied for MVD use)
//
//-----------------------------------------------------------

#ifndef PNDHYPKALMANTASK_HH
#define PNDHYPKALMANTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include "FairGeanePro.h"

// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;
class TH1D;
class TGeoManager;
class FairField;

class PndHypKalmanTask : public FairTask {
 public:
  // Constructors/Destructors ---------
  PndHypKalmanTask();
  ~PndHypKalmanTask();

  // Operators

  // Accessors -----------------------

  // Modifiers -----------------------
  void SetTrackBranchName(const TString &name) { fTrackBranchName = name; }
  void AddHitBranch(unsigned int detId, const TString &m) { fHitBranchMap[detId] = m; };
  void SetPersistence(Bool_t opt = kTRUE) { fPersistence = opt; }

  // Operations ----------------------

  virtual InitStatus Init();

  virtual void Exec(Option_t *opt);

  void WriteHistograms(const TString &filename);

 private:
  // Private Data Members ------------
  TClonesArray *fTrackArray;
  TClonesArray *fTrArray;
  TString fTrackBranchName;
  std::map<unsigned int, TString> fHitBranchMap;
  Bool_t fPersistence;

  GFRecoHitFactory *fTheRecoHitFactory;
  FairField *fField;

  TH1D *fPH;       // momentum histo;
  TH1D *fChi2H;    // chi2 histo;
  TH1D *fMassV0;   // chi2 histo;
  TH1D *fMassETAC; // chi2 histo;
  TH1D *fMasses;   // all particle histo;

  Int_t fTrackcount;
  Int_t fEvent;
  TGeant3 *gMC3;
  FairGeanePro *fPro;
  // Private Methods -----------------

 public:
  ClassDef(PndHypKalmanTask, 2);
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
