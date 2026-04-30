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
 * PndTrackingQASummary.cxx
 *
 *  Created on: 05.05.2023
 *      Author: tstockmanns
 */

#include "PndTrackingQASummary.h"

ClassImp(PndTrackingQASummary);

int PndTrackingQASummary::GetAllPossibleTracks()
{
  int result = 0;
  result += GetResult(TrackingQA::qualityNumbers::kMcPossiblePrim);
  result += GetResult(TrackingQA::qualityNumbers::kMcPossibleSec);
  return result;
}

int PndTrackingQASummary::GetAllTracksWithAtLeast3Hits()
{
  int result = 0;
  result += GetResult(TrackingQA::qualityNumbers::kMcAtLeastThreePrim);
  result += GetResult(TrackingQA::qualityNumbers::kMcAtLeastThreeSec);
  result += GetAllPossibleTracks();
  return result;
}

int PndTrackingQASummary::GetAllTracks()
{
  int result = GetAllTracksWithAtLeast3Hits();
  result += GetResult(TrackingQA::qualityNumbers::kMcLessThanThreePrim);
  result += GetResult(TrackingQA::qualityNumbers::kLessThanThreeSec);
  return result;
}

int PndTrackingQASummary::GetFoundPossibleTracks()
{
  int result = GetAllPossibleTracks();
  result -= GetResult(TrackingQA::qualityNumbers::kPossiblePrim);
  result -= GetResult(TrackingQA::qualityNumbers::kPossibleSec);
  return result;
}

int PndTrackingQASummary::GetAllTracksWithHitsNotFound()
{
  int result = GetResult(TrackingQA::qualityNumbers::kAtLeastThreePrim);
  result += GetResult(TrackingQA::qualityNumbers::kAtLeastThreeSec);
  result += GetResult(TrackingQA::qualityNumbers::kPossiblePrim);
  result += GetResult(TrackingQA::qualityNumbers::kPossibleSec);

  return result;
}
