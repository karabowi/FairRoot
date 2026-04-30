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
#ifndef PNDEMCPACKCLUSTERS_H
#define PNDEMCPACKCLUSTERS_H

#include "FairTask.h"
#include <vector>		

#include "FairTSBufferFunctional.h"
#include "TStopwatch.h"

class TClonesArray;
class TObjectArray;
class PndEmcDigi;
class PndEmcCluster;
class PndEmcRecoPar;
class BinaryFunctor;


class PndEmcPackClusters : public FairTask
{
public:

  // Constructors

  PndEmcPackClusters(Int_t verbose=0);

  // Destructor

  virtual ~PndEmcPackClusters( );

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void FinishTask();
 
  ///  Override EmcRecoPar timebunch cutting parameter - to be set in ns!
  void SetTimebunchCutTime(Double_t time) { fTimebunchCutTime = time; fAutoTime=kFALSE; } 
  
private:

	/** Output array of PndEmcClusters **/
	TClonesArray* fDigiArray;		//!< input digis
	TClonesArray* fClusterArray;	//!< active clusters
	TClonesArray* fDigiOutArray;	//!< active digis

	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/

	Double_t fTimebunchCutTime; //!< Defines how mcuh time there should be between clusters (should be imported from clustering algorithm)
	BinaryFunctor* fClusterFunctor;

	Int_t fNrOfEvents;

	/** Get parameter containers **/
	virtual void SetParContainers();

        PndEmcPackClusters(const  PndEmcPackClusters& L);
        PndEmcPackClusters& operator= (const  PndEmcPackClusters&) {return *this;};

	TStopwatch fTimer;
	
	Bool_t fAutoTime;  // toggle manual overwriting of timebunch cut time

	ClassDef(PndEmcPackClusters, 1)
};
#endif // PNDEMCPACKCLUSTERS_HH
