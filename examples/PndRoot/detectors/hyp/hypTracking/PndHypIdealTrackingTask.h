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
// -----                 PndMvdIdealTrackingTask header file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------

/** PndMvdIdealTrackingTask.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The ideal tracking produces tracks of type PndSdsHit by using MC information
 ** directly, but with the correct data interface.
 **/

#ifndef PNDHYPIDEALTRACKINGTASK_H
#define PNDHYPIDEALTRACKINGTASK_H

// Root includes
#include "TVector3.h"
#include "TRandom.h"

// framework includes
#include "FairTask.h"
#include "FairField.h"
class PndMCTrack;
class TClonesArray;

class PndHypIdealTrackingTask : public FairTask {
 public:
  /** Default constructor **/
  PndHypIdealTrackingTask();

  /** Destructor **/
  ~PndHypIdealTrackingTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  Double_t GetTrackDip(PndMCTrack *myTrack);
  Double_t GetTrackCurvature(PndMCTrack *myTrack);

 private:
  TString fBranchName;
  /** Input array of Points **/
  TClonesArray *fHitArray;
  TClonesArray *fDigiStripArray;
  TClonesArray *fMctruthArray;
  TClonesArray *fClusterStripArray;
  TClonesArray *fPointArray;

  /** Output array of Hits **/
  TClonesArray *fTrackOutputArray;

  void Register();

  void Reset();

  void ProduceHits();

  ClassDef(PndHypIdealTrackingTask, 1);
};

#endif
