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
// PndDrcLutNode.h
//
// Created on: 09.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------
// Container for look-up table

#ifndef PNDDRCLUTNODE_H
#define PNDDRCLUTNODE_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>
#include <iostream>

class PndDrcLutNode : public TObject {

 public:
  // Default constructor
  PndDrcLutNode();

  // Standard constructors
  PndDrcLutNode(Int_t detectorId);

  // Copy constructor
  PndDrcLutNode(PndDrcLutNode &node)
    : TObject(node), fDetectorId(node.fDetectorId), fSize(node.fSize), fNodePos(node.fNodePos), fPosArray(node.fPosArray), fNodeArray(node.fNodeArray),
      fPathIdArray(node.fPathIdArray), fPathArray(node.fPathArray), fTimeArray(node.fTimeArray){};

  // Modifiers
  void AddEntry(Int_t nodeId, TVector3 dir, Double_t path, Int_t pathid, Double_t time, TVector3 pos);
  void SetPos(TVector3 pos) { fNodePos = pos; }

  // Accessors
  Int_t Entries() { return fSize; }
  Double_t GetDetectorId() { return fDetectorId; }

  TVector3 GetEntry(Int_t entry) { return fNodeArray[entry]; }
  Int_t GetPathId(Int_t entry) { return fPathIdArray[entry]; }
  Double_t GetPath(Int_t entry) { return fPathArray[entry]; }
  Double_t GetTime(Int_t entry) { return fTimeArray[entry]; }
  TVector3 GetPos() { return fNodePos; }
  TVector3 GetHitPos(Int_t entry) { return fPosArray[entry]; }

 protected:
  Int_t fDetectorId;
  Int_t fSize;
  TVector3 fNodePos;

  std::vector<TVector3> fPosArray;
  std::vector<TVector3> fNodeArray;
  std::vector<Int_t> fPathIdArray;
  std::vector<Double_t> fPathArray;
  std::vector<Double_t> fTimeArray;

  ClassDef(PndDrcLutNode, 3)
};

#endif // PNDDRCLUTNODE_H
