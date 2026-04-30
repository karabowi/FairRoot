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
//	Class PndEmcInspectClusters
//      Displays clusters and their content.
//
// Environment:
//	Software developed for the PANDA experiment at GSI.
//
// Author List:
//      Marcel Tiemens
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCINSPECTCLUSTERS_H
#define PNDEMCINSPECTCLUSTERS_H

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
#include "TH1I.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TColor.h"
#include "TStyle.h"
#include "TStopwatch.h"
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
class PndEmcInspectClusters : public FairTask {
 public:
  // Constructor
  PndEmcInspectClusters(Int_t verbose = 0);
  // Destructor
  virtual ~PndEmcInspectClusters();

  // Methods
  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);
  virtual void FinishTask();

  /*
  private:
    // don't allow copying (-Weffc++)
    PndEmcInspectClusters(const PndEmcInspectClusters&);	// no implementation
    PndEmcInspectClusters& operator= (const PndEmcInspectClusters&);	// no implementation
  */
 private:
  /** Input array of PndEmcCluster%s **/
  TClonesArray *fClusterArray;

  PndEmcGeoPar *fGeoPar;   //< Geometry parameter container
  PndEmcDigiPar *fDigiPar; //< Digitisation parameter container
  PndEmcRecoPar *fRecoPar; //< Reconstruction parameter container

  std::vector<Double_t> fClusterPosParam;

  Int_t fEventCounter;
  Int_t fNrOfEvents;
  Int_t fNrOfDigis;
  Int_t fClusterCounter;
  Int_t nInspProg;
  Double_t stoptime;

  TStopwatch fTimer;

  // declare histograms to be filled
  TH1I *hEventMultiplicity;

  /** Get parameter containers **/
  virtual void SetParContainers();

  /* Verbosity level */
  // Int_t fVerbose;	//do not shadow FairTask::fVerbose

  ClassDef(PndEmcInspectClusters, 1);
};
#endif
