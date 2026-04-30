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

// $Id: TProtoUnpackEvent.cxx 478 2009-10-29 12:26:09Z linev $
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TProtoUnpackEvent.h"

//***********************************************************
TProtoUnpackEvent::TProtoUnpackEvent() : TGo4EventElement() {}
//***********************************************************
TProtoUnpackEvent::TProtoUnpackEvent(const char *name) : TGo4EventElement(name) {}
//***********************************************************
TProtoUnpackEvent::~TProtoUnpackEvent() {}
//***********************************************************

//-----------------------------------------------------------
void TProtoUnpackEvent::Clear(Option_t *)
{
  void *destfield;
  // cout << "+++ event clear" << endl;

  destfield = (void *)&amplitude[0];
  memset(destfield, 0, sizeof(amplitude));

  destfield = (void *)&energy[0];
  memset(destfield, 0, sizeof(energy));

  destfield = (void *)&time[0];
  memset(destfield, 0, sizeof(time));

  destfield = (void *)&waveform[0];
  memset(destfield, 0, sizeof(waveform));

  destfield = (void *)&trigger[0];
  memset(destfield, 0, sizeof(trigger));

  rawDataLength = 0;
  destfield = (void *)&rawData[0];
  memset(destfield, 0, sizeof(rawData));

  return;
}
