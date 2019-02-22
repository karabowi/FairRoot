/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *              GNU Lesser General Public Licence (LGPL) version 3,             *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----               FairSimulationConfig header file                -----
// -----            Created 2019.02.19 by R. Karabowicz                -----
// -------------------------------------------------------------------------

#ifndef FAIR_SIMULATION_CONFIG_H
#define FAIR_SIMULATION_CONFIG_H

class FairSimulationConfig
{
  public:
    FairSimulationConfig();
    virtual ~FairSimulationConfig();

    virtual void Setup(const char* mcEngine);
};

#endif

