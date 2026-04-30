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
// -----                      PndEventCheck header file                   -----
// -----                  Created 16/09/19  by T.Stockmann             -----
// -------------------------------------------------------------------------

/** @class PndEventCheck
 **
 ** Checks an event if the MCTrackInfo matches with the PndEventRequirements
 ** @author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **/

#pragma once

#include "PndParticleRequirements.h"
#include "TVector3.h"
#include "TVector2.h"
#include "PndEventRequirements.h"
#include "TClonesArray.h"
#include <iostream>
#include <vector>

class PndEventCheck {
 public:
  /**  Default constructor  **/
  PndEventCheck();
  PndEventCheck(PndEventRequirements &requirements);

  /**  Destructor  **/
  virtual ~PndEventCheck();

  /**  Output to screen  **/
  void Print(std::ostream &out = std::cout);

  void InitRequirements() { fEventRequirements.InitRequirements(); }

  friend std::ostream &operator<<(std::ostream &out, PndEventCheck &event)
  {
    event.Print(out);
    return out;
  }
  void SetRequirements(PndEventRequirements &req) { fEventRequirements = req; }
  PndEventRequirements GetEventRequirements() const { return fEventRequirements; }

  std::vector<std::vector<int>> CheckEvent(TClonesArray *mcTrackInfo);
  std::vector<std::vector<int>> GetEventResult() const { return fEventResult; }

  /**  Setters  **/

 protected:
  int CheckElectronPair(PndMCTrackInfo *trackInfo, TClonesArray *mcTrackInfos);

 private:
  PndEventRequirements fEventRequirements;
  std::vector<std::vector<int>> fEventResult;

  ClassDef(PndEventCheck, 1);
};

