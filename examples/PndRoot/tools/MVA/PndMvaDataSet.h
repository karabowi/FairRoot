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

/***************************************
 * Class interface of DataSet class.   *
 * Author: M.Babai (M.Babai@rug.nl)    *
 * License:                            *
 * Version:                            *
 ***************************************/
//#pragma once
#ifndef PND_MVA_DATASET_H
#define PND_MVA_DATASET_H

// C++ includes
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <limits>
#include <typeinfo>
#include <exception>
#include <utility>

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"

// Local includes
#include "PndMvaClass.h"
#include "PndMvaVariable.h"
#include "PndMvaVarPCATransform.h"

// ========================================================================
// Application type
typedef enum AppType {
  UNKAPP = 0,
  TRAIN = 1,         // Training algorithm.
  CLASSIFY = 2,      // Read weights to do classification.
  TMVATRAIN = 10,    // Provide input for TMVA Training.
  TMVACLS = 20,      // TMVA classification.
  PRE_INIT_EVTS = 30 // Pre-initialized event data.
} AppType;

// Normalization schemes
typedef enum NormType {
  NONORM = 0, // Do nothing
  VARX = 1,   // Use Sample variance
  MINMAX = 2, // Use Sample Min and Max
  MEDIAN = 3, // Use median and interquartile range (IQR).
  VARNORM = 4 // Variable Normalize Transform
} NormType;

////=================== Exception =================

class PndMvaDataSetException : public std::exception {
 public:
  explicit PndMvaDataSetException() : m_message("UNKNOWN_MvaDataSetException"){};

  explicit PndMvaDataSetException(std::string const &val) : m_message(val){};

  virtual ~PndMvaDataSetException() throw(){};

  virtual char const *what() const throw() { return m_message.c_str(); };

  virtual std::string const &what() { return m_message; };

 private:
  std::string m_message;
};
////___________________ Exception _________________

// ==================== Data set class ==========================
class PndMvaDataSet {
 public:
  /**
   * Constructor.
   *@param InputEvtsParam  Input event data (attributes).
   *@param classNames      Names of available Labels (classes).
   *@param varNames        Available variable names.
   *@param type            Application Type.
   *
   * Note: The data from "InputEvtsParam" is copied into the internal
   * container. Use with caution in case of large data sets.
   */
  explicit PndMvaDataSet(std::vector<std::pair<std::string, std::vector<float> *>> const &InputEvtsParam, std::vector<std::string> const &classNames,
                         std::vector<std::string> const &varNames, AppType type);
  /**
   * Constructor.
   *@param inputFilename  Input File name.
   *@param classNames     Names of available Labels (classes).
   *@param varNames       Available variable names.
   *@param type           Application Type.
   */
  explicit PndMvaDataSet(std::string const &WeightFile, std::vector<std::string> const &classNames, std::vector<std::string> const &varNames, AppType type);

  //! Destructor
  virtual ~PndMvaDataSet();

  /**
   * Write the normalized DataSet to the out-put file.
   * @param  outFile  File name to write to
   */
  // void WriteDataSet(std::string const& outFile) __attribute__ ((deprecated));
  virtual void WriteDataSet(std::string const &outFile);

  /**
   * Initialize the class conditional means vectors and modifies the
   * attributes of the available labels for this data set
   * (PndMvaClass::NTrainEx); these are the events that are not to be
   * excluded. These events are specified by the exclude indices set.
   *
   *@param excludeIndxs The set of event indices to exclude during the
   * computation.
   */
  virtual void InitClsCondMeans(std::set<size_t> const &excludeIndxs);

  /**
   * If trimming is needed.
   *@param t If trim.
   */
  inline void SetTrim(bool t);

  //! Get available data vectors.
  inline std::vector<std::pair<std::string, std::vector<float> *>> const &GetData() const;

  //! Get the list of available classes (labels).
  inline std::vector<PndMvaClass> const &GetClasses() const;

  //! Get the list of available variables.
  inline std::vector<PndMvaVariable> const &GetVars() const;

  //! Get classconditional means for all classes (labels).
  inline std::map<std::string, std::vector<float> *> const &GetClassCondMeans() const;

  //! Get name of input file name (weight/event file).
  inline std::string const &GetInFileName() const;

  //========================= PCA =====================//
  /**
   * Parameter decorrelation.
   *
   * Performs PCA (Principal component analysis) on the input dataset.
   */
  virtual void PCATransForm();

  /**
   *@return If PCA was applied.
   */
  inline bool Used_PCA() const;

  /**
   *@param t If apply PCA.
   */
  inline void Use_PCA(bool t);

  /**
   * Get PCA object
   *@return PCA object containing PCA parameters.
   */
  inline PndMvaVarPCATransform const &Get_PCA() const;

  //_________________________ PCA _____________________//

  /**
   *@retun Normalization type.
   */
  inline NormType GetNormType() const;

  /**
   *@param t Normalization type (VARX, MINMAX, MEDIAN).
   */
  inline void SetNormType(NormType t);

  /**
   *@return Application type.
   */
  inline AppType GetAppType() const;

  /**
   *@param t Application type.*/
  inline void SetAppType(AppType t);

  /**
   * Init Dataset. Determine how to handle input, based on the
   * application type.
   */
  virtual void Initialize();

  inline size_t GetRndSeed() const;
  inline void SetRndSeed(size_t const sd);

  //______________________________________________________________
 protected:
  /**
   * Read input event data.
   */
  void ReadInput();

  /**
   * Read Weights and parameters from file.
   */
  void ReadWeightsFromFile();

  //==============================================================
 private:
  // Private to avoid mistakes.
  PndMvaDataSet(PndMvaDataSet const &other);
  PndMvaDataSet &operator=(PndMvaDataSet const &other);

  /**
   * Creates a data set with equal number of events for each class.
   */
  void Trim();

  /**
   * Normalize event dataset using one of available methods.
   */
  void NormalizeDataSet();

  /**
   * Init labels (class names).
   *@param labels The list of labels to use.
   */
  void InitClasses(std::vector<std::string> const &labels);

  /**
   * Init Variables.
   *@param variables The list of variables to use.
   */
  void InitVariables(std::vector<std::string> const &variables);

  // Validate the input file
  void ValidateWeightFile();

  /**
   * Class conditional mean for a given label (class). Stored in class
   * conditional means container.
   *@param clsName Label for which CCM is computed.
   *@param exCluds Set of indices of events that are not to be used.
   */
  void CompClsCondMean(std::string const &clsName, std::set<size_t> const &exCluds);

  /**
   * Computes Variance (unbiased estimator) for each parameter in the
   * feature list.
   */
  void ComputeVariance();

  /**
   * Determines the median for parameters of the loaded DataSet.
   */
  void DetermineMedian();

  /**
   * Determine Min Max difference.
   */
  void MinMaxDiff();

  /**
   * Find minimum and maximum per variable.
   */
  void FindMinMax();

  /**
   * VariableNormalizeTransform. Linear interpolation.
   */
  void VarNormalize();

  // __________________________ Member parameters ___________
  //! Input File name
  std::string m_input;

  //! Classes.
  std::vector<PndMvaClass> m_classes;

  //! Variables.
  std::vector<PndMvaVariable> m_vars;

  //! Container to keep  the Event data feature vectors
  std::vector<std::pair<std::string, std::vector<float> *>> m_events;

  //! Container to keep  the Class Conditional means
  std::map<std::string, std::vector<float> *> m_ClassCondMeans;

  // PCA transformation.
  PndMvaVarPCATransform m_PCA;

  // If PCA was applied.
  bool m_UsePCA;

  // Normalization scheme
  NormType m_NormType;

  // Application type.
  AppType m_AppType;
  bool m_trim;
  size_t m_RND_seed;
};
// End of class interface definition.

// ============= Inline implementation ==================

inline size_t PndMvaDataSet::GetRndSeed() const
{
  return this->m_RND_seed;
};

inline void PndMvaDataSet::SetRndSeed(size_t const sd)
{
  this->m_RND_seed = sd;
};

inline std::vector<std::pair<std::string, std::vector<float> *>> const &PndMvaDataSet::GetData() const
{
  assert(m_events.size() != 0);
  return m_events;
};

inline std::vector<PndMvaClass> const &PndMvaDataSet::GetClasses() const
{
  return m_classes;
};

inline std::vector<PndMvaVariable> const &PndMvaDataSet::GetVars() const
{
  return m_vars;
};

inline std::map<std::string, std::vector<float> *> const &PndMvaDataSet::GetClassCondMeans() const
{
  return m_ClassCondMeans;
};

inline std::string const &PndMvaDataSet::GetInFileName() const
{
  return m_input;
};

inline bool PndMvaDataSet::Used_PCA() const
{
  return m_UsePCA;
};
inline void PndMvaDataSet::Use_PCA(bool t)
{
  m_UsePCA = t;
};
inline PndMvaVarPCATransform const &PndMvaDataSet::Get_PCA() const
{
  return m_PCA;
};
inline NormType PndMvaDataSet::GetNormType() const
{
  return m_NormType;
};
inline void PndMvaDataSet::SetNormType(NormType t)
{
  m_NormType = t;
};
inline AppType PndMvaDataSet::GetAppType() const
{
  return m_AppType;
};
inline void PndMvaDataSet::SetAppType(AppType t)
{
  m_AppType = t;
};
inline void PndMvaDataSet::SetTrim(bool t)
{
  m_trim = t;
};
#endif
