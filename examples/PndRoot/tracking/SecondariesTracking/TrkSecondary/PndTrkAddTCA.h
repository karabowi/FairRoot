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

#ifndef PNDTRKADDTCA_H
#define PNDTRKADDTCA_H

// fairroot
#include "FairTask.h"
// pandaroot
#include "PndTrack.h"

class TClonesArray;
class PndTrkAddTCA : public FairTask {

 public:
  /** Default constructor **/
  PndTrkAddTCA();

  /** Destructor **/
  ~PndTrkAddTCA();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

 private:
  Int_t fEventCounter;
  TClonesArray *fTrackArray;
  TClonesArray *fPrimaryTrackArray;
  TClonesArray *fSecondaryTrackArray;

  TClonesArray *fTrackCandArray;

  ClassDef(PndTrkAddTCA, 1);
};

#endif
