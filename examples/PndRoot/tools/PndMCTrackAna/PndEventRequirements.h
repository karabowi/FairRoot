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
// -----                      PndEventRequirements header file                   -----
// -----                  Created 16/09/19  by T.Stockmann             -----
// -------------------------------------------------------------------------

/** PndEventRequirements.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 ** Data class to contain additional information for an MCTrack
 **
 **/

#pragma once

#include "PndParticleRequirements.h"
#include "TVector3.h"
#include "TVector2.h"
#include <iostream>

class PndEventRequirements {

 public:
  /**  Default constructor  **/
  PndEventRequirements();

  /**  Destructor  **/
  virtual ~PndEventRequirements();

  // needs to be called before first use once the FairRootManager is available (in the init of the task using it)
  void InitRequirements();

  friend std::ostream &operator<<(std::ostream &out, PndEventRequirements &track)
  {
    int i = 0;
    for (auto req : track.fTrackRequirements) {
      out << i++ << " : " << req << std::endl;
    }
    return out;
  }

  /**  Setters  **/
  void AddTrackRequirement(PndParticleRequirements val)
  {
    if (val.GetCheckTracking() == true)
      fChargedTracks.push_back(fTrackRequirements.size());
    if (val.GetCheckNeutral() == true)
      fNeutralTracks.push_back(fTrackRequirements.size());
    fTrackRequirements.push_back(val);
  }

  std::vector<PndParticleRequirements> GetRequirements() { return fTrackRequirements; }
  int GetNRequirements() const { return fTrackRequirements.size(); }
  int GetNParticles() { return fTrackRequirements.size(); }
  std::vector<int> GetChargedRequirements() const { return fChargedTracks; }
  std::vector<int> GetNeutralRequirements() const { return fNeutralTracks; }
  void ClearFound()
  {
    for (auto &track : fTrackRequirements)
      track.ClearFound();
  }

 protected:

 private:
  std::vector<PndParticleRequirements> fTrackRequirements;
  std::vector<int> fChargedTracks;  // index to requirement of charged particle
  std::vector<int> fNeutralTracks;  // index to requirement of neutral particle

  ClassDef(PndEventRequirements, 1);
};

