/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                       FairEventData header file               -----
// -----          Created 03/03/2016                 R. Karabowicz     -----
// -------------------------------------------------------------------------

#ifndef FAIREVENTDATA_H
#define FAIREVENTDATA_H

#include "TObject.h"
#include "FairEventHeader.h"

// class to store FairEventHeader and some data with it...
class FairEventData : public FairEventHeader
{
 public : 
  FairEventData();
  virtual ~FairEventData();
  
  TObject* GetObject()                    { return fStoredObject; }
  void     SetObject(TObject* tempObject) { fStoredObject = tempObject; }
 private :
  TObject* fStoredObject;
  
  ClassDef(FairEventData,1);
};

#endif /* FAIREVENTDATA_H_ */
