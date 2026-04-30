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

#include "PndEmcFittedPulseshape_nw.h"
#include "math.h"


// Class Member definitions -----------
PndEmcFittedPulseshape_nw::PndEmcFittedPulseshape_nw() : fv1(0),  ftcr(0),  ftcf(0),  ft_r(0), fres(0), fTau(0), fN(0) {
}

PndEmcFittedPulseshape_nw::PndEmcFittedPulseshape_nw(double v1, double tcr, double tcf, double t_r, double res, double tau, double N) : fv1(v1),  ftcr(tcr),  ftcf(tcf),  ft_r(t_r),  fres(res),
 fTau(tau), fN(N) {
}

double PndEmcFittedPulseshape_nw::operator() (const double t, const double amp, const double toffset) const {
	if(t<toffset) {
		return 0.0;
	}  
       // printf("--------t  %f \n ",t);
       // printf("--------toffset  %f \n ",toffset);
	double t_norm = (t-toffset) / fTau;
/*Double_t v1 = 0;
   Double_t v2 = par[0];
   Double_t tdr = toffset;
   Double_t tcr = 2.11196*12.5;
   Double_t tcf = 24.2213*12.5;
   //Double_t t=x[0];
   //
   Double_t t_r = 4.2157*12.5;
    
 */   double tdf = toffset+ft_r;
      double v2 = 1;/*
 Double_t res = 0;
   if (t<tdr)res = v1;*/
   if(t<toffset)  return fres;
   if(t<tdf) return fv1 + amp*(v2 - exp(-(t-toffset )/ftcr));
   else return fv1 + amp*exp(-(t-tdf)/ftcf);
// printf("--------toffset  %f \n ",res);
 //printf("--------toffset  %f \n ",amp * pow(t_norm, fN) * exp(-fN * t_norm));
	//return amp * pow(t_norm, fN) * exp(-fN * t_norm); 
//return res;
  /*                           2589.46   +/-   130.536             45
p1                        =   -0.0650321   +/-   0.0430632         0.2
p2                        =      2.11196   +/-   0.240612    
p3                        =      24.2213   +/-   0.341696    
p4                        =       4.2157   +/-   0.193906 */
  
}

ClassImp(PndEmcFittedPulseshape_nw);
