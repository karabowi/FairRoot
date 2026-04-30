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
//	Class BSEmcPhiRecoHitSplitter
//      Implementation of PhiRecoHitSplitter which splits clusters based on
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
#ifndef BSEMCPHIRECOHITSPLITTER_HH
#define BSEMCPHIRECOHITSPLITTER_HH

// Path of file:
// ----- $pandaroot/emc/EmcReco

#include <PndPersistencyTask.h>
#include <map>
#include <memory>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

//---------------
// C++ Headers --
//---------------
#include "TObject.h"
#include "TString.h"

#include "FairTask.h"

#include "PndTCAConstContainer.h"
#include "PndTCAMutableContainer.h"

#include "BSEmcCluster.h"
#include "BSEmcCrystalPositionPar.h"
#include "BSEmcDigi.h"
#include "BSEmcRecoHit.h"

class BSEmcCrystalPositionPar;
class TBuffer;
class TClass;
class TMemberInspector;
template <class T>
class PndTCAConstContainerI;
template <class T>
class PndMutableContainerI;
//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

class BSEmcCluster;
class BSEmcRecoHit;
class BSEmcDigi;

/**
 * @class BSEmcPhiRecoHitSplitter
 * @brief splits clusters based on local maxima in the Phi direction for use with Bremstrahlung correction.
 * @details new version of PndEmcPhiBumpSplitter
 * @author Phil Strother
 * @author Binsong Ma
 * @author Ermias Atomssa
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPhiRecoHitSplitter : public PndPersistencyTask {
 public:
  // Constructor
  BSEmcPhiRecoHitSplitter(Int_t t_verbose = 0);
  // Destructor
  virtual ~BSEmcPhiRecoHitSplitter();

  // Methods
  virtual InitStatus Init() /*override*/;
  virtual void Exec(Option_t *t_opt) /*override*/;
  virtual void FinishTask() /*override*/;

  void SetStorageOfData(Bool_t t_p = kTRUE) { SetPersistency(t_p); };
  void CreatePhiHits(const std::vector<const BSEmcDigi *> &t_digiArray, const std::vector<const BSEmcCluster *> &t_clusterArray, const TString &t_clusterBranchName,
                     BSEmcCrystalPositionPar *t_positionPar);

 protected:
  /** Get parameter containers **/
  virtual void SetParContainers() /*override*/;
  BSEmcRecoHit *AddPhiRecoHit();

 private:
  // don't allow copying (-Weffc++)
  BSEmcPhiRecoHitSplitter(const BSEmcPhiRecoHitSplitter &);            // no implementation
  BSEmcPhiRecoHitSplitter &operator=(const BSEmcPhiRecoHitSplitter &); // no implementation

 private:
  /** Input array of BSEmcDigi%s **/
  PndTCAConstContainer<BSEmcDigi> *fBarrelDigiArray{new PndTCAConstContainer<BSEmcDigi>()};
  PndTCAConstContainer<BSEmcDigi> *fFwEndcapDigiArray{new PndTCAConstContainer<BSEmcDigi>()};
  PndTCAConstContainer<BSEmcDigi> *fBwEndcapDigiArray{new PndTCAConstContainer<BSEmcDigi>()};
  PndTCAConstContainer<BSEmcDigi> *fShashlikDigiArray{new PndTCAConstContainer<BSEmcDigi>()};
  PndTCAConstContainer<BSEmcCluster> *fBarrelClusterArray{new PndTCAConstContainer<BSEmcCluster>()};
  PndTCAConstContainer<BSEmcCluster> *fFwEndcapClusterArray{new PndTCAConstContainer<BSEmcCluster>()};
  PndTCAConstContainer<BSEmcCluster> *fBwEndcapClusterArray{new PndTCAConstContainer<BSEmcCluster>()};
  PndTCAConstContainer<BSEmcCluster> *fShashlikClusterArray{new PndTCAConstContainer<BSEmcCluster>()};

  /** Output array of Phi RecoHits%s **/
  PndTCAMutableContainer<BSEmcRecoHit> *fPhiRecoHitArray{new PndTCAMutableContainer<BSEmcRecoHit>{"Emc", kTRUE}};
  BSEmcCrystalPositionPar *fBarrelPosition{nullptr};
  BSEmcCrystalPositionPar *fFWECPosition{nullptr};
  BSEmcCrystalPositionPar *fBWECPosition{nullptr};
  BSEmcCrystalPositionPar *fShashlikPosition{nullptr};
  ClassDef(BSEmcPhiRecoHitSplitter, 2);
};
#endif /*BSEMCPHIRECOHITSPLITTER_HH*/
