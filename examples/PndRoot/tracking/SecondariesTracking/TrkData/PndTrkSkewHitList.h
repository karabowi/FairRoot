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

/** PndTrkSkewHitList
 **
 ** @author Lia Lavezzi
 **/

#ifndef PNDTRKSKEWHITLIST_H
#define PNDTRKSKEWHITLIST_H 1

#include "TVector3.h"
#include "PndTrkSkewHit.h"
#include "FairHit.h"

#include "TClonesArray.h"

#include <iostream>

#define MAXNOFHITS 1000 // CHECK consistency

class PndTrkSkewHitList : public TObject {

 public:
  PndTrkSkewHitList();
  PndTrkSkewHitList(const PndTrkSkewHitList &hlist);

  ~PndTrkSkewHitList();
  PndTrkSkewHitList &operator=(const PndTrkSkewHitList &hlist);

  // CHECK private?
  void AddHit(PndTrkSkewHit *shit);
  void AddHit(PndTrkSkewHit shit);
  void AddHit(PndTrkHit *shit);
  PndTrkSkewHit *GetHit(int index);

  void Reset();
  void Clear(Option_t *opt = "");

  inline Int_t GetNofHits() { return fHitList.GetEntriesFast(); }

  void Print();
  void Draw(Color_t color = kBlack);

 protected:
  TClonesArray fHitList;

  ClassDef(PndTrkSkewHitList, 1);
};

#endif
