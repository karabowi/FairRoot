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
#include <PndMQFileSinkBursts.h>
#include <csignal>

/// FairRoot - FairMQ - base/MQ
#include "FairMQLogger.h"
#include "runSimpleMQStateMachine.h"

// 9-PixelDetector example

// ////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  try {
    std::string filename;
    std::vector<std::string> classname;
    std::vector<std::string> branchname;

    namespace po = boost::program_options;
    po::options_description fileSink_options("FileSink options");
    fileSink_options.add_options()("file-name", po::value<std::string>(&filename), "Path to the output file")(
      "class-name", po::value<std::vector<std::string>>(&classname), "class name")("branch-name", po::value<std::vector<std::string>>(&branchname), "branch name");

    FairMQProgOptions config;
    config.AddToCmdLineOptions(fileSink_options);

    config.ParseAll(argc, argv);

    PndMQFileSinkBursts fileSink;
    //     fileSink.SetProperty(PndMQFileSinkBursts::OutputFileName,filename);

    if (classname.size() != branchname.size()) {
      LOG(error) << "The classname size (" << classname.size() << ") and branchname size (" << branchname.size() << ") MISMATCH!!!";
    }

    //	fileSink.AddOutputBranch("FairEventHeader","EventHeader.");
    //	for ( unsigned int ielem = 0 ; ielem < classname.size() ; ielem++ ) {
    //	  fileSink.AddOutputBranch(classname.at(ielem),branchname.at(ielem));
    //	}

    runStateMachine(fileSink, config);

  } catch (std::exception &e) {
    LOG(error) << "Unhandled Exception reached the top of main: " << e.what() << ", application will now exit";
    return 1;
  }

  return 0;
}
