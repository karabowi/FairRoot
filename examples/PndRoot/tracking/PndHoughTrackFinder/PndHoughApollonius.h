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

/////////////////////////////////////////////////////////////
//  PndHoughApollonius
//  Calculate Apollonius Circles
/////////////////////////////////////////////////////////////////

/** PndHoughApollonius
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 29.10.2018
 *@updated 23.11.2021
 *@version 3.1
 **
 ** PANDA class for calculating Apollonius circles
 ** Task level RECO
 **/

#ifndef PndHoughApollonius_H_
#define PndHoughApollonius_H_

#include "FairTask.h"
#include "PndTrackCand.h"
#include <vector>
#include "PndHoughData.h"

class PndHoughApollonius {

 public:
  PndHoughApollonius(){};

  PndHoughApollonius(PndHoughData *data)
    : fMapFairLinktoFairHit(data->GetMapFairLinktoFairHit()), fMapFairLinktoIsochrone(data->GetMapFairLinktoIsochrone()),
      fMapFairLinktoIsochroneError(data->GetMapFairLinktoIsochroneError()), fData(data){};

  virtual ~PndHoughApollonius(){};
  /** @brief      This functions brings the FairLinks in a simpler structure for using cuda. */
  void ApolloniusCuda(std::vector<std::vector<FairLink>> &multiplets, double apolloniusCircles[]);
  /** @brief      Calculation of Apollonius circles on CPU. (This is the same as implemented in cuda). */
  static void ApolloniusCudaCalcCPU(int num, double hit0[], double hit1[], double hit2[], double apolloniusCircles[]);

 private:
  std::map<FairLink, FairHit *> fMapFairLinktoFairHit;
  std::map<FairLink, Double_t> fMapFairLinktoIsochrone;
  std::map<FairLink, Double_t> fMapFairLinktoIsochroneError;
  PndHoughData *fData = nullptr;

  ClassDef(PndHoughApollonius, 2);
};

#endif /*PndHoughApollonius_H_*/
