/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                       FairEventData source file               -----
// -----          Created 03/03/2016                 R. Karabowicz     -----
// -------------------------------------------------------------------------

#include "FairEventData.h"

// -----   Default constructor   -------------------------------------------
FairEventData::FairEventData()
  : FairEventHeader()
  , fStoredObject(NULL)
{

}
// -----   Destructor   ----------------------------------------------------
FairEventData::~FairEventData() 
{ 
  if(fStoredObject)
    delete fStoredObject;
  fStoredObject=NULL;
}
// -------------------------------------------------------------------------

ClassImp(FairEventData)
