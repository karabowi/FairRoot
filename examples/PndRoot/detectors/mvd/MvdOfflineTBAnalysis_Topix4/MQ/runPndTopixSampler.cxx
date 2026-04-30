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
 * runExample1Sampler.cxx
 *
 * @since 2013-04-23
 * @author D. Klein, A. Rybalchenko
 */

#include <iostream>

#include "boost/program_options.hpp"

#include "FairMQLogger.h"
#include "FairMQParser.h"
#include "FairMQProgOptions.h"
#include "PndMQTopix4Sampler.h"

#ifdef NANOMSG
#include "FairMQTransportFactoryNN.h"
#else
#include "FairMQTransportFactoryZMQ.h"
#endif

using namespace boost::program_options;

int main(int argc, char **argv)
{
  PndMQTopix4Sampler sampler;
  sampler.CatchSignals();

  FairMQProgOptions config;

  try {
    std::string text;
    int fe;
    double timecorr;

    options_description samplerOptions("Sampler options");
    samplerOptions.add_options()(
      "FileName",
      value<std::string>(&text)->default_value("/Users/Stockmanns/PandaSoftware/pandaRoot/pandaRoot_ActualTrunk/macro/mvd/TestBeam/2014-10-27-00-06-02--101-data--1.txt"),
      "FileName of data to send out")("FE", value<int>(&fe)->default_value(1), "Front-End Number")("TimeCorrection", value<double>(&timecorr)->default_value(0.0),
                                                                                                   "Time offset between FE chips");

    config.AddToCmdLineOptions(samplerOptions);

    config.ParseAll(argc, argv);

    std::string filename = config.GetValue<std::string>("config-json-file");
    std::string id = config.GetValue<std::string>("id");

    config.UserParser<FairMQParser::JSON>(filename, id);

    sampler.fChannels = config.GetFairMQMap();

    LOG(info) << "PID: " << getpid();

#ifdef NANOMSG
    FairMQTransportFactory *transportFactory = new FairMQTransportFactoryNN();
#else
    FairMQTransportFactory *transportFactory = new FairMQTransportFactoryZMQ();
#endif

    sampler.SetTransport(transportFactory);

    sampler.SetProperty(PndMQTopix4Sampler::Id, id);
    sampler.SetProperty(PndMQTopix4Sampler::FileName, text);

    sampler.ChangeState("INIT_DEVICE");
    sampler.WaitForEndOfState("INIT_DEVICE");

    sampler.ChangeState("INIT_TASK");
    sampler.WaitForEndOfState("INIT_TASK");

    sampler.ChangeState("RUN");
    sampler.InteractiveStateLoop();
  } catch (std::exception &e) {
    LOG(error) << e.what();
    LOG(info) << "Command line options are the following: ";
    config.PrintHelp();
    return 1;
  }

  return 0;
}
