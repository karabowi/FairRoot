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

#ifndef BSEMCOUTOFCLUSTERCORRECTIONPROCESS_HH
#define BSEMCOUTOFCLUSTERCORRECTIONPROCESS_HH

#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"

class FairParGenericSet;
class BSEmcCluster;
class BSEmcRecoHit;
class BSEmcClusteringPar;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcClusteringData;
template <class T>
class PndMutableContainerI;

/**
 * @class BSEmcOutOfClusterCorrectionProcess
 * @brief Each low energetic cluster the closest BSEmcRecoHits are searched for and its energy is split between the BSEmcRecoHits
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcOutOfClusterCorrectionProcess : public PndProcess {
 public:
  BSEmcOutOfClusterCorrectionProcess();
  virtual ~BSEmcOutOfClusterCorrectionProcess();
  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  void CorrectingRecoHitsForLossOfLowEnergyCluster(const std::vector<const BSEmcCluster *> &t_clusterCont, const std::vector<BSEmcRecoHit *> &t_recoHitsCont) const;
  void SetClusteringParName(const std::string &t_parName) { fClusteringParName = t_parName; }

  void SetClusterBranchName(const TString &t_branchname) { fClusterBranchname = t_branchname; }
  void SetRecoHitBranchName(const TString &t_branchname) { fRecoHitBranchname = t_branchname; }

 private:
  void UpdateNearestRecoHitEnergies(const BSEmcCluster *t_cluster, std::vector<BSEmcRecoHit *> &t_nearestHits) const;
  std::vector<BSEmcRecoHit *> FindClosestRecoHits(const BSEmcCluster *t_cluster, const std::vector<BSEmcRecoHit *> &t_recoHitsCont) const;

 private:
  TString fClusterBranchname{""};
  TString fRecoHitBranchname{""};

  PndContainerI<BSEmcCluster> *fClusterArray{nullptr};
  PndMutableContainerI<BSEmcRecoHit> *fRecoHitArray{nullptr};
  std::string fClusteringParName{""};

  Double_t fSearchConeAngle{0};
  Double_t fMinClusterEnergy{0};
  Double_t fClusterActiveTime{0};
  ClassDef(BSEmcOutOfClusterCorrectionProcess, 1);
};

#endif /*BSEMCOUTOFCLUSTERCORRECTIONPROCESS_HH*/
