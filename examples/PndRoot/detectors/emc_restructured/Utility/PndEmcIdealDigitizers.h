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

#ifndef BSEMCIDEALDIGITIZERS_HH
#define BSEMCIDEALDIGITIZERS_HH

#include "PndPersistencyTask.h"

class BSEmcBarrelIdealDigitizer : public PndPersistencyTask {
 public:
  BSEmcBarrelIdealDigitizer();
  ClassDef(BSEmcBarrelIdealDigitizer, 1);
};

class BSEmcBwEndcapIdealDigitizer : public PndPersistencyTask {
 public:
  BSEmcBwEndcapIdealDigitizer();
  ClassDef(BSEmcBwEndcapIdealDigitizer, 1);
};

class BSEmcFwEndcapIdealDigitizer : public PndPersistencyTask {
 public:
  BSEmcFwEndcapIdealDigitizer();
  ClassDef(BSEmcFwEndcapIdealDigitizer, 1);
};

class BSEmcShashlikIdealDigitizer : public PndPersistencyTask {
 public:
  BSEmcShashlikIdealDigitizer();
  ClassDef(BSEmcShashlikIdealDigitizer, 1);
};

class BSEmcIdealDigitizer : public PndPersistencyTask {
 public:
  BSEmcIdealDigitizer();
  ClassDef(BSEmcIdealDigitizer, 1);
};

#endif /*BSEMCIDEALDIGITIZERS_HH*/
