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

#ifndef BSEMCPRECLUSTERTIMEBUNCHERPROCESS_HH
#define BSEMCPRECLUSTERTIMEBUNCHERPROCESS_HH

#include <memory>
#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "FairTSBufferFunctional.h"

#include "PndContainerI.h"
#include "PndProcess.h"

#include "BSEmcPrecluster.h"

class BSEmcPrecluster;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcClusteringData;
template <class T>
class PndMutableContainerI;

/**
 * @class BSEmcPreclusterTimeBuncherProcess
 * @brief Process to create precluster time bunches using TimeGap on timesorted precluster container
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPreclusterTimeBuncherProcess : public PndProcess {
 public:
  BSEmcPreclusterTimeBuncherProcess(const TString &t_sourceBranchName);
  virtual ~BSEmcPreclusterTimeBuncherProcess();

  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;
  virtual void Process() /*override*/;

  void SetPreclusterSourceBranchName(const TString &t_branchname) { fSourceBranchName = t_branchname; }
  void SetPreclusterTargetBranchName(const TString &t_branchname) { fTargetBranchName = t_branchname; }

 private:
  PndMutableContainerI<BSEmcPrecluster> *fPreclusters{nullptr};
  TString fSourceBranchName{""};
  TString fTargetBranchName{""};
  std::string fClusteringParName{""};
  Double_t fTimebunchCutTime{0};
  std::unique_ptr<TimeGap> fFunctor{new TimeGap()};

  ClassDef(BSEmcPreclusterTimeBuncherProcess, 1);
};

#endif /*BSEMCPRECLUSTERTIMEBUNCHERPROCESS_HH*/
