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
//	Class PndEmcViewClusters
//      Displays clusters and their content.
//
// Environment:
//	Software developed for the PANDA experiment at GSI.
//
// Author List:
//      Marcel Tiemens
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCVIEWCLUSTERS_H
#define PNDEMCVIEWCLUSTERS_H

// Path of file:
// ----- $pandaroot/emc/EmcReco

//---------------
// C++ Headers --
//---------------
#include <vector>
#include <map>

#include "FairTask.h"
#include "TObject.h"
#include "PndEmcDataTypes.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TColor.h"
#include "TStyle.h"
#include "TClonesArray.h"
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
class PndEmcViewClusters : public FairTask {
 public:
  // Constructor
  PndEmcViewClusters(Int_t verbose = 0);
  // Destructor
  virtual ~PndEmcViewClusters();

  // Methods
  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);
  virtual void FinishTask();

  void SetStorageOfData(Bool_t p = kTRUE) { fPersistance = p; };

  void SetClusterEnergyThreshold(Double_t ethres) { Ethres = ethres; }
  void SetMode(Int_t mode) { fMode = mode; }

 protected:
  /** Get parameter containers **/
  virtual void SetParContainers();

 private:
  // don't allow copying (-Weffc++)
  PndEmcViewClusters(const PndEmcViewClusters &);            // no implementation
  PndEmcViewClusters &operator=(const PndEmcViewClusters &); // no implementation

 private:
  /** Input array of PndEmcDigi%s **/
  TClonesArray *fClusterArray;

  PndEmcGeoPar *fGeoPar;   //< Geometry parameter container
  PndEmcDigiPar *fDigiPar; //< Digitisation parameter container
  PndEmcRecoPar *fRecoPar; //< Reconstruction parameter container

  std::vector<Double_t> fClusterPosParam;

  Double_t Ethres;
  Int_t fMode;

  // declare histograms to be filled
  TH1F *h1;
  TH2D *h2;

  TCanvas *c2;

  Bool_t fPersistance; //!< switch to turn on/off storing the arrays to a file

  /* Verbosity level */
  // Int_t fVerbose;	//do not shadow FairTask::fVerbose

  ClassDef(PndEmcViewClusters, 1);
};
#endif
