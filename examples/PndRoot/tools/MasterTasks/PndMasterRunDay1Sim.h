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

#pragma once

#include "PndMasterRunSim.h"

class PndMasterRunDay1Sim : public PndMasterRunSim {
 public:
  PndMasterRunDay1Sim();
  virtual ~PndMasterRunDay1Sim();

  virtual void CreateGeometry();

  virtual void SetupTargetMode() { SetTargetMode(1); };

 private:
  /** @cond CLASSIMP */
  ClassDef(PndMasterRunDay1Sim, 1);
  /** @endcond */
};
