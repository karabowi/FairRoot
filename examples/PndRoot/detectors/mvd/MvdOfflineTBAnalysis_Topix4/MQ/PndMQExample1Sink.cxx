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

/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQExample1Sink.cxx
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <PndMQExample1Sink.h>

#include "FairMQLogger.h"

using namespace std;

PndMQExample1Sink::PndMQExample1Sink() {}

void PndMQExample1Sink::Run()
{
  while (CheckCurrentState(RUNNING)) {
    unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());

    if (fChannels.at("data-in").at(0).Receive(msg) > 0) {
      LOG(info) << "Received message: \"" << string(static_cast<char *>(msg->GetData()), msg->GetSize()) << "\"";
    }
  }
}

PndMQExample1Sink::~PndMQExample1Sink() {}
