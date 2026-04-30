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

#ifndef PNDEMCFPGAINTEGRATINGANALYSER_HH
#define PNDEMCFPGAINTEGRATINGANALYSER_HH

#include "PndEmcPSAFPGASampleAnalyser.h"
#include <string>

class PndEmcPSAFPGAFilterCF;
class PndEmcPSAFPGAFilterMA;
class PndEmcPSAFPGAFilterDelay;
class PndEmcPSAFPGALinFitter;

#define MAX_NUMBER_OF_HITS 100

class PndEmcPSAFPGAIntegratingAnalyser : public PndEmcPSAFPGASampleAnalyser {

 public:
  PndEmcPSAFPGAIntegratingAnalyser();
  PndEmcPSAFPGAIntegratingAnalyser(const std::string &fname);
  virtual ~PndEmcPSAFPGAIntegratingAnalyser();

  virtual float baseline() { return baseline_value; };
  virtual void reset();
  virtual void put(float valueToStore);

  ClassDef(PndEmcPSAFPGAIntegratingAnalyser, 1);
};

#endif
