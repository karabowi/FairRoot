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

//-----------------------------------------------------------
// Description:
//      Experimentally derived Pulseshape of the form:
//	f(x) = A * (t')^ N * exp(-N t')
//-----------------------------------------------------------
//#pragma once
#ifndef PNDEMCFITTEDPULSESHAPE_NW_H
#define PNDEMCFITTEDPULSESHAPE_NW_H

#include "PndEmcAbsPulseshape.h"

/**
 * @brief Experimentally derived Pulseshape
 *
 * of the form f(x) = A * (t')^ N * exp(-N t')
 * @ingroup PndEmc
 */
class PndEmcFittedPulseshape_nw : public PndEmcAbsPulseshape {

 public:
	  PndEmcFittedPulseshape_nw();
	  PndEmcFittedPulseshape_nw(double v1, double tcr, double tcf, double t_r, double res, double tau, double N);
	  virtual ~PndEmcFittedPulseshape_nw(){};
          
	  virtual double operator() (const double t, const double amp, const double toffset) const;
	private:
    
	  double fTau;
	  double fN;
           double fv1;// = 0;
           double ftcr;// = 26.3995;//2.11196*12.5;
          double ftcf;// = 302.76625;//24.2213*12.5;
          double ft_r;// = 52.69625;//4.2157*12.5;
          double fres;// = 0;
	ClassDef(PndEmcFittedPulseshape_nw,1)
};

#endif
