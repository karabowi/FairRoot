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

#ifndef BSEMCADDLINKSTOCLUSTER_HH
#define BSEMCADDLINKSTOCLUSTER_HH

#include <string>
#include <vector>

#include "Rtypes.h"

#include "PndContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"

#include "BSEmcCluster.h"

#include "BSEmcDigi.h"

class BSEmcCluster;
class BSEmcDigi;
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
 * @class BSEmcAddLinksToCluster
 * @brief Helper Process to add links of BSEmcMCDeposits to clusters and
 * subclusters
 * @details To be run after SubCluster/Bump creation and before
 * BSEmcMCPropagationProcess
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcAddLinksToCluster : public PndProcess {
 public:
  BSEmcAddLinksToCluster();
  virtual ~BSEmcAddLinksToCluster();

  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;
  virtual void Process() /*override*/;

  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchname = t_branchname; }
  void SetClusterBranchName(const TString &t_branchname) { fClusterBranchname = t_branchname; }
  void SetSubClusterBranchName(const TString &t_branchname) { fSubClusterBranchname = t_branchname; }

 private:
  TString fDigiBranchname{""};
  TString fClusterBranchname{""};
  TString fSubClusterBranchname{""};

  template <class T>
  void SetDigiLinksFor(const std::vector<T *> &t_cluster);
  void SetClusterLinksFor(const std::vector<BSEmcSubCluster *> &t_subcluster);

  PndContainerI<BSEmcDigi> *fDigis{nullptr};
  PndMutableContainerI<BSEmcCluster> *fCluster{nullptr};
  PndMutableContainerI<BSEmcSubCluster> *fSubCluster{nullptr};
  ClassDef(BSEmcAddLinksToCluster, 1);
};

#endif /*BSEMCADDLINKSTOCLUSTER_HH*/
