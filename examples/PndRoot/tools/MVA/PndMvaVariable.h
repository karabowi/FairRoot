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
 * MVA variable class definition.             *
 * Author: M.Babai@rug.nl                     *
 * LICENSE:                                   *
 * Version:                                   *
 * License:                                   *
 * *******************************************
 */
//#pragma once
#ifndef PND_MVA_VARIABLE_H
#define PND_MVA_VARIABLE_H

/**
 * Struct to describe a single variable of the feature vector, storing
 * its name and normalization factor.
 */
struct PndMvaVariable {
  /**
   * Constructor.
   *@param name Variable name.
   *@param normFactor normalization factor for this variable.
   *@param mean Current variable sample mean value.
   */
  explicit PndMvaVariable(std::string const &name = "UNKNOWN_VAR", float normFactor = 1.0, float mean = 0.0, float min = 0.0, float max = 0.0);

  //! Destructor.
  virtual ~PndMvaVariable();

  // Copy Const.
  PndMvaVariable(PndMvaVariable const &oth);

  // Assign.
  PndMvaVariable &operator=(PndMvaVariable const &oth);

  std::string Name; /**< Name of the variable. */
  /**
   * Normalization factor of the variable.
   * Sample Variance IQR.
   */
  float NormFactor; // Sigma
  float Mean;       // Mean value
  float Min;        // Minimum value
  float Max;        // Maximum value

 private:
  bool operator==(PndMvaVariable const &oth) const;
  bool operator>(PndMvaVariable const &oth) const;
  bool operator<(PndMvaVariable const &oth) const;
}; // End of interface.

//_________________________ Implement. ___________________
//! Constructor implementation.
inline PndMvaVariable::PndMvaVariable(std::string const &name, float normFactor, float mean, float min, float max)
  : Name(name), NormFactor(normFactor), Mean(mean), Min(min), Max(max){};

//! Destructor
inline PndMvaVariable::~PndMvaVariable(){};

//! Copy constructor
inline PndMvaVariable::PndMvaVariable(PndMvaVariable const &oth) : Name(oth.Name), NormFactor(oth.NormFactor), Mean(oth.Mean), Min(oth.Min), Max(oth.Max){};

//! Assignment operator
inline PndMvaVariable &PndMvaVariable::operator=(PndMvaVariable const &oth)
{
  // check for self-assignment
  if (this != &oth) { // Not equal, thus deep copy
    this->Name = oth.Name;
    this->NormFactor = oth.NormFactor;
    this->Mean = oth.Mean;
    this->Min = oth.Min;
    this->Max = oth.Max;
  }
  return (*this);
} // End of interface definition.
#endif
