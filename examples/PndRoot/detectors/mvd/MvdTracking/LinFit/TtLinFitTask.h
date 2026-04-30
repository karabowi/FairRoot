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

// Simone Bianco
// 15.07.2010

#ifndef TTLINFITTASK_H
#define TTLINFITTASK_H

// Base Class Headers ----------------
#include "FairTask.h"
#include <TGraph2D.h>
#include <TGraph2DErrors.h>
#include "TH1F.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include <iostream>

using namespace std;

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;

class TtLinFitTask : public FairTask {

 public:
  // Constructors/Destructors ---------
  TtLinFitTask();
  // TtLinFitTask(const TtLinFitTask& o){};
  //  TtLinFitTask& operator=(const TtLinFitTask& o) { return *this;};
  virtual ~TtLinFitTask();

  virtual InitStatus Init();

  virtual void Exec(Option_t *opt);

 private:
  // Input Data------------
  TClonesArray *fTCandArray;
  TString fTCandBranchName;

  // Output Data----------
  TClonesArray *fTrackArray;

  // Bool_t firstIt;

  Int_t fTrackcount;

  Int_t fEvent;

  Double_t fEloss[6]; // energy loss in each sensor

  // Fitting ------------

  void MyFit(Double_t *x, Double_t *y, Double_t *z, Double_t *Erx, Double_t *Ery, Double_t *Erz, Double_t *par, Double_t &chiX, Double_t &chiY);

  ClassDef(TtLinFitTask, 1);
};

#endif
