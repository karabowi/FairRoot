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

/**PndEventDisplay
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 * @brief display of hits inside the gGeoManager
 * @since 29.10.2007
 *
 * This class allows the display of event information within the gGeoManager.
 * First a new group has to be created via the AddNewGroup command. A Group contains a unique
 * name for the informations which should be displayed and a pointer to a PndGeoHitList.
 * After the creation of the group hits can be added to the group by the name of the group and
 * various imput possibilities.
 */

#ifndef PNDEVENTDISPLAY_H
#define PNDEVENTDISPLAY_H

#include "PndGeoHitList.h"

#include "FairHit.h"

#include "TString.h"
#include "TClonesArray.h"

#include <map>

class PndEventDisplay {
 public:
  PndEventDisplay();
  PndEventDisplay(PndEventDisplay &display);
  virtual ~PndEventDisplay();

  void AddNewGroup(TString groupName, PndGeoHitList *newList);
  void AddHit(TString groupName, FairHit *hit, Bool_t vis = kTRUE);
  void AddHit(TString groupName, Double_t x, Double_t y, Double_t z, Bool_t vis = kTRUE);
  void AddHit(TString groupName, TGeoMatrix *mat, Bool_t vis = kTRUE);
  void AddHits(TString groupName, TClonesArray *hits, Bool_t vis = kTRUE);
  void SetHits(TString groupName, TClonesArray *hits, Bool_t vis = kTRUE);
  void ClearHits(TString groupName);

  PndGeoHitList *GetHitList(TString ListName);
  std::map<TString, PndGeoHitList *> GetHitListMap() { return fHitListMap; };
  PndEventDisplay &operator=(PndEventDisplay &display)
  {
    fHitListMap = display.GetHitListMap();
    return *this;
  };

 private:
  std::map<TString, PndGeoHitList *> fHitListMap; //< Map between the name of a list and a pointer to the list object

  ClassDef(PndEventDisplay, 1);
};

#endif /*PNDEVENTDISPLAY_H*/
