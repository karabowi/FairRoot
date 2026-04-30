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

#include <iostream>
#include <string>

#include "PndDocuSample.h"

PndDocuSample::PndDocuSample() : fInitialized(false){};

PndDocuSample::PndDocuSample(std::string _s, int _i) : fString(_s), fNumber(_i), fInitialized(true)
{
  PrintValues();
}

/**
 * @brief ... or in the declaration file
 * @details With the header file documentations coming first.
 */
void PndDocuSample::PrintValues()
{
  std::cout << "Values are currently set to: " << std::endl;
  std::cout << "  fString: " << fString << std::endl;
  std::cout << "  fNumber: " << fNumber << std::endl;
}

/**
 * @brief Squares the current number
 * @details Squares @p fNumber
 */
void PndDocuSample::SquareNumber()
{
  fNumber *= fNumber;
}

/**
 * @brief Adds the current string to itself
 */
void PndDocuSample::AddString()
{
  fString += fString;
}

/** @cond CLASSIMP */
ClassImp(PndDocuSample);
/** @endcond */
