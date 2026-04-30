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
// -----                    PndGemMagneticFieldVsTrackParameters header file                 -----
// -----                  Created 12/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemMagneticFieldVsTrackParameters
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 12.02.2009
 *@version 1.0
 **
 ** PANDA task class for digitising GEM
 ** Task level SIM
 ** Produces objects of type PndGemDigi out of PndGemMCPoint.
 **/

#ifndef PNDGEMMAGNETICFIELDVSTRACKPARAMETERS_H
#define PNDGEMMAGNETICFIELDVSTRACKPARAMETERS_H 1

#include "FairTask.h"

#include "TStopwatch.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TF1.h"
#include "TF2.h"

#include <list>
#include <map>

class TClonesArray;
class PndGemDigiPar;

class PndGemMagneticFieldVsTrackParameters : public FairTask {

 public:
  /** Default constructor **/
  PndGemMagneticFieldVsTrackParameters();

  /** Standard constructor **/
  PndGemMagneticFieldVsTrackParameters(Int_t iVerbose);

  /** Constructor with name **/
  PndGemMagneticFieldVsTrackParameters(const char *name, Int_t iVerbose);

  /** Destructor **/
  virtual ~PndGemMagneticFieldVsTrackParameters();

  /** Execution **/
  virtual void Exec(Option_t *opt);

 private:
  //  FairGeoStsPar*     fGeoPar;       /** Geometry parameter container **/
  //  PndGemDigiPar*    fDigiPar;      /** Digitisation parameter container **/
  //  PndGemDigiScheme* fDigiScheme;   /** Digitisation scheme **/
  PndGemDigiPar *fDigiPar;

  Double_t fTrackFinderOnHits_ParThetaA;
  Double_t fTrackFinderOnHits_ParThetaB;

  Double_t fTrackFinderOnHits_ParTheta0;
  Double_t fTrackFinderOnHits_ParTheta1;
  Double_t fTrackFinderOnHits_ParTheta2;
  Double_t fTrackFinderOnHits_ParTheta3;

  Double_t fTrackFinderOnHits_ParRadPhi0;
  Double_t fTrackFinderOnHits_ParRadPhi2;

  Double_t fTrackFinderOnHits_ParMat0[3];
  Double_t fTrackFinderOnHits_ParMat1[3];

  TClonesArray *fMCTrackArray;
  TClonesArray *fMCPointArray;

  TStopwatch fTimer;

  /** Event counter **/
  Int_t fNofEvents; // event counter

  TList *fHistoList;
  // theta angle vs radius vs momentum
  TH3F *fhThetaVsRadiusVsMomentumAll;
  TH3F *fhThetaVsRadiusVsMomentum[10];
  TF2 *ffThetaVsRadiusVsMomentum[10];
  // radius vs angle
  TH2F *fhRadiusVsAngle;
  TH2F *fhRadiusVsAnglePair[20];
  TF1 *ffRadiusVsAngle[3];
  // momentum vs phi angle difference
  TH3F *fhMomentumVsPhiDiffVsRadius[10];
  TH2F *fhMomentumVsPhiDiffAll[10];
  TH2F *fhMomentumVsPhiDiff[10][20];
  TF2 *ffMomentumVsPhiDiffVsRadius[10];
  TF1 *ffMomentumVsPhiDiffAll[10];
  TF1 *ffMomentumVsPhiDiff[10][20];
  // transverse momentum vs trajectory helix radius
  TH2F *fhMomTransVsHRadius[10];
  // track phi versus hits phis
  TH3F *fhTrackPhiVsHitPhis[10];
  TH2F *fhTrackPhiVsCalcPhi[10];

  Int_t Fill1StationHistograms();
  Int_t Fill2StationsHistograms();

  void CreateHistos();

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Finish **/
  virtual void Finish();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  /** Reset eventwise counters **/
  void Reset();

  ClassDef(PndGemMagneticFieldVsTrackParameters, 1);
};

#endif
