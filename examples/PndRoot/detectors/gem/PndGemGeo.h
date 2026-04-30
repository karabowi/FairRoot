/********************************************************************************
 * Copyright (C) 2019-2024 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#ifndef PNDGEMGEO_H
#define PNDGEMGEO_H

#include "FairGeoSet.h"   // for FairGeoSet

#include <Rtypes.h>   // for PndGemGeo::Class, ClassDef, FairTutPropGeo::Streamer

class PndGemGeo : public FairGeoSet
{
  public:
    PndGemGeo();
    ~PndGemGeo() override = default;
    ClassDefOverride(PndGemGeo, 1);
};

#endif   // PNDGEMGEO_H
