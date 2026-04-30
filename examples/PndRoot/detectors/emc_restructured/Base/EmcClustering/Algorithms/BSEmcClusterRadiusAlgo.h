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

#ifndef BSEMCCLUSTERRADIUSALGO_H
#define BSEMCCLUSTERRADIUSALGO_H

#include "RtypesCore.h"

/**
 * @class BSEmcClusterRadiusAlgo
 * @brief Helper class to determine a precluster radius (used in MakePrecluster)
 * @details Preclusters are merged by looking at overlapping radii of two precluster
 * @author Marcel Tiemens (?)
 * @author Viktor Rodin
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcClusterRadiusAlgo {
 public:
  BSEmcClusterRadiusAlgo(){};
  ~BSEmcClusterRadiusAlgo(){};

  Double_t GetModifiedRadius(Double_t t_clusradius, Int_t t_n = 1) const
  {
    if (t_n == 0) {
      t_n = 1;
    }
    return t_clusradius + t_n * fSingleCrystalRadius;
  }
  // Add single crystal radius to precluster radius, because distance to crystal centre is taken and not its size.
  // If this correction is not implemented, preclusters consisting of a single crystal will get a radius of 0 and will never be merged to another cluster.
  // Added multiplier n to allow the use of the diameter, or manually increase the radius of a cluster
  Double_t GetModifiedXRadius(Double_t xrad, Int_t n = 1) const { return xrad + n * fSingleCrystalRadius; }
  Double_t GetModifiedYRadius(Double_t yrad, Int_t n = 1) const { return yrad + n * fSingleCrystalRadius; }

  void SetSingleCrystalRadius(Double_t t_radius) { fSingleCrystalRadius = t_radius; }
  Double_t GetSingleCrystalRadius() const { return fSingleCrystalRadius; }

 private:
  Double_t fSingleCrystalRadius{1.84}; // cm = single crystal radius = sqrt(2)*1.3 = 1.8385, 1.3 = 0.5*crystal size
};

#endif /*BSEMCCLUSTERRADIUSALGO_H*/
