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
#ifndef PNDEMCMERGEPRECLUSTERS_H
#define PNDEMCMERGEPRECLUSTERS_H

#include "FairTask.h"
#include <vector>		

#include "FairTSBufferFunctional.h"
#include "TStopwatch.h"
#include "TH1.h"

class TClonesArray;
class TObjectArray;
class PndEmcDigi;
class PndEmcCluster;
class PndEmcGeoPar;
class PndEmcRecoPar;
class BinaryFunctor;


class PndEmcMergePreclusters : public FairTask
{
public:

  // Constructors

  PndEmcMergePreclusters(Int_t verbose=0, Bool_t storeclusters=kTRUE);

  // Destructor

  virtual ~PndEmcMergePreclusters( );

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void FinishTask();

  void SetStorageOfData(Bool_t val); 	//!< Method to specify whether clusters are stored or not.

  /// Restoring digis makes only sense if reconstruction is done timebased	
  void StoreClusterBaseDigis(Bool_t val = kTRUE) { fStoreClusterBase = kTRUE; }
 
  ///  Override EmcRecoPar timebunch cutting parameter - to be set in ns!
  void SetTimebunchCutTime(Double_t time) { fTimebunchCutTime = time; fAutoTime=kFALSE; } 
  void SetPreclusterTimeThreshold(Double_t dt) { fClusterActiveTime = dt; }
  void SetAutoDetermineTimecuts(Bool_t autotime) {fAutoTime=!autotime; }

  /// Set minimum cluster energy
  void SetClusterMinimumEnergy(Double_t minE) { fClusterEnergyCut = minE; }
  void EnableRemovalOfLowEnergyClusters(Bool_t enable) {fRemoveLowEclus = enable;} /** Enable/disable removal of low energy clusters. Significantly slows down cluster finding task, but will speed up future processing and reconstruction tasks **/

  void SetPositionMethod(Int_t method) { fPosMethod = method; } /** Set cluster position method: 
			0 = default method (logarithmic weighing), 
			1 = simplified method (xpos=(xmax+xmin)/2, ypos=(ymax+ymin)/2) using the REAL x,y position, 
			2 = simplified method (xpos=(xmax+xmin)/2, ypos=(ymax+ymin)/2) using the MAPPED XPad,YPad position, 
			3 = simplified method using the REAL x,y position of the DIGI WITH HIGHEST ENERGY,
			4 = simplified method using the MAPPED XPad,YPad position of the DIGI WITH HIGHEST ENERGY.
		**/
  void SetNeighbourMethod(Int_t nbmethod) { fNbMethod = nbmethod; } /** Set cluster neighbour nbmethod: 
			0 = default method (for position, logarithmic weighing; for radius, distance of digi furthest from position) -> use this CIRCLE for neighbour relations),  
			1 = simplified method (for position, xpos=(xmax+xmin)/2 ypos=(ymax+ymin)/2; for radius, r=max(ysize, xsize) -> use this CIRCLE for neighbour relations), 
			2 = simplified method (for position, xpos=(xmax+xmin)/2 ypos=(ymax+ymin)/2; for radius, rx=(xmax-xmin)/2, ry=(ymax-ymin)/2 -> use this RECTANGULAR BOX for neighbour relations). 
			3 = simplified method (for position, xpos=(xmax+xmin)/2 ypos=(ymax+ymin)/2; for radius, r=max(ysize, xsize) -> use this SQUARE BOX for neighbour relations). 
		**/

protected:
  virtual void FinishClusters();
  void FinishCluster(PndEmcCluster* tmpcluster);
  virtual void RemoveLowEnergyClusters();
  
private:
	/** Input array of preclusters and corresponding CbmDigis **/
	TClonesArray* fDigiArray;
	TClonesArray* fPreclusterArray;

	/** Output array of PndEmcClusters **/
	TClonesArray* fClusterArray;	//!< active clusters

	PndEmcGeoPar*     fGeoPar;       /** Geometry parameter container **/
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	
	Double_t fClusterEnergyCut;
	
	Double_t fTimebunchCutTime; //!< Defines how much time there should be between timebunches in timebased reconstruction
	Double_t fClusterActiveTime; //!< Set time threshold for separating digis within a timebunch
	BinaryFunctor* fClusterFunctor;

	std::vector<Double_t> fClusterPosParam;

	Int_t fPosMethod;
	Int_t fNbMethod;
	Int_t evtCounter;
	Int_t precCounter;
	Int_t fNrOfPres;
	Int_t nMrgProg;
	Int_t nTotClusters;
	Int_t fRemovedClusters;
	Int_t nTotDigis;
	Int_t wrongConnection;
	Double_t CNtotRtime;
	Double_t CNtotCtime;

	TH1D* hDx;
	TH1D* hDy;
	TH1D* hDz;
	TH1D* hTimeDifference;

	Bool_t fStoreClusters;
	Bool_t fStoreClusterBase; 	//restore digis in case of a timebased run
	Bool_t fRemoveLowEclus;
	Bool_t fAutoTime; // toggle manual overwriting of timebunch cut time

	/** Get parameter containers **/
	virtual void SetParContainers();

	TStopwatch fTimer;

	ClassDef(PndEmcMergePreclusters, 2)
};
#endif // PNDEMCMERGEPRECLUSTERS_HH
