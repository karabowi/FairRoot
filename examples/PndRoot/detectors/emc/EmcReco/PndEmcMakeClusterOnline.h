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
//---------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCMAKECLUSTERONLINE_H
#define PNDEMCMAKECLUSTERONLINE_H

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
class PndEmcRecoPar;
class BinaryFunctor;

/**
 * @brief Task to create clusters from digis.
 * 
 * This is an alternative to PndEmcMakeCluster, using a different method to
 * form clusters.
 * @author M. Tiemens <m.tiemens@rug.nl>
 * @ingroup PndEmc
 */
class PndEmcMakeClusterOnline : public PndPersistencyTask
{
public:

  // Constructors

  PndEmcMakeClusterOnline(Int_t verbose=0, Bool_t storeclusters=kTRUE);

  // Destructor

  virtual ~PndEmcMakeClusterOnline( );

  /** Virtual method Init */
  virtual InitStatus Init();

  /** Virtual method Exec */
  virtual void Exec(Option_t* opt);

 /** Virtual method Finish Task */
  virtual void FinishTask();

  void SetStorageOfData(Bool_t val); 	//!< Method to specify whether clusters are stored or not.

  /// Restoring digis makes only sense if reconstruction is done timebased	
  void StoreClusterBaseDigis(Bool_t val = kTRUE) { fStoreClusterBase = val; }
 
  ///  Override EmcRecoPar time gap parameter ..to be set in ns!!! 
  void SetTimebunchCutTime(Double_t time) { fTimebunchCutTime = time; } 
  /// Set time threshold for separating digis within a timebunch in ns
  void SetClusterActiveTime(Double_t dt) { fClusterActiveTime = dt; }

  /// Set minimum cluster energy
  void SetClusterMinimumEnergy(Double_t minE) { fClusterEnergyCut = minE; }
  void EnableRemovalOfLowEnergyClusters(Bool_t enable) {fRemoveLowEclus = enable;} /** Enable/disable removal of low energy clusters. Slows down cluster finding task, but will speed up future processing and reconstruction tasks */

  void SetAutoDetermineTimecuts(Bool_t autodet) {fAutoDetermine = autodet;}

protected:
  virtual void FinishClusters();
  void FinishCluster(PndEmcCluster* tmpcluster);
  virtual void RemoveLowEnergyClusters();
  
private:
	/** Input array of PndEmcDigis */
	TClonesArray* fDigiArray;

	/** Output array of PndEmcClusters */
	TClonesArray* fClusterArray;	//!< active clusters

	PndEmcGeoPar*     fGeoPar;       /** Geometry parameter container */
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container */
	
	Double_t fDigiEnergyTresholdBarrel; /**< Energy threshold for digis from the barrel section */
	Double_t fDigiEnergyTresholdFWD; /**< Energy threshold for digis from the FwEndcap section */
	Double_t fDigiEnergyTresholdBWD; /**< Energy threshold for digis from the BwEndcap section */
	Double_t fDigiEnergyTresholdShashlyk;/**< Energy threshold for digis from the Shashlyk section */
	Double_t fClusterEnergyCut; /**< Energy threshold for clusters */
	
	Double_t fTimebunchCutTime; // Defines how long clusters are kept open in timebased reconstruction
	Double_t fClusterActiveTime; 
	BinaryFunctor* fDigiFunctor;

	Int_t fNrOfEvents;
	Int_t fNrOfDigis;
	Int_t nOnlProg;
	Int_t digiCounter;
	Int_t evtCounter;
	Double_t OtotRtime;
	Double_t OtotCtime;

	std::vector<Double_t> fClusterPosParam;

	Bool_t fStoreClusters;
	Bool_t fStoreClusterBase; 	//re-store digis in case of a timebased run
	Bool_t fRemoveLowEclus;
	Bool_t fAutoDetermine;

	TH1I* hClusMultiplicity;
	TH1I* hEventsPerCluster;
	TH1D* hTimeDifference;

	/** Get parameter containers */
	virtual void SetParContainers();

	TStopwatch fTimer;

	ClassDef(PndEmcMakeClusterOnline, 1)
};
#endif // PNDEMCMAKECLUSTERONLINE_HH
