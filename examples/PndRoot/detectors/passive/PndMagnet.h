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

#ifndef MAGNET_H
#define MAGNET_H

#include "TNamed.h"
#include "TArrayI.h"
#include "TClonesArray.h"
#include "FairDetector.h"
#include "FairModule.h"

class PndMagnet : public FairModule {
 public:
  PndMagnet(const char *name, const char *Title = "PND Magnet");
  PndMagnet();
  virtual ~PndMagnet();
  void ConstructGeometry();
  void ConstructASCIIGeometry();
  Bool_t CheckIfSensitive(std::string name);

        virtual FairModule* CloneModule() const;

  ClassDef(PndMagnet, 1) // PNDMagnet
};

#endif // MAGNET_H
