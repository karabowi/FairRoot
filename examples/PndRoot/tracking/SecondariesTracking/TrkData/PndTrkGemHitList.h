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

/** PndTrkGemHitList

 ** @author Lia Lavezzi
 **/

#ifndef PNDTRKGEMHITLIST_H
#define PNDTRKGEMHITLIST_H 1

#include "PndTrkHitList.h"

#include "TVector3.h"
#include "TClonesArray.h"
#include "PndTrkHit.h"
#include "PndGemHit.h"

#include <map>

#define MAXNOFGEMHITS 1000 // CHECK consistency

class PndTrkGemHitList : public PndTrkHitList {

 public:
  PndTrkGemHitList();
  PndTrkGemHitList(Int_t iregion);
  ~PndTrkGemHitList();

  static PndTrkGemHitList *Instance();
  PndTrkGemHitList *Instanciate();

  void AddHit(Int_t hitid, Int_t detid, FairHit *hit);
  void AddNonCombiHits(Int_t detID, TClonesArray *array, std::map<int, bool> hitTousable);

  void Clear()
  {
    ClearList();
    fgemInstance = nullptr;
  }

 protected:
  static PndTrkGemHitList *fgemInstance;

  Int_t fIRegion;
  ClassDef(PndTrkGemHitList, 1);
};

#endif
