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
// -----                     PndGemFindClustersTB header file                -----
// -----                  Created 15/06/2013 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemFindClustersTB
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15/06/2013
 *@version 1.0
 **
 ** PANDA task class for finding clusters in the GEM TimeBasedSimulation
 ** Task level RECO
 ** Produces objects of type PndGemCluster out of PndGemDigi.
 **/

#ifndef PNDGEMFINDCLUSTERSTB_H
#define PNDGEMFINDCLUSTERSTB_H 1

#include "FairTask.h"
#include "FairTSBufferFunctional.h"

#include "TStopwatch.h"

#include <map>
#include <set>

class TClonesArray;
class PndGemDigiPar;
class PndGemSensor;
class PndGemStation;

struct DigiClusterTB {
  Int_t detId;
  std::vector<Int_t> digiNr;
  std::vector<Int_t> chanNr;
  std::vector<Double_t> sigADC;
  std::vector<Double_t> sigTDC;
  Double_t cluTDC;
  Double_t cluADC;
  Double_t cluPos;
  Int_t cluPMn;
  Int_t cluPMx;
  Double_t cluMPs;
  Double_t cluMVl;
};

class PndGemFindClustersTB : public FairTask {

 public:
  /** Default constructor **/
  PndGemFindClustersTB();

  /** Standard constructor **/
  PndGemFindClustersTB(Int_t iVerbose);

  /** Constructor with task name **/
  PndGemFindClustersTB(const char *name, Int_t iVerbose = 0);

  /** Destructor **/
  virtual ~PndGemFindClustersTB();

  /** Execution **/
  virtual void Exec(Option_t *opt);

  void SetInBranchName(TString ibn) { fInBranchName = ibn.Data(); }

 private:
  PndGemDigiPar *fDigiPar; /** Digitisation parameters **/
  TClonesArray *fDigis;    /** Input array of PndGemDigi **/
  TClonesArray *fClusters; /** Output array of PndGemCluster **/

  Int_t fTNofEvents;
  Int_t fTNofDigis;
  Int_t fTNofClusters;

  BinaryFunctor *fFunctor; //!

  Double_t fPrepTime;
  Double_t fCreateTime;
  Double_t fSortTime;
  Double_t fCheckTime;
  Double_t fAnaTime;
  Double_t fWriteTime;
  Double_t fAllTime;

  TString fInBranchName;

  std::vector<DigiClusterTB> fDigiClusters;

  TStopwatch fTimer;

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  /** Look for clusters **/
  Int_t CreateClusters();

  /** Write clusters **/
  Int_t WriteClusters();

  void AnalyzeClusters();

  void SortClusters();
  void SortCluster(Int_t clus);
  void PrintClusters();
  void PrintCluster(Int_t clus);

  void CheckClusters();
  void CheckCluster(Int_t clus);

  void JoinTwoClusters(Int_t clus1, Int_t clus2);

  void AddDigiToCluster(Int_t digiNr, Int_t clusNr);

  /** Check if digi belongs to some cluster **/
  Bool_t CompareDigiToClusters(Int_t digiNumber);

  /** Finish at the end of each event **/
  virtual void Finish();

  ClassDef(PndGemFindClustersTB, 1);
};

#endif
