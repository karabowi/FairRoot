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

#ifndef BSEMCCREATERECOHITPROCESS_HH
#define BSEMCCREATERECOHITPROCESS_HH
#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"

#include "BSEmcEnergyCalculationAlgo.h"
#include "BSEmcPositionAlgo.h"
#include "BSEmcXClMoments.h"

class FairParGenericSet;
class BSEmcClusteringPar;
class BSEmcDigi;
class BSEmcCluster;
class BSEmcSubCluster;
class BSEmcRecoHit;
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
 * @class BSEmcCreateRecoHitProcess
 * @brief Creates for each BSEmcSubCluster a BSEmcRecoHit and fills it with
 * additional information such as Zernike/Lateral Moments, etc.
 * @details Will set default BSEmcRecoHit Container to be fRecoHitBranchname if set.
 * Processes attached to the same task that follow this task will fetch
 * fRecoHitBranchname if they themselves do not specify the requested RecoHit-Branchname.
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcCreateRecoHitProcess : public PndProcess {
 public:
  BSEmcCreateRecoHitProcess();
  virtual ~BSEmcCreateRecoHitProcess();
  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  std::vector<BSEmcRecoHit *> CreateRecoHits(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<const BSEmcCluster *> &t_clusterCont,
                                             const std::vector<const BSEmcSubCluster *> &t_subclusterCont) const;

  void SetClusteringParName(const std::string &t_parName) { fClusteringParName = t_parName; }
  void SetClusterPropertiesParName(const std::string &t_parName) { fClusterPropertiesParName = t_parName; }

  void SetPositionParName(const std::string &t_parName) { fPositionParName = t_parName; }
  void SetNeighbouringRelationParName(const std::string &t_parName) { fNeighbouringRelationParName = t_parName; }

  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchname = t_branchname; }
  void SetClusterBranchName(const TString &t_branchname) { fClusterBranchname = t_branchname; }
  void SetSubClusterBranchName(const TString &t_branchname) { fSubClusterBranchname = t_branchname; }
  void SetRecoHitBranchName(const TString &t_branchname) { fRecoHitBranchname = t_branchname; }

 private:
  std::string fClusteringParName{""};
  std::string fClusterPropertiesParName{""};
  std::string fPositionParName{""};
  std::string fNeighbouringRelationParName{""};

  TString fDigiBranchname{""};
  TString fClusterBranchname{""};
  TString fSubClusterBranchname{""};
  TString fRecoHitBranchname{""};

  PndContainerI<BSEmcDigi> *fDigiArray{nullptr};              //!
  PndContainerI<BSEmcCluster> *fClusterArray{nullptr};        //!
  PndContainerI<BSEmcSubCluster> *fSubClusterArray{nullptr};  //!
  PndMutableContainerI<BSEmcRecoHit> *fRecoHitArray{nullptr}; //!
  std::unique_ptr<BSEmcEnergyCalculationAlgo> fEnergyProcess{new BSEmcEnergyCalculationAlgo};
  std::unique_ptr<BSEmcXClMoments> fMomentsProcess{new BSEmcXClMoments};
  std::unique_ptr<BSEmcPositionAlgo> fPositionAlgo{new BSEmcPositionAlgo};
  ClassDef(BSEmcCreateRecoHitProcess, 1);
};

#endif /*BSEMCCREATERECOHITPROCESS_HH*/
