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
// -----                     BSEmcBwEndcapContFact header file                -----
// -----               Created 2019/05/24  by B. Salisbury                  -----
// -------------------------------------------------------------------------

#ifndef BSEMCBWENDCAPCONTFACT_HH
#define BSEMCBWENDCAPCONTFACT_HH

#include "Rtypes.h"

#include "FairContFact.h"

class FairContainer;
class FairParSet;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcBwEndcapContFact
 * @brief Backward Endcap specific container factory to load BSEmcBWEndcapDigiPar
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup BwEndcap
 */
class BSEmcBwEndcapContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  BSEmcBwEndcapContFact();
  ~BSEmcBwEndcapContFact() {}
  virtual FairParSet *createContainer(FairContainer * /*unused*/) /*override*/;

  ClassDef(BSEmcBwEndcapContFact, 0) // Factory for all EMC parameter containers
};

#endif /*BSEMCBWENDCAPCONTFACT_HH*/
