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
//      An ideal pattern recognition
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      adapted by A. Sanchez for HYP purpose
//
//-----------------------------------------------------------

#ifndef PNDHYPDPATTERNRECOTASK_HH
#define PNDHYPDPATTERNRECOTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include <map>

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;
class AbsBFieldIfc;
class FairField;
class FairGeanePro;

class PndHypDPatternRecoTask : public FairTask {
 public:
  // Constructors/Destructors ---------
  PndHypDPatternRecoTask();
  ~PndHypDPatternRecoTask();

  // Operators

  // Accessors -----------------------

  // Modifiers -----------------------
  void SetVtxAbsName(TString name)
  {
    fVtx = true;
    fVtxName = name;
  }
  void AddHitBranch(unsigned int detId, const TString &m) { fHitBranchNameMap[detId] = m; };
  void SetPersistence(Bool_t opt = kTRUE) { fPersistence = opt; }
  void SetField(FairField *f) { fField = f; }
  void SetHitFL(Bool_t opt) { fMCvalue = opt; }
  void UseGeane(bool f = true) { fUseGeane = f; }
  void UseMVD(bool mvd) { fUseMVD = mvd; }

  // Operations ----------------------

  virtual InitStatus Init();

  virtual void Exec(Option_t *opt);

 private:
  // Private Data Members ------------
  // TString _pointBranchName;
  TClonesArray *fPointArray;
  TClonesArray *fSdsArray;
  TClonesArray *fTrackArray;
  TClonesArray *fHitArray;
  TClonesArray *fMcArray;

  // TGeoManager* fGeom;

  std::map<unsigned int, TString> fHitBranchNameMap;
  std::map<unsigned int, TClonesArray *> fHitBranchMap;
  int fEventNr;

  TString fVtxName;
  Bool_t fPersistence;
  Bool_t fUseGeane;
  bool fUseMVD;
  Bool_t fMCvalue;
  Bool_t fVtx;

  GFRecoHitFactory *fTheRecoHitFactory;

  FairField *fField;
  FairGeanePro *fGeanePro;

  // Private Methods -----------------

  Int_t GetChargeIon(Int_t ion);

 public:
  ClassDef(PndHypDPatternRecoTask, 4)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
