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

#ifndef BSEMCCLUSTERERS_HH
#define BSEMCCLUSTERERS_HH

#include <string>

#include "PndPersistencyTask.h"

class BSEmcCorrection;
class BSEmcErrorMatrixCalculation;
//---------------------------------------------------------------
class BSEmcBaseClusterer : public PndPersistencyTask {
 public:
  BSEmcBaseClusterer(const std::string &t_detectorName, BSEmcCorrection *t_ecf, BSEmcErrorMatrixCalculation *t_errorCalc, Bool_t t_removeSplitoff = kTRUE);
  ClassDef(BSEmcBaseClusterer, 1);
};

//---------------------------------------------------------------
class BSEmcClusterer : public PndPersistencyTask {
 public:
  BSEmcClusterer();
  ClassDef(BSEmcClusterer, 1);
};

#endif /*BSEMCCLUSTERERS_HH*/
