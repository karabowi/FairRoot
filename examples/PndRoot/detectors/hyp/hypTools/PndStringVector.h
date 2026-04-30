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

// -----------------------------------------------------------------
//
//          TPndStringVector
//
//          Version 1.0
//            by
//          Tobias Stockmanns
//  Seperates an input string into substrings and stores them in a
//  string vector. The seperation criteria is a string of characters.
//  If no delimiters are given the default delimiter " " is used.
//
//  Example:
//    std::vector<std::string> Output
//    TPndStringVector Input("Column:Row Type: Test ", ": ");
//    Output = Input.GetStringVector();
//
//    Output[0] = "Column"
//    Output[1] = "Row"
//    Output[2] = "Type"
//    Output[3] = "Test"
//
#ifndef STRINGVECTOR_H
#define STRINGVECTOR_H

#include <string>
#include <vector>
#include "Rtypes.h"

class PndStringVector {
 public:
  PndStringVector() : fFirstDel(false), fLastDel(false){};
  ~PndStringVector(){};
  PndStringVector(std::string AInput, std::string ADelimiter = " ");
  void SetInput(std::string AInput) { fInput = AInput; };
  void SetDelimiter(std::string ADelimiter) { fDelimiter = ADelimiter; };
  void ResetVector() { fStrings.clear(); };
  std::vector<std::string> GetStringVector(void);
  void TestFirst()
  {
    if (fInput.find_first_of(fDelimiter) == 0)
      fFirstDel = true;
    else
      fFirstDel = false;
  }
  void TestLast()
  {
    if (fInput.find_last_of(fDelimiter) == fInput.size() - 1)
      fLastDel = true;
    else
      fLastDel = false;
  }
  void Print();

 private:
  std::string::size_type fStartPos;
  std::vector<std::string> fStrings;
  std::string fInput;
  std::string fDelimiter;
  std::string fOutput;
  std::string GetString(void);
  bool fFirstDel; // first element in the string was a delimiter
  bool fLastDel;  // last element in the string was a delimiter
};

#endif
