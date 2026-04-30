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

#ifndef L1XYMeasurementInfo_h
#define L1XYMeasurementInfo_h 1

class L1XYMeasurementInfo {

  //* information about measurement

 public:
  L1XYMeasurementInfo() : C00(Vc::Zero), C10(Vc::Zero), C11(Vc::Zero) {}

  L1XYMeasurementInfo(const float_v &c00, const float_v &c10, const float_v &c11) : C00(c00), C10(c10), C11(c11) {}

  float_v C00, C10, C11;
};

#endif
