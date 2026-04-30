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
// Description:
//	Class EmcERatioLocMaxMaxFinder./
//      Searches for local maxima in a cluster based on the ratio
//      between the energy of the maxima crystal and that of
//      its neighbours
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother
//      Helmut Schmuecker                   Ruhr-Uni Bochum
//
// Copyright Information:
//	Copyright (C) 1997	            Imperial College
// Modified:
// M. Babai
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMC2DLOCALMAXFINDERTEST_H
#define PNDEMC2DLOCALMAXFINDERTEST_H

#include "FairTask.h"
#include "TObject.h"
#include "TClonesArray.h"
#include "PndEmcDataTypes.h"

class PndEmcDigi;
class PndEmcCluster;
class PndEmcTwoCoordIndex;
class PndEmcGeoPar;
class PndEmcDigiPar;
class PndEmcRecoPar;

/**
 * @brief Searches for local maxima in a cluster
 * @ingroup PndEmc
 */
class PndEmc2DLocMaxFinderTEST : public FairTask {
 public:
  // Constructors
  PndEmc2DLocMaxFinderTEST(Int_t verbose = 0);
  // Destructor
  virtual ~PndEmc2DLocMaxFinderTEST();

  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);
  virtual void FinishTask();

  void SetStorageOfData(Bool_t p = kTRUE) { fPersistance = p; };

 protected:
  /** Get parameter containers **/
  virtual void SetParContainers();
  /** Test to see whether the given Digi is a local maximum, within the set of
   * crystals given in "amongstTheseNeighbours".  Note that this set need
   * all be crystals that actually were within the cluster. */
  virtual bool isALocalMax(const PndEmcDigi *const, // const PndEmcCluster * const,
                           //			    const PndEmcCoordIndexSet &amongstTheseNeighbours ) const;
                           std::vector<Int_t> &amongstTheseNeighbours) const; /////

 private:
  // Methods
  //  void getNeighbourDigis( PndEmcCoordIndexSet &, PndEmcCoordIndexSet &, int,
  //			  std::map<Int_t, Int_t>) const;
  void getNeighbourDigis(std::vector<Int_t> &, PndEmcCoordIndexSet &, int, Int_t, Int_t) const; /////

  // don't allow copying (-Weffc++)
  PndEmc2DLocMaxFinderTEST(const PndEmc2DLocMaxFinderTEST &);            // no implementation
  PndEmc2DLocMaxFinderTEST &operator=(const PndEmc2DLocMaxFinderTEST &); // no implementation

  /** Input array of PndEmcClusters **/
  TClonesArray *fClusterArray;
  /** Input array of PndEmcDigis **/
  TClonesArray *fDigiArray;
  TClonesArray *fEmcDigi; ///// // (for retrieving EmcDigis using FairLinks)

  PndEmcGeoPar *fGeoPar;   //!< Geometry parameter container
  PndEmcDigiPar *fDigiPar; //!< Digitisation parameter container
  PndEmcRecoPar *fRecoPar; //!< Reconstruction parameter container

  Bool_t fPersistance;

  // Data members
  Double_t fMaxECut;
  Double_t fNeighbourECut;
  Double_t fCutSlope;
  Double_t fCutOffset;
  Double_t fERatioCorr;
  Int_t fTheNeighbourLevel;

  /* Verbosity level */
  // Int_t fVerbose;	//do not shadow FairTask::fVerbose

  ClassDef(PndEmc2DLocMaxFinderTEST, 2);
};
#endif // PNDEMC2DLOCALMAXFINDERTEST_HH
