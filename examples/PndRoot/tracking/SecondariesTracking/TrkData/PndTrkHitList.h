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

/** PndTrkHitList
 **
 ** @author Lia Lavezzi
 **/

#ifndef PNDTRKHITLIST_H
#define PNDTRKHITLIST_H 1

#include "TVector3.h"
#include <TClonesArray.h>
#include "PndTrkHit.h"
#include "FairHit.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>

#define MAXNOFHITS 1000 // CHECK consistency

class PndTrkHitList : public TObject {

 public:
  PndTrkHitList();
  ~PndTrkHitList();

  // CHECK private?
  void AddHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, TVector3 &pos);
  void AddHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, Int_t tubeID, TVector3 &pos, Double_t isochrone);
  void AddHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, Int_t sensorID, TVector3 &pos);
  void AddHit(PndTrkHit *hit);

  void AddTCA(Int_t detID, TClonesArray *array);

  virtual void AddHit(Int_t hitid, Int_t detid, FairHit *hit);

  PndTrkHit *GetHit(int index);
  std::vector<PndTrkHit> GetHitList();
  PndTrkHit *GetHitByID(int id);

  inline Int_t GetNofHits() { return hitlist.size(); }

  void RemoveHit(PndTrkHit *hit)
  {
    std::vector<PndTrkHit>::iterator it = find(hitlist.begin(), hitlist.end(), *hit);
    if (it != hitlist.end())
      hitlist.erase(it);
  }

  void Print();
  void Draw(Color_t color = kBlack);
  void ClearList() { hitlist.clear(); }

 protected:
  std::vector<PndTrkHit> hitlist;

  ClassDef(PndTrkHitList, 1);
};

#endif
