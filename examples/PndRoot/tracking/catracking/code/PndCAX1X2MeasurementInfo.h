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

//-*- Mode: C++ -*-
// *****************************************************************************
//                                                                             *
// @Autors: I.Kulakov; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#ifndef PNDCAX1X2MEASUREMENTINFO_H
#define PNDCAX1X2MEASUREMENTINFO_H

class PndCAX1X2MeasurementInfo {
 public:
  PndCAX1X2MeasurementInfo(const float_v &c00, const float_v &c10, const float_v &c11) : fC00(c00), fC10(c10), fC11(c11) {}

  const float_v &C00() const { return fC00; }
  const float_v &C10() const { return fC10; }
  const float_v &C11() const { return fC11; }

 private:
  float_v fC00, fC10, fC11; // c00 = <x1,x1>; c10 = <x2,x1>; c11 = <x2,x2>
};

#endif
