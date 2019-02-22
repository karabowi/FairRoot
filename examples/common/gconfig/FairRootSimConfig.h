/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *              GNU Lesser General Public Licence (LGPL) version 3,             *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----               FairRootSimConfig header file                   -----
// -----            Created 2019.02.19 by R. Karabowicz                -----
// -------------------------------------------------------------------------

#ifndef FAIRROOT_SIM_CONFIG_H
#define FAIRROOT_SIM_CONFIG_H

#include "FairYamlSimConfig.h"

class TString;

class FairRootSimConfig : public FairYamlSimConfig
{
  public:
    FairRootSimConfig();
    virtual ~FairRootSimConfig();

 private:
    virtual void SetupStack();
};

#endif


