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

/*
 * PndPreselectSttHits.h
 *
 *  Created on: 06.07.2021
 *      Author: tstockmanns
 */

#pragma once

#include "TClonesArray.h"
#include <vector>

#include "PndSttHit.h"

class PndStt2GeoHandler;

class PndPreselectSttHits {
 public:

  PndPreselectSttHits(){};


  virtual ~PndPreselectSttHits();

  virtual std::vector<std::vector<PndSttHit *>> PreselectSttHits() = 0;
  virtual std::vector<PndTrackCand> GetPndTrackCands(std::vector<std::vector<PndSttHit *>> &solutions);

  virtual void Init(TClonesArray *sttHits, PndStt2GeoHandler *geoH = nullptr) = 0;
  //virtual void Init(TClonesArray *sttHits, TClonesArray *tubeArray = nullptr) = 0;

  virtual void AddAdditionalBranches(TString name, TClonesArray *array)
  {
    fBranchMap[name] = array;
  } ///< Search for tracks only in given branches. If no BranchName is given all tracking detectors are taken

 protected:
  void SetSttHitsArray(TClonesArray* array){fSttHits = array;}

  TClonesArray* fSttHits= nullptr;
  std::map<TString, TClonesArray *> fBranchMap;
};
