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

#ifndef PNDTRACKINGCLONEINFO_H
#define PNDTRACKINGCLONEINFO_H

#include <vector>
#include <map>
#include "FairLink.h"
#include <TClonesArray.h>

class PndTrackingCloneInfo : public TObject {

 public:
  // Constructor, takes the track, MC track and (for the moment) SttHit arrays
  PndTrackingCloneInfo();
  PndTrackingCloneInfo(std::map<FairLink, Int_t>, TClonesArray *);
  // std::map<FairLink, FairLink> timeBasedTrackIdMCId is the map between the FairLink of the track (Key) and the FairLink of the MCTrack

  ~PndTrackingCloneInfo();

  // Function which finds the number of clones
  void CalcNumClones(std::map<FairLink, Int_t>, TClonesArray *);

  int GetNumClones() { return fNumClonesTimeBased; };

 protected:
  Int_t fNumClonesTimeBased; // Number of clones which were found

  ClassDef(PndTrackingCloneInfo, 2);
};

#endif
