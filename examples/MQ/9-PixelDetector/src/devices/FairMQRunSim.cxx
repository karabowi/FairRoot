/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * FairMQRunSim.cpp
 *
 * @since 2016-03-08
 * @author R. Karabowicz
 */

#include "FairMQRunSim.h"

#include "FairMQLogger.h"
#include "FairMQMessage.h"
#include "FairMQProgOptions.h"

using namespace std;

FairMQRunSim::FairMQRunSim()
  : FairMQDevice()
  , fMaxIndex(10)
  , fEventCounter(0)
  , fRunSim(new FairRunSim())
{

}

void FairMQRunSim::InitTask() 
{
}

void FairMQRunSim::PreRun()
{
}

bool FairMQRunSim::ConditionalRun()
{
  LOG(INFO) << "running event " << fEventCounter;
  fEventCounter++;

  if ( fEventCounter == 5 )
    fRunSim->Run(10);

  if ( fEventCounter > fMaxIndex ) return false;

  return true;
}

void FairMQRunSim::PostRun()
{
}

FairMQRunSim::~FairMQRunSim()
{
}
