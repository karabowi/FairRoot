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

#ifndef PNDGEOHITLIST_H
#define PNDGEOHITLIST_H

#include "FairHit.h"

#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoVolume.h"
#include "TString.h"
#include "TClonesArray.h"

#include <vector>
#include <iostream>

class PndGeoHitList {
 public:
  PndGeoHitList();
  PndGeoHitList(TString groupName, TString topNodeName, TGeoVolume *baseVolume);
  virtual ~PndGeoHitList();
  PndGeoHitList(const PndGeoHitList &) = delete;
  PndGeoHitList &operator=(const PndGeoHitList &) = delete;

  void CreateNewGroup(TString groupName, TString topNodeName, TGeoVolume *baseVolume);
  void AddHit(FairHit *hit, Bool_t vis = kTRUE);
  void AddHit(Double_t x, Double_t y, Double_t z, Bool_t vis = kTRUE);
  void AddHit(TGeoMatrix *mat, Bool_t vis = kTRUE);
  void AddHits(TClonesArray *hitList, Bool_t vis = kTRUE);
  void SetHits(TClonesArray *hitList, Bool_t vis = kTRUE);
  void RemoveGeoHits(Int_t from);
  void CreateGeoHit(TGeoMatrix *mat);

  TString GetName() const { return fGroupName; };
  const std::vector<TGeoNode *> GetNodes() const { return fNodeList; };
  void SetVisibility(TString NodeName, bool value);
  void SetVisibility(TGeoHMatrix pos, bool value);
  void SetVisibility(Int_t id, bool value);
  void SetAllInvisible();
  Int_t GetNodeId(TString NodeName);
  Int_t GetNodeId(TGeoHMatrix pos);

 private:
  TString fGroupName;
  TString fVolName;
  std::vector<TGeoNode *> fNodeList;
  TGeoVolume *fGroupVolume;
  TGeoVolume *fTopVolume;
  TGeoVolume *fBaseVolume;

  void CreateGeoHits(Int_t number);
  void SetGeoHitsInvisible(Int_t from);

  ClassDef(PndGeoHitList, 1);
};

#endif /*PNDGEOHITLIST_H*/
