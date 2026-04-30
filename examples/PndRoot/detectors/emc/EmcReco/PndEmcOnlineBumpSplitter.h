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
//	Class PndEmcOnlineBumpSplitter
//      Implementation of OnlineBumpSplitter which splits clusters based on
//      local maxima, determined in a 0th order approximation, i.e. by looking
//		for the two most energetic digis and checking if they are neighbours.
//
// Environment:
//	Software developed for the PANDA experiment at GSI.
//
// Author List:
//      Marcel Tiemens
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCONLINEBUMPSPLITTER_H
#define PNDEMCONLINEBUMPSPLITTER_H

// Path of file:
// ----- $pandaroot/emc/EmcReco

//---------------
// C++ Headers --
//---------------
#include <vector>
#include <map>

#include "FairTask.h"
#include "TObject.h"
#include "TClonesArray.h"
#include "PndEmcDataTypes.h"
//#include "PndEmcDigiCalibrator.h"
//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

class PndEmcCluster;
class PndEmcBump;
class PndEmcDigi;
class PndEmcTwoCoordIndex;

class PndEmcGeoPar;
class PndEmcDigiPar;
class PndEmcRecoPar;

class PndEmcSharedDigi;

/**
 * @brief splits clusters based on local maxima in the Phi
 * direction for use with Bremstrahlung correction.
 * @ingroup PndEmc
 */
class PndEmcOnlineBumpSplitter : public FairTask {
 public:
  // Constructor
  PndEmcOnlineBumpSplitter(Int_t verbose = 0);
  // Destructor
  virtual ~PndEmcOnlineBumpSplitter();

  // Methods
  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);
  virtual void FinishTask();

  void SetStorageOfData(Bool_t p = kTRUE) { fPersistance = p; };
  PndEmcBump *AddBump();
  void SetMinimumSeedEnergy(Double_t minE) { fSeedEnergyThreshold = minE; };
  void SingleBumpCluster(PndEmcCluster *, Int_t);

 protected:
  /** Get parameter containers **/
  virtual void SetParContainers();

 private:
  // don't allow copying (-Weffc++)
  PndEmcOnlineBumpSplitter(const PndEmcOnlineBumpSplitter &);            // no implementation
  PndEmcOnlineBumpSplitter &operator=(const PndEmcOnlineBumpSplitter &); // no implementation

 private:
  /** Input array of PndEmcDigi%s **/
  TClonesArray *fDigiArray;
  TClonesArray *fEmcDigi;
  TClonesArray *fClusterArray;

  /** Output array of PndEmcBump%s **/
  TClonesArray *fBumpArray;

  PndEmcGeoPar *fGeoPar;   //< Geometry parameter container
  PndEmcDigiPar *fDigiPar; //< Digitisation parameter container
  PndEmcRecoPar *fRecoPar; //< Reconstruction parameter container

  std::vector<Double_t> fClusterPosParam;

  Double_t fSeedEnergyThreshold; //!< Minimal seed digi energy to be considered as a bump. Default value = 0.03 GeV (default minimal cluster energy value)

  Bool_t fPersistance; //!< switch to turn on/off storing the arrays to a file

  /* Verbosity level */
  // Int_t fVerbose;	//do not shadow FairTask::fVerbose

  // added for time information
  // PndEmcDigiCalibrator digiCalibrator;
  // Int_t HowManyDidis;

  ClassDef(PndEmcOnlineBumpSplitter, 1);
};
#endif
