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

#ifndef BSEMCPOSITIONPAR_HH
#define BSEMCPOSITIONPAR_HH
#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "FairParGenericSet.h"
#include "FairParamList.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcPositionPar
 * @brief Mini parameter set to contain position relevant parameter
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPositionPar : public FairParGenericSet {
 public:
  static const std::string fgParameterName;
  BSEmcPositionPar(const char *t_name = "EmcPositionPar", const char *t_title = "Position Values for emc detector", const char *t_context = "TestDefaultContext");

  virtual ~BSEmcPositionPar(void);

  void clear(void) /*override*/ {}

  void putParams(FairParamList *t_list) /*override*/;

  Bool_t getParams(FairParamList *t_list) /*override*/;

  Double_t GetDigiPositionDepth() const { return fEmcDigiPositionDepth; }
  const Text_t *GetDigiPositionMethod() const { return fDigiPosMethod; }

 private:
  Double_t fEmcDigiPositionDepth{6.2};
  Text_t fDigiPosMethod[100]{"DEPTH"};
  ClassDef(BSEmcPositionPar, 1)
};

#endif /*BSEMCPOSITIONPAR_HH*/
