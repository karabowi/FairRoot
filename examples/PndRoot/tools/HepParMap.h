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

// ============================================
// HepParMap -- String based parameter manager
// ============================================
// contact: K.Götzen, GSI
// ============================================

#ifndef HEPPARMAP_H
#define HEPPARMAP_H

#include <vector>
#include <map>
#include <set>
#include "TString.h"

// -----------------------------------------------------------------------
//   some handy type defs
// -----------------------------------------------------------------------
namespace parmaptypes {
  typedef double                    TDbl;
  typedef TString                   TStr;
  typedef int                       TInt;

  typedef std::vector<int>          TvInt;
  typedef std::vector<double>        TvDbl;
  typedef std::vector<TString>       TvStr;

  typedef std::set<TString>          TsStr;
  typedef std::map<TString, TString> TmStrStr;
}


// -------------------------------------------------------------------------
//  HepParMap: string to parameter manager
// -------------------------------------------------------------------------

class HepParMap {
public:

  HepParMap(parmaptypes::TStr opt, parmaptypes::TStr delim="", parmaptypes::TStr assign="", int strip = 3);
  ~HepParMap() {};
  
  bool    Exists(TString pname) {return fParMap.count(pname)>0;}
  
  double  Dbl(parmaptypes::TStr pname, double pval=0.0, parmaptypes::TStr desc="") { Register(pname, desc, Form("%g", pval)); return Exists(pname) ? fParMap[pname].Atof() : pval;};
  int     Int(parmaptypes::TStr pname, int pval=0, parmaptypes::TStr desc="")      { Register(pname, desc, Form("%d", pval)); return Exists(pname) ? fParMap[pname].Atoi() : pval;};
  TString Str(parmaptypes::TStr pname, TString pval="", parmaptypes::TStr desc="") { Register(pname, desc, pval);             return Exists(pname) ? fParMap[pname]        : pval;};
  parmaptypes::TvDbl   VDbl(parmaptypes::TStr pname, parmaptypes::TvDbl pval={}, parmaptypes::TStr desc="", bool fill=false, parmaptypes::TStr delim=",");
  parmaptypes::TvInt   VInt(parmaptypes::TStr pname, parmaptypes::TvInt pval={}, parmaptypes::TStr desc="", bool fill=false, parmaptypes::TStr delim=",");
  parmaptypes::TvStr   VStr(parmaptypes::TStr pname, parmaptypes::TvStr pval={}, parmaptypes::TStr desc="", bool fill=false, parmaptypes::TStr delim=",");
  
  parmaptypes::TvStr   ParList() { return fParList; }
  int                  NPar()    { return fParMap.size(); }
  parmaptypes::TvStr   CheckRecall(bool verbose=false, TString txt="Unknown parameters: ");
  parmaptypes::TsStr   Recall()  { return fRecall; }
  int                  Print(bool full=false);
  
  parmaptypes::TvStr split(parmaptypes::TStr s, parmaptypes::TStr delim=":", unsigned int strip=0, bool removeEmpty=false);
  parmaptypes::TStr  join(parmaptypes::TvStr vs, parmaptypes::TStr sep="",unsigned int strip=0);
  parmaptypes::TvStr d2str(parmaptypes::TvDbl vdbl, parmaptypes::TStr fmt="%g") { parmaptypes::TvStr vstr; for (auto d:vdbl) vstr.push_back(Form(fmt.Data(), d)); return vstr; }
  parmaptypes::TvStr i2str(parmaptypes::TvInt vint, parmaptypes::TStr fmt="%d") { parmaptypes::TvStr vstr; for (auto i:vint) vstr.push_back(Form(fmt.Data(), i)); return vstr; }
  
private:
  void Msg(parmaptypes::TStr msg);
  void Register(parmaptypes::TStr pname, parmaptypes::TStr desc, parmaptypes::TStr preset);

  parmaptypes::TmStrStr fParMap;    // map key -> value
  parmaptypes::TmStrStr fPresets;   // store defaults from recall
  parmaptypes::TmStrStr fDescript;  // description of parameter
  parmaptypes::TvStr fParList;      // list of keys
  parmaptypes::TsStr fRecall;       // list of recalled keys
  int fStrip;
};


#endif
