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
//	Class PndEmcErrorMatrix
//      Calculate Error Matrix for the given EmcCluster
//      with parametrization defined by the given parameter PndEmcErrorMatrixPar
//
//------------------------------------------------------------------------

//#pragma once
#ifndef PNDEMCERRORMATRIX_H
#define PNDEMCERRORMATRIX_H

#include "TMatrixD.h"
#include "PndEmcCluster.h"
#include "TString.h"
#include <cstdlib>

class PndEmcErrorMatrixPar;
class PndEmcErrorMatrixParObject;

/**
 * @brief Calculate Error Matrix for the given EmcCluster with parametrization defined by the given parameter PndEmcErrorMatrixPar
 *
 * @ingroup PndEmc
 */
class PndEmcErrorMatrix {
 public:
  // Constructors
  PndEmcErrorMatrix();
  // Destructor
  ~PndEmcErrorMatrix();

  void Init(PndEmcErrorMatrixParObject *par);
  void InitFromFile(Int_t geomVersion);
  PndEmcErrorMatrixParObject *GetParObject();

  // Error Matrix for Cluster
  // 4x4 matrix in E, theta, phi, R
  TMatrixD GetErrorMatrix(const PndEmcCluster &cluster) const;

  // 4x4 matrix in (px, py, pz, E)
  TMatrixD Get4MomentumErrorMatrix(const PndEmcCluster &cluster) const;

  // 7x7 matrix in (x, y, z, px, py, pz, E)
  TMatrixD GetErrorP7(const PndEmcCluster &cluster) const;

 private:
  PndEmcErrorMatrixParObject *fErrorMatrixParObject;

  PndEmcErrorMatrix(const PndEmcErrorMatrix &L);
  PndEmcErrorMatrix &operator=(const PndEmcErrorMatrix &) { return *this; };
};

// helper function to convert error matrix
// copied from BbrGeom/BbrError.cc
TMatrixD similarityWith(const TMatrixD &mat, const TMatrixD &m1);

#endif
