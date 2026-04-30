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
 * PndTrackFunctor.cxx
 *
 *  Created on: 06.02.2017
 *      Author: Stockmanns
 */
#include "PndTrackFunctor.h"
#include "FairLogger.h"

PndTrackFunctor *PndTrackFunctor::make_PndTrackFunctor(std::string functorName)
{
  if (functorName.compare("StandardTrackFunctor") == 0)
    return new StandardTrackFunctor();
  if (functorName.compare("OnlySttFunctor") == 0)
    return new OnlySttFunctor();
  if (functorName.compare("OnlySttTimeBasedFunctor") == 0)
    return new OnlySttTimeBasedFunctor();
  if (functorName.compare("RiemannMvdSttGemFunctor") == 0)
    return new RiemannMvdSttGemFunctor();
  if (functorName.compare("CircleHoughTrackFunctor") == 0)
    return new CircleHoughTrackFunctor();
  if (functorName.compare("FtsTrackFunctor") == 0)
    return new FtsTrackFunctor();
  if (functorName.compare("NoFtsTrackFunctor") == 0)
    return new NoFtsTrackFunctor();
  else {
    LOG(error) << " PndTrackFunctor: No valid functorName given: " << functorName;
    return new AllTracksFunctor();
  }
}
