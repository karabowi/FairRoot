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

/* ********************************************
 * MVA Tools and function definitions.        *
 * Author: M.Babai@rug.nl                     *
 * Version:                                   *
 * License:                                   *
 * *******************************************
 */
#pragma once
#ifndef PND_MVA_TOOLS_H
#define PND_MVA_TOOLS_H

// C++
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>

// ROOT
#include "TFile.h"
#include "TTree.h"

/**
 * Structure used to hold the classifier output (label and distance or
 * prob.) for each example in the test set together with the original
 * class name. Used for ROC curves.
 */
struct ClassifierOutPuts {
  // public:
  // Constructors
  explicit ClassifierOutPuts() : realLabel("ALABEL"), givenLabel("NOLABEL"), sgValue(0.00), bgValue(0.00), mom(0.00){};

  /**
   *@param Rlabel True label.
   *@param Glabel Given label.
   *@param sgVal  Classifier output for each signal.
   *@param bgVal  Classifier output for each background.
   */
  explicit ClassifierOutPuts(std::string const &Rlabel, std::string const &Glabel, float sgVal, float bgVal, float p)
    : realLabel(Rlabel), givenLabel(Glabel), sgValue(sgVal), bgValue(bgVal), mom(p){};

  // Destructor
  virtual ~ClassifierOutPuts(){};

  // Copy Const
  ClassifierOutPuts(ClassifierOutPuts const &ot) : realLabel(ot.realLabel), givenLabel(ot.givenLabel), sgValue(ot.sgValue), bgValue(ot.bgValue), mom(ot.mom){};

  // Operators.
  ClassifierOutPuts &operator=(ClassifierOutPuts const &ot)
  {
    // check for self-assignment
    if (this != &ot) {
      // Copy (deep)
      this->realLabel = ot.realLabel;
      this->givenLabel = ot.givenLabel;
      this->sgValue = ot.sgValue;
      this->bgValue = ot.bgValue;
      this->mom = ot.mom;
    }
    return (*this);
  };

  inline bool operator>(ClassifierOutPuts const &ot) const { return (this->sgValue > ot.sgValue); };

  inline bool operator<(ClassifierOutPuts const &ot) const { return (this->sgValue < ot.sgValue); };

  // Variables
  std::string realLabel;  // Original label
  std::string givenLabel; // Given label
  float sgValue;          // Classifier output for label signal
  float bgValue;          // Classifier output for label background
  float mom;              // Momentum (reco.)

  // protected:
 private:
  //==
  inline bool operator==(ClassifierOutPuts const &ot) const;
};

/**
 * Structure to hold the ROC points.
 */
struct ROCPoints {
  // Constructors
  explicit ROCPoints() : FP_rate(0.0), TP_rate(0.0), TN_rate(0.0), FN_rate(0.0), fp(0), tp(0), fn(0), tn(0), thr(0.0){};
  /**
   *@param fpr False positief rate.
   *@param tpr True positief rate.
   *@param tnr True negatief rate.
   *@param fnr False negatief rate.
   *@param nfp False positief count.
   *@param ntp True positief count.
   *@param nfn False negatief.
   *@param ntn True negatief.
   *@param curThr Current treshold value.
   */
  explicit ROCPoints(float const fpr, float const tpr, float const tnr, float const fnr, size_t const nfp, size_t const ntp, size_t const nfn, size_t const ntn, float const curThr)
    : FP_rate(fpr), TP_rate(tpr), TN_rate(tnr), FN_rate(fnr), fp(nfp), tp(ntp), fn(nfn), tn(ntn), thr(curThr){};

  // Destructor
  virtual ~ROCPoints(){};

  // Copy Const
  ROCPoints(ROCPoints const &ot) : FP_rate(ot.FP_rate), TP_rate(ot.TP_rate), TN_rate(ot.TN_rate), FN_rate(ot.FN_rate), fp(ot.fp), tp(ot.tp), fn(ot.fn), tn(ot.tn), thr(ot.thr){};

  // Operators.
  ROCPoints &operator=(ROCPoints const &ot)
  {
    // check for self-assignment
    if (this != &ot) {
      this->FP_rate = ot.FP_rate;
      this->TP_rate = ot.TP_rate;
      this->TN_rate = ot.TN_rate;
      this->FN_rate = ot.FN_rate;
      this->fp = ot.fp;
      this->tp = ot.tp;
      this->fn = ot.fn;
      this->tn = ot.tn;
      this->thr = ot.thr;
    }
    return (*this);
  };

  // Variables
  float FP_rate; // False positief rate
  float TP_rate; // True positief rate
  float TN_rate; // True negatief rate
  float FN_rate; // False negatief rate
  size_t fp;     // False positief count
  size_t tp;     // True positief count
  size_t fn;     // False negatief
  size_t tn;     // True negatief
  float thr;     // Treshold value

  //  protected:

 private:
  bool operator==(ROCPoints const &ot) const;
  bool operator>(ROCPoints const &ot) const;
  bool operator<(ROCPoints const &ot) const;
};

//____________ Functions and modifiers.
//______________________________________________________________
/**
 * Function to produce ROC curve. This will work if the classifier can
 * produce probs or scores.
 *@param input Vector containing classifier outputs for a given test
 * data set. Note that this parameter will be modified (sorted).
 *@param SigName Signal name.
 *@param BgName  Background name.
 *@param sigCnt  Number of signal events.
 *@param bgCnt   Number of background events.
 *@param Roc     The list of ROC points (output var).
 */
void Produce_ROC(std::vector<ClassifierOutPuts> &input, std::string const &SigName, std::string const &BgName, size_t sigCnt, size_t bgCnt, std::vector<ROCPoints> &Roc);
/**
 * Print the list of classifier outputs.
 *@param OutPutList The list of outputs created by a classifier.
 */
void print(std::vector<ClassifierOutPuts> const &OutPutList);

/**
 * Print the output map.
 *@param ClsMapOut The map to print.
 */
void print(std::map<std::string, float> const &ClsMapOut);

/**
 * Reads the event data from the inputfile.
 *@param inFile The input file.
 *@param varNames Name of the parameters (branches per tree).
 *@param classNames Name of the labels to read.
 *@param Outcontainer Output is stored here.
 */
std::map<std::string, size_t> *readEvents(char const *infile, std::vector<std::string> const &varNames, std::vector<std::string> const &classNames,
                                          std::vector<std::pair<std::string, std::vector<float> *>> &Outcontainer);

/**
 * Print list of ROC objects.
 *@param RocList The list of ROC points
 */
void printRoc(std::vector<ROCPoints> const &RocList);

/**
 * Write the list of ROC objects in a file.
 *@param FileName The name of the file to write into.
 *@param RocList The list of the ROC poits to write.
 */
void WriteRocToFile(std::string const &FileName, std::vector<ROCPoints> const &RocList);

//____________________ C style function declarations
#ifdef __cplusplus
extern "C" {
#endif
// Place your C code and/or C headers here.
#ifdef __cplusplus
}
#endif

#endif
// interface definition
/*
  Test for GCC > 3.2.0
  #if __GNUC__ > 3 || (__GNUC__ == 3 && (__GNUC_MINOR__ > 2 || (__GNUC_MINOR__ == 2 && __GNUC_PATCHLEVEL__ > 0) ) )
*/
