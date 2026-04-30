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
// PndDrcLutNodeH.h
//
// Created on: 09.07.2013
// Author: R.Dzhygadlo at gsi.de
// Author: M.Patsyuk at gsi.de
// -----------------------------------------
// Container for look-up table

#ifndef PNDDRCLUTNODEH_H
#define PNDDRCLUTNODEH_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>

class PndDrcLutNodeH : public TObject {

 public:
  // Default constructor
  PndDrcLutNodeH();

  // Standard constructors
  PndDrcLutNodeH(Int_t nodeId);
  PndDrcLutNodeH(Int_t nodeId, TVector3 dir, Double_t time);

  // Copy constructor
  PndDrcLutNodeH(PndDrcLutNodeH &node) { *this = node; }

  // Modifiers
  void AddEntry(TVector3 dir, Int_t amb, Double_t time);
  void SetPos(TVector3 dir) { fNodePos = dir; }

  // Accessors
  Int_t Entries() { return fSize; }
  Int_t GetNodeId() { return fNodeId; }
  Int_t GetAmbiguity(Int_t entry);
  Double_t GetTime(Int_t entry);
  TVector3 GetDirection(Int_t entry);
  TVector3 GetPos() { return fNodePos; }

 protected:
  Int_t fNodeId;
  Int_t fSize;

  std::vector<TVector3> fNodeArray;
  std::vector<Int_t> fAmbiguity;
  std::vector<Double_t> fTimeArray;
  TVector3 fNodePos;
  ClassDef(PndDrcLutNodeH, 2)
};

#endif // PNDDRCLUTNODEH_H
