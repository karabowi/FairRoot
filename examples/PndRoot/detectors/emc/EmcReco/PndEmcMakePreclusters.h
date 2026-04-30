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
#ifndef PNDEMCMAKEPRECLUSTERS_H
#define PNDEMCMAKEPRECLUSTERS_H

#include "FairTask.h"
#include <vector>		

#include "FairTSBufferFunctional.h"
#include "TStopwatch.h"
#include "TH1.h"

class TClonesArray;
class TObjectArray;
class PndEmcDigi;
class PndEmcCluster;
class PndEmcPrecluster;
class PndEmcGeoPar;
class PndEmcRecoPar;
class BinaryFunctor;


class PndEmcMakePreclusters : public FairTask
{
public:

  // Constructors

  PndEmcMakePreclusters(Int_t verbose=0, Bool_t storeclusters=kTRUE);

  // Destructor

  virtual ~PndEmcMakePreclusters( );

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void FinishTask();

  void SetStorageOfData(Bool_t val); 	//!< Method to specify whether clusters are stored or not.

  /// Restoring digis makes only sense if reconstruction is done timebased	
  void StoreClusterBaseDigis(Bool_t val = kTRUE) { fStoreClusterBase = kTRUE; }
 
  /// Override EmcRecoPar cluster active time parameter ..to be set in ns!!! 
  void SetTimebunchCutTime(Double_t time) { fTimebunchCutTime = time; } 
  void SetClusterActiveTime(Double_t dt) { fClusterActiveTime = dt; }

  void SetPositionMethod(Int_t method) { fPosMethod = method; } /** Set precluster position and radius method: 0 = default method (for position, logarithmic weighing; for radius, distance of digi furthest from position), 1 = simplified method (for position, xpos=(xmax+xmin)/2 ypos=(ymax+ymin)/2; for radius, r=max(ysize, xsize)) for preclusters only, 2 = simplified method for preclusters and clusters. Using the simplified method is less accurate, but saves a very large amount of resources **/
  void SetDigiPositionType(Int_t type=-1) { fDigiPosType = type; } // select whether to use mapped or real digi position

  void SetAutoDetermineTimecuts(Bool_t autodet) {fAutoDetermine = autodet;}

protected:
  virtual void FinishPreclusters();
  void FinishPrecluster(PndEmcPrecluster* tmpcluster);
  
private:
	/** Input array of CbmDigis **/
	TClonesArray* fDigiArray;

	/** Output array of PndEmcClusters **/
	TClonesArray* fPreclusterArray;	//!< active clusters

	PndEmcGeoPar*     fGeoPar;       /** Geometry parameter container **/
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	
	Double_t fDigiEnergyTresholdBarrel; 
	Double_t fDigiEnergyTresholdFWD; 
	Double_t fDigiEnergyTresholdBWD; 
	Double_t fDigiEnergyTresholdShashlyk;
	
	Double_t fTimebunchCutTime; //!< Defines how long clusters are kept open in timebased reconstruction
	Double_t fClusterActiveTime; //!< Set time threshold for separating digis within a timebunch
	BinaryFunctor* fDigiFunctor;

	std::vector<Double_t> fClusterPosParam;

	Int_t fPosMethod;
	Int_t fDigiPosType;
	Int_t evtCounter;
	Int_t digiCounter;
	Int_t nTotDigisPassed;
	Int_t fNrOfDigis;
	Int_t fNrOfEvents;
	Int_t nPrecProg;
	Double_t DCtotRtime;
	Double_t DCtotCtime;

	TH1I* hNdigis;
//	TH1D* hRadDiff;
//	TH1D* hPosDiff;

	Bool_t fStoreClusters;
	Bool_t fStoreClusterBase; 	//restore digis in case of a timebased run
	Bool_t fAutoDetermine;

	/** Get parameter containers **/
	virtual void SetParContainers();

	TStopwatch fTimer;

	ClassDef(PndEmcMakePreclusters, 1)
};
#endif // PNDEMCMAKEPRECLUSTERS_HH
