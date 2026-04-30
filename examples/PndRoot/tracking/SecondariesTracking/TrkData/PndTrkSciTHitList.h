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

/** PndTrkSciTHitList

 ** @author Lia Lavezzi
 **/

#ifndef PNDTRKSCITHITLIST_H
#define PNDTRKSCITHITLIST_H 1

#include "PndTrkHitList.h"

#include "TVector3.h"
#include "PndTrkHit.h"

#define MAXNOFSCITHITS 1000 // CHECK consistency

class PndTrkSciTHitList : public PndTrkHitList {

 public:
  PndTrkSciTHitList();
  PndTrkSciTHitList(Int_t iregion);
  ~PndTrkSciTHitList();

  static PndTrkSciTHitList *Instance();
  PndTrkSciTHitList *Instanciate();

  void AddHit(Int_t hitid, Int_t detid, FairHit *hit);

  void Clear()
  {
    ClearList();
    fscitInstance = nullptr;
  }

 protected:
  static PndTrkSciTHitList *fscitInstance;

  Int_t fIRegion;
  ClassDef(PndTrkSciTHitList, 1);
};

#endif
