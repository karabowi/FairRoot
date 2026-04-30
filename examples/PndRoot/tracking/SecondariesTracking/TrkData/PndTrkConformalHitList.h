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

/** PndTrkConformalHitList
 **
 ** @author Lia Lavezzi
 **/

#ifndef PNDTRKCONFORMALHITLIST_H
#define PNDTRKCONFORMALHITLIST_H 1

#include "TVector3.h"
#include "PndTrkConformalHit.h"
#include "PndTrkConformalTransform.h"
#include "FairHit.h"

#include "TClonesArray.h"

#include <iostream>
/* #include <cmath> */
/* #include <vector> */
/* #include <iterator> */
/* #include <algorithm> */

#define MAXNOFHITS 1000 // CHECK consistency

// class TObjArray;
class PndTrkConformalHitList : public TObject {

 public:
  PndTrkConformalHitList();
  PndTrkConformalHitList(PndTrkConformalTransform *conformal);
  PndTrkConformalHitList(const PndTrkConformalHitList &hlist);

  ~PndTrkConformalHitList();
  PndTrkConformalHitList &operator=(const PndTrkConformalHitList &hlist);

  // CHECK private?
  void AddHit(PndTrkConformalHit *chit);
  PndTrkConformalHit *GetHit(int index);

  void Reset();
  void Clear(Option_t *opt = "");

  void SetConformalTransform(PndTrkConformalTransform *conformal) { fConformal = conformal; }
  PndTrkConformalTransform *GetConformalTransform() { return fConformal; }
  inline Int_t GetNofHits() { return fHitList.GetEntriesFast(); }

  void Print();
  void Draw(Color_t color = kBlack);

 protected:
  /*   std::vector<PndTrkConformalHit *> hitlist; */
  PndTrkConformalTransform *fConformal;
  TClonesArray fHitList;

  ClassDef(PndTrkConformalHitList, 1);
};

#endif
