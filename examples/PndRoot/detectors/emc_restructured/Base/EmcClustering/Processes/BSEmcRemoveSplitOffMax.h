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

//--------------------------------------------------------------------------
// Description:
//	Class EmcERatioLocMaxMaxFinder./
//      Searches for local maxima in a cluster based on the ratio
//      between the energy of the maxima crystal and that of
//      its neighbours
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother
//      Helmut Schmuecker                   Ruhr-Uni Bochum
//
// Copyright Information:
//	Copyright (C) 1997	            Imperial College
// Modified:
// M. Babai
//------------------------------------------------------------------------
#ifndef BSEMCREMOVESPLITOFFMAX_HH
#define BSEMCREMOVESPLITOFFMAX_HH
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"

#include "BSEmcCluster.h"

#include "BSEmcGeoNeighbouringRelationPar.h"

class BSEmcClusteringPar;
class BSEmcDigi;
class BSEmcCluster;
class BSEmcGeoNeighbouringRelationPar;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcClusteringData;
struct BSEmcDigiInfo_t;
template <class T>
class PndContainerI;
template <class T>
class PndMutableContainerI;

/**
 * @class BSEmcRemoveSplitOffMax
 * @brief Process to be run after BSEmcLocalMaxFindingProcess to remove maxima from cluster that match certain criteria
 * @details To be run after
 * @author Phil Strother
 * @author Helmut Schmuecker                   Ruhr-Uni Bochum
 * @author M. Babai
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcRemoveSplitOffMax : public PndProcess {
 public:
  BSEmcRemoveSplitOffMax();
  virtual ~BSEmcRemoveSplitOffMax();
  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  void RemoveSplitOffMax(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcCluster *> &t_clusterCont);

  void SetClusteringParName(const std::string &t_parName) { fClusteringParName = t_parName; }
  void SetNeighbouringRelationParName(const std::string &t_parName) { fNeighbouringRelationParName = t_parName; }

  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchname = t_branchname; }
  void SetClusterBranchName(const TString &t_branchname) { fClusterBranchname = t_branchname; }

 protected:
  Bool_t IsSplitOffOrHadron(const BSEmcDigi *t_digi, const std::vector<const BSEmcDigi *> &t_neighbours) const;
  std::vector<const BSEmcDigi *>
  GetNeigboursOf(const std::vector<const BSEmcDigi *> &t_digiCont, const BSEmcDigiInfo_t &t_digi, const std::vector<BSEmcDigiInfo_t> &t_potentialneighbours) const;

 private:
  std::string fClusteringParName{""};
  std::string fNeighbouringRelationParName{""};

  TString fDigiBranchname{""};
  TString fClusterBranchname{""};

  PndContainerI<BSEmcDigi> *fDigiArray{nullptr};
  PndMutableContainerI<BSEmcCluster> *fClusterArray{nullptr};
  Double_t fCutSlope{-1};
  Double_t fCutOffset{-1};
  Double_t fERatioCorr{-1};
  Double_t fNeighbourECut{-1};
  Double_t fMaximumsEnergyThreshold{-1};
  BSEmcGeoNeighbouringRelationPar *fNeighbouringRelationPar{nullptr};
  ClassDef(BSEmcRemoveSplitOffMax, 1);
};

#endif /*BSEMCREMOVESPLITOFFMAX_HH*/
