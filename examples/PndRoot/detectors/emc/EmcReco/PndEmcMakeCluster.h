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

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: $
// Software developed for the PANDA Detector at GSI.
// Author List:
//	Jan Zhong
//	Marcel Tiemens
//	Áron Kripkó
//---------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCMAKECLUSTER_H
#define PNDEMCMAKECLUSTER_H

#include <PndPersistencyTask.h>
#include <vector>

#include "FairTSBufferFunctional.h"
#include "TStopwatch.h"
#include "TCanvas.h"
#include "TH1.h"

class TClonesArray;
class TObjectArray;
class PndEmcDigi;
class PndEmcCluster;
class PndEmcGeoPar;
class PndEmcDigiPar;
class PndEmcRecoPar;
class BinaryFunctor;

/*!
 * \brief Task to cluster PndEmcDigis
 *
 * Task to cluster PndEmcDigis. In the eventbased version clusters are built within one event, all adjacent digis go in one cluster.
 * The timebased version assigns an active time to every cluster. Once expired, no digis can be added any more to cluster.
 * The spatial cluster condition remains the same as in the eventbased reconstruction
 * @ingroup PndEmc
 */
class PndEmcMakeCluster : public PndPersistencyTask {
 public:
  // Constructors
  PndEmcMakeCluster(Int_t verbose = 0, Bool_t storeclusters = kTRUE);
  // Destructor
  virtual ~PndEmcMakeCluster();

  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);

  /// Finish clusters after subtasks have been executed
  virtual void ExecuteTasks(Option_t *option)
  {
    PndPersistencyTask::ExecuteTasks(option);
    FinishClusters();
  }

  void SetStorageOfData(Bool_t val); //!< Method to specify whether clusters are stored or not.

  /*!
   * \brief Method to specify whether underlying digis are stored or not.
   *
   *	Restoring digis makes only sense if reconstruction is done timebased
   */
  void StoreClusterBaseDigis(Bool_t val = kTRUE) { fStoreClusterBase = val; }

  void SetClusterActiveTime(Double_t time) { fClusterActiveTime = time; } //!<  Override EmcRecoPar cluster active time parameter ..to be set in ns!!!

  /// Set minimum cluster energy
  void SetClusterMinimumEnergy(Double_t minE) { fClusterEnergyCut = minE; }
  void EnableRemovalOfLowEnergyClusters(Bool_t enable) { fRemoveLowEclus = enable; }
  void EnableBetterNeutralReconstruction(Bool_t enable) { fMerge = enable; }

 protected:
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void FinishClusters();
  virtual void RemoveLowEnergyClusters();

 private:
  void FinishCluster(PndEmcCluster *tmpcluster);

  void cleansortmclist(std::vector<Int_t> &newlist, TClonesArray *mcTrackArray);
  // don't allow copying (-Weffc++)

  /** Input array of PndEmcDigis */
  TClonesArray *fDigiArray;

  /** Input array of Hits and MC Tracks ... needed for MC **/
  TClonesArray *fHitArray;
  TClonesArray *fMCTrackArray;

  /** Output array of PndEmcClusters */
  TClonesArray *fClusterArray; //!< active clusters

  PndEmcGeoPar *fGeoPar;   /** Geometry parameter container */
  PndEmcDigiPar *fDigiPar; /** Digitisation parameter container **/
  PndEmcRecoPar *fRecoPar; /** Reconstruction parameter container */

  /** Verbosity level **/
  Int_t fVerbose;

  Double_t fDigiEnergyTresholdBarrel;   /**< Energy threshold for digis from the barrel section */
  Double_t fDigiEnergyTresholdFWD;      /**< Energy threshold for digis from the FwEndcap section */
  Double_t fDigiEnergyTresholdBWD;      /**< Energy threshold for digis from the BwEndcap section */
  Double_t fDigiEnergyTresholdShashlyk; /**< Energy threshold for digis from the Shashlyk section */
  Double_t fClusterEnergyCut;           /**< Energy threshold for clusters */

  Double_t fMaxECut;

  Double_t fTimebunchCutTime; // Defines how long clusters are kept open in timebased reconstruction
  Double_t fClusterActiveTime;
  BinaryFunctor *fDigiFunctor;

  Int_t fNrOfEvents;
  Int_t fNrOfDigis;
  Int_t nOnlProg;
  Int_t digiCounter;
  Int_t evtCounter;

  std::vector<Double_t> fClusterPosParam;

  Bool_t fStoreClusters;
  Bool_t fStoreClusterBase; // re-store digis in case of a timebased run
  Bool_t fMerge;
  Bool_t fRemoveLowEclus;

  TH1I *hClusMultiplicity;
  TH1I *hEventsPerCluster;
  TH1D *hTimeDifference;

  TStopwatch fTimer;

  ClassDef(PndEmcMakeCluster, 3)
};
#endif // PNDEMCMAKECLUSTER_HH
