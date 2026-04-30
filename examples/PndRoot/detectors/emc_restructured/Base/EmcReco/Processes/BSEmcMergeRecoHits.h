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

#ifndef BSEMCMERGERECOHITS_HH
#define BSEMCMERGERECOHITS_HH

#include "BSEmcCluster.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcEnergyCalculationAlgo.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcPositionAlgo.h"
#include "PndContainerI.h"
#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"
#include <vector>

class FairParGenericSet;
class BSEmcClusteringPar;

class BSEmcDigi;
class BSEmcCluster;
class BSEmcSubCluster;
class BSEmcRecoHit;

class BSEmcMergeRecoHits : public PndProcess {
 public:
  BSEmcMergeRecoHits();
  virtual ~BSEmcMergeRecoHits();
  virtual void SetDetectorName(const std::string &t_detectorName);
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister);
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;

  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchname = t_branchname; }
  void SetClusterBranchName(const TString &t_branchname) { fClusterBranchname = t_branchname; }
  void SetSubClusterBranchName(const TString &t_branchname) { fSubClusterBranchname = t_branchname; }
  void SetRecoHitBranchName(const TString &t_branchname) { fRecoHitBranchname = t_branchname; }

 protected:
  /**
   * @brief Calculate the distance of two given RecoHit positions.
   *
   * @param firstposition
   * @param secondposition
   * @return double - return distance between Reco Hits
   */
  virtual double CalculateDistance(const TVector3 &firstposition, const TVector3 &secondposition);

  /**
   * @brief Find the Reco Hit, which lies closest to the given RecoHit.
   *
   * @param CurrentRecoHit
   * @return Int_t - SubClusterId of the closest hit
   */
  virtual Int_t FindClosestHit(const BSEmcRecoHit *CurrentRecoHit);

  /**
   * @brief Combine the digis of the given SubCluster and the temporary
   * SubCluster and re-calculate energy and position.
   *
   * @param TmpSubCluster
   * @param SubCluster
   */
  virtual void FillTmpSubcluster(BSEmcSubCluster *TmpSubCluster, const BSEmcSubCluster *SubCluster);

  /**
   * @brief Update the energy values of the main RecoHit based on the temporary
   * SubCluster.
   *
   * @param MainSubCluster
   * @param MainSubClusterId
   */
  virtual void UpdateRecoHit(const BSEmcSubCluster *MainSubCluster, const Int_t MainSubClusterId);

  /**
   * @brief Remove SplitOff RecoHits and compress the RecoHitArray.
   *
   */
  virtual void CleanUpRecoHitArray();

  std::string fClusteringParName{""};
  std::string fClusterPropertiesParName{""};
  std::string fPositionParName{""};
  std::string fNeighbouringRelationParName{""};
  BSEmcGeoNeighbouringRelationPar *fNeighbouringRelationPar = nullptr;
  const BSEmcCrystalPositionPar *fPositionPar;

  TString fDigiBranchname{""};
  TString fClusterBranchname{""};
  TString fSubClusterBranchname{""};
  TString fRecoHitBranchname{""};

  PndContainerI<BSEmcDigi> *fDigiArray{nullptr};
  PndContainerI<BSEmcCluster> *fClusterArray{nullptr};
  PndContainerI<BSEmcSubCluster> *fSubClusterArray{nullptr};
  PndMutableContainerI<BSEmcRecoHit> *fRecoHitArray{nullptr};
  std::unique_ptr<BSEmcEnergyCalculationAlgo> fEnergyProcess{new BSEmcEnergyCalculationAlgo};
  std::unique_ptr<BSEmcPositionAlgo> fPositionProcess{new BSEmcPositionAlgo};
  Double_t fClusterEnergyCut;

  ClassDef(BSEmcMergeRecoHits, 1);
};

#endif /*BSEMCMERGERECOHITS_HH*/
