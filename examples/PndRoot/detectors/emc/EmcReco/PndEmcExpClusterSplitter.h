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

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class PndEmcExpClusterSplitter.
//      Concrete implementation of EmcAbsClusterSplitter which splits
//      on the basis of exponential distance from the bump centroid.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother               
//
// Copyright Information:
//	Copyright (C) 1997               Imperial College
//
// Modified:
// M. Babai
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCEXPCLUSTERSPLITTER_H
#define PNDEMCEXPCLUSTERSPLITTER_H

//---------------
// C++ Headers --
//---------------
#include <PndPersistencyTask.h>
#include <vector>
#include <map>
#include "TArrayD.h"                      
#include "TVector3.h"

#include "TObject.h"
#include "PndEmcDataTypes.h"
#include "PndEmcDigiCalibrator.h"
//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

class PndEmcCluster;
class PndEmcBump;
class PndEmcDigi;
class PndEmcSharedDigi;
class PndEmcTwoCoordIndex;

class PndEmcGeoPar;
class PndEmcDigiPar;
class PndEmcRecoPar;


/**
 * @brief splits clusters on the basis of exponential distance from the bump centroid
 * @ingroup PndEmc
 */
class PndEmcExpClusterSplitter: public PndPersistencyTask
{
public:
	// Constructor
	PndEmcExpClusterSplitter(Int_t verbose=0);
	// Destructor
	virtual ~PndEmcExpClusterSplitter( );
	
	// Methods
	virtual InitStatus Init();
	virtual void Exec(Option_t* opt);
	virtual void FinishTask();
	
	void SetStorageOfData(Bool_t p = kTRUE) {SetPersistency(p);};
	PndEmcBump* AddBump();
	PndEmcSharedDigi* AddSharedDigi(PndEmcDigi*, Double_t weight);
    Double_t LateralDevelopment(const Double_t TotEnergy, const TVector3 *DetPos, const TVector3 *SeedPos, const TVector3 *ShowerCenter, const Double_t RM);

protected:
	/** Get parameter containers **/
	virtual void SetParContainers();

private:
	// don't allow copying (-Weffc++)
	PndEmcExpClusterSplitter(const PndEmcExpClusterSplitter&);	// no implementation
	PndEmcExpClusterSplitter& operator= (const PndEmcExpClusterSplitter&);	// no implementation

private:
	/** Input array of PndEmcCluster%s **/
	TClonesArray* fDigiArray;
	TClonesArray* fClusterArray;
	
	/** Output array of PndEmcBump%s **/
	TClonesArray* fBumpArray;
	TClonesArray* fSharedDigiArray;

	PndEmcGeoPar*     fGeoPar;       //!< Geometry parameter container
	PndEmcDigiPar*    fDigiPar;      //!< Digitisation parameter container
	PndEmcRecoPar*    fRecoPar;      //!< Reconstruction parameter container
	
	std::vector<Double_t> fClusterPosParam;
	
	Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
	// Data members
	Double_t fMoliereRadius;
	Double_t fMoliereRadiusShashlyk;
	Double_t fExponentialConstant;
	TArrayD fParArray1;
	TArrayD fParArray2;
    TArrayD fParArray3;
    TArrayD fParArray4;

	Int_t fMaxIterations;
	Double_t fCentroidShift;
	Int_t fMaxBumps;
	Double_t fMinDigiEnergy;
	
	/* Verbosity level */
	// Int_t fVerbose;	//do not shadow PndPersistencyTask::fVerbose
	
	//added for time information
	PndEmcDigiCalibrator digiCalibrator;
	Int_t HowManyDidis;

	ClassDef(PndEmcExpClusterSplitter,2);
};
#endif // EMCABSCLUSTERSPLITTER_HH
