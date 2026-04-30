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
// -----                     BSEmcFwEndcapBlindDeconvContFact header file                -----
// -----               Created 2022/09/21  by B. Salisbury                  -----
// -------------------------------------------------------------------------

#ifndef BSEMCFWENDCAPBLINDDECONVCONTFACT_HH
#define BSEMCFWENDCAPBLINDDECONVCONTFACT_HH

#include "Rtypes.h"

#include "FairContFact.h"

class FairContainer;
class FairParSet;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcFwEndcapBlindDeconvContFact
 * @brief Forward Endcap specific container factory to load BSEmcFwEndcapBlindDeconvPar
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2022-09
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapBlindDeconvContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  BSEmcFwEndcapBlindDeconvContFact();
  virtual ~BSEmcFwEndcapBlindDeconvContFact() {}
  virtual FairParSet *createContainer(FairContainer * /*unused*/) /*override*/;

  ClassDef(BSEmcFwEndcapBlindDeconvContFact, 0) // Factory for all EMC parameter containers
};

#endif /*BSEMCFWENDCAPBLINDDECONVCONTFACT_HH*/
