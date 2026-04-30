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
// -----                 PndLmdStripClusterTask header file            -----
// -------------------------------------------------------------------------

#ifndef PNDLMDSTRIPCLUSTERTASK_H
#define PNDLMDSTRIPCLUSTERTASK_H

#include "FairGeanePro.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndSdsStripClusterTask.h"
class PndLmdStripClusterTask : public PndSdsStripClusterTask {
 public:
  /** Default constructor **/

  PndLmdStripClusterTask();

  /** Destructor **/
  virtual ~PndLmdStripClusterTask();
  /** Virtual method Init **/
  // virtual InitStatus Init();
  virtual void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);
  virtual void SetBranchNames();
  virtual void SetParContainers();
  virtual void SetCalculators();
  void SetClusterType() { fClusterType = static_cast<int>(DetectorType::kMVDClusterStrip); }
  TVector3 AddMSErr(TVector3 hit, TVector3 hiterr);
  virtual Bool_t Backmap(TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, Double_t meanboterr, TVector3 &hitPos, TMatrixD &hitCov, Int_t &sensorID);
  // inline void SetyRotation(const Double_t inrot=0){fyRotation=inrot;};

  void Exec(Option_t *opt);
  InitStatus Init();
  PndGeoHandling *fGeoH;
  FairRun *ana;
  FairRuntimeDb *rtdb;

  void SetMSflag(bool fflag) { flagMS = fflag; };
  ClassDef(PndLmdStripClusterTask, 2);

 private:
  bool flagMS;
  //   TList* fAlignParamList;
  Double_t fPbeam;
  void combitransToLumiFrame(TVector3 &hitPos);
  void rotateToLumiFrame(TVector3 &hitPos);
  TMatrixD rotateToLumiFrame(TMatrixD &hitCov);
  /* void alignmentCorr(TVector3& hitPos, int sensID); */
  /* double fShiftX[32],fShiftY[32],fShiftZ[32]; */
  /* double fRotateX[32],fRotateY[32],fRotateZ[32]; */
};

#endif /* LMDCLUSTERTASK_H */
