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


/// std
#include <PndMQMergerTest.h>
#include <csignal>

/// FairRoot - FairMQ - base/MQ
#include "FairMQLogger.h"
#include "runSimpleMQStateMachine.h"

// 9-PixelDetector example

// ////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  try {

    po::options_description merger_options("Merger options");

    FairMQProgOptions config;
    config.AddToCmdLineOptions(merger_options);

    config.ParseAll(argc, argv);

    PndMQMergerTest testMerger;

    runStateMachine(testMerger, config);

  } catch (std::exception &e) {
    LOG(error) << "Unhandled Exception reached the top of main: " << e.what() << ", application will now exit";
    return 1;
  }

  return 0;
}
