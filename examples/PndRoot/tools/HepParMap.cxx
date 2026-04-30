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

#include "HepParMap.h"
#include <iostream>

using namespace parmaptypes;

// -----------------------------------------------------------------------
//   Parameter map str -> str: turn opt string "a=3:b=173.3:c=herbert" in parameter map<TStr, TStr>
// -----------------------------------------------------------------------
HepParMap::HepParMap(TStr opt, TStr delim, TStr assign, int strip) : fStrip(strip)
{
  if (delim=="")  delim=":";
  if (assign=="") assign="=";
  
  fParMap.clear();
  fParList.clear();
  
  if (!opt.EndsWith(delim)) opt = opt+delim;
  
  TvStr list = split(opt, delim);
  int lena = assign.Length();
  
  for (auto x:list) {
    // ##### a boolean (no assignment character) will be an int set to 1
    if (!x.Contains(assign)) x += assign+"1";
    
    // ##### split assignment X = Y to X and Y (and remove white space)
    TStr par = x(0, x.Index(assign));
    TStr val = x(x.Index(assign)+lena, x.Length());
    
    // ##### parameter name will always be stripped (kBoth)
    par = par.Strip(TStr::kBoth);
    val = val.Strip(static_cast<TStr::EStripType>(strip));
    
    if (par!="") {
      fParMap[par] = val;
      fParList.push_back(par);
    }
  }
}

// -----------------------------------------------------------------------
// register variable recall (store name, default, description)
// -----------------------------------------------------------------------
void HepParMap::Register(TStr pname, TStr desc, TStr preset)
{
  fRecall.insert(pname);
  fDescript[pname] = desc;
  fPresets[pname]  = preset;
}

// -------------------------------------------------------------------------
//   split string
// -------------------------------------------------------------------------
TvStr HepParMap::split(TStr s, TStr delim, unsigned int strip, bool removeEmpty)
{
  // ##### Special case whitespace (WS): squeeze all to single white space separation, i.e. replace tabs and multiple blanks by single blank
  if (delim == "WS") {
    s.ReplaceAll("\t", " ");
    while (s.Contains("  "))
      s.ReplaceAll("  ", " ");
    delim = " ";
  }

  // ##### if s not empty string, append delimiter to avoid special treatment of last element
  if (s!="") s = s+delim;
  
  int dlen = delim.Length();
  int slen = s.Length();
  
  TvStr res;

  while (s.Contains(delim)) {
    int pos = s.Index(delim);
    TStr elem = s(0, pos);
    if (strip>0) elem = elem.Strip(static_cast<TStr::EStripType>(strip));
    s = s(pos+dlen, slen);
    if (elem!="" || !removeEmpty) res.push_back(elem);
  }
  
  return res;
}

// -------------------------------------------------------------------------
//   join string
// -------------------------------------------------------------------------
TStr HepParMap::join(TvStr vs, TStr sep,unsigned int strip) 
{
  TStr res;
  int ns = vs.size();
  for (int i=0; i<ns; ++i) {
    if (strip>0) vs[i] = vs[i].Strip(static_cast<TStr::EStripType>(strip));
    res += vs[i];
    if (i<ns-1) res += sep;
  }
  return res;
}

// -------------------------------------------------------------------------
//   Print Message and optionally exit execution
// -------------------------------------------------------------------------
void HepParMap::Msg(TStr msg)
{
  // ##### line ends with '$nn' -> no new line
  bool nlb = msg.EndsWith("$nn");
  if (nlb) msg = msg(0,msg.Length()-3);

  printf("#####  HepParMap  --  %s%s", msg.Data(), nlb ? "" : "\n");
}

// -----------------------------------------------------------------------
//   pick parameter from parmap
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// pick vector<double> 
// -----------------------------------------------------------------------
TvDbl HepParMap::VDbl(TStr pname, TvDbl pval, TStr desc, bool fill, TStr delim)
{
  if (delim=="") delim = ",";   // default delimiter
  Register(pname, desc, join(d2str(pval), ","));
  
  if (fParMap.count(pname)>0) {
    TvDbl vpar;
    for (auto x:split(fParMap[pname], delim)) vpar.push_back(x.Atof());
        
    // ##### if new vector is shorter than default, fill missing elements
    if (fill)
      while (vpar.size()<pval.size())
        vpar.push_back(pval[vpar.size()]);
      
    return vpar;
  } 
  else return pval;
}

// -----------------------------------------------------------------------
// pick vector<int> 
// --------------------------------------------------------------------
TvInt HepParMap::VInt(TStr pname, TvInt pval, TStr desc, bool fill, TStr delim)
{
  if (delim=="") delim = ",";   // default delimiter
  Register(pname, desc, join(i2str(pval), ","));

  if (fParMap.count(pname)>0) {
    TvInt vpar;
    for (auto x:split(fParMap[pname], delim)) vpar.push_back(x.Atoi());

    // ##### if new vector is shorter than default, fill missing elements
    if (fill)
      while (vpar.size()<pval.size())
        vpar.push_back(pval[vpar.size()]);

    return vpar;
  } 
  else return pval;
}

// -----------------------------------------------------------------------
// pick vector<TStr> 
// --------------------------------------------------------------------
TvStr HepParMap::VStr(TStr pname, TvStr pval, TStr desc, bool fill, TStr delim)
{
  if (delim=="") delim = ",";   // default delimiter  
  Register(pname, desc, join(pval, ","));

  if (fParMap.count(pname)>0) {
    TvStr vpar;
    for (auto x:split(fParMap[pname], delim, fStrip)) vpar.push_back(x);

    // ##### if new vector is shorter than default, fill missing elements
    if (fill)
      while (vpar.size()<pval.size())
        vpar.push_back(pval[vpar.size()]);

    return vpar;
  } 
  else return pval;
}

// -----------------------------------------------------------------------
// Print Parmap
// --------------------------------------------------------------------
int HepParMap::Print(bool full)
{
  int maxlen = 0;
  int maxlenval = 0;
  for (auto x:fParMap) {
		if (maxlen<x.first.Length()) maxlen = x.first.Length();
		if (maxlenval<x.second.Length()) maxlenval = x.second.Length();
	}
  if (full) {
		for (auto x:fPresets) {
			if (maxlen<x.first.Length() && fParMap.find(x.first)==fParMap.end()) maxlen = x.first.Length();
			if (maxlenval<x.second.Length() && fParMap.find(x.first)==fParMap.end()) maxlenval = x.second.Length();
		}
	}
  TStr frmt = Form("%%-%ds = %%-%ds%%s\n", maxlen, maxlenval+2);
  
  for (auto x:(full ? fPresets : fParMap)) {
    TStr var = x.first;
    TStr val = (fParMap.find(x.first)!=fParMap.end() ? fParMap[x.first] : x.second);
    if (val=="") val = "\"\"";
    TStr def  = full && fParMap.find(x.first)!=fParMap.end() ? Form("(default = \"%s\")", x.second.Data()) : "";
    TStr desc = full && fDescript[x.first]!="" ? "  // "+fDescript[x.first]+" "+def : def;
    printf(frmt, var.Data(), val.Data(), desc.Data());
  }
  
  if (full) CheckRecall(true,"\nUnknown parameters: ");
  
  return fParMap.size();  
}

// -----------------------------------------------------------------------
// Check all given parameters were recalled (check for typos)
// --------------------------------------------------------------------
TvStr HepParMap::CheckRecall(bool verbose, TStr txt)
{
  TvStr norec;
  for (auto p:fParList)
    if (fRecall.count(p)==0) norec.push_back(p);
  
  if (verbose && norec.size()>0) 
    std::cout << txt << join(norec,", ") << std::endl;

  return norec;
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
//  END OF HEPPARMAP
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

