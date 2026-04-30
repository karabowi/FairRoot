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

#ifndef BSEMCPARAMETERLOADINGPROCESS_HH
#define BSEMCPARAMETERLOADINGPROCESS_HH

#include "TObjString.h"
#include "TString.h"

#include "FairRun.h"

#include "PndProcess.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcPositionPar.h"
#include "BSEmcStructure.h"

/**
 * @class BSEmcParameterLoadingProcess
 * @brief Process to load neighbouring and position parameters from txtfiles or position values via BSEmcStructure
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcParameterLoadingProcess : public PndProcess {
 public:
  BSEmcParameterLoadingProcess(const TString &t_neighbouringFilename = "EmcCrystalsNeighbouringRelations.txt", const TString &t_positionparFilename = "")
    : PndProcess("BSEmcParameterLoadingProcess"), fNeighbouringFilename(t_neighbouringFilename), fPositionParFilename(t_positionparFilename){};
  virtual ~BSEmcParameterLoadingProcess(){};
  virtual void Process(){};

  void SetNeighbouringFilename(const TString &t_neighbouringFilename) { fNeighbouringFilename = t_neighbouringFilename; }
  void SetPositionParFilename(const TString &t_positionFilename) { fPositionParFilename = t_positionFilename; }

  void SetDetectorName(const std::string &t_detectorName);

  void SetupParameters(const PndParameterRegister *t_parameter);

 private:
  TString fNeighbouringFilename{"EmcCrystalsNeighbouringRelations.txt"};
  TString fPositionParFilename{""};

  void GetPositionParameterFromCrystals(BSEmcCrystalPositionPar *t_par, Int_t t_lowerDetectorId, Int_t t_upperDetectorId);

  void ReadPositionParParameter(BSEmcCrystalPositionPar *t_par, Int_t t_lowerDetectorId, Int_t t_upperDetectorId);

  void FillPositionParameter(BSEmcCrystalPositionPar *t_posPar, Int_t t_inputVersion, Int_t t_lowerDetectorId, Int_t t_upperDetectorId);

  void SetupPositionParameter(BSEmcCrystalPositionPar *t_posPar, BSEmcPositionPar *t_digiPar);

  void SetupPositionParameters(const PndParameterRegister *t_paramRegister);

  void ReadNeighbouringParameter(BSEmcGeoNeighbouringRelationPar *t_par, Int_t t_lowerId, Int_t t_upperId);

  void LoadNeighbouringParameter(BSEmcGeoNeighbouringRelationPar *t_par, Int_t t_version, Int_t t_lowerId, Int_t t_upperId);

  void SetupNeighbouringParameters(const PndParameterRegister *t_paramRegister);
};

#endif /*BSEMCPARAMETERLOADINGPROCESS_HH*/
