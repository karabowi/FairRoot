#ifndef PNDTOOLS_NAMESPACE_H
#define PNDTOOLS_NAMESPACE_H

#include <set>
#include <map>
#include <vector>
#include <cstdarg>
#include <string>


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
//   define namespace for aux functions
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
class TF1;

namespace pndtools {
// -----------------------------------------------------------------------
//   some handy type defs
// -----------------------------------------------------------------------
  using namespace std;

  typedef double           TDbl;
  typedef string           TStr;
  typedef int              TInt;

  typedef vector<TInt>     TvInt;
  typedef vector<TDbl>     TvDbl;
  typedef vector<TStr>     TvStr;

  typedef set<TStr>        TsStr;
  typedef map<TStr, TStr>  TmStrStr;
  
  TStr Form(TStr fmt, ...);
  void Msg(TStr fcn, TStr msg);
  
  TStr strip(TStr ss);
  TvStr split(TStr s, TStr delim=":", TInt strp=0, bool removeEmpty=false);
  TStr join(TvStr vs, TStr sep="", TInt strp=0);
  TvStr d2str(TvDbl vdbl, TStr fmt="%g");
  TvStr i2str(TvInt vint, TStr fmt="%d");
  TvDbl str2d(TvStr vstr);
  TvInt str2i(TvStr vstr);
  
  void PrintFcn(TF1 *f, bool compact=true); 
}


#endif
