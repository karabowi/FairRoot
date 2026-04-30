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

/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors:                                                       *
 *     Copyright 2009       Matthias Kretz <kretz@kde.org>                *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#ifndef PNDFTSARRAYIO_H
#define PNDFTSARRAYIO_H

#include <iostream>
#include "PndFTSArray.h"

namespace {
namespace AnsiColor {
static const char *const green = "\033[1;40;32m";
static const char *const yellow = "\033[1;40;33m";
static const char *const blue = "\033[1;40;34m";
static const char *const normal = "\033[0m";
} // namespace AnsiColor
} // anonymous namespace

template <typename T>
inline std::ostream &operator<<(std::ostream &out, const PndFTSArray<T> &a)
{
  out << AnsiColor::blue << "{" << AnsiColor::normal;
  for (int i = 0; i < a.Size(); ++i) {
    if (i > 0) {
      out << AnsiColor::blue << ", " << AnsiColor::normal;
    }
    out << a[i];
  }
  out << AnsiColor::blue << "}" << AnsiColor::normal;
  return out;
}
#endif // ALIHLTARRAYIO_H
