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

#ifndef BSEMCMCPROPAGATIONPROCESS_HH
#define BSEMCMCPROPAGATIONPROCESS_HH

#include <map>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndConstContainer.h"
#include "PndContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"

#include "BSEmcMCDeposit.h"

class BSEmcCluster;
class BSEmcDigi;
class BSEmcSubCluster;
class FairLink;
class FairMultiLinkedData;
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
 * @class BSEmcMCPropagationProcess
 * @brief Process to determine entering and exiting tracks into cluster
 * @details Potentially outdated with Tobias Stockmanns changes to
 * BSEmcMCDepositProducerProcess
 *
 * needs to be run after BSEmcAddLinksToCluster, as BSEmcMCDeposit links are
 * required here
 * @author Tobias Stockmanns
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcMCPropagationProcess : public PndProcess {
 public:
  BSEmcMCPropagationProcess();
  virtual ~BSEmcMCPropagationProcess();
  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  template <class T>
  void AssignLinks(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<T *> &t_array);

  void SetMCDepositBranchName(const TString &t_branchname) { fMCDepositBranchname = t_branchname; }
  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchname = t_branchname; }
  void SetClusterBranchName(const TString &t_branchname) { fClusterBranchname = t_branchname; }
  void SetSubClusterBranchName(const TString &t_branchname) { fSubClusterBranchname = t_branchname; }

 private:
  void ResetLinksBesideDepositLinks(BSEmcCluster *t_cluster);

  class LinkScoreBoard {
   public:
    Short_t score{0};

    void SetValShift(Bool_t val, Int_t shift)
    {
      if (shift < 4) {
        score |= static_cast<int>(val) << shift;
      }
    }

    Bool_t GetValShift(Int_t shift) { return (score >> shift & 0x1) != 0; }
  };

  void FillScoreBoard(FairMultiLinkedData t_tracks, std::map<FairLink, LinkScoreBoard> &t_scoreBoard, Int_t t_shift);
  void AddTracksEnteringExiting(BSEmcCluster *t_cluster, const FairMultiLinkedData &t_tracksEntering, const FairMultiLinkedData &t_tracksExiting);

 private:
  TString fMCDepositBranchname{""};
  TString fDigiBranchname{""};
  TString fClusterBranchname{""};
  TString fSubClusterBranchname{""};

  PndContainerI<BSEmcDigi> *fDigiArray{nullptr};                   //!< active clusters
  PndMutableContainerI<BSEmcCluster> *fClusterArray{nullptr}; //!< active clusters
  PndMutableContainerI<BSEmcSubCluster> *fSubClusterArray{nullptr}; //!< active clusters
  PndContainerI<BSEmcMCDeposit> *fMCDepositArray{nullptr};                 //!< active clusters

  ClassDef(BSEmcMCPropagationProcess, 1);
};

#endif /*BSEMCMCPROPAGATIONPROCESS_HH*/
