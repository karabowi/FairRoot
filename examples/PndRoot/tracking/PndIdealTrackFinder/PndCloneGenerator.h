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

/*
 * PndCloneGenerator.h
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#ifndef PndCloneGenerator_H_
#define PndCloneGenerator_H_

// framework includes
#include "FairTask.h"
#include "FairMCPoint.h"
#include "PndTrackCand.h"
#include "TClonesArray.h"
#include "TDatabasePDG.h"
#include <math.h>
#include <PndIdealTrackFinder.h>

class PndCloneGenerator : public PndIdealTrackFinder {
 public:
  PndCloneGenerator();
  virtual ~PndCloneGenerator();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

 protected:
  virtual void CreateTrackCandClones();
  virtual void SetFirstLastHit(std::map<FairLink, PndTrackCand> &trackCandMap);

 private:
  std::map<FairLink, PndTrackCand> fTrackCandCloneMap;

  ClassDef(PndCloneGenerator, 2);
};

#endif /* PndCloneGenerator_H_ */
