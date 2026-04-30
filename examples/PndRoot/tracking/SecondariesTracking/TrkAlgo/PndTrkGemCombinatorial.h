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

#ifndef PNDTRKGEMCOMBITASK_H
#define PNDTRKGEMCOMBITASK_H

// fairroot
#include "FairTask.h"

// ROOT
#include "TH2F.h"
#include "TCanvas.h"

class TClonesArray;

class PndTrkGemCombinatorial : public FairTask {

 public:
  /** Default constructor **/
  PndTrkGemCombinatorial();
  PndTrkGemCombinatorial(TClonesArray *gemhitarray, int verbose);

  /** Destructor **/
  ~PndTrkGemCombinatorial();
  std::map<int, bool> CombinatorialSuppression();

  void SwitchOnDisplay();
  void DrawGeometry();
  void DrawMCPoints();

  void SetMCPointTCA(TClonesArray *gempointarray) { fGemPointArray = gempointarray; }
  inline void Evaluate() { fMCEval = kTRUE; }

 private:
  TClonesArray *fGemHitArray;
  TClonesArray *fGemPointArray;

  // display
  Bool_t fDisplayOn, fMCEval;
  Int_t fVerbose;
  TH2F *hxy, *hxy1, *hxy2, *hxy3;
  TCanvas *display;

  ClassDef(PndTrkGemCombinatorial, 1);
};

#endif
