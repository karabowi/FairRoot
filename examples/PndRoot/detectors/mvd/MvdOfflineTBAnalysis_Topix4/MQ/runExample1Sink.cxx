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
 * runExample1Sink.cxx
 *
 * @since 2013-04-23
 * @author D. Klein, A. Rybalchenko
 */

#include <PndMQExample1Sink.h>
#include <iostream>

#include "boost/program_options.hpp"

#include "FairMQLogger.h"
#include "FairMQParser.h"
#include "FairMQProgOptions.h"

#ifdef NANOMSG
#include "FairMQTransportFactoryNN.h"
#else
#include "FairMQTransportFactoryZMQ.h"
#endif

using namespace boost::program_options;

int main(int argc, char **argv)
{
  PndMQExample1Sink sink;
  sink.CatchSignals();

  FairMQProgOptions config;

  try {
    if (config.ParseAll(argc, argv)) {
      return 0;
    }

    std::string filename = config.GetValue<std::string>("config-json-file");
    std::string id = config.GetValue<std::string>("id");

    config.UserParser<FairMQParser::JSON>(filename, id);

    sink.fChannels = config.GetFairMQMap();

    LOG(info) << "PID: " << getpid();

#ifdef NANOMSG
    FairMQTransportFactory *transportFactory = new FairMQTransportFactoryNN();
#else
    FairMQTransportFactory *transportFactory = new FairMQTransportFactoryZMQ();
#endif

    sink.SetTransport(transportFactory);

    sink.SetProperty(PndMQExample1Sink::Id, id);

    sink.ChangeState("INIT_DEVICE");
    sink.WaitForEndOfState("INIT_DEVICE");

    sink.ChangeState("INIT_TASK");
    sink.WaitForEndOfState("INIT_TASK");

    sink.ChangeState("RUN");
    sink.InteractiveStateLoop();
  } catch (std::exception &e) {
    LOG(error) << e.what();
    LOG(info) << "Command line options are the following: ";
    config.PrintHelp();
    return 1;
  }

  return 0;
}
