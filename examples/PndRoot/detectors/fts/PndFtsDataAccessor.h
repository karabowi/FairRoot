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

// *************************************************************************
// Author: Martin Galuska
// martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
// Data Accessor for Ivan Kisel's group
//
// Created: 27.11.2013
// Modified: 15.01.2014
//
// *************************************************************************

#ifndef FTSDATAACCESSH
#define FTSDATAACCESSH

//#include "TObject.h"
//#include "TObjArray.h"
//#include "TArrayI.h"
//#include "TBenchmark.h"
#include "TVector3.h"
#include "TDatabasePDG.h"
#include "TClonesArray.h"
//#include "Riostream.h"
#include "FairTask.h"
#include "PndGeoFtsPar.h"
#include <cmath>

class FairField;

class PndFtsDataAccessor : public FairTask {

 public:
  PndFtsDataAccessor();
  virtual ~PndFtsDataAccessor();

  virtual void Exec(Option_t *option);
  virtual InitStatus Init();
  virtual void Finish(){};

  void Reset(){};
  void Register();

  void SetParContainers();

  void SetFtsActivity(Bool_t act = kTRUE) { fBranchActive[0] = act; }
  void SetGemActivity(Bool_t act = kTRUE) { fBranchActive[1] = act; }
  void SetMvdActivity(Bool_t act = kTRUE)
  {
    fBranchActive[2] = act;
    fBranchActive[3] = act;
  }

  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

 protected:
  TClonesArray *fMCTracks;    //! Array of PndMCTrack
  TClonesArray *fMCPoints[4]; //! Array of event's points
  TClonesArray *fHits[4];     //! Array of event's hits
  Int_t fBranchIDs[4];        //! Array of Branch IDs
  Bool_t fBranchActive[4];    //! Array of Branch Activeness

  // for B field access
  FairField *fField;
  Double_t By;
  Double_t po[3], BB[3];

  Bool_t fPersistence; // Persistence of tracking TCAs

  TDatabasePDG *pdg; //! Particle DB

  PndGeoFtsPar *fFtsParameters;
  TClonesArray *fTubeArrayFts;

  PndFtsDataAccessor(const PndFtsDataAccessor &L);
  PndFtsDataAccessor &operator=(const PndFtsDataAccessor &) { return *this; }

  ClassDef(PndFtsDataAccessor, 1);
};
#endif
