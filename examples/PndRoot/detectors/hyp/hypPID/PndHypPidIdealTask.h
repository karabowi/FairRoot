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
// -----        Modified 12.12.08 for hyp purpose by A. Sanchez        -----
// -------------------------------------------------------------------------

/** PndMvdPidIdealTask.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The ideal tracking produces tracks of type PndSdsHit by using MC information
 ** directly, but with the correct data interface.
 **/

#ifndef PNDHYPPIDIDEALTASK_H
#define PNDHYPPIDIDEALTASK_H

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
#include "PndHypPidCand.h"

class TClonesArray;

class PndHypPidIdealTask : public FairTask {
 public:
  /** Default constructor **/
  PndHypPidIdealTask(const TString algoName = "ideal");
  //  virtual ~PndHypPidIdealTask();
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

  ClassDef(PndHypPidIdealTask, 1);
};

#endif
