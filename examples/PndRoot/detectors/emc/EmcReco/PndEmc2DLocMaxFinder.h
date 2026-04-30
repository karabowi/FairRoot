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
#ifndef PNDEMC2DLOCALMAXFINDER_H
#define PNDEMC2DLOCALMAXFINDER_H

#include <PndPersistencyTask.h>
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
class PndEmc2DLocMaxFinder : public PndPersistencyTask {
 public:
  // Constructors
  PndEmc2DLocMaxFinder(Int_t verbose = 0);
  // Destructor
  virtual ~PndEmc2DLocMaxFinder();

  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);

  void SetStorageOfData(Bool_t p = kTRUE) { SetPersistency(p); };

 protected:
  /** Get parameter containers **/
  virtual void SetParContainers();
  /** Test to see whether the given Digi is a local maximum, within the set of
   * crystals given in "amongstTheseNeighbours".  Note that this set need
   * all be crystals that actually were within the cluster. */
  virtual bool isALocalMax(const PndEmcDigi *const, const PndEmcCluster *const, const PndEmcCoordIndexSet &amongstTheseNeighbours) const;

 private:
  // Methods
  void getNeighbourDigis(PndEmcCoordIndexSet &, PndEmcCoordIndexSet &, int, std::map<Int_t, Int_t>) const;
  // don't allow copying (-Weffc++)
  PndEmc2DLocMaxFinder(const PndEmc2DLocMaxFinder &);            // no implementation
  PndEmc2DLocMaxFinder &operator=(const PndEmc2DLocMaxFinder &); // no implementation

  /** Input array of PndEmcClusters **/
  TClonesArray *fClusterArray;
  /** Input array of PndEmcDigis **/
  TClonesArray *fDigiArray;

  PndEmcGeoPar *fGeoPar;   //!< Geometry parameter container
  PndEmcDigiPar *fDigiPar; //!< Digitisation parameter container
  PndEmcRecoPar *fRecoPar; //!< Reconstruction parameter container

  // Data members
  Double_t fMaxECut;
  Double_t fNeighbourECut;
  Double_t fCutSlope;
  Double_t fCutOffset;
  Double_t fERatioCorr;
  Int_t fTheNeighbourLevel;

  /* Verbosity level */
  // Int_t fVerbose;	//do not shadow PndPersistencyTask::fVerbose

  ClassDef(PndEmc2DLocMaxFinder, 2);
};
#endif // PNDEMC2DLOCALMAXFINDER_HH
