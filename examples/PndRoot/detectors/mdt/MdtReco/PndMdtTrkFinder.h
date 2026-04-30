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

#ifndef PNDMDTTRKFINDER_H
#define PNDMDTTRKFINDER_H 1

#include <map>
#include <vector>

#include "FairTask.h"
#include "PndMdtTrk.h"
#include "PndMdtRecoPar.h"

#include "PndTrack.h"
#include "FairGeanePro.h"

class TClonesArray;

using std::map;
using std::vector;

class PndMdtTrkFinder : public FairTask {

 public:
  /** Default constructor **/
  PndMdtTrkFinder();

  /** Destructor **/
  ~PndMdtTrkFinder();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void SetParContainers();

 private:
  PndMdtTrk *AddTrk(PndMdtTrk *track);

  /** Input array of PndMdtHit **/
  TClonesArray *fHitArray;

  /** Output array of PndMdtTrk **/
  TClonesArray *fTrkArray;

  // PndMdtRecoPar *fRecoPar;

  ClassDef(PndMdtTrkFinder, 1);
};

#endif
