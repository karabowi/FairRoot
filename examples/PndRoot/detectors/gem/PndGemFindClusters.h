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
// -----                     PndGemFindClusters header file                -----
// -----                  Created 15/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemFindClusters
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15/02/2009
 *@version 1.0
 **
 ** PANDA task class for finding hits in the GEM
 ** Task level RECO
 ** Produces objects of type PndGemHits out of PndGemDigi.
 **/

#ifndef PNDGEMFINDCLUSTERS_H
#define PNDGEMFINDCLUSTERS_H 1

#include "FairTask.h"

#include "TStopwatch.h"

#include <map>
#include <set>

class TClonesArray;
class PndGemDigiPar;
class PndGemSensor;
class PndGemStation;

struct DigiCluster {
  Int_t detId;
  std::vector<Int_t> digiNr;
  std::vector<Int_t> chanNr;
  std::vector<Double_t> sigADC;
  Double_t cluTDC;
  Double_t cluADC;
  Double_t cluPos;
  Int_t cluPMn;
  Int_t cluPMx;
  Double_t cluMPs;
  Double_t cluMVl;
};

class PndGemFindClusters : public FairTask {

 public:
  /** Default constructor **/
  PndGemFindClusters();

  /** Standard constructor **/
  PndGemFindClusters(Int_t iVerbose);

  /** Constructor with task name **/
  PndGemFindClusters(const char *name, Int_t iVerbose = 0);

  /** Destructor **/
  virtual ~PndGemFindClusters();

  /** Execution **/
  virtual void Exec(Option_t *opt);

 private:
  PndGemDigiPar *fDigiPar; /** Digitisation parameters **/
  TClonesArray *fDigis;    /** Input array of PndGemDigi **/
  TClonesArray *fClusters; /** Output array of PndGemDigi **/

  std::map<Double_t, Int_t> fTimeOrderedDigis;
  std::vector<DigiCluster> fDigiClusters;

  Int_t fTNofEvents;
  Int_t fTNofDigis;
  Int_t fTNofClusters;

  TStopwatch fTimer;

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  /** Sort digis timewise  **/
  Int_t SortDigis();

  //  /** Find clusters  **/
  // void FindClusters();//not used

  /** Look for clusters **/
  Int_t CreateClusters();

  /** Write clusters  **/
  Int_t WriteClusters();

  // /** Compare digi to clusters **/
  // Bool_t CompareDigiToClusters(Int_t digiNumber);

  /** Compare digi to digis **/
  Bool_t CompareDigiToClustersDigis(Int_t digiNumber);

  //  /** Analyze clusters **/
  // void AnalyzeClusters();//not used

  ///** Clear clusters **/
  // void ClearClusters();//not used

  // void ClearClusters2();

  void SortClusters();
  void SortCluster(Int_t clus);
  void PrintClusters();
  void PrintCluster(Int_t clus);

  void JoinTwoClusters(Int_t clus1, Int_t clus2);

  void AddDigiToCluster(Int_t digiNr, Int_t clusNr);

  /** Finish at the end of each event **/
  virtual void Finish();

  ClassDef(PndGemFindClusters, 1);
};

#endif
