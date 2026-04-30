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
//	Class BSEmcOnlineBumpSplitter
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
#ifndef BSEMCONLINEBUMPSPLITTER_HH
#define BSEMCONLINEBUMPSPLITTER_HH

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "FairTask.h"

#include "PndContainerI.h"
#include "PndProcess.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcDigi.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcPositionAlgo.h"
#include "BSEmcSubCluster.h"

class BSEmcCluster;
class BSEmcCrystalPositionPar;
class BSEmcDigi;
class BSEmcGeoNeighbouringRelationPar;
class BSEmcSubCluster;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcClusteringData;
template <class T>
class PndContainerI;
template <class T>
class PndMutableContainerI;

/**
 * @class BSEmcOnlineBumpSplitter
 * @brief old @brief splits clusters based on local maxima in the Phi
 * direction for use with Bremstrahlung correction.
 * @details copied from Process() function description : Very fast and simple bump splitter, intended for online usage. For each cluster, the two most energetic digis are
 * retrieved. If they are not neighbours, and the energy difference is above a certain threshold, the cluster is split into bumps. Each bump has as location and timestamp that of
 * the retrieved high-energy digis. The cluster energy is divided according to the energy deposition in those digis.
 *
 * to be run after BSEmcLocalMaxFindingProcess or BSEmcRemoveSplitOffMax
 * @author Marcel Tiemens
 * @author Viktor Rodin
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcOnlineBumpSplitter : public PndProcess {
 public:
  BSEmcOnlineBumpSplitter();
  virtual ~BSEmcOnlineBumpSplitter();
  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_paramRegister) /*override*/;
  virtual void Process() /*override*/;
  virtual void PostProcess();
  std::vector<BSEmcSubCluster *> Subcluster(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcCluster *> &t_clusterCont);

  void SetMinimumSeedEnergy(Double_t minE) { fSeedEnergyThreshold = minE; };

  void SetPositionParName(const std::string &t_parName) { fPositionParName = t_parName; }
  void SetNeighbouringRelationParName(const std::string &t_parName) { fNeighbouringRelationParName = t_parName; }

  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchname = t_branchname; }
  void SetClusterBranchName(const TString &t_branchname) { fClusterBranchname = t_branchname; }
  void SetSubClusterBranchName(const TString &t_branchname) { fSubClusterBranchname = t_branchname; }

 protected:
  BSEmcSubCluster *AddBump();
  void SingleBumpCluster(BSEmcCluster *, Int_t, const std::vector<const BSEmcDigi *> &t_digiCont);

 private:
  TString fDigiBranchname{""};
  TString fClusterBranchname{""};
  TString fSubClusterBranchname{""};

  PndContainerI<BSEmcDigi> *fDigiArray{nullptr};
  PndMutableContainerI<BSEmcCluster> *fClusterArray{nullptr};
  PndMutableContainerI<BSEmcSubCluster> *fSubClusterArray{nullptr};
  std::vector<BSEmcSubCluster *> fSubClusterVec{};
  std::string fNeighbouringRelationParName{""};
  BSEmcGeoNeighbouringRelationPar *fNeighbouringRelationPar{nullptr};
  std::string fPositionParName{""};
  BSEmcCrystalPositionPar *fPositionPar{nullptr};
  std::unique_ptr<BSEmcPositionAlgo> fPositionProcess{new BSEmcPositionAlgo};
  Double_t fSeedEnergyThreshold{0.03}; //!< Minimal seed digi energy to be considered as a bump. Default value = 0.03 GeV (default minimal cluster energy value)

  ClassDef(BSEmcOnlineBumpSplitter, 1);
};
#endif /*BSEMCONLINEBUMPSPLITTER_HH*/
