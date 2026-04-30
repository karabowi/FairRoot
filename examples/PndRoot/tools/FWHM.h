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

/*
 * FWHM.h
 *
 *  Created on: 22.11.2023
 *      Author: tstockmanns
 */

#ifndef TOOLS_FWHM_H_
#define TOOLS_FWHM_H_

#include <TH1.h>

static double FWHM(TH1* h1){
  int bin1 = h1->FindFirstBinAbove(h1->GetMaximum()/2);
  int bin2 = h1->FindLastBinAbove(h1->GetMaximum()/2);
  double fwhm = h1->GetBinCenter(bin2) - h1->GetBinCenter(bin1);

//  std::cout << h1->GetName() << std::endl;
//  std::cout << "FWHM: max: " << h1->GetMaximum() << " halfMax " << h1->GetMaximum()/2 << " bin1 - bin2 " << bin1 << " - " << bin2 << " fwhm " << fwhm << std::endl;
  return fwhm;
}

#endif /* TOOLS_FWHM_H_ */
