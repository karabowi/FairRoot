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

//
// Created by Bartosz Sobol on 20.10.2021.
//

#pragma once

namespace PndFtsTrackFinder::PndFtsUtility {

template <typename T>
T PlusIfNotMax(T accu, T curr)
{
  if (curr < std::numeric_limits<T>::max()) {
    return accu + curr;
  }
  return accu;
}

template <typename T>
bool IfNotMax(const T x)
{
  return x < std::numeric_limits<T>::max();
}

} // namespace PndFtsTrackFinder::PndFtsUtility
