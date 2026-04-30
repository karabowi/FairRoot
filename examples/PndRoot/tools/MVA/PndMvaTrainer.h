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
 * MVA classifiers trainers interface.          *
 * Author: M. Babai                             *
 * M.Babai@rug.nl                               *
 * Version: 0.1 beta1.                          *
 * LICENSE:                                     *
 * **********************************************
 */
//#pragma once
#ifndef PND_MVA_TRAINER_H
#define PND_MVA_TRAINER_H

// C++ includes
#include <cassert>
#include <limits>
#include <ctime>
#include <iomanip>
#include <cstdlib>

// ROOT and PandaRoot
#include "TMVA/Tools.h"
#include "TMVA/PDEFoam.h"
#include "TMVA/Event.h"

class TRandom3;

// Local includes
#include "PndMvaDataSet.h"
#include "PndMvaUtil.h"

#define TRAIN_INC_FOAM 0

class PndMvaTrainer {
  //==============================================
  //================ Public =======================
 public:
  /**
   * Constructor.
   *@param InPut The input parameters.
   *@param ClassNames Names of available labels(classes).
   *@param VarNames Names of available variables.
   */
  explicit PndMvaTrainer(std::vector<std::pair<std::string, std::vector<float> *>> const &InputEvtsParam, std::vector<std::string> const &ClassNames,
                         std::vector<std::string> const &VarNames, bool trim = true);

  /**
   * Constructor.
   *@param InPut, The input filename.
   *@param ClassNames Names of available classes.
   *@param VarNames Names of available variables.
   */
  explicit PndMvaTrainer(std::string const &InPut, std::vector<std::string> const &ClassNames, std::vector<std::string> const &VarNames, bool trim = true);

  //! Destructor
  virtual ~PndMvaTrainer();

  //! Derived classes need to implement this methode.
  virtual void Train() = 0;

  /**
   * Store weights in the output File. If output file name is not
   * specified, then write nothing.
   */
  virtual void storeWeights() = 0;

  /**
   * Creates test and train data sets.
   * @param percent Percent of the data set to be used for testing.
   */
  void SetTestSetSize(size_t percent = 50);

  /**
   * Set the indices of events that are going to be used for testing.
   *@param testSet Set containing the indices of the test events.
   */
  void SetTestSet(std::set<size_t> const &testSet);

  /**
   * Select input data normalization scheme.
   */
  void NormalizeData(NormType t = NONORM);

  /**
   * Parameter decorrelation.
   * Performs PCA (Principal component analysis) on the input dataset.
   */
  void PCATransForm();

  /**
   * Setter to set the weightfile name.
   *@param outFile Output filename.
   */
  inline void SetOutPutFile(std::string const &outFile);

  /**
   * Writes the train and test errors evaluations to a given file.
   *@param FileName Output file name.
   */
  void WriteErroVect(std::string const &FileName) const;

  /**
   * Get the list of objects that contain the classifier evaluation
   * results.
   *@return  List of evaluation objects.
   */
  inline std::vector<StepError> const &GetErrorValues() const;

  /**
   * Initialize data structures.
   */
  virtual void Initialize();

  /**
   * Get the indices of the events selected to be used for testing.
   *@return A set containing the indices of test events.
   */
  inline std::set<size_t> const &GetTestEvetIdx() const;

  /**
   * Get the list of available classes (labels).
   *@retrun Vector containing available labels.
   */
  inline std::vector<PndMvaClass> const &GetClasses() const;

  /**
   * Get the list of available variables.
   *@retrun Vector containing available Parameters (features).
   */
  inline std::vector<PndMvaVariable> const &GetVariables() const;

  /**
   * Classifier evaluation.
   */
  virtual void EvalClassifierError();

  inline size_t GetRndSeed() const;
  inline void SetRndSeed(size_t const sd);

  //______________________________________________
  //================ Protected ===================
 protected:
  /**
   * Set application type for the current object
   */
  inline void SetAppType(AppType t);

  /**
   * Write the training and normalization data to outFile.
   */
  void WriteToWeightFile(std::vector<std::pair<std::string, std::vector<float> *>> const &weights) const;

#if (TRAIN_INC_FOAM > 0)
  /**
   * Writes generated TMVA Foams to file.
   *@param foams The list of the foams to be stored.
   * Note that the implementation of TMVA foams here is in early
   * experimental stage.
   */
  void WriteToWeightFile(std::vector<TMVA::PDEFoam *> const &foams) const;
#endif
  // void WriteDataSetToOutFile();

  //! Indices of the test set.
  std::set<size_t> m_testSet_indices;

  //! Data set. Holds event values
  PndMvaDataSet m_dataSets;

  //! Container to keep per step error values.
  std::vector<StepError> m_StepErro;

  //! Output filename.
  std::string m_outFile;

  //! Random seed
  size_t m_RND_seed;

  void splitTetsSet();
  //______________________________________________
  //================ Private =====================
 private:
  //! To avoid mistakes.
  PndMvaTrainer(PndMvaTrainer const &other);
  PndMvaTrainer &operator=(PndMvaTrainer const &other);

  // trim or not
  bool m_trim;
  size_t m_testSetSize;
}; // End of class definition.

//========================= Inline implementations =================
inline size_t PndMvaTrainer::GetRndSeed() const
{
  return this->m_RND_seed;
};

inline void PndMvaTrainer::SetRndSeed(size_t const sd)
{
  this->m_RND_seed = sd;
};

inline void PndMvaTrainer::SetOutPutFile(std::string const &outFile)
{
  m_outFile = outFile;
};

inline void PndMvaTrainer::SetAppType(AppType t)
{
  m_dataSets.SetAppType(t);
};

inline std::set<size_t> const &PndMvaTrainer::GetTestEvetIdx() const
{
  return m_testSet_indices;
};

//! Get the list of available classes (labels).
inline std::vector<PndMvaClass> const &PndMvaTrainer::GetClasses() const
{
  return m_dataSets.GetClasses();
};

//! Get the list of available variables
inline std::vector<PndMvaVariable> const &PndMvaTrainer::GetVariables() const
{
  return m_dataSets.GetVars();
};

// @return  List of evaluation objects.
inline std::vector<StepError> const &PndMvaTrainer::GetErrorValues() const
{
  return m_StepErro;
};
#endif
