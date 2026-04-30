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
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class BSEmcExpClusterSplittingProcess.
//      Concrete implementation of BSEmcAbsClusterSplitter which splits
//      on the basis of exponential distance from the bump centroid.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother
//
// Copyright Information:
//	Copyright (C) 1997               Imperial College
//
// Modified:
// M. Babai
//------------------------------------------------------------------------

#ifndef BSEMCLATERALCLUSTERSPLITTINGPROCESS_HH
#define BSEMCLATERALCLUSTERSPLITTINGPROCESS_HH

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"
#include "TVector3.h"

#include "BSEmcExpClusterSplittingProcess.h"
#include "BSEmcLateralClusterSplittingPar.h"

class BSEmcDigi;
class BSEmcCluster;
class BSEmcSubCluster;
class FairParGenericSet;
class BSEmcClusteringPar;
class BSEmcCrystalPositionPar;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcClusteringData;
template <class T>
class PndContainerI;
template <class T>
class PndMutableContainerI;

/**
 * @class BSEmcLateralClusterSplittingProcess
 * @brief splits clusters on the basis of more complicated lateral shower developement parametrization
 *  distance from the subCluster centroid
 * @details to be run after BSEmcLocalMaxFindingProcess or BSEmcRemoveSplitOffMax
 * @author Guang Zhao
 * @date 2024-02
 * @ingroup Barrel
 */
class BSEmcLateralClusterSplittingProcess : public BSEmcExpClusterSplittingProcess {
 public:
  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_paramRegister) /*override*/;
  void SetLateralClusterSplittingParName(const std::string t_parName) { fLateralClusterSplittingParName = t_parName; }

  BSEmcLateralClusterSplittingProcess() : BSEmcExpClusterSplittingProcess(){};
  virtual ~BSEmcLateralClusterSplittingProcess(){};

 private:
  std::string fLateralClusterSplittingParName{""};
  TArrayD fParArray1;
  TArrayD fParArray2;
  TArrayD fParArray3;
  TArrayD fParArray4;

 private:
  virtual Double_t
  CalculateWeight(Int_t t_currentDigiDetID, Int_t t_currentMaxDetId, const TVector3 &t_currentdigisPosition, const std::map<Int_t, DigiLocationInfo> &t_centroidPositions) const;
  Double_t LateralDevelopment(const Double_t t_totalEnergy, const TVector3 &t_currentDigiPos, const TVector3 &t_maxDigiPos, const TVector3 &t_showerCenter,
                              const Double_t t_moliereRadius) const;

  ClassDef(BSEmcLateralClusterSplittingProcess, 0);
};
#endif /*BSEMCLATERALCLUSTERSPLITTINGPROCESS_HH*/
