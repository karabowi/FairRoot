#include "pndtools.h"
#include "TF1.h"

//using std::vector;
//using std::map;
//using std::string;
//using std


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
//   define namespace for aux functions
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

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
  
  // -----------------------------------------------------------------------
  // generate formatted string
  // -----------------------------------------------------------------------
  TStr Form(TStr fmt, ...)
  {
    char buffer[1024]; // Adjust size as needed
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt.c_str(), args);
    va_end(args);
    return TStr(buffer);  
  }
  
  // -------------------------------------------------------------------------
  //   Print Message and optionally exit execution
  // -------------------------------------------------------------------------
  void Msg(TStr fcn, TStr msg)
  {
    // ##### line ends with '$nn' -> no new line
    bool nlb = msg.substr(msg.size()-3,3) == "$nn";
    if (nlb) msg = msg.substr(0,msg.size()-3);

    printf("#####  %s  --  %s%s", fcn.c_str(), msg.c_str(), nlb ? "" : "\n");
  }

  // -------------------------------------------------------------------------
  //   strip string
  // -------------------------------------------------------------------------
  TStr strip(TStr ss)
  {
    while (ss[0]==' ')           ss=ss.substr(1,ss.size()-1);
    while (ss[ss.size()-1]==' ') ss=ss.substr(0,ss.size()-1);
    return ss;
  }

  // -------------------------------------------------------------------------
  //   split string
  // -------------------------------------------------------------------------
  TvStr split(TStr s, TStr delim, TInt strp, bool removeEmpty)
  {
    // ##### Special case whitespace (WS): squeeze all to single white space separation, i.e. replace tabs and multiple blanks by single blank
    if (delim == "WS") {
      while (int(s.find("\t"))>=0) s.replace(s.find("\t"),1,"");
      while (int(s.find("  "))>=0) s.replace(s.find("  "),2," ");
      delim = " ";
    }

    // ##### if s not empty string, append delimiter to avoid special treatment of last element
    if (s.size()>0) s = s+delim;
    
    TInt dlen = delim.size();
    TInt slen = s.size();
    
    TvStr res;
    
    while (int(s.find(delim))>=0) {
      TInt pos = s.find(delim);
      TStr elem = s.substr(0, pos);
      if (strp>0) elem = strip(elem);
      s = s.substr(pos+dlen, slen);
      if (elem!="" || !removeEmpty) res.push_back(elem);
    }
    
    return res;
  }

  // -------------------------------------------------------------------------
  //   join string
  // -------------------------------------------------------------------------
  TStr join(TvStr vs, TStr sep, TInt strp) 
  {
    TStr res;
    TInt ns = vs.size();
    for (TInt i=0; i<ns; ++i) {
      if (strp>0) vs[i] = strip(vs[i]);//.Strip(static_cast<TString::EStripType>(strip));
      res += vs[i];
      if (i<ns-1) res += sep;
    }
    return res;
  }
  
  // -------------------------------------------------------------------------
  //   vector<double> -> vector<string>
  // -------------------------------------------------------------------------
  TvStr d2str(TvDbl vdbl, TStr fmt) 
  {
    TvStr vstr;
    for (auto d:vdbl) vstr.push_back(Form(fmt.c_str(), d)); 
    return vstr;
  }
  
  // -------------------------------------------------------------------------
  //   vector<int> -> vector<string>
  // -------------------------------------------------------------------------
  TvStr i2str(TvInt vint, TStr fmt)
  {
    TvStr vstr;
    for (auto i:vint) vstr.push_back(Form(fmt.c_str(), i));
    return vstr;
  }
  
  // -------------------------------------------------------------------------
  //   vector<string> -> vector<double>
  // -------------------------------------------------------------------------
  TvDbl str2d(TvStr vstr) 
  {
    TvDbl vdbl;
    for (auto s:vstr) vdbl.push_back(atof(s.c_str()));
    return vdbl;
  }
  
  // -------------------------------------------------------------------------
  //   vector<string> -> vector<int>
  // -------------------------------------------------------------------------
  TvInt str2i(TvStr vstr) 
  {
    TvInt vint;
    for (auto s:vstr) vint.push_back(atoi(s.c_str()));
    return vint;
  }
  
  // --------------------------------------------------------------------------
  // print TF1 with parameters
  // --------------------------------------------------------------------------
  void PrintFcn(TF1 *f, bool compact) 
  {
    if (nullptr==f) return;
    
    TvStr vname;
    TvDbl vval, verr;
    int npar = f->GetNpar();
    int maxlen = -1;
    
    // ##### scan all parameters
    for (int ip=0;ip<npar; ++ip) {
      TStr pname = f->GetParName(ip);
      if (int(pname.size())>maxlen) maxlen = pname.size();
      vname.push_back(pname);
      vval.push_back(f->GetParameter(ip));
      verr.push_back(f->GetParError(ip));
    }
        
    TStr fmt = compact ? Form("%%s = %%g%%s") : Form("%%-%ds = %%7.2f +- %%7.2f\n", maxlen);
    printf("fml = %s = %s%s", f->GetTitle(), f->GetExpFormula().Data(), compact ? ", par = { " : "\n");
    
    for (int ip=0;ip<npar; ++ip) 
      if (compact)
        printf(fmt.c_str(), vname[ip].c_str(), vval[ip], ip<npar-1 ? ", " : " }\n");
      else
        printf(fmt.c_str(), vname[ip].c_str(), vval[ip], verr[ip]);
   
    //for (int ip=0;ip<f->GetNpar(); ++ip) {
      //TString comm;
      //double pmin, pmax, pval = f->GetParameter(ip);
      //f->GetParLimits(ip, pmin, pmax);
      //if (pmin==pmax && pmax!=0.) comm=" (fixed)";
      //else if (fabs(pval-pmin)/fabs(pmin)<1e-6) comm=" (at lower limit)";
      //else if (fabs(pmax-pval)/fabs(pmax)<1e-6) comm=" (at upper limit)";
      ////printf("%-20s -> %5.3g +- %5.3f%s\n", f->GetParName(ip), f->GetParameter(ip), f->GetParError(ip), comm.Data());
      //if (list) {
        //printf(fval.Data(), f->GetParameter(ip));
        //cout << (ip<f->GetNpar()-1 ? ", " : " }\n");
      //} 
      //else
        //printf(fmt.Data(), f->GetParName(ip), f->GetParameter(ip), f->GetParError(ip), comm.Data());
    //}
  }

}
