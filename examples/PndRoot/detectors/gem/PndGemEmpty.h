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

//* $Id: */

// -------------------------------------------------------------------------
// -----                     PndGemEmpty header file                -----
// -----                  Created 15/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemEmpty
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15/02/2009
 *@version 1.0
 **
 ** PANDA task class for finding clusters in the GEM TimeBasedSimulation
 ** Task level RECO
 ** Produces objects of type PndGemDigi out of PndGemDigi.
 **/

#ifndef PNDGEMEMPTY_H
#define PNDGEMFINDC_H 1

#include "FairTask.h"
#include "FairTSBufferFunctional.h"

#include "TStopwatch.h"

#include <map>
#include <set>

class TClonesArray;
class PndGemDigiPar;
class PndGemSensor;
class PndGemStation;

class PndGemEmpty : public FairTask {

 public:
  /** Default constructor **/
  PndGemEmpty();

  /** Standard constructor **/
  PndGemEmpty(Int_t iVerbose);

  /** Constructor with task name **/
  PndGemEmpty(const char *name, Int_t iVerbose = 0);

  /** Destructor **/
  virtual ~PndGemEmpty();

  /** Execution **/
  virtual void Exec(Option_t *opt);

  void SetInBranchName(TString ibn) { fInBranchName = ibn.Data(); }

 private:
  BinaryFunctor *fFunctor; //!
  TClonesArray *fInBranch;
  TClonesArray *fOutBranch;
  TString fInBranchName;

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  /** Finish at the end of each event **/
  virtual void Finish();

  ClassDef(PndGemEmpty, 1);
};

#endif
