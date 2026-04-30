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

#ifndef BSMCPAR_HH
#define BSMCPAR_HH
#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "FairParGenericSet.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcMCPar
 * @brief Container for runtime parameters that are required for the
 * transformation from BSEmcMCPoints to BSEmcMCDeposits
 * @details based on PndEmcDigiPar
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup EmcBase
 */
class BSEmcMCPar : public FairParGenericSet {
 public:
  static const std::string fgParameterName;
  BSEmcMCPar(const char *t_name = "EmcMCPar", const char *t_title = "Emc Geometry Version Flag", const char *t_context = "TestDefaultContext");
  ~BSEmcMCPar(void);
  void clear(void) /*override*/;

  Double_t GetEnergyDepositThreshold() const { return fEnergyDepositThreshold; }
  Double_t GetCutMotherParticle() const { return fCutMotherParticle; }
  Double_t GetCutSameTrack() const { return fCutSameTrack; }
  const Text_t *GetNonUniformityFile() const { return fNonUniformityFile; }

  void putParams(FairParamList * /*unused*/) /*override*/;
  Bool_t getParams(FairParamList * /*unused*/) /*override*/;

 private:
  Double_t fEnergyDepositThreshold{-1};
  Double_t fCutMotherParticle{-1};
  Double_t fCutSameTrack{-1};

  Text_t fNonUniformityFile[100]{};

  ClassDef(BSEmcMCPar, 2)
};

#endif /*BSMCPAR_HH*/
