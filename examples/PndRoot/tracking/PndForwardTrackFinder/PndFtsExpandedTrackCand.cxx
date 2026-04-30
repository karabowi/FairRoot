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
 * PndFtsExpandedTrackCand.cpp
 *
 *  Created on: Jun 2, 2016
 *      Author: kibellus
 */

#include "PndFtsExpandedTrackCand.h"

PndFtsExpandedTrackCand::~PndFtsExpandedTrackCand()
{
  // TODO Auto-generated destructor stub
}

PndFtsExpandedTrackCand::PndFtsExpandedTrackCand(vector<PndFtsHit *> h1, vector<PndFtsHit *> h2, vector<PndLineApproximation> l)
{
  vector<PndFtsHit *> hits;
  for (size_t i = 0; i < h1.size(); i++)
    hits.push_back(h1[i]);
  for (size_t i = 0; i < h2.size(); i++)
    hits.push_back(h2[i]);
  for (size_t i = 0; i < hits.size(); i++) {
    fPndTrackCand.AddHit(hits[i]->GetEntryNr(), i);
  }
  fSourceHits = hits;
  fApproximations = l;
}
