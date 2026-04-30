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

#ifndef PIPE_H
#define PIPE_H

#include "TNamed.h"
#include "TArrayI.h"
#include "TClonesArray.h"
#include "FairDetector.h"
#include "FairModule.h"

class PndPipe : public FairModule {
 public:
  PndPipe(const char *name, const char *Title = "PND Pipe");
  PndPipe();

  virtual ~PndPipe();
  virtual void ConstructGeometry();
  Bool_t CheckIfSensitive(std::string name);

        virtual FairModule* CloneModule() const;

  ClassDef(PndPipe, 1) // PNDPIPE
};

#endif // PIPE_H
