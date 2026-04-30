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

//-------------------------------------------------------------------------
// Author:      Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Changes:
// Date:        30.11.2015
// Description: Particle Identification
//-------------------------------------------------------------------------

#ifndef PNDDISCTASKPID_H
#define PNDDISCTASKPID_H

#include <PndPersistencyTask.h>
#include "PndDetectorList.h"
#include "FairMCEventHeader.h"
#include "FairTSBufferFunctional.h"

#include "TString.h"

class TClonesArray;

class PndDiscTaskPID : public PndPersistencyTask {
 public:
  PndDiscTaskPID();
  PndDiscTaskPID(const char *name);
  virtual ~PndDiscTaskPID();

  // Implement FairTask interface:
  virtual InitStatus Init();
  virtual InitStatus ReInit();
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void FinishTask();

  void RunTimeBased(Bool_t time_based) { is_time_based = time_based; }

 protected:
  TString branch_name_digits; ///< Branch name where digitized hits are stored
  TString folder_name_digits; ///< Folder name for digits

  TClonesArray *tclarr_digits; ///<
  TClonesArray *tclarr_recon_results;
  TClonesArray *tclarr_particles_in;
  // TClonesArray * tclarr_particles;        ///< to cache the pointer to particle MC TClonesArray returned by IO manager.
  TClonesArray *tclarr_pid_results; ///< results of reconstruction (pattern matching)

  // FairMCEventHeader * fMcEventHeader;     //!
  Bool_t is_time_based; ///< Time based buffering on/off

  double gauss(int x, double mean, double rms);
  double mean(std::vector<double> values);
  double deviation(double mean, std::vector<double> values);

  ClassDef(PndDiscTaskPID, 1)
};

#endif // PNDDISCTASKPID_H
