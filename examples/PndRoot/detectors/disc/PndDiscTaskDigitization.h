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
// Author:      Oliver Merle (Oliver.Merle@exp2.physik.uni-giessen.de)
// Changes:     Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Digitization of Monte Carlo hits
//-------------------------------------------------------------------------

#ifndef PNDDISCTASKDIGITIZATION_H
#define PNDDISCTASKDIGITIZATION_H

#define USESENSORGRID

// project specific
#include "PndDiscNoiseGeneration.h"
#include "PndDiscPhotodetector.h"
#include "PndDiscSensorGrid.h"

// FAIR/PROOT
#include <PndPersistencyTask.h>
#include "PndDetectorList.h"
#include "FairMCEventHeader.h"
#include "FairWriteoutBuffer.h"

// ROOT
#include "TString.h"
#include <Math/Interpolator.h> // ROOT imports from GSL -> GPL License !!!

// cpp
#include <set>

// class TClonesArray;

class PndDiscTaskDigitization : public PndPersistencyTask {
 public:
  PndDiscTaskDigitization();
  PndDiscTaskDigitization(const char *name);
  virtual ~PndDiscTaskDigitization();

  // Implement FairTask interface:
  virtual InitStatus Init();
  virtual InitStatus ReInit();
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void FinishTask();

  void RunTimeBased(Bool_t time_based) { is_time_based = time_based; }

  // Additional functionality
  void FilterParticleSpecies(Int_t pdg);
  void SetRunMixed(Bool_t is_mixed = kTRUE) ///< Run in mixed input mode
  {
    is_run_mixed = is_mixed;
  }

 protected:
  TString branch_name_mc_point; ///< Branch name where mc points were stored
  TString branch_name_digits;   ///< Branch name where digitized hits shall be stored
  TString folder_name_digits;   ///< Folder name for output in root file

  Int_t mc_point_branch_id;                 ///< Cache branch id of the mc point branch for linking with FairLink
  TClonesArray *tclarr_mc_points;           //! to cache the pointer to input TClonesArray returned by IO manager.
  TClonesArray *tclarr_particle_tracks_in;  //!
  TClonesArray *tclarr_particle_tracks_out; //!
  TClonesArray *array;
  std::set<int> particle_types; //!  particle types to filter in output

  FairWriteoutBuffer *writeout_buffer; ///< Buffer for so calles "time based simulation"

  FairMCEventHeader *fMcEventHeader; //!

  Bool_t is_run_mixed;
  Bool_t is_time_based; ///< Time based buffering on/off
  Bool_t is_persistent; ///< Write buffer to disc on/off

#ifdef USESENSORGRID
  DiscDIRC_Photodetector *photo_detector; //!
#else
  // ROOT::Math::Interpolator pde_interpolator;  //!
  DiscDIRC_NoiseGeneration noise_generator; //!
#endif

  ClassDef(PndDiscTaskDigitization, 1)
};

#endif // PNDDISCTASKDIGITIZATION_H
