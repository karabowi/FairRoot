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
// -----					BSEmcLateralClusterSplittingPar
// -----
// -----	Created 29/04/2019 by B. Salisbury <salisbury@hiskp.uni-bonn.de>
// -------------------------------------------------------------------------

#ifndef BSEMCLATERALCLUSTERSPLITTINGPAR_HH
#define BSEMCLATERALCLUSTERSPLITTINGPAR_HH

#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TArrayD.h"
#include "TObject.h"

#include "FairParGenericSet.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcLateralClusterSplittingPar
 * @brief Container for latera cluster splitting parameter
 * @details
 * @author Guang Zhao
 * @date 2024-02
 * @ingroup Barrel
 */
class BSEmcLateralClusterSplittingPar : public FairParGenericSet {
 public:
  static const std::string fgParameterName;
  BSEmcLateralClusterSplittingPar(const char *t_name = "EmcLateralClusterSplittingPar", const char *t_title = "Emc Geometry Version Flag",
                                  const char *t_context = "TestDefaultContext");
  ~BSEmcLateralClusterSplittingPar(void);
  void clear(void) /*override*/;

  virtual void putParams(FairParamList *t_list) /*override*/;
  virtual Bool_t getParams(FairParamList *t_list) /*override*/;

  TArrayD GetParArray1() { return fParArray1; };
  TArrayD GetParArray2() { return fParArray2; };
  TArrayD GetParArray3() { return fParArray3; };
  TArrayD GetParArray4() { return fParArray4; };

 private:
  TArrayD fParArray1;
  TArrayD fParArray2;
  TArrayD fParArray3;
  TArrayD fParArray4;

  ClassDef(BSEmcLateralClusterSplittingPar, 0)
};

#endif /*BSEMCLATERALCLUSTERSPLITTINGPAR_HH*/
