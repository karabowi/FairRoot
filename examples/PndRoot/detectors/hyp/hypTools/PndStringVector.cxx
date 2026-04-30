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

#include "PndStringVector.h"
#include <iostream>
#include <sstream>

PndStringVector::PndStringVector(std::string AInput, std::string ADelimiter)
{
  SetInput(AInput);
  SetDelimiter(ADelimiter);
}

std::string PndStringVector::GetString(void)
{
  std::string::size_type pos;
  std::string aString;
  pos = fInput.find_first_of(fDelimiter.c_str(), fStartPos);
  if (pos - fStartPos == 0) { // first value at StartPos is a Delimiter
    fStartPos += 1;
    return "";
  }

  else if (pos != std::string::npos) { // a delimiter was found after StartPos
    aString = fInput.substr(fStartPos, pos - fStartPos);
    fStartPos = pos + 1;
    return aString;
  }

  else { // no delimiter was found after StartPos
    aString = fInput.substr(fStartPos, pos - fStartPos);
    fStartPos = pos;
    return aString;
  }
}

std::vector<std::string> PndStringVector::GetStringVector(void)
{
  fStartPos = 0;
  std::string value;
  ResetVector();
  TestFirst();
  TestLast();

  std::cout << " vector " << std::endl;
  std::cout << " vector " << std::endl;

  while (fStartPos != std::string::npos) {
    value = GetString();
    if (value.length() > 0)
      fStrings.push_back(value);
    std::cout << " vector " << std::endl;
  }
  return fStrings;
}

void PndStringVector::Print()
{
  std::cout << "PndStringVector for: " << fInput << std::endl;
  for (Int_t i = 0; i < (int)fStrings.size(); i++)
    std::cout << i << ": " << fStrings[i] << std::endl;
}
