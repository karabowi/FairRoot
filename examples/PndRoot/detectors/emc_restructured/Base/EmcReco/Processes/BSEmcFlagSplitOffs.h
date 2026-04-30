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

#ifndef BSEMCFLAGSPLITOFFS_HH
#define BSEMCFLAGSPLITOFFS_HH

#include "BSEmcCluster.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcSplitOffPar.h"
#include "PndContainerI.h"
#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"
#include "TF1.h"
#include <vector>

class FairParGenericSet;

class BSEmcDigi;
class BSEmcCluster;
class BSEmcSubCluster;
class BSEmcRecoHit;

class BSEmcFlagSplitOffs : public PndProcess {
 public:
  BSEmcFlagSplitOffs();
  virtual ~BSEmcFlagSplitOffs();
  virtual void SetDetectorName(const std::string &t_detectorName);
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister);
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;

  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchname = t_branchname; }
  void SetClusterBranchName(const TString &t_branchname) { fClusterBranchname = t_branchname; }
  void SetSubClusterBranchName(const TString &t_branchname) { fSubClusterBranchname = t_branchname; }
  void SetRecoHitBranchName(const TString &t_branchname) { fRecoHitBranchname = t_branchname; }
  struct Cluster_Info {
    const std::vector<const BSEmcDigi *> &DigiArray;
    const std::vector<const BSEmcCluster *> &ClusterArray;
    const BSEmcCluster *cluster;
    const std::vector<const BSEmcSubCluster *> &SubclusterArray;

    Cluster_Info(const std::vector<const BSEmcDigi *> &digis, const std::vector<const BSEmcCluster *> &clusters, const BSEmcCluster *current_cluster,
                 const std::vector<const BSEmcSubCluster *> &subclusters)
      : DigiArray(digis), ClusterArray(clusters), cluster(current_cluster), SubclusterArray(subclusters)
    {
    }
  };

  /**
   * @brief Create a Map that links Clusters to their corresponding Subclusters.
   *
   * @return std::map<int,std::vector<BSEmcSubCluster*>>
   */
  std::map<int, std::vector<const BSEmcSubCluster *>> CreateClusterMap();

  /**
   * @brief Search the Subcluster-array for the Subcluster that belongs to the
   * current maximum.
   *
   * @param max - current maximum
   * @param subcluster_array - array of Subclusters of the corresponding Cluster
   * @return BSEmcSubCluster* - Subcluster belonging to the current maximum
   */
  const BSEmcSubCluster *FindSubCluster(const BSEmcDigiInfo_t &max, std::vector<const BSEmcSubCluster *> subcluster_array);

  /**
   * @brief Create an array of all the secondary maxima of a Cluster.
   *
   * @param centralmax - Central maximum of the cluster.
   * @param maxima - All maxima of the cluster.
   * @return std::vector<BSEmcDigiInfo_t>
   */
  std::vector<BSEmcDigiInfo_t> FindSecondaryMaxima(const BSEmcDigiInfo_t &centralmax, const std::vector<BSEmcDigiInfo_t> &maxima);

  /**
   * @brief Calculate the shower mass of a Cluster.
   *        The shower mass corresponds to the invariant mass of a cluster, if
   * each crystal is treated as a photon with the crystal's energy.
   *
   * @param DigiArray
   * @param cluster
   * @return double - shower mass of the Cluster in GeV
   */
  double CalculateShowerMass(const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcCluster *cluster);

  /**
   * @brief Evaluate if a 2 PED (Particle Energy Deposition) cluster is
   * determined to include a Split-Off maximum based on a cut on the shower
   * mass.
   *        The cut was determined in MC studies.
   *
   * @param DigiArray
   * @param cluster
   * @return true - if Cluster is flagged to contain a Split-Off
   * @return false - if Cluster is not flagged to contain a Split-Off
   */
  bool ShowerMassCut(const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcCluster *cluster);

  /**
   * @brief Calculate the shower mass of two Subclusters of a Cluster.
   *        The shower mass corresponds to the invariant mass of a cluster, if
   * each crystal is treated as a photon with the crystal's energy.
   *
   * @param DigiArray
   * @param subcluster1
   * @param subcluster2
   * @return double - shower mass of the two Sublusters in GeV
   */
  double CalculateHighPEDShowerMass(const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcSubCluster *subcluster1, const BSEmcSubCluster *subcluster2);

  /**
   * @brief Evaluate if a high (>2) PED (Particle Energy Deposition) cluster
   * (build out of 2 SubClusters) is determined to include a Split-Off maximum
   * based on a cut on the shower mass.
   *        The cut was determined in MC studies.
   *
   * @param DigiArray
   * @param subcluster1
   * @param subcluster2
   * @return true - if Subclusters are flagged to contain a Split-Off
   * @return false - if Subclusters are not flagged to contain a Split-Off
   */
  bool HighPEDCut(const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcSubCluster *subcluster1, const BSEmcSubCluster *subcluster2);

  /**
   * @brief Calculate the distance between the centers of two given forward
   * endcap crystals.
   *
   * @param DigiArray
   * @param firstmax
   * @param secondmax
   * @return double - distance in cm
   */
  double CalculateDistance(const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcDigiInfo_t &firstmax, const BSEmcDigiInfo_t &secondmax);

  /**
   * @brief Find the next closest Cluster to the current maximum and calculate
   * the distance to its closest digi.
   *
   * @param ClusterArray
   * @param DigiArray
   * @param max - Info about the current maximum.
   * @return double - distance in cm
   */
  double FindSmallestDistance(const std::vector<const BSEmcCluster *> &ClusterArray, const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcDigiInfo_t &max);

  /**
   * @brief Find the next closest Cluster to the current maximum and return its
   * energy.
   *
   * @param ClusterArray
   * @param DigiArray
   * @param max - Info about the current maximum.
   * @return double - energy of next closest cluster
   */
  double FindEnergyClosestCluster(const std::vector<const BSEmcCluster *> &ClusterArray, const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcDigiInfo_t &max);

  /**
   * @brief Evaluate if a 1 PED (Particle Energy Deposition) cluster is
   * determined to be a Split-Off maximum based on a cut on the distance to the
   * next closest cluster and the ratio of the cluster energies.
   *        The cut was determined in MC studies.
   *
   * @param ratio - current cluster energy divided by energy of next closest
   * cluster
   * @param distance - distance to next closest cluster
   * @return true - if Cluster is flagged as a Split-Off
   * @return false - if Cluster is not flagged as a Split-Off
   */
  bool RatioCut(const double ratio, const double distance);

  /**
   * @brief Collect the crystal Ids of Subclusters from high (>2) PED (Particle
   * Energy Deposition) clusters that are marked to be Split-Off,
   *        based on a cut on the shower mass.
   *
   * @param ClusterInfo - Contains all of the important information relevant to
   * the current cluster.
   * @param SplitOffSubclusterIds - CentralCrystalIds of Subclusters flagged to
   * be SplitOff are filled into this vector
   */
  void FlagHighPED(Cluster_Info &ClusterInfo, std::vector<int> &SplitOffSubclusterIds);

  /**
   * @brief Collect the crystal Ids of Subclusters from 2 PED (Particle Energy
   * Deposition) clusters that are marked to be Split-Off,
   *        based on a cut on the shower mass.
   *
   * @param ClusterInfo - Contains all of the important information relevant to
   * the current cluster.
   * @param SplitOffSubclusterIds - CentralCrystalIds of Subclusters flagged to
   * be SplitOff are filled into this vector
   */
  void Flag2PED(Cluster_Info &ClusterInfo, std::vector<int> &SplitOffSubclusterIds);

  /**
   * @brief Collect the crystal Ids of Subclusters from 1 PED (Particle Energy
   * Deposition) clusters that are marked to be Split-Off,
   *        based on a cut on the energy ratio and the distance to the next
   * closest Cluster.
   *
   * @param ClusterInfo - Contains all of the important information relevant to
   * the current cluster.
   * @param SplitOffSubclusterIds - CentralCrystalIds of Subclusters flagged to
   * be SplitOff are filled into this vector.
   */
  void Flag1PED(Cluster_Info &ClusterInfo, std::vector<int> &SplitOffSubclusterIds);

 private:
  std::string fPositionParName{""};
  std::string fNeighbouringRelationParName{""};
  std::string fSplitOffParName{""};
  BSEmcGeoNeighbouringRelationPar *fNeighbouringRelationPar{nullptr};
  const BSEmcCrystalPositionPar *fPositionPar;
  BSEmcSplitOffPar *fSplitOffPar{nullptr};

  TString fDigiBranchname{""};
  TString fClusterBranchname{""};
  TString fSubClusterBranchname{""};
  TString fRecoHitBranchname{""};

  PndContainerI<BSEmcDigi> *fDigiArray{nullptr};
  PndContainerI<BSEmcCluster> *fClusterArray{nullptr};
  PndContainerI<BSEmcSubCluster> *fSubClusterArray{nullptr};
  PndMutableContainerI<BSEmcRecoHit> *fRecoHitArray{nullptr};

  std::unique_ptr<TF1> f1PEDFunction;
  std::unique_ptr<TF1> f2PEDFunction;
  std::unique_ptr<TF1> fHighPEDFunction;

  ClassDef(BSEmcFlagSplitOffs, 1);
};

#endif /*BSEMCFLAGSPLITOFFS_HH*/
