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

// -----------------------------------------
// PndDrcLutInfo.h
//
// Created on: 13.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------
// Container for look-up table

#ifndef PNDDRCLUTINFO_H
#define PNDDRCLUTINFO_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>

class PndDrcLutInfo : public TObject {

 public:
  // Default constructor
  PndDrcLutInfo();

  // Standard constructors

  // Copy constructor
  // PndDrcLutInfo (PndDrcLutInfo& node) { *this = node; }  // not needed

  // Modifiers
  void AddAngle(Double_t val);
  void AddPixelEnd(Double_t val);
  void AddTime(Double_t val);
  void AddTime(Double_t val, Bool_t ref);
  void AddLutTime(Double_t val, Bool_t ref);
  void AddPath(Double_t val);
  void AddChDiff(Double_t val);
  void AddNOfBounces(Double_t val);
  void AddNOfEVReflections(Int_t val);
  void AddLambda(Double_t val);
  void AddTruePath(Double_t val);
  void AddHitTime(Double_t val);

  void SetChPartDir(TVector3 val);
  void SetChPartDirInBar(TVector3 val);
  void SetChPartDirInBar2(TVector3 val);
  void SetChPartPdg(Int_t val);
  void SetCherenkovMC(Double_t val);
  void SetCherenkovReal(Double_t val);

  // Accessors
  Int_t AngleEntries() { return fAnglesSize; }
  Int_t PixelEntries() { return fPixelSize; }
  Int_t TimeEntries() { return fTimesSize; }
  Int_t PathEntries() { return fPathsSize; }
  Int_t ChDiffEntries() { return fChDiffsSize; }
  Int_t NOfBouncesEntries() { return fNOfBouncesSize; }
  Int_t NOfEVReflectionEntries() { return fNOfEVReflectionsSize; }
  Int_t NOfLambdas() { return fLambdasSize; }
  Int_t NOfTruePaths() { return fTruePathsSize; }
  Int_t NOfHitTimes() { return fHitTimesSize; }

  TVector3 GetChPartDir() { return fChPartDir; }
  TVector3 GetChPartDirInBar() { return fChPartDirInBar; }
  TVector3 GetChPartDirInBar2() { return fChPartDirInBar2; }
  Int_t GetChPartPdg() { return fChPartPdg; }
  Double_t GetCherenkovMC() { return fCherenkovMC; }
  Double_t GetCherenkovReal() { return fCherenkovReal; }

  Double_t GetAngle(Int_t entry);
  Int_t GetPixelEnd(Int_t entry);
  Double_t GetChDiff(Int_t entry);
  Int_t GetNOfBounces(Int_t entry);
  Int_t GetNOfEVReflections(Int_t entry);
  Double_t GetTime(Int_t entry);
  Double_t GetLutTime(Int_t entry) { return fLutTimesArray[entry]; }
  Double_t GetLambda(Int_t entry) { return fLambdaArray[entry]; }
  Double_t GetHitTime(Int_t entry) { return fHitTimeArray[entry]; }
  Double_t GetTruePath(Int_t entry) { return fTruePathArray[entry]; }
  Double_t GetPath(Int_t entry) { return fPathsArray[entry]; }
  Bool_t GetReflected(Int_t entry) { return fReflected[entry]; }
  Bool_t GetLutReflected(Int_t entry) { return fLutReflected[entry]; }

 protected:
  Int_t fAnglesSize;
  Int_t fPixelSize;
  Int_t fTimesSize;
  Int_t fLutTimesSize;
  Int_t fPathsSize;
  Int_t fChDiffsSize;
  Int_t fNOfBouncesSize;
  Int_t fNOfEVReflectionsSize;
  Int_t fLambdasSize;
  Int_t fHitTimesSize;
  Int_t fTruePathsSize;

  // arrays of values based on the LUT ambiguities:
  std::vector<Double_t> fAnglesArray;

  std::vector<Int_t> fPixelEndArray;
  std::vector<Double_t> fTimesArray;
  std::vector<Double_t> fLutTimesArray;
  std::vector<Double_t> fPathsArray;
  std::vector<Double_t> fChDiffsArray;
  std::vector<Int_t> fNOfBouncesArray;
  std::vector<TVector3> fPhotonStartPosition;
  std::vector<Int_t> fNOfEVReflectionsArray;
  std::vector<Double_t> fLambdaArray;
  std::vector<Double_t> fHitTimeArray;
  std::vector<Double_t> fTruePathArray;
  std::vector<Bool_t> fReflected;
  std::vector<Bool_t> fLutReflected;

  // MC information
  TVector3 fChPartDir;
  TVector3 fChPartDirInBar;
  TVector3 fChPartDirInBar2;
  Int_t fChPartPdg;
  Double_t fCherenkovMC;
  Double_t fCherenkovReal;

  ClassDef(PndDrcLutInfo, 2)
};

#endif
