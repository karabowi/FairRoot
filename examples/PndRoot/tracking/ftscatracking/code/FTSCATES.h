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
// @Autors: I.Kulakov; M.Pugach; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Pugach@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#ifndef FTSCATES_H
#define FTSCATES_H

struct TES { // Type for an Element on Station
  TES() : s(-1), e(-1) {}
  TES(char is, unsigned int ie) : s(is), e(ie) {}
  TES(unsigned int ie, char is) : s(is), e(ie) {} // to prevent confusion

  operator unsigned int() { return e; }

  friend bool operator!=(const TES &a, const TES &b) { return (a.s != b.s) || (a.e != b.e); }

  char s;         // index of station
  unsigned int e; // index of element
};

struct TESV { // Type for an Element on Station
  TESV() : s(-1), e(-1) {}
  TESV(const TES &i) : s(i.s), e(i.e) {}
  TESV(int_v is, uint_v ie) : s(is), e(ie) {}
  TESV(uint_v ie, int_v is) : s(is), e(ie) {} // to prevent confusion

  operator uint_v() const { return e; }
  TES operator[](int i) const { return TES(s[i], e[i]); }
  float_m IsValid() const { return static_cast<float_m>(s >= Vc::Zero); }

  friend float_m operator==(const TESV &a, const TES &b) { return (a.s == static_cast<unsigned int>(b.s)) & (a.e == b.e); }

  int_v s;  // index of station
  uint_v e; // index of element
};

#endif
