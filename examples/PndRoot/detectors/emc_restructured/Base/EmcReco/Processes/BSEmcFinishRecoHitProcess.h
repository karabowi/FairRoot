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

#ifndef BSEMCCFINISHRECOHITPROCESS_HH
#define BSEMCCFINISHRECOHITPROCESS_HH

#include <string>
#include <vector>

#include "Rtypes.h"

#include "PndContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"

#include "BSEmcClusteringPar.h"

class FairParGenericSet;
class BSEmcCorrection;
class BSEmcErrorMatrixCalculation;
class BSEmcRecoHit;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcClusteringData;
template <class T>
class PndMutableContainerI;

/**
 * @class BSEmcFinishRecoHitProcess
 * @brief Applies energy correction and error calculation for each BSEmcRecoHit
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcFinishRecoHitProcess : public PndProcess {
 public:
  BSEmcFinishRecoHitProcess();
  virtual ~BSEmcFinishRecoHitProcess();
  virtual void SetupParameters(const PndParameterRegister * /*t_parameter*/) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  void Apply(const std::vector<BSEmcRecoHit *> &t_recoHitCont) const;
  void SetECF(BSEmcCorrection *t_ecf) { fECF = t_ecf; }
  void SetErrorMatrixCalculator(BSEmcErrorMatrixCalculation *t_errorMatrixCalculator) { fErrorCalculator = t_errorMatrixCalculator; }

  void SetRecoHitBranchName(const TString &t_branchname) { fRecoHitBranchname = t_branchname; }

 private:
  BSEmcCorrection *fECF{nullptr};
  BSEmcErrorMatrixCalculation *fErrorCalculator{nullptr};

  TString fRecoHitBranchname{""};

  PndMutableContainerI<BSEmcRecoHit> *fRecoHitArray{nullptr};
  std::string fErrorCalculationParName{""};

  ClassDef(BSEmcFinishRecoHitProcess, 1);
};

#endif /*BSEMCCFINISHRECOHITPROCESS_HH*/
