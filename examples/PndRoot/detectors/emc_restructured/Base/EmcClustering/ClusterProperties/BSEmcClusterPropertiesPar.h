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
// -----					BSEmcClusterPropertiesPar
// -----
// -----	Created 29/04/2019 by B. Salisbury <salisbury@hiskp.uni-bonn.de>
// -------------------------------------------------------------------------

#ifndef BSEMCCLUSTERPROPERTIESPAR_HH
#define BSEMCCLUSTERPROPERTIESPAR_HH

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
 * @class BSEmcClusterPropertiesPar
 * @brief Container for clusterproperties such as Position
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcClusterPropertiesPar : public FairParGenericSet {
 public:
  static const std::string fgParameterName;
  BSEmcClusterPropertiesPar(const char *t_name = "EmcClusterPropertiesPar", const char *t_title = "Emc Geometry Version Flag", const char *t_context = "TestDefaultContext");
  ~BSEmcClusterPropertiesPar(void);
  void clear(void) /*override*/;

  const Text_t *GetEmcClusterPosMethod() const { return fEmcClusterPosMethod; }
  Double_t GetOffsetParmA() const { return fOffsetParmA; };
  Double_t GetOffsetParmB() const { return fOffsetParmB; };
  Double_t GetOffsetParmC() const { return fOffsetParmC; };

  void putParams(FairParamList * /*unused*/) /*override*/;
  Bool_t getParams(FairParamList * /*unused*/) /*override*/;

 private:
  Double_t fOffsetParmA{-1};
  Double_t fOffsetParmB{-1};
  Double_t fOffsetParmC{-1};

  Text_t fEmcClusterPosMethod[10]{}; // lilo, linear

  ClassDef(BSEmcClusterPropertiesPar, 2)
};

#endif /*BSEMCCLUSTERPROPERTIESPAR_HH*/
