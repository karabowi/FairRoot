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

/* **********************************************
 * MVA variable transformation interface.       *
 * Principal Components Analysis (PCA).         *
 * Author: M.Babai@rug.nl                       *
 * LICENSE:                                     *
 * Version:                                     *
 * License:                                     *
 * **********************************************
 */
/*
 * This code is directly based on the Cern Root implementation of PCA.
 */
//#pragma once
#ifndef PND_MVA_VAR_PCA_TRANSFORM_H
#define PND_MVA_VAR_PCA_TRANSFORM_H

// C & C++ includes
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>

// ROOT includes.
#include "TPrincipal.h"

class PndMvaVarPCATransform {
 public:
  //! Constructor
  explicit PndMvaVarPCATransform();

  //! Destructor.
  virtual ~PndMvaVarPCATransform();

  /**
   * Prepare Transformation for the given dataset events.
   *@param dat Collection of the event feature vectors.
   */
  bool InitPCATranformation(std::vector<std::pair<std::string, std::vector<float> *>> const &dat);

  /**
   * Transforms the current event variables
   *@param evd Vector containing the event to transform.
   *@return Transformed event.
   */
  std::vector<float> *Transform(std::vector<float> const &evd) const;

  //! Get mean values vector
  inline TVectorD const &GetMeanValues() const;

  //! Get Eigenvectors matrix.
  inline TMatrixD const &GetEigenVectors() const;

  //! Set mean values vector.
  void SetMeanVector(TVectorD const &vect);

  //! Set Eigenvectors matrix.
  void SetEigenVectors(TMatrixD const &mat);

 private:
  //! To avoid mistakes.
  PndMvaVarPCATransform(PndMvaVarPCATransform const &ot);
  PndMvaVarPCATransform &operator=(PndMvaVarPCATransform const &ot);

  /*
   * Given a list of n-dimensional data points, Computes PCA for the
   * current dataset.
   */
  void ComputePrincipalComponents(std::vector<std::pair<std::string, std::vector<float> *>> const &dat);

  // Mean values
  TVectorD *m_MeanValues;

  // Eigenvectors
  TMatrixD *m_EigenVectors;
};
//____________________ ENd of interface definition

/**
 * Get mean values vector.
 */
inline TVectorD const &PndMvaVarPCATransform::GetMeanValues() const
{
  return (*m_MeanValues);
}

/**
 * Get Eigenvectors matrix.
 */
inline TMatrixD const &PndMvaVarPCATransform::GetEigenVectors() const
{
  return (*m_EigenVectors);
}
#endif
