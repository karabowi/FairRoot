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
// Description: Track Reconstruction
//-------------------------------------------------------------------------

#ifndef PNDDISCTASKRECONSTRUCTION_H
#define PNDDISCTASKRECONSTRUCTION_H

#include <PndPersistencyTask.h>
#include "PndDetectorList.h"
#include "FairMCEventHeader.h"
#include "FairTSBufferFunctional.h"

#include "TString.h"

class TClonesArray;

class PndDiscTaskReconstruction : public PndPersistencyTask {
 public:
  PndDiscTaskReconstruction();
  PndDiscTaskReconstruction(const char *name);
  virtual ~PndDiscTaskReconstruction();

  // Implement FairTask interface:
  virtual InitStatus Init();
  virtual InitStatus ReInit();
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void FinishTask();

  void RunTimeBased(Bool_t time_based) { is_time_based = time_based; }
  void SetFlag_ExportPatterns(Bool_t flag) { flag_export_patterns = flag; }

  void SetAvgWavelength(Double_t const &val) { average_wl = val; }
  Double_t const &GetAvgWavelength() const { return average_wl; }

  void SetMinWavelength(Double_t const &val) { minimum_wl = val; }
  Double_t const &GetMinWavelength() const { return minimum_wl; }

  double gauss(int x, double mean, double rms);
  double mean(std::vector<double> values);
  double deviation(double mean, std::vector<double> values);

 protected:
  TString branch_name_digits; ///< Branch name where digitized hits are stored
  TString folder_name_digits; ///< Folder name for digits

  TClonesArray *tclarr_digits; ///<
  TClonesArray *tclarr_digits_out;
  TClonesArray *tclarr_particles_out;
  TClonesArray *tclarr_particles;     ///< to cache the pointer to particle MC TClonesArray returned by IO manager.
  TClonesArray *tclarr_tracks;        ///< to cache the pointer to fitted tracks TClonesArray returned by IO manager.
  TClonesArray *tclarr_recon_results; ///< results of reconstruction (pattern matching)

  // FairMCEventHeader * fMcEventHeader;     //!
  Bool_t is_time_based; ///< Time based buffering on/off
  // Bool_t is_persistent;                 ///< Write buffer to disc on/off
  Bool_t flag_export_patterns; ///< Write the pattern hypothesis and measured patterns to file.

  StopTime start_functor; // do I really need two of them, I doubt it.
  StopTime stop_functor;

  Double_t average_wl;
  Double_t minimum_wl;

  ClassDef(PndDiscTaskReconstruction, 1)
};

#endif // PNDDISCTASKRECONSTRUCTION_H
