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
//	Class PndEmcPhiBumpSplitter
//      Implementation of PhiBumpSplitter which splits clusters based on
//      local maxima in the Phi Direction for use with Bremstrahlung correction
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
// Binsong Ma, Ermias Atomssa
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCPHIBUMPSPLITTER_H
#define PNDEMCPHIBUMPSPLITTER_H

// Path of file:
// ----- $pandaroot/emc/EmcReco

//---------------
// C++ Headers --
//---------------
#include <PndPersistencyTask.h>
#include <vector>
#include <map>

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
class PndEmcPhiBumpSplitter : public PndPersistencyTask {
 public:
  // Constructor
  PndEmcPhiBumpSplitter(Int_t verbose = 0);
  // Destructor
  virtual ~PndEmcPhiBumpSplitter();

  // Methods
  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);
  virtual void FinishTask();

  void SetStorageOfData(Bool_t p = kTRUE) { SetPersistency(p); };
  PndEmcBump *AddPhiBump();
  // PndEmcSharedDigi* AddPhiBumpSharedDigi(PndEmcDigi*, Double_t);

 protected:
  /** Get parameter containers **/
  virtual void SetParContainers();

 private:
  // don't allow copying (-Weffc++)
  PndEmcPhiBumpSplitter(const PndEmcPhiBumpSplitter &);            // no implementation
  PndEmcPhiBumpSplitter &operator=(const PndEmcPhiBumpSplitter &); // no implementation

 private:
  /** Input array of PndEmcDigi%s **/
  TClonesArray *fDigiArray;
  TClonesArray *fClusterArray;

  /** Output array of PndEmcBump%s **/
  TClonesArray *fPhiBumpArray;
  // TClonesArray* fPhiBumpSharedDigiArray;

  PndEmcGeoPar *fGeoPar;   //< Geometry parameter container
  PndEmcDigiPar *fDigiPar; //< Digitisation parameter container
  PndEmcRecoPar *fRecoPar; //< Reconstruction parameter container

  std::vector<Double_t> fClusterPosParam;

  Bool_t fPersistance; //!< switch to turn on/off storing the arrays to a file

  /* Verbosity level */
  // Int_t fVerbose;	//do not shadow FairTask::fVerbose

  // added for time information
  // PndEmcDigiCalibrator digiCalibrator;
  // Int_t HowManyDidis;

  ClassDef(PndEmcPhiBumpSplitter, 2);
};
#endif
