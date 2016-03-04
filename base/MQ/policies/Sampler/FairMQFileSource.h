/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#include "FairSourceMQInterface.h"
#include "FairEventData.h"

typedef FairSourceMQInterface<FairFileSource,TClonesArray,tca>  FairMQFileSource_t;

typedef FairSourceMQInterface<FairFileSource,TClonesArray,FevtData> FairMQFileSource_f;
