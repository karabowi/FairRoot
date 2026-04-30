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

// -------------------------------------------------------------------------
// -----                PndMvdPidIdealTask header file                 -----
// -----                Created 20/03/07  by R.Kliemt                  -----
// -------------------------------------------------------------------------

/** PndMvdPidIdealTask.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The ideal tracking produces tracks of type PndSdsHit by using MC information
 ** directly, but with the correct data interface.
 **/

#ifndef PNDMVDPIDIDEALTASK_H
#define PNDMVDPIDIDEALTASK_H

// Root includes
#include "TVector3.h"
#include "TGeoPolygon.h"
#include "TRandom.h"
#include "TObject.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

// framework includes
#include "FairTask.h"
#include "FairField.h"
#include "PndMvdPidCand.h"

class TClonesArray;

class PndMvdPidIdealTask : public FairTask {
 public:
  /** Default constructor **/
  PndMvdPidIdealTask(const TString algoName = "ideal");
  PndMvdPidIdealTask(PndMvdPidIdealTask &) = delete;
  //  virtual ~PndMvdPidIdealTask();
  PndMvdPidIdealTask &operator=(PndMvdPidIdealTask &) = delete;
  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

 private:
  TString fBranchName;
  TString fAlgoName;

  /** Input array of Points **/
  TClonesArray *fPointArray;
  TClonesArray *fMcArray;
  /** Output array of Hits **/
  TClonesArray *fTrackOutputArray;

  void Register();

  void Reset();

  void ProduceHits();

  ClassDef(PndMvdPidIdealTask, 1);
};

#endif
