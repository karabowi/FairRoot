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

#ifndef BSEMCDIGITIMEBUNCHERPROCESS_HH
#define BSEMCDIGITIMEBUNCHERPROCESS_HH

#include <memory>
#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "FairTSBufferFunctional.h"

#include "PndContainerI.h"
#include "PndProcess.h"

#include "BSEmcDigi.h"

class BSEmcDigi;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcClusteringData;
template <class T>
class PndMutableContainerI;

/**
 * @class BSEmcDigiTimeBuncherProcess
 * @brief Process to create digi time bunches using TimeGap on timesorted digi container
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcDigiTimeBuncherProcess : public PndProcess {
 public:
  BSEmcDigiTimeBuncherProcess(const TString &t_sourcebranchname = "");
  virtual ~BSEmcDigiTimeBuncherProcess();

  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;
  virtual void Process() /*override*/;
  void SetDigiSourceBranchName(const TString &t_branchname) { fSourceBranchName = t_branchname; }
  void SetDigiTargetBranchName(const TString &t_branchname) { fTargetBranchName = t_branchname; }

 private:
  PndMutableContainerI<BSEmcDigi> *fDigis{nullptr};
  TString fSourceBranchName{""};
  TString fTargetBranchName{""};
  std::string fClusteringParName{""};
  Double_t fTimebunchCutTime{0};
  std::unique_ptr<TimeGap> fDigiFunctor{new TimeGap()};
  Int_t fEvent{0};
  ClassDef(BSEmcDigiTimeBuncherProcess, 1);
};

#endif /*BSEMCDIGITIMEBUNCHERPROCESS_HH*/
