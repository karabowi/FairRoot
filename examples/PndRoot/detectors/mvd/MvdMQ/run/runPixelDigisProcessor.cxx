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
#include <csignal>

/// FairRoot - FairMQ - base/MQ
#include "FairMQLogger.h"
#include "runSimpleMQStateMachine.h"

// 9-PixelDetector example
#include "PndMQMvdPixelDigiProcessorBursts.h"

// ////////////////////////////////////////////////////////////////////////
namespace po = boost::program_options;

using channelBranches = std::vector<std::pair<std::string, std::string>>;

namespace std {
static inline std::istream &operator>>(std::istream &is, std::pair<std::string, std::string> &into)
{
  char ch;
  while (is >> ch && ch != '=')
    into.first += ch;
  return is >> into.second;
}
} // namespace std

int main(int argc, char **argv)
{
  try {

    FairMQProgOptions config;

    config.ParseAll(argc, argv);

    PndMQMvdPixelDigiProcessorBursts processor;

    runStateMachine(processor, config);

  } catch (std::exception &e) {
    LOG(error) << "Unhandled Exception reached the top of main: " << e.what() << ", application will now exit";
    return 1;
  }

  return 0;
}
