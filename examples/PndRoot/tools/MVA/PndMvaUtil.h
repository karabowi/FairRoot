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
 * MVA Utility functions and data definitions.*
 * Author: M.Babai@rug.nl                     *
 * Version:                                   *
 * License:                                   *
 * *******************************************
 */
//#pragma once
#ifndef PND_MVA_UTIL_H
#define PND_MVA_UTIL_H

#include <typeinfo>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <cassert>

// =========================================================
//! Structure to hold the per step error values.
struct StepError {
  //! Constructor
  StepError() : m_step(0), m_trErr(0.0), m_tsErr(0.0), m_MisClsTest(std::map<std::string, float>()), m_MisClsTrain(std::map<std::string, float>()){};

  /**
   *@param step Currents step.
   *@param trErr Current training error.
   *@param tsErr Current test error.
   *@param MisClsTest  Mis-classified test events per label.
   *@param MisClsTrain Mis-classified train events per label.
   */
  explicit StepError(size_t step, float trErr, float tsErr, std::map<std::string, float> const &MisClsTest, std::map<std::string, float> const &MisClsTrain)
    : m_step(step), m_trErr(trErr), m_tsErr(tsErr), m_MisClsTest(MisClsTest), m_MisClsTrain(MisClsTrain){};

  //! Destructor
  virtual ~StepError(){};

  //! Copy!
  StepError(StepError const &ot) : m_step(ot.m_step), m_trErr(ot.m_trErr), m_tsErr(ot.m_tsErr), m_MisClsTest(ot.m_MisClsTest), m_MisClsTrain(ot.m_MisClsTrain){};

  //! Assignment.
  StepError &operator=(StepError const &ot)
  {
    // check for self-assignment
    if (this != &ot) { // Not equal, thus deep copy
      this->m_step = ot.m_step;
      this->m_trErr = ot.m_trErr;
      this->m_tsErr = ot.m_tsErr;
      this->m_MisClsTest = ot.m_MisClsTest;
      this->m_MisClsTrain = ot.m_MisClsTrain;
    }
    return (*this);
  };

  unsigned int m_step;                        //! Step number
  float m_trErr;                              //! Train Error
  float m_tsErr;                              //! Test Error
  std::map<std::string, float> m_MisClsTest;  /**< Mis-classified test events/label*/
  std::map<std::string, float> m_MisClsTrain; /**< Mis-classified train events/label*/

 private:
  //! Operator <
  inline bool operator<(StepError const &other) const;
  //! Operator >
  inline bool operator>(StepError const &other) const;
};
// ========================================================================

/**
 * Class to hold the computed Euclidean distances between the current
 * example and the available LVQ protoTypes (codeBook) in LVQ2.1
 * implementation.
 */
struct PndMvaDistObj {
  //! Constructor
  explicit PndMvaDistObj() : m_idx(0), m_dist(0.0), m_cls("UNKNOWN_LABEL"){};

  explicit PndMvaDistObj(size_t const id, float const dist, std::string const &cls) : m_idx(id), m_dist(dist), m_cls(cls){};

  // Copy
  PndMvaDistObj(PndMvaDistObj const &ot) : m_idx(ot.m_idx), m_dist(ot.m_dist), m_cls(ot.m_cls){};

  // Destructor
  virtual ~PndMvaDistObj(){};

  //! operator =
  PndMvaDistObj &operator=(PndMvaDistObj const &ot)
  {
    // check for self-assignment
    if (this != &ot) { // Not equal, thus deep copy
      this->m_idx = ot.m_idx;
      this->m_dist = ot.m_dist;
      this->m_cls = ot.m_cls;
    }
    return (*this);
  };

  //! Operator <
  inline bool operator<(PndMvaDistObj const &other) const { return (this->m_dist < other.m_dist); };

  //! Operator >
  inline bool operator>(PndMvaDistObj const &other) const { return (this->m_dist > other.m_dist); };

  size_t m_idx;      /**< Index of the prototype. */
  float m_dist;      /**< Distance to the current example. */
  std::string m_cls; /**< Class name of the prototype. */

 private:
  bool operator==(PndMvaDistObj const &ot) const;
}; // End interface PndMvaDistObj

//! Less than, comparison funtion.
inline bool CompLess(PndMvaDistObj const *a, PndMvaDistObj const *b)
{
  assert(a && b);
  return ((*a).m_dist < (*b).m_dist);
};

// ========================================================================

/**
 * Binary Minimum function.
 */

template <typename T>
inline T const &minFunct(T const &a, T const &b)
{
  // or: return comp(a,b)?a:b; for the comp version
  return (a < b) ? a : b;
};

//! Less than, comparison funtion.
template <typename T>
inline bool compareL(T const *l, T const *r)
{
  assert(l && r);
  return ((*l) < (*r));
};

/**
 * If 2 sets are disjoint.
 *
 * Input two sorted (ASC) sequences.
 *@return true if the sets are disjoint.
 *
 * NOTE: In some cases one might consider using std::set_intersection
 * from <algorithm>
 */
template <typename Set1, typename Set2>
bool is_disjoint(Set1 const &set1, Set2 const &set2)
{
  // IF one of the sets is empty. O(1) true on empty sets per
  // definition.
  if (set1.empty() || set2.empty()) {
    return true;
  }
  // Start and end iterators of the first sequence.
  typename Set1::const_iterator it1 = set1.begin();
  typename Set1::const_iterator it1End = set1.end();

  // Start and end iterators of the second sequence.
  typename Set2::const_iterator it2 = set2.begin();
  typename Set2::const_iterator it2End = set2.end();

  // This holds because the sequences are pre-sorted. O(1)
  if (*it1 > *set2.rbegin() || *it2 > *set1.rbegin()) {
    return true;
  }

  // Investigate element-wise.
  while ((it1 != it1End) && (it2 != it2End)) {
    if (*it1 == *it2) {
      return false;
    }

    if (*it1 < *it2) {
      it1++;
    } else {
      it2++;
    }
  } // WHILE
  return true;
}

/**
 * Computes the Euclidean distance between two given vectors of
 * event features.
 */
float ComputeDist(std::vector<float> const &EvtData, std::vector<float> const &Example);

// Convert string to int or size_t
int str2int(std::string const &str);
unsigned int str2Uint(std::string const &str);

// Convert int to string
std::string int2str(int n);

//______________ C style function declarations
#ifdef __cplusplus
extern "C" {
#endif
// Place your C code here.

#ifdef __cplusplus
}
#endif

#endif // interface definition
