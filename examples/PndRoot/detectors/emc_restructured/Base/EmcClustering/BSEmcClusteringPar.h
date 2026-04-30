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
// -----					BSEmcClusteringPar
// -----
// -----	Created 29/04/2019 by B. Salisbury <salisbury@hiskp.uni-bonn.de>
// -------------------------------------------------------------------------

#ifndef BSEMCCLUSTERINGPAR_HH
#define BSEMCCLUSTERINGPAR_HH

#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"

#include "FairParGenericSet.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcClusteringPar
 * @brief Container for clustering parameter
 * @details Digi Threshold, Maximum removing parameter, SubClustering, etc. based on PndEmcRecoPar
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcClusteringPar : public FairParGenericSet {
 public:
  static const std::string fgParameterName;
  BSEmcClusteringPar(const char *t_name = "EmcClusteringPar", const char *t_title = "Emc Geometry Version Flag", const char *t_context = "TestDefaultContext");
  ~BSEmcClusteringPar(void);
  void clear(void) /*override*/;

  Double_t GetDigiEnergyThreshold() const { return fDigiEnergyThreshold; }
  Double_t GetClusterActiveTime() const { return fClusterActiveTime; }

  Double_t GetMaximumsEnergyThreshold() const { return fMaximumsEnergyThreshold; }
  Double_t GetCutSlope() const { return fCutSlope; }
  Double_t GetCutOffset() const { return fCutOffset; }
  Double_t GetERatioCorr() const { return fERatioCorr; }
  Double_t GetNeighbourECut() const { return fNeighbourECut; }

  Double_t GetMoliereRadius() const { return fMoliereRadius; }
  Double_t GetExponentialConstant() const { return fExponentialConstant; }
  Int_t GetMaxIterations() const { return fMaxIterations; }
  Double_t GetCentroidShift() const { return fCentroidShift; }
  Int_t GetMaxSubClusters() const { return fMaxSubClusters; }
  Double_t GetMinDigiEnergy() const { return fMinDigiEnergy; }

  Double_t GetClusterEnergyCut() const { return fClusterEnergyCut; }
  Double_t GetSearchConeAngle() const { return fSearchConeAngle; }

  void putParams(FairParamList * /*unused*/) /*override*/;
  Bool_t getParams(FairParamList * /*unused*/) /*override*/;

 private:
  // MakeDigiCluster
  Double_t fDigiEnergyThreshold{-1};
  Double_t fClusterActiveTime{-1};
  // LocalMaxFinder
  Double_t fMaximumsEnergyThreshold{-1};
  Double_t fCutSlope{-1};
  Double_t fCutOffset{-1};
  Double_t fERatioCorr{-1};
  Double_t fNeighbourECut{-1};

  // ExpClusterSplitter
  Double_t fMoliereRadius{-1};
  Double_t fExponentialConstant{-1};
  Int_t fMaxIterations{-1};
  Double_t fCentroidShift{-1};
  Int_t fMaxSubClusters{-1};
  Double_t fMinDigiEnergy{-1};

  Double_t fClusterEnergyCut{-1};
  Double_t fSearchConeAngle{-1};

  ClassDef(BSEmcClusteringPar, 2)
};

#endif /*BSEMCCLUSTERINGPAR_HH*/
