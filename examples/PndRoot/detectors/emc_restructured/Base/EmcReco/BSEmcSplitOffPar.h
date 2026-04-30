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


#ifndef BSEMCSPLITOFFPAR_HH
#define BSEMCSPLITOFFPAR_HH

#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"

#include "FairParGenericSet.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcSplitOffPar
 * @brief Container for SplitOff parameters
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcSplitOffPar : public FairParGenericSet {
 public:
  static const std::string fgParameterName;
  BSEmcSplitOffPar(const char *t_name = "EmcSplitOffPar", const char *t_title = "Emc Geometry Version Flag", const char *t_context = "TestDefaultContext");
  ~BSEmcSplitOffPar(void);
  void clear(void) /*override*/;

  Text_t *Get1PEDFunction()  { return f1PEDFunction; }
  Text_t *Get2PEDFunction()  { return f2PEDFunction; }
  Text_t *GetHighPEDFunction()  { return fHighPEDFunction; }

  void putParams(FairParamList * /*unused*/) /*override*/;
  Bool_t getParams(FairParamList * /*unused*/) /*override*/;

 private:

  Text_t f1PEDFunction[30];
  Text_t f2PEDFunction[30];
  Text_t fHighPEDFunction[30];

  ClassDef(BSEmcSplitOffPar, 2)
};

#endif /*BSEMCSPLITOFFPAR_HH*/
