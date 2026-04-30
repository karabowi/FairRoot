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

#ifndef PNDCATIMER_H
#define PNDCATIMER_H

/*
 *=====================================================
 *
 *  Authors: I.Kulakov
 *
 */

#include "TStopwatch.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

using std::cout;
using std::endl;
using std::ios;
using std::map;
using std::setw;
using std::string;
using std::vector;

class PndCATimerInfo {
 public:
  PndCATimerInfo() : fName(""), fReal(0), fCpu(0){};
  PndCATimerInfo(const string &name) : fName(name), fReal(0), fCpu(0){};

  void Clear()
  {
    fReal = 0;
    fCpu = 0;
  }

  void operator=(TStopwatch &sw)
  {
    fReal = sw.RealTime();
    fCpu = sw.CpuTime();
  };
  void operator+=(TStopwatch &sw)
  {
    fReal += sw.RealTime();
    fCpu += sw.CpuTime();
  };
  void operator+=(const PndCATimerInfo &t)
  {
    fReal += t.fReal;
    fCpu += t.fCpu;
  }
  PndCATimerInfo operator/(const float f) const
  {
    PndCATimerInfo r;
    r.fName = fName;
    r.fReal = fReal / f;
    r.fCpu = fCpu / f;
    return r;
  }

  // void Print(){ cout << fReal << "/" << fCpu; };
  void PrintReal() { cout << fReal; };
  float Real() { return fReal; };
  string &Name() { return fName; };

 private:
  string fName;
  float fReal, fCpu;
};

class PndCATFIterTimerInfo {
 public:
  void Clear()
  {
    for (unsigned int i = 0; i < fTIs.size(); i++)
      fTIs[i].Clear();
  };

  void Add(string name)
  {
    fNameToI[name] = fTIs.size();
    fTIs.push_back(PndCATimerInfo(name));
  };
  PndCATimerInfo &operator[](string name) { return fTIs[fNameToI[name]]; };
  PndCATimerInfo &operator[](int i) { return fTIs[i]; };
  void operator+=(PndCATFIterTimerInfo &t)
  {
    for (unsigned int i = 0; i < fTIs.size(); ++i)
      fTIs[i] += t[i];
  }
  PndCATFIterTimerInfo operator/(float f)
  {
    PndCATFIterTimerInfo r;
    r.fNameToI = fNameToI;
    r.fTIs.resize(fTIs.size());
    for (unsigned int i = 0; i < fTIs.size(); ++i) {
      r.fTIs[i] = fTIs[i] / f;
    }
    return r;
  }

  void PrintReal(int f = 0)
  {
    if (f) {
      PrintNames();
      cout << endl;
    }
    fTIs[0].PrintReal();
    for (unsigned int i = 1; i < fTIs.size(); ++i) {
      cout << " | " << setw(fTIs[i].Name().size());
      fTIs[i].PrintReal();
    }
    if (f)
      cout << endl;
  };
  void PrintNames()
  {
    cout << fTIs[0].Name();
    for (unsigned int i = 1; i < fTIs.size(); ++i) {
      cout << " | " << fTIs[i].Name();
    }
  };

 private:
  map<string, int> fNameToI;
  vector<PndCATimerInfo> fTIs;
};

class PndCATFTimerInfo {
 public:
  PndCATFTimerInfo(){};
  PndCATFTimerInfo(int n) { fTIIs.resize(n); };
  void SetNIter(int n) { fTIIs.resize(n); };

  void Clear()
  {
    for (unsigned int i = 0; i < fTIIs.size(); i++)
      fTIIs[i].Clear();
    fTIAll.Clear();
  };

  void Add(string name)
  {
    for (unsigned int i = 0; i < fTIIs.size(); ++i)
      fTIIs[i].Add(name);
    fTIAll.Add(name);
  }; // use after setniter
  PndCATFIterTimerInfo &GetTimerAll() { return fTIAll; };
  PndCATFIterTimerInfo &operator[](int i) { return fTIIs[i]; };
  void operator+=(PndCATFTimerInfo &t)
  {
    for (unsigned int i = 0; i < fTIIs.size(); ++i)
      fTIIs[i] += t[i];
    fTIAll += t.GetAllInfo();
  }
  PndCATFTimerInfo operator/(float f)
  {
    PndCATFTimerInfo r;
    r.fTIAll = fTIAll / f;
    r.SetNIter(fTIIs.size());
    for (unsigned int i = 0; i < fTIIs.size(); ++i) {
      r.fTIIs[i] = fTIIs[i] / f;
    }
    return r;
  }

  void Calc()
  {
    fTIAll = fTIIs[0];
    for (unsigned int i = 1; i < fTIIs.size(); ++i)
      fTIAll += fTIIs[i];
  }

  PndCATFIterTimerInfo &GetAllInfo() { return fTIAll; };
  void PrintReal()
  {
    cout.precision(1);
    cout.setf(ios::fixed);
    cout << " stage "
         << " : ";
    fTIAll.PrintNames();
    cout << endl;
    for (unsigned int i = 0; i < fTIIs.size(); ++i) {
      cout << " iter " << i << " : ";
      fTIIs[i].PrintReal();
      cout << endl;
    }
    cout << " all   "
         << " : ";
    fTIAll.PrintReal();
    cout << endl;
  };

 private:
  vector<PndCATFIterTimerInfo> fTIIs;
  PndCATFIterTimerInfo fTIAll;
};

#endif
