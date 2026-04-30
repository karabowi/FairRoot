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

#ifndef BSEMCLATERALCLUSTERSPLITTINGCONTFACT_HH
#define BSEMCLATERALCLUSTERSPLITTINGCONTFACT_HH

#include "Rtypes.h"

#include "FairContFact.h"

class FairContainer;
class FairParSet;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcLateralClusterSplittingContFact
 * @brief container factory for lateral clustering specific parameters
 * @details Based on PndEmc.cxx
 * @author Guang Zhao <zhaog@ihep.ac.cn>
 * @date 2024-02
 * @ingroup Barrel
 */
class BSEmcLateralClusterSplittingContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  BSEmcLateralClusterSplittingContFact();
  ~BSEmcLateralClusterSplittingContFact() {}
  virtual FairParSet *createContainer(FairContainer * /*unused*/) /*override*/;

  ClassDef(BSEmcLateralClusterSplittingContFact, 0) // Factory for all EMC parameter containers
};

#endif /*BSEMCLATERALCLUSTERSPLITTINGCONTFACT_HH*/
