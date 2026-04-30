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
// -----                  PndGemSmearingTask header file               -----
// -----                  Created 04/11/08  by R.Kliemt                -----
// -------------------------------------------------------------------------

/** PndGemSmearingTask.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The ideal tracking produces tracks of type PndGemHit by using MC information
 ** directly, but with the correct data interface.
 **/

#ifndef PNDGEMSMEARINGTASK_H
#define PNDGEMSMEARINGTASK_H

// Root includes
#include "TVector3.h"
#include "TRandom.h"
#include "TString.h"
#include "TGeoManager.h"

// framework includes
#include "FairTask.h"

// PndMvd includes
#include "PndGemMCPoint.h"
#include "PndGemDigiPar.h" //eoHandling.h"

class TClonesArray;

class PndGemSmearingTask : public FairTask {
 public:
  /** Default constructor **/
  PndGemSmearingTask();

  PndGemSmearingTask(Double_t sx, Double_t sy, Double_t sz);

  /** Destructor **/
  virtual ~PndGemSmearingTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

 private:
  PndGemDigiPar *fDigiPar;

  TString fBranchName;
  //  PndGeoHandling* fGeoH;
  /** Input array of Points **/
  TClonesArray *fPointArray;
  TClonesArray *fMctruthArray;

  /** Output array of Hits **/
  TClonesArray *fHitOutputArray;
  /** Properties **/
  Double_t fSigmaX; // Variance vector of sensor smearing
  Double_t fSigmaY; // Variance vector of sensor smearing
  Double_t fSigmaZ; // Variance vector of sensor smearing
  PndGemMCPoint *fCurrentPndGemMCPoint;
  TGeoHMatrix *fCurrentTransMat;
  TMatrixT<Double_t> fHitCovMatrix;
  /** smearing and geometry access **/
  void InitTransMat();
  void smear(TVector3 &pos);
  void smearLocal(TVector3 &pos);
  void CalcGFDetPlane(TVector3 &o, TVector3 &u, TVector3 &v);

  void Register();

  void Reset();

  void ProduceHits();

  ClassDef(PndGemSmearingTask, 1);
};

#endif
