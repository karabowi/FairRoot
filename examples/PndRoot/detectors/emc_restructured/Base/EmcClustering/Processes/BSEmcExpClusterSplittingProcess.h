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
//	Class BSEmcExpClusterSplittingProcess.
//      Concrete implementation of BSEmcAbsClusterSplitter which splits
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

#ifndef BSEMCEXPCLUSTERSPLITTINGPROCESS_HH
#define BSEMCEXPCLUSTERSPLITTINGPROCESS_HH

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"
#include "TVector3.h"

#include "PndContainerI.h"
#include "PndProcess.h"

#include "BSEmcCluster.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcCrystalPositionPar.h"
#include "BSEmcEnergyCalculationAlgo.h"
#include "BSEmcPositionAlgo.h"

class BSEmcDigi;
class BSEmcCluster;
class BSEmcSubCluster;
class FairParGenericSet;
class BSEmcClusteringPar;
class BSEmcCrystalPositionPar;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcClusteringData;
template <class T>
class PndContainerI;
template <class T>
class PndMutableContainerI;

struct DigiLocationInfo : public BSEmcDigiInfo_t {
  Double_t fEnergy{0};
  TVector3 fLocation{0, 0, 0};
  Double_t fSubClusterEnergy{0};
};

/**
 * @class BSEmcExpClusterSplittingProcess
 * @brief splits clusters on the basis of exponential distance from the subCluster centroid
 * @details to be run after BSEmcLocalMaxFindingProcess or BSEmcRemoveSplitOffMax
 * @author Phil Strother
 * @author M. Babai
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcExpClusterSplittingProcess : public PndProcess {
 public:
  // Constructor
  BSEmcExpClusterSplittingProcess();
  // Destructor
  virtual ~BSEmcExpClusterSplittingProcess();

  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_paramRegister) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  std::vector<BSEmcSubCluster *> SubClustering(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcCluster *> &t_clusterCont);
  void SetClusteringParName(const std::string &t_parName) { fClusteringParName = t_parName; }
  void SetClusterPropertiesParName(const std::string &t_parName) { fClusterPropertiesParName = t_parName; }
  void SetPositionParName(const std::string &t_parName) { fPositionParName = t_parName; }
  void SetNeighbouringRelationParName(const std::string &t_parName) { fNeighbouringRelationParName = t_parName; }

  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchname = t_branchname; }
  void SetClusterBranchName(const TString &t_branchname) { fClusterBranchname = t_branchname; }
  void SetSubClusterBranchName(const TString &t_branchname) { fSubClusterBranchname = t_branchname; }

 private:
  std::string fClusteringParName{""};
  std::string fClusterPropertiesParName{""};
  std::string fPositionParName{""};
  std::string fNeighbouringRelationParName{""};

  TString fDigiBranchname{""};
  TString fClusterBranchname{""};
  TString fSubClusterBranchname{""};

  PndContainerI<BSEmcDigi> *fDigiArray{nullptr};
  PndMutableContainerI<BSEmcCluster> *fClusterArray{nullptr};
  PndMutableContainerI<BSEmcSubCluster> *fSubClusterArray{nullptr};
  std::vector<BSEmcSubCluster *> fSubClusterVector{};

  Double_t fExponentialConstant{-1};
  Int_t fMaxIterations{-1};
  Double_t fCentroidShift{-1};
  Int_t fMaxSubClusters{-1};
  Double_t fMinDigiEnergy{-1};

  std::unique_ptr<BSEmcPositionAlgo> fPositionProcess{new BSEmcPositionAlgo};
  std::unique_ptr<BSEmcEnergyCalculationAlgo> fEnergyProcess{new BSEmcEnergyCalculationAlgo};
  Double_t fClusterEnergyCut{-1};

 protected:
  Double_t fMoliereRadius{-1};
  BSEmcCrystalPositionPar *fPositionPar{nullptr};

 private:
  std::vector<DigiLocationInfo> GetEnergyAndLocationInfo(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcDigiInfo_t> &t_digis) const;
  void ClearSubClusters(std::map<Int_t, BSEmcSubCluster *> &t_tmpsubClusters) const;
  virtual Double_t
  CalculateWeight(Int_t t_currentDigiDetID, Int_t t_currentMaxDetId, const TVector3 &t_currentdigisPosition, const std::map<Int_t, DigiLocationInfo> &t_centroidPositions) const;
  void UpdateCentroidPositions(const std::map<Int_t, BSEmcSubCluster *> &t_subClusters, std::map<Int_t, DigiLocationInfo> &t_centroidPositions) const;
  void AddDigiWithWeightToSubCluster(Int_t t_digiIdx, Double_t t_weight, BSEmcSubCluster *t_subcluster);
  void AddSubClusters(const std::map<Int_t, BSEmcSubCluster *> &t_subClusters, const std::vector<BSEmcCluster *> &t_clusterCont, Int_t t_clusterIdx);
  void SplitIntoMultipleSubCluster(BSEmcCluster *t_cluster, Int_t t_clusterIdx, const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcCluster *> &t_clusterCont);
  void SplitIntoSingleSubCluster(BSEmcCluster *t_cluster, Int_t t_clusterIdx);

  ClassDef(BSEmcExpClusterSplittingProcess, 2);
};
#endif /*BSEMCEXPCLUSTERSPLITTINGPROCESS_HH*/
