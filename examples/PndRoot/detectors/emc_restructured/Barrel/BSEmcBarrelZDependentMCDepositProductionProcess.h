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

// -------------------------------------------------------------------------
// -----                 PndEmcHitProducer header file                 -----
// -----                 Created 14/08/06  by S.Spataro                -----
// -------------------------------------------------------------------------
#ifndef BSEEMBARRELZDEPENDENTMCDEPOSITPRODUCTIONPROCESS_HH
#define BSEEMBARRELZDEPENDENTMCDEPOSITPRODUCTIONPROCESS_HH

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "BSEmcMCDepositProductionProcess.h"

class BSEmcBarrelDigiNonuniformityPar;
class BSEmcMCPar;
class BSEmcMCPoint;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcBarrelZDependentMCDepositProductionProcess
 * @brief Deposit Producer taking the z dependency of the lightyield of a crystal
 * into account.
 * @details this version is there for the "emc_module12_2018v1_EmcDetector.root" geometry
 * @author S.Spataro
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Barrel
 */
class BSEmcBarrelZDependentMCDepositProductionProcess : public BSEmcMCDepositProductionProcess {
 public:
  BSEmcBarrelZDependentMCDepositProductionProcess();
  virtual ~BSEmcBarrelZDependentMCDepositProductionProcess() {}
  virtual void SetupParameters(const PndParameterRegister *t_paramRegister) /*override*/;
  void IsDayOneSetup(Bool_t t_isDayOneSetup) { fIsDayOneSetup = t_isDayOneSetup; }

 protected:
  virtual Double_t GetEnergy(const BSEmcMCPoint *t_point) const /*override*/;
  virtual Bool_t SkipPoint(const BSEmcMCPoint *t_point) const /*override*/;

 protected:
  BSEmcBarrelDigiNonuniformityPar *fNonUniformityPar{nullptr};

 private:
  void Setup(BSEmcMCPar *t_par);
  Bool_t fIsDayOneSetup{kFALSE};
  ClassDef(BSEmcBarrelZDependentMCDepositProductionProcess, 2);

};

#endif /*BSEEMBARRELZDEPENDENTMCDEPOSITPRODUCTIONPROCESS_HH*/
