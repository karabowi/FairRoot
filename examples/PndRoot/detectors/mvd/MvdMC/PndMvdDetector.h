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

// -------------------------------------------------------------------------
// -----                         PndMvdDetector header file                    -----
// -----                  Created 6/04/06  by T. Stockmanns            -----
// -----                          Based on PndMvdDetector                      -----
// -------------------------------------------------------------------------

/**  PndMvdDetector.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Defines the active detector PndMvdDetector. Constructs the geometry and
 ** registers MCPoints.
 **/

#ifndef PNDMVDDETECTOR_H
#define PNDMVDDETECTOR_H

#include "PndSdsDetector.h"

class PndMvdDetector : public PndSdsDetector {

 public:
  /** Default constructor **/
  PndMvdDetector();

  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndMvdDetector(const char *name, Bool_t active);

  /** Destructor **/
  virtual ~PndMvdDetector();

  /** Setting in/output names for MVD **/
  void SetBranchNames(TString outBranchname, TString folderName);
  virtual void SetBranchNames();
  virtual void SetSpecialPhysicsCuts();
  virtual void SetDefaultSensorNames();
  virtual void Initialize() { PndSdsDetector::Initialize(); };

    
    FairModule* CloneModule() const override;

 private:
        PndMvdDetector(const PndMvdDetector&);
    PndMvdDetector& operator=(const PndMvdDetector&);

  ClassDef(PndMvdDetector, 6);
};

#endif
