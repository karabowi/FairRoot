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
// -----                     PndEmcContFact header file                -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------

#ifndef BSEMCBARRELCONTFACT_HH
#define BSEMCBARRELCONTFACT_HH

#include "Rtypes.h"

#include "FairContFact.h"

class FairContainer;
class FairParSet;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcBarrelContFact
 * @brief container factory for Barrel specific parameters (in this case the BSEmcBarrelDigiNonuniformityPar)
 * @details Based on PndEmc.cxx
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Barrel
 */
class BSEmcBarrelContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  BSEmcBarrelContFact();
  ~BSEmcBarrelContFact() {}
  virtual FairParSet *createContainer(FairContainer * /*unused*/) /*override*/;

  ClassDef(BSEmcBarrelContFact, 0) // Factory for all EMC parameter containers
};

#endif /*BSEMCBARRELCONTFACT_HH*/
