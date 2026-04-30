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

/**
 * @class PndGFRecoHitFactoryCreator
 * @brief Helper class to connect Panda hits with genfit2 measurements via the genfit MeasurementFactory
 *
 * @date 19.04.2022
 * @author Tobias Stockmanns
 */

#pragma once

#include <MeasurementProducer.h>
#include <MeasurementFactory.h>
#include <AbsMeasurement.h>

class PndGFRecoHitFactoryCreator {
 public:
  PndGFRecoHitFactoryCreator();
  virtual ~PndGFRecoHitFactoryCreator();

  void AddBranch(TString branchName) { fBranches[branchName] = nullptr; }

  Bool_t Init();

  genfit::AbsMeasurementProducer<genfit::AbsMeasurement> *CreateProducer(TString branchName, TClonesArray *tcArray);
  genfit::MeasurementFactory<genfit::AbsMeasurement> *GetFactory() { return fTheRecoHitFactory; }

 protected:
  void FillRecoHitFactory();
  void SetDefaultBranches();
  void InitBranches();

 private:
  genfit::MeasurementFactory<genfit::AbsMeasurement> *fTheRecoHitFactory = nullptr;
  TClonesArray *fTubeArray = nullptr;
  TClonesArray *fFtsTubeArray = nullptr;
  std::map<TString, TClonesArray *> fBranches; //< branchName, matching TClonesArray;
};
