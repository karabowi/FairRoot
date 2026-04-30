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
// -----                     BSEmcFwEndcapLightpulserContFact header file                -----
// -----               Created 2019/05/24  by B. Salisbury                  -----
// -------------------------------------------------------------------------

#ifndef BSEMCFWENDCAPLIGHTPULSERCONTFACT_HH
#define BSEMCFWENDCAPLIGHTPULSERCONTFACT_HH

#include "Rtypes.h"

#include "FairContFact.h"

class FairContainer;
class FairParSet;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcFwEndcapLightpulserContFact
 * @brief Forward Endcap specific container factory to load BSEmcFWEndcapDigiPar
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapLightpulserContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  BSEmcFwEndcapLightpulserContFact();
  virtual ~BSEmcFwEndcapLightpulserContFact() {}
  virtual FairParSet *createContainer(FairContainer * /*unused*/) /*override*/;

  ClassDef(BSEmcFwEndcapLightpulserContFact, 0) // Factory for all EMC parameter containers
};

#endif /*BSEMCFWENDCAPLIGHTPULSERCONTFACT_HH*/
