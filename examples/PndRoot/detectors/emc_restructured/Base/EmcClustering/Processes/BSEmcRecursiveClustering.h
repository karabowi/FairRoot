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

#ifndef BSEMCRECURSIVECLUSTERING_HH
#define BSEMCRECURSIVECLUSTERING_HH

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndContainerI.h"
#include "PndProcess.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcEnergyCalculationAlgo.h"
#include "BSEmcGeoItem.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcPositionAlgo.h"

class BSEmcDigi;
class BSEmcCluster;
class BSEmcClusteringPar;
class PndParameterRegister;
class BSEmcGeoItem;
class BSEmcGeoNeighbouringRelationPar;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcClusteringData;
template <class T>
class PndContainerI;
template <class T>
class PndMutableContainerI;

/**
 * @class BSEmcRecursiveClustering
 * @brief Recursive clustering
 * @details Very simplistic spatial clustering using a seed crystal and recursively adding all neighbouring fired crystals
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup EmcBase
 */
class BSEmcRecursiveClustering : public PndProcess {
 public:
  BSEmcRecursiveClustering();
  virtual ~BSEmcRecursiveClustering();

  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_paramRegister) /*override*/;
  virtual void Process() /*override*/;
  std::vector<BSEmcCluster *> Clustering(const std::vector<const BSEmcDigi *> &t_digis);

  void SetClusteringParName(const std::string &t_parName) { fClusteringParName = t_parName; }
  void SetClusterPropertiesParName(const std::string &t_parName) { fClusterPropertiesParName = t_parName; }
  void SetPositionParName(const std::string &t_parName) { fPositionParName = t_parName; }
  void SetNeighbouringRelationParName(const std::string &t_parName) { fNeighbouringRelationParName = t_parName; }

  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchname = t_branchname; }
  void SetClusterBranchName(const TString &t_branchname) { fClusterBranchname = t_branchname; }

 protected:
  std::string fClusteringParName{""};
  std::string fClusterPropertiesParName{""};
  std::string fPositionParName{""};
  std::string fNeighbouringRelationParName{""};
  Double_t fClusterActiveTime{-1};
  Double_t fDigiEnergyThreshold{-1};

  TString fDigiBranchname{""};
  TString fClusterBranchname{""};

  PndContainerI<BSEmcDigi> *fDigiArray{nullptr};
  PndMutableContainerI<BSEmcCluster> *fClusterArray{nullptr};

  BSEmcGeoNeighbouringRelationPar *fNeighbouringRelationPar{nullptr};
  std::unique_ptr<BSEmcPositionAlgo> fPositionProcess{new BSEmcPositionAlgo};
  std::unique_ptr<BSEmcEnergyCalculationAlgo> fEnergyProcess{new BSEmcEnergyCalculationAlgo};
  std::map<Int_t, std::unique_ptr<BSEmcGeoItem>> fIDToCrystal{};
  std::set<BSEmcGeoItem *> fChangedCrystals{};

 protected:
  void FillGeoItems(const std::vector<const BSEmcDigi *> &t_digis);
  std::vector<std::vector<BSEmcGeoItem *>> SpatialClustering(const std::vector<const BSEmcDigi *> &t_digis);
  std::vector<BSEmcCluster *> ConvertToCluster(const std::vector<std::vector<BSEmcGeoItem *>> &t_clusterstubs, const std::vector<const BSEmcDigi *> &t_digis) const;
  void Reset();

  ClassDef(BSEmcRecursiveClustering, 1);
};

#endif /*BSEMCRECURSIVECLUSTERING_HH*/
