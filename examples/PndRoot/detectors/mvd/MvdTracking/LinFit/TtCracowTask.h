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

#ifndef TTCRACOWTASK_H
#define TTCRACOWTASK_H

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include <iostream>

using namespace std;

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;

class TtCracowTask : public FairTask {

 public:
  // Constructors/Destructors ---------
  TtCracowTask();
  // TtCracowTask(const TtCracowTask& o){};
  //  TtCracowTask& operator=(const TtCracowTask& o) { return *this;};
  virtual ~TtCracowTask();
  TtCracowTask(const TtCracowTask &) = delete;
  TtCracowTask &operator=(const TtCracowTask &) = delete;

  virtual InitStatus Init();

  virtual void Exec(Option_t *opt);

  static void SumDistance2(int &, double *, double &sum, double *par, int); // for Fitter

  static double distance2(double x, double y, double z, double *p);

  static double distance2Single(double x, double y, double z, double ex, double ey, double ez, double *p);

  virtual void FinishEvent();

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

  ClassDef(TtCracowTask, 1);
};

#endif
