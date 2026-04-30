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

#ifndef BSEMCDETECTORS_HH
#define BSEMCDETECTORS_HH

#include "BSEmcDetector.h"
#include "BSEmcShashlikDetector.h"

class FairRunSim;

//---------------------------------------------------------------
class BSEmcBarrel : public BSEmcDetector {
 public:
  BSEmcBarrel(Bool_t t_persistency = kFALSE);
  virtual ~BSEmcBarrel() = default;
  ClassDef(BSEmcBarrel, 1);
};

//---------------------------------------------------------------
class BSEmcBwEndcap : public BSEmcDetector {
 public:
  BSEmcBwEndcap(Bool_t t_persistency = kFALSE);
  virtual ~BSEmcBwEndcap() = default;
  ClassDef(BSEmcBwEndcap, 1);
};

//---------------------------------------------------------------
class BSEmcFwEndcap : public BSEmcDetector {
 public:
  BSEmcFwEndcap(Bool_t t_persistency = kFALSE);
  virtual ~BSEmcFwEndcap() = default;
  ClassDef(BSEmcFwEndcap, 1);
};

//---------------------------------------------------------------
class BSEmcShashlik : public BSEmcShashlikDetector {
 public:
  BSEmcShashlik(Bool_t t_persistency = kFALSE);
  virtual ~BSEmcShashlik() = default;
  ClassDef(BSEmcShashlik, 1);
};

//---------------------------------------------------------------
class PndEmcDetectors {
 public:
  static void AddEmcSetup(FairRunSim *t_run, Bool_t t_persistency = kFALSE);
  virtual ~PndEmcDetectors() = default;
  ClassDef(PndEmcDetectors, 1);
};

#endif /*BSEMCDETECTORS_HH*/
