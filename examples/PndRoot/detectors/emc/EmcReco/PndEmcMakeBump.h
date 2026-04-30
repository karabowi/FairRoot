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
//      This module takes Clusters (Connected Regions) and slits them
//      up into Bumps. They are defined by local maxima
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen Gowdy           University of Edinburgh
//      Phil Strother           Imperial College
//
// Dima Melnychuk, adaption for PANDA
// Modified:
// M. Babai
//------------------------------------------------------------------------
#ifndef PNDEMCMAKEBUMP_H
#define PNDEMCMAKEBUMP_H

#include <PndPersistencyTask.h>

class TClonesArray;
class TObjectArray;

class PndEmcCluster;
class PndEmcDigi;
class PndEmcSharedDigi;
class PndEmcBump;
class PndEmcTwoCoordIndex;

/**
 * @brief Takes clusters and slits them up into bumps
 * 
 * Consists of the subtasks PndEmc2DLocMaxFinder, PndEmcExpClusterSplitter and PndEmcPhiBumpSplitter.
 * This task is mostly empty and just calls the subtasks.
 * @ingroup PndEmc
 */
class PndEmcMakeBump  : public PndPersistencyTask
{
public:
  // Constructors
  PndEmcMakeBump(Int_t verbose=0, Bool_t storebumps=kTRUE);
  // Destructor
  virtual ~PndEmcMakeBump( );

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  
  void SetStorageOfData(Bool_t val); //!< Method to specify whether bumps are stored or not.
  
protected:
  /** Get parameter containers **/
  virtual void SetParContainers();

private:
  /** Verbosity level **/
  Int_t fVerbose;
  Bool_t fPersistance;
  static Int_t fEventCounter;
  static Int_t fNrOfEvents;
  
  ClassDef(PndEmcMakeBump,2);
};
#endif //PNDEMCMAKEBUMP_HH
