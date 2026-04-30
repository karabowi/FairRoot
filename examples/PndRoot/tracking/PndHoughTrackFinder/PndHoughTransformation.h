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

/////////////////////////////////////////////////////////////////
//  PndHoughTransformation
//  Hough transformation based on the apollonius problem
/////////////////////////////////////////////////////////////////

/** PndHoughTransformation
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 29.10.2018
 *@updated 23.11.2021
 *@version 2.0
 **
 ** PANDA class for a hough transformation based on the apollonius problem
 ** Task level RECO
 **/

#ifndef PndHoughTransformation_H_
#define PndHoughTransformation_H_

#include "PndHoughMultipletCreator.h"
#include "PndHoughApollonius.h"
#include "PndHoughSpace.h"
#include "PndHoughUtilities.h"
#include "PndHoughData.h"

#include <vector>
using std::vector;

class PndHoughTransformation {

 public:
  PndHoughTransformation(PndHoughData *data) //: fHoughSpace(data->GetHoughSpace())
  {
    fPndHoughMultipletCreator = new PndHoughMultipletCreator();
    fPndHoughApollonius = new PndHoughApollonius(data);
    // fPndHoughUtilities = new PndHoughUtilities(data);
    fData = data;
  };

  virtual ~PndHoughTransformation()
  {
    delete fPndHoughMultipletCreator;
    delete fPndHoughApollonius;
    // delete fPndHoughUtilities;
  };

  /** @brief Main class of the Hough transformation: the Hough space is filled and the maximum is returned */
  vector<TVector3> FindMaximaWithHoughTransformation(PndTrackCand &trackCand);
  /** @brief Here the Hough space is filled by calcualting all Apollonius circles.  */
  void HoughSpace(PndTrackCand &trackCand);
  /** @brief Selects one maximum of many by choosing the one with the smalles RMS to the track */
  TVector3 MaximumSelectiondCandToTrack(PndTrackCand &trackCand, vector<TVector3> &TrackCircleVec);

  virtual void SetUtilities(PndHoughUtilities *utilities) { fPndHoughUtilities = utilities; };

 private:
  // PndHoughSpace *fHoughSpace = nullptr;
  PndHoughMultipletCreator *fPndHoughMultipletCreator = nullptr;
  PndHoughApollonius *fPndHoughApollonius = nullptr;
  PndHoughUtilities *fPndHoughUtilities = nullptr;
  PndHoughData *fData = nullptr;

  ClassDef(PndHoughTransformation, 1);
};

#endif /*PndHoughTransformation_H_*/
