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

#ifndef BSEMCPOSITIONCONTFACT_HH
#define BSEMCPOSITIONCONTFACT_HH

#include "Rtypes.h"

#include "FairContFact.h"

class FairContainer;
class FairParSet;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcPositionContFact
 * @brief EMC specific container factory to load geometry Parameters, such as NeighbouringRelation, CrystalPosition and PositionPar
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPositionContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  BSEmcPositionContFact();
  ~BSEmcPositionContFact() {}
  virtual FairParSet *createContainer(FairContainer * /*unused*/) /*override*/;

  ClassDef(BSEmcPositionContFact, 0) // Factory for all EMC parameter containers
};

#endif /*BSEMCPOSITIONCONTFACT_HH*/
