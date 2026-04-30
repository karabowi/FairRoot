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

/*************************************
 * The structure of the classes.     *
 * Author: M.Babai@rug.nl            *
 * Version:                          *
 * License:                          *
 *************************************/
//#pragma once
#ifndef PND_MVA_CLASS_H
#define PND_MVA_CLASS_H

/**
 * Struct to describe a class of particles. It stores the name of the class,
 * the number of examples available and their indices.
 */
struct PndMvaClass {
  //! Constructor.
  /**
   *@Param name Class name.
   */
  explicit PndMvaClass(std::string const &name);

  //! Destructor
  virtual ~PndMvaClass();

  //! Copy constructor
  PndMvaClass(PndMvaClass const &oth);

  //! = Operator
  PndMvaClass &operator=(PndMvaClass const &oth);

  std::string Name; /**< Name of the class.*/
  size_t NExamples; /**< Number of examples available of the class.*/
  size_t StartIdx;  /**< Start index of events of this class.*/
  size_t EndIdx;    /**< End index of events of this class.*/
  size_t NTrainEx;  /**< Number of examples in the train set.*/

 private:
  bool operator==(PndMvaClass const &oth) const;
  bool operator>(PndMvaClass const &oth) const;
  bool operator<(PndMvaClass const &oth) const;
}; // End of interface.

//_________________________ Implement. __________________
/**
 * Constructor implementation.
 *@Param name Class name.
 */
inline PndMvaClass::PndMvaClass(std::string const &name) : Name(name), NExamples(0), StartIdx(0), EndIdx(0), NTrainEx(0){};

//! Copy Constructor.
inline PndMvaClass::PndMvaClass(PndMvaClass const &oth) : Name(oth.Name), NExamples(oth.NExamples), StartIdx(oth.StartIdx), EndIdx(oth.EndIdx), NTrainEx(oth.NTrainEx){};

//! = operator.
inline PndMvaClass &PndMvaClass::operator=(PndMvaClass const &oth)
{
  // check for self-assignment
  if (this != &oth) // Not self-assign
  {
    this->Name = oth.Name;
    this->NExamples = oth.NExamples;
    this->StartIdx = oth.StartIdx;
    this->EndIdx = oth.EndIdx;
    this->NTrainEx = oth.NTrainEx;
  }
  return (*this);
};

//! Destructor
inline PndMvaClass::~PndMvaClass(){};
#endif
