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

#ifndef BSEMCMCDEPOSITPRODUCERS_HH
#define BSEMCMCDEPOSITPRODUCERS_HH

#include "PndPersistencyTask.h"
#include "PndProcessTask.h"

#include "BSEmcBarrelZDependentMCDepositProductionProcess.h"
#include "BSEmcCounter.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcMCDepositProducerTask.h"
#include "BSEmcMCDepositProductionProcess.h"
#include "BSEmcParameterLoadingProcess.h"

//---------------------------------------------------------------
class BSEmcBarrelMCDepositProducer : public PndPersistencyTask {
public:
  BSEmcBarrelMCDepositProducer();
  ClassDef(BSEmcBarrelMCDepositProducer, 1);
};

//---------------------------------------------------------------
class BSEmcBwEndcapMCDepositProducer : public PndPersistencyTask {
public:
  BSEmcBwEndcapMCDepositProducer();
  ClassDef(BSEmcBwEndcapMCDepositProducer, 1);
};

//---------------------------------------------------------------
class BSEmcFwEndcapMCDepositProducer : public PndPersistencyTask {
public:
  BSEmcFwEndcapMCDepositProducer();
  ClassDef(BSEmcFwEndcapMCDepositProducer, 1);
};

//---------------------------------------------------------------
class BSEmcShashlikMCDepositProducer : public PndPersistencyTask {
public:
  BSEmcShashlikMCDepositProducer();
  ClassDef(BSEmcShashlikMCDepositProducer, 1);
};

//---------------------------------------------------------------
class BSEmcMCDepositProducer : public PndPersistencyTask {
public:
  BSEmcMCDepositProducer();
  ClassDef(BSEmcMCDepositProducer, 1);
};

#endif /*BSEMCMCDEPOSITPRODUCERS_HH*/
