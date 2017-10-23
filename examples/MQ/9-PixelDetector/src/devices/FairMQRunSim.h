/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * FairMQRunSim.h
 *
 * @since 2017-10-18
 * @author R. Karabowicz
 */

#ifndef FAIRMQRUNSIM_H_
#define FAIRMQRUNSIM_H_

#include <string>

#include "FairRunSim.h"
#include "FairMQDevice.h"

class FairMQRunSim : public FairMQDevice
{
  public:
    FairMQRunSim();
    virtual ~FairMQRunSim();

    FairRunSim* GetFairRunSimPointer() { return fRunSim; }

    virtual bool ConditionalRun();

 protected:
    virtual void InitTask();
    virtual void PreRun();
    virtual void PostRun();
    
 private: 
    int64_t         fMaxIndex;
    int             fEventCounter;

    FairRunSim*     fRunSim;

    FairMQRunSim(const FairMQRunSim&);
    FairMQRunSim& operator=(const FairMQRunSim&);
};

#endif /* FAIRMQRUNSIM_H_ */
