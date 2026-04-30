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

#ifndef PNDCATES_H
#define PNDCATES_H

struct PndCATES { // Type for an Element on Station
  PndCATES() : s(-1), e(-1) {}
  PndCATES(char is, unsigned int ie) : s(is), e(ie) {}
  PndCATES(unsigned short int ie, char is) : s(is), e(ie) {} // to prevent confusion

  operator unsigned int() { return e; }

  friend bool operator!=(const PndCATES &a, const PndCATES &b) { return (a.s != b.s) || (a.e != b.e); }

  char s;         // index of station
  unsigned int e; // index of element
};

struct PndCATESV { // Type for an Element on Station
  PndCATESV() : s(-1), e(-1) {}
  PndCATESV(const PndCATES &i) : s(i.s), e(i.e) {}
  PndCATESV(int_v is, uint_v ie) : s(is), e(ie) {}
  PndCATESV(uint_v ie, int_v is) : s(is), e(ie) {} // to prevent confusion

  operator uint_v() const { return e; }
  PndCATES operator[](int i) const { return PndCATES(s[i], e[i]); }
  float_m IsValid() const { return static_cast<float_m>(s >= Vc::Zero); }

  friend float_m operator==(const PndCATESV &a, const PndCATES &b) { return (a.s == static_cast<unsigned int>(b.s)) & (a.e == b.e); }

  int_v s;  // index of station
  uint_v e; // index of element
};

#endif
