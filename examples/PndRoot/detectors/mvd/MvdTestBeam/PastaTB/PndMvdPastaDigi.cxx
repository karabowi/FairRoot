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
 * PndMvdPastaDigi.cxx
 *
 *  Created on: 23.09.2017
 *      Author: Stockmanns
 */

#include <PndMvdPastaDigi.h>

ClassImp(PndMvdPastaDigi);
ClassImp(ThresholdDataFullMode);
ClassImp(FrameHeader);
ClassImp(RunSummary);

FrameHeader::FrameHeader() : frameId(0), nEvents(0) {}

ThresholdDataFullMode::ThresholdDataFullMode() : t_coarse(0), t_soc(0), t_eoc(0), channelId(0), tacId(0) {}

RunSummary::RunSummary()
  : fCrcMatchCount(0), fCrcErrorCount(0), fSingleWordFrames(0), fAllPartialResets(0), fWrongHitCount(0), fWrongFrameCount(0), fMissingFrames(0), fSuperFrameCount(0)
{
}

PndMvdPastaDigi::PndMvdPastaDigi() : fTimeBranch(), fEnergyBranch(), fFrameHeader(), fTimeStamp(0), fToT(0) {}

PndMvdPastaDigi::~PndMvdPastaDigi()
{
  // TODO Auto-generated destructor stub
}

int PndMvdPastaDigi::GetChannelId(enum PndMvdPastaDigi::branchId id)
{
  switch (id) {
  case TIME: return fTimeBranch.channelId; break;
  case ENERGY: return fEnergyBranch.channelId; break;
  default: return -1;
  }
}
