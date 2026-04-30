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
 *              PndMQSdsHitLoaderhis software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * runPndMQSdsHitLoaderestDetectorSamplerBin.cxx
 *
 * @since 2013-04-29
 * @author A. Rybalchenko, N. Winckler
 */

#include <iostream>

#include "boost/program_options.hpp"

#include "FairMQLogger.h"
#include "FairMQSampler.h"
#include "FairMQParser.h"

#ifdef NANOMSG
#include "FairMQTransportFactoryNN.h"
#else
#include "FairMQTransportFactoryZMQ.h"
#endif

// data format for the task
#include "PndMQSdsHitLoader.h"
#include "PndSdsHit.h"
#include "FairMQProgOptions.h"
#include "FairMQSampler.h"

// boost data format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using namespace std;
using namespace boost::program_options;

int main(int argc, char **argv)
{

  FairMQProgOptions config;

  FairMQSampler<PndMQSdsHitLoader> sampler;
  sampler.CatchSignals();

  try {
    std::string InputFile, ParFile, Branch;

    options_description samplerOptions("Sampler options");
    samplerOptions.add_options()("InputFile", value<std::string>(&InputFile)->default_value(""))("ParameterFile", value<std::string>(&ParFile)->default_value(""))(
      "Branch", value<std::string>(&Branch)->default_value(""));

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

    sampler.SetProperty(FairMQDevice::Id, id);
    sampler.SetProperty(FairMQSampler<PndMQSdsHitLoader>::InputFile, config.GetValue<string>("InputFile"));
    sampler.SetProperty(FairMQSampler<PndMQSdsHitLoader>::ParFile, config.GetValue<string>("ParameterFile"));
    sampler.SetProperty(FairMQSampler<PndMQSdsHitLoader>::Branch, config.GetValue<string>("Branch"));
    // sampler.SetProperty(FairMQSampler::EventRate, options.eventRate);

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
