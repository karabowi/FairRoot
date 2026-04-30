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

#ifndef BSEMCCLUSTERINGPROCESS_HH
#define BSEMCCLUSTERINGPROCESS_HH

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndContainerI.h"
#include "PndProcess.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcEnergyCalculationAlgo.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcPositionAlgo.h"

class BSEmcDigi;
class BSEmcCluster;
class BSEmcSubCluster;
class BSEmcClusteringPar;
class BSEmcGeoNeighbouringRelationPar;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcClusteringData;
template <class T>
class PndContainerI;
template <class T>
class PndMutableContainerI;

struct DigiClusteringCandidate {
  Int_t DigiIdx{-1}; // digi index in fDigis
  Int_t NNeighbours{0};
  std::vector<Int_t> Neighbours{}; // indices of neighbours in fDigiCandidates
  Int_t ClusterNumber{-1};
  Double_t Time{0};
  Int_t DetectorId{-1};
};

/**
 * @class BSEmcClusteringProcess
 * @brief Rewritten version of old PndEmcMakeCluster algorithm for easier understanding and debugging
 * @details
 * @author Jan Zhong
 * @author Marcel Tiemens
 * @author Áron Kripkó
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcClusteringProcess : public PndProcess {
 public:
  BSEmcClusteringProcess();
  virtual ~BSEmcClusteringProcess();

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
  std::string fDigiBranchName{""};
  Double_t fClusterActiveTime{-1};
  Double_t fDigiEnergyThreshold{-1};

  TString fDigiBranchname{""};
  TString fClusterBranchname{""};

  PndContainerI<BSEmcDigi> *fDigiArray{nullptr};
  PndMutableContainerI<BSEmcCluster> *fClusterArray{nullptr};

  std::vector<DigiClusteringCandidate> fDigiCandidates{};
  Int_t fNClusters{0};
  Int_t fNDigisPassed{-1};
  std::vector<std::pair<Int_t, Int_t>> fClusterNumbersForSameCluster{};
  BSEmcGeoNeighbouringRelationPar *fNeighbouringRelationPar{nullptr};
  std::unique_ptr<BSEmcPositionAlgo> fPositionProcess{new BSEmcPositionAlgo};
  std::unique_ptr<BSEmcEnergyCalculationAlgo> fEnergyProcess{new BSEmcEnergyCalculationAlgo};

 protected:
  void CreateClusterableDigiCandidates(const std::vector<const BSEmcDigi *> &t_digis);
  void FindNeighboursOfEveryDigi();
  Bool_t AreDigisNeighbours(Int_t t_digi1, Int_t t_digi2) const;
  void PrimaryClustering();
  void SecondaryClustering();
  void CleanUpClusterNumbering();
  std::vector<BSEmcCluster *> CreateEmcCluster();
  void AssignNeighbouringDigisSameClusterNumber(DigiClusteringCandidate &t_digi);

  ClassDef(BSEmcClusteringProcess, 1);
};

#endif /*BSEMCCLUSTERINGPROCESS_HH*/
