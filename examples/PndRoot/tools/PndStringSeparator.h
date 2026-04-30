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

/**
 * @class PndStringSeparator
 *
 * @brief Separates a string into substrings at arbitrary delimiters
 * @details
 * # Separation into sub-strings
 * Separates an input string into substrings and stores them in a
 * string vector. The separation criteria is a string of characters.
 * If no delimiters are given the default delimiter " " is used.
 *
 * ## Example
 * ```
 *    std::vector<std::string> fOutput;
 *    PndStringSeparator fInput("Column:Row Type: Test ", ": ");
 *    fOutput = fInput.GetStringVector();
 *
 *    fOutput[0] = "Column"
 *    fOutput[1] = "Row"
 *    fOutput[2] = "Type"
 *    fOutput[3] = "Test"
 * ```
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 */

#include <string>
#include <vector>
#include "Rtypes.h"

class PndStringSeparator {
 public:
  PndStringSeparator() : fStartPos(0), fStrings(), fInput(), fDelimiter(), fOutput(), fFirstDel(false), fLastDel(false){}
  ~PndStringSeparator(){}
  explicit PndStringSeparator(std::string AInput, std::string ADelimiter = " ");
  void SetInput(std::string AInput) { fInput = AInput; }
  void SetDelimiter(std::string ADelimiter) { fDelimiter = ADelimiter; }
  void ResetVector() { fStrings.clear(); }
  std::vector<std::string> GetStringVector(void);


  std::string Replace(std::string from, std::string to);

  void Print();

 protected:
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
  bool GetIfFirst() { return fFirstDel; }
  bool GetIfLast() { return fLastDel; }

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

