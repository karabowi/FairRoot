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
// 
//---------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCDISTRIBUTEDCLUSTERING_H
#define PNDEMCDISTRIBUTEDCLUSTERING_H

#include "FairTask.h"
#include <vector>		

#include "FairTSBufferFunctional.h"
#include "TStopwatch.h"
#include "TH1.h"
#include "TVector3.h"

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
 * This is an alternative to PndEmcMakeCluster, that more closely resembles
 * the design of the readout hardware.
 * @author M. Tiemens <m.tiemens@rug.nl>
 * @ingroup PndEmc
 */
class PndEmcDistributedClustering : public FairTask
{
public:

  // Constructors

  PndEmcDistributedClustering(Int_t verbose=0, Bool_t storeclusters=kTRUE);

  // Destructor

  virtual ~PndEmcDistributedClustering( );

  /** Virtual method Init */
  virtual InitStatus Init();

  /** Virtual method Exec */
  virtual void Exec(Option_t* opt);

 /** Virtual method Finish Task */
  virtual void FinishTask();

  void SetStorageOfData(Bool_t val); 	//!< Method to specify whether clusters are stored or not.

  /// Restoring digis makes only sense if reconstruction is done timebased	
  void StoreClusterBaseDigis(Bool_t val = kTRUE) { fStoreClusterBase = kTRUE; }
 
  /// Override EmcRecoPar cluster active time parameter ..to be set in ns!!! 
  void SetTimebunchCutTime(Double_t time) { fTimebunchCutTime = time; } 
  /// Set time threshold for separating digis within a timebunch in ns
  void SetClusterActiveTime(Double_t dt) { fClusterActiveTime = dt; }

  /// Set minimum cluster energy
  void SetClusterMinimumEnergy(Double_t minE) { fClusterEnergyCut = minE; }
  void EnableRemovalOfLowEnergyClusters(Bool_t enable) {fRemoveLowEclus = enable;} /**< Enable/disable removal of low energy clusters. Slows down cluster finding task, but will speed up future processing and reconstruction tasks */



  void SetPositionMethod(Int_t method) { fPosMethod = method; } /**< Set precluster position and radius method.
			@param method Position and radius method:
			0 = default method (for position, logarithmic weighing; for radius, distance of digi furthest from position), 
			1 = simplified method (for position, xpos=(xmax-xmin)/2 ypos=(ymax-ymin)/2; for radius, r=max(ysize, xsize)) for preclusters only, 
			2 = simplified method for preclusters and clusters. Using the simplified method is less accurate, but saves a very large amount of resources 
		*/

  void SetNeighbourMethod(Int_t nbmethod) { fNbMethod = nbmethod; } /**< Set cluster neighbour method.
			@param nbmethod Neighbour method: 
			0 = default method (for position, logarithmic weighing; for radius, distance of digi furthest from position) -> use this CIRCLE for neighbour relations),  
			1 = simplified method (for position, xpos=(xmax+xmin)/2 ypos=(ymax+ymin)/2; for radius, r=max(ysize, xsize) -> use this CIRCLE for neighbour relations), 
			2 = simplified method (for position, xpos=(xmax+xmin)/2 ypos=(ymax+ymin)/2; for radius, rx=(xmax-xmin)/2, ry=(ymax-ymin)/2 -> use this RECTANGULAR BOX for neighbour relations). 
			3 = simplified method (for position, xpos=(xmax+xmin)/2 ypos=(ymax+ymin)/2; for radius, r=max(ysize, xsize) -> use this SQUARE BOX for neighbour relations). 
		*/

  void SetAutoDetermineTimecuts(Bool_t autodet) {fAutoDetermine = autodet;}

  void SetFactor(TVector3* factor) {fFactor = factor;}

protected:
  virtual void FinishPreclusters();
  void FinishPrecluster(PndEmcCluster* tmpcluster);
  virtual void FinishClusters();
  void FinishCluster(PndEmcCluster* tmpcluster, int);
  virtual void RemoveLowEnergyClusters();
  virtual void MapDistancesToSingleHitClusters();
  
private:
	/** Input array of PndEmcDigis */
	TClonesArray* fDigiArray;
	TClonesArray* fPreclusterArray;

	/** Output array of PndEmcClusters */
	TClonesArray* fClusterArray;	

	PndEmcGeoPar*     fGeoPar;       /**< Geometry parameter container */
	PndEmcRecoPar*    fRecoPar;      /**< Reconstruction parameter container */
	
	Double_t fDigiEnergyTresholdBarrel; /**< Energy threshold for digis from the barrel section */
	Double_t fDigiEnergyTresholdFWD; /**< Energy threshold for digis from the FwEndcap section */
	Double_t fDigiEnergyTresholdBWD; /**< Energy threshold for digis from the BwEndcap section */
	Double_t fDigiEnergyTresholdShashlyk;/**< Energy threshold for digis from the Shashlyk section */
	Double_t fClusterEnergyCut; /**< Energy threshold for clusters */
	
	Double_t fTimebunchCutTime; // Defines how long clusters are kept open in timebased reconstruction
	Double_t fClusterActiveTime; 
	BinaryFunctor* fDigiFunctor; 

	std::vector<Double_t> fClusterPosParam;

	Int_t fNrOfEvents;
	Int_t fPosMethod;
	Int_t fNbMethod;
	Int_t evtCounter;
	Int_t digiCounter;
	Int_t fNrOfDigis;
	Int_t nDistProg;
	Int_t fRemovedClusters;
	Double_t DCtotRtime;
	Double_t DCtotCtime;
	Double_t CNtotRtime;
	Double_t CNtotCtime;
	bool tagSingleClus;
	bool printClusters;
	Bool_t fAutoDetermine;

	TH1I* hNdigis;
//	TH1I* hClusMultiplicity;
//	TH1D* hTimeDifference;
	TH1D* hSingle;
	TH1D* hDistancesToSingleHitClusters;
//	TH1D* hRadDiff;
//	TH1D* hPosDiff;

	Bool_t fStoreClusters;
	Bool_t fStoreClusterBase; 	//restore digis in case of a timebased run
	Bool_t fRemoveLowEclus;

	/** Get parameter containers **/
	virtual void SetParContainers();

	TStopwatch fTimer;
	int counter;
	double stoptime;

	TVector3* fFactor;

	ClassDef(PndEmcDistributedClustering, 2)
};
#endif // PNDEMCDISTRIBUTEDCLUSTERING_HH
