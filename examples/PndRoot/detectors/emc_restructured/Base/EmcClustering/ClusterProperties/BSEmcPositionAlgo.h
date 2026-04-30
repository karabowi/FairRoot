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

#ifndef BSEMCPOSITIONALGO_HH
#define BSEMCPOSITIONALGO_HH

#include <functional>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"
#include "TVector3.h"

struct BSEmcCrystalPositionData;
class BSEmcCrystalPositionPar;
class BSEmcClusterPropertiesPar;
class BSEmcDigi;
class BSEmcCluster;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcPositionAlgo
 * @brief Helper class to calculate the cluster position
 * @details Offers three different position calculation methods
 * 1. LiLo Linear/Logarithmic weighting of each cluster crystal by its energy
 * 2. Grav linear weighted position sum divided by the cluster energy
 * 3. Linear Crystal energy divided by cluster energy as weight of crystal position
 *
 * CalculatePosition will choose one of the 3 methods, depending on BSEmcClusteringPar::GetEmcClusterPosMethod()
 * @author Xiaorong Shi            Lawrence Livermore National Lab
 * @author Steve Playfer           University of Edinburgh
 * @author Stephen Gowdy           University of Edinburgh
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPositionAlgo {
 public:
  BSEmcPositionAlgo();
  virtual ~BSEmcPositionAlgo();

  void SetClusterPropertiesPar(const BSEmcClusterPropertiesPar *t_par);
  void SetPositionPar(const BSEmcCrystalPositionPar *t_position) { fPositionPar = t_position; }
  virtual TVector3 CalculatePosition(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digiArray) const;

  TVector3 CalculateLinearPosition(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digiArray) const;
  TVector3 CalculateGravPosition(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digiArray) const;
  TVector3 CalculateLiLoPosition(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digiArray) const;

 protected:
  BSEmcCrystalPositionData GetClusterCrystal(const BSEmcCluster *t_cluster, Double_t t_theta, Double_t t_phi) const;

 private:
  std::vector<Double_t> fClusterPosParam{};
  std::string fClusterPosMethod{""};
  const BSEmcCrystalPositionPar *fPositionPar{nullptr};
  std::function<TVector3(const BSEmcCluster *, const std::vector<const BSEmcDigi *> &)> fPositionCalculationMethod{};

  ClassDef(BSEmcPositionAlgo, 1);
};

#endif /*BSEMCPOSITIONALGO_HH*/
