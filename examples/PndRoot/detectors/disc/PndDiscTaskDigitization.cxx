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

// project
#include "PndDiscTaskDigitization.h"
#include "PndDiscDigitizedHit.h"
#include "PndDiscSensorMCPoint.h"
#include "PndDiscParticleMCPoint.h"

// FAIR/PROOT
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairEventHeader.h"
#include "FairLogger.h"
#include "PndDiscWriteoutBuffer.h"

// ROOT
#include "TClonesArray.h"
#include "TString.h"
#include "TGeoManager.h"
#include "TRandom.h"
#include "TMath.h"

// cpp
#include <cstdlib>

// Definition of the sellmeier equation to calculate the refractive index from the photon wavelength
Double_t n_phase_sellmeier(Double_t *coeff, Double_t lambda_um)
{
  double lambda_um_sq = lambda_um * lambda_um;
  return sqrt(coeff[0] / (1.0 - (coeff[3] / lambda_um_sq)) + coeff[1] / (1.0 - (coeff[4] / lambda_um_sq)) + coeff[2] / (1.0 - (coeff[5] / lambda_um_sq)) + 1.0);
}

PndDiscTaskDigitization::PndDiscTaskDigitization()
  : PndPersistencyTask("PndDiscTaskDigitization"), mc_point_branch_id(0), tclarr_mc_points(nullptr), writeout_buffer(nullptr), fMcEventHeader(nullptr), is_time_based(kTRUE),
    is_persistent(kTRUE)
#ifndef USESENSORGRID
    ,
    pde_interpolator(0, ROOT::Math::Interpolation::kLINEAR)
#endif
{
  particle_types.insert(1);

  // Defining the TClonesArrays
  tclarr_particle_tracks_out = new TClonesArray("PndDiscParticleMCPoint"); // TClonesArray for MCTruth tracks
  array = new TClonesArray("PndDiscDigitizedHit");                         // TClonesArray for digitized hits

  // Defining the branch- and foldernames
  branch_name_mc_point = "DiscSensorMCPoint";
  branch_name_digits = "DiscDigit";
  folder_name_digits = "DiscDIRC";
}

PndDiscTaskDigitization::~PndDiscTaskDigitization()
{
  if (writeout_buffer != nullptr)
    delete writeout_buffer;
}

InitStatus PndDiscTaskDigitization::ReInit()
{
  return kSUCCESS;
}

InitStatus PndDiscTaskDigitization::Init()
{
  // Get IO manager instance:
  FairRootManager *io_manager = FairRootManager::Instance();
  if (!io_manager) {
    LOG(fatal) << "FairRootManager instance is nullptr !!!";
    return kFATAL;
  }

  // Get sensor hits from input tree:
  tclarr_mc_points = (TClonesArray *)io_manager->GetObject(branch_name_mc_point);
  if (!tclarr_mc_points) {
    LOG(error) << "Branch " << branch_name_mc_point.Data() << " is not accessible through FairRootManager.";
    return kERROR;
  }
  mc_point_branch_id = io_manager->GetBranchId(branch_name_mc_point);

  // Create the buffer for time based simulation:
  // writeout_buffer = new DiscDIRC_WriteoutBuffer(branch_name_digits, folder_name_digits, is_persistent);
  // writeout_buffer->SetVerbose(1);
  // writeout_buffer = (DiscDIRC_WriteoutBuffer*)io_manager->RegisterWriteoutBuffer(branch_name_digits, writeout_buffer);

  // Set buffering mode:
  // writeout_buffer->ActivateBuffering(is_time_based);

  // MCTruth output if particle types are set
  if (particle_types.size() > 0) {
    // get particles from input tree
    tclarr_particle_tracks_in = (TClonesArray *)io_manager->GetObject("DiscParticleMCPoint");
    if (!tclarr_particle_tracks_in) {
      LOG(error) << "GetObject(\"DiscParticleMCPoint\") returned nullptr";
      return kERROR;
    }

    // create output branch for particle tracks
    // tclarr_particle_tracks_out = new TClonesArray("DiscParticleMCPoint");
    FairRootManager::Instance()->Register("DiscMCTruthTracks", "DiscDIRC", tclarr_particle_tracks_out, GetPersistency());
    FairRootManager::Instance()->Register("DiscDigit", "DiscDIRC", array, GetPersistency());
  }

  // Initialize PDE values of Sensor:
  {
    double pde_wl_nm[74] = {269.949916528, 275.959933222, 280.634390651, 285.30884808,  289.983305509, 295.993322204, 302.003338898, 308.013355593, 313.355592654, 320.701168614,
                            326.711185309, 333.388981636, 338.731218698, 346.74457429,  351.41903172,  358.764607679, 366.110183639, 375.459098497, 382.804674457, 389.482470785,
                            398.16360601,  407.512520868, 414.190317195, 420.20033389,  424.207011686, 430.217028381, 434.89148581,  440.233722871, 442.904841402, 446.911519199,
                            452.25375626,  456.260434057, 459.59933222,  462.938230384, 468.948247078, 474.958263773, 478.964941569, 483.639398998, 488.98163606,  492.320534224,
                            498.998330551, 504.340567613, 508.347245409, 513.021702838, 517.696160267, 522.370617696, 525.041736227, 530.383973289, 533.722871452, 539.065108514,
                            545.075125209, 550.41736227,  556.427378965, 563.772954925, 569.782971619, 577.128547579, 585.809682805, 593.823038397, 601.168614357, 608.514190317,
                            610.517529215, 615.859766277, 621.202003339, 630.550918197, 637.228714524, 644.574290484, 652.587646077, 659.933222037, 670.61769616,  676.627712855,
                            682.637729549, 689.983305509, 696.661101836, 702.003338898};

    double pde_efficiency[74] = {0.152452, 0.159915, 0.168443, 0.173774, 0.179104, 0.182836, 0.186034, 0.189232, 0.191898, 0.195096, 0.200959, 0.206823, 0.208955,
                                 0.210021, 0.211087, 0.214819, 0.220682, 0.224947, 0.227612, 0.227079, 0.223348, 0.219616, 0.215352, 0.211620, 0.208955, 0.204691,
                                 0.198827, 0.194563, 0.190832, 0.186567, 0.180704, 0.176439, 0.171109, 0.166311, 0.157249, 0.147655, 0.141258, 0.136461, 0.132196,
                                 0.128998, 0.124733, 0.122601, 0.119403, 0.114606, 0.108742, 0.100746, 0.095416, 0.085821, 0.078891, 0.068230, 0.061301, 0.054904,
                                 0.050107, 0.045842, 0.040512, 0.037313, 0.032516, 0.028252, 0.025586, 0.022388, 0.021855, 0.020256, 0.018124, 0.015991, 0.013859,
                                 0.013326, 0.011727, 0.010661, 0.009062, 0.007463, 0.006397, 0.005330, 0.004264, 0.003731};

#ifdef USESENSORGRID
    photo_detector = new DiscDIRC_Photodetector(DiscDIRC_Photodetector::DESIGN_LRD);
    photo_detector->SetPDE(74, pde_wl_nm, pde_efficiency);
#else
    // pde_interpolator.SetData(74, pde_wl_nm, pde_efficiency);
#endif
  }

  return kSUCCESS;
}

void PndDiscTaskDigitization::Exec(Option_t *)
{
  PndDiscSensorMCPoint *mc_point = nullptr;
  Int_t i = 0, n_mc_points = tclarr_mc_points->GetEntriesFast();
  FairEventHeader *event_header = (FairEventHeader *)FairRootManager::Instance()->GetObject("EventHeader.");
  Int_t input_file_id = event_header->GetInputFileId();

  // --------------------------------------------------------
  // some fixed design parameters:
  // --------------------------------------------------------
  // Double_t rms_time_ns = 0.021; //[R.K. 01/2017] unused variable?
  Double_t binning_ns = 0.050;
  Double_t radiator_thickness = 2.0;
  Double_t rms_roughness_nm = 1.5;
  // Double_t dead_time              = 20.0; //[R.K. 01/2017] unused variable?
  // need refractive index for reflection probability:
  Double_t sellmeier_coeff[6] = {0.473115591, 0.631038719, 0.906404498, 0.012995717, 0.0041280992, 98.7685322};
  // --------------------------------------------------------

  array->Clear();

  for (i = 0; i < n_mc_points; i++) {
    mc_point = (PndDiscSensorMCPoint *)tclarr_mc_points->At(i); // Get Entries from Sensor
    Double_t photon_momentum = sqrt((mc_point->GetPx() * mc_point->GetPx()) + (mc_point->GetPy() * mc_point->GetPy()) + (mc_point->GetPz() * mc_point->GetPz()));
    Double_t wavelength_nm = (1.239841939 / photon_momentum) * 1E-06;
    Double_t rindex = n_phase_sellmeier(sellmeier_coeff, wavelength_nm * 1E-3);
    Double_t sin_iref_angle = sin(mc_point->GetTotalReflectionAngle());
    Double_t reflection_probability = 1. - pow(4. * TMath::Pi() * rms_roughness_nm * sin_iref_angle * rindex / wavelength_nm, 2);

    // Apply reflection probability
    Int_t n_reflections = (Int_t)(mc_point->GetLength() * sin_iref_angle / radiator_thickness);
    if (gRandom->Uniform(1.0) > pow(reflection_probability, n_reflections))
      continue;

    // --------------------------------------------------------
    // Detection - digitize the hit ...
    // --------------------------------------------------------

    double absolute_time = mc_point->GetTime() + FairRootManager::Instance()->GetEventTime();
    double tdc_time_ns;
    SensorGrid::PixelInfo pixel_info;

    int pixel_id = photo_detector->Detect(mc_point->GetX(), mc_point->GetY(), absolute_time, wavelength_nm, pixel_info, tdc_time_ns);
    if (pixel_id < 0)
      continue;

    tdc_time_ns = (floor(tdc_time_ns / binning_ns) + 0.5) * binning_ns;

    Double_t hit_x, hit_z;
    photo_detector->GetGrid()->PixelToPosition(pixel_info, hit_x, hit_z);

    int classifier = 0;
    if (is_run_mixed && input_file_id == 0)
      classifier = 2;

    Int_t copy_number = mc_point->GetDetectorID();
    div_t res = div(copy_number, 27);
    Int_t detector_id = res.quot;
    Int_t readout_id = res.rem;

    if (gGeoManager == nullptr) {
      LOG(fatal) << "gGeoManager is nullptr - cannot retrieve geo information !!!";
    }

    PndDiscDigitizedHit *digit =
      new PndDiscDigitizedHit(FairLink(input_file_id, event_header->GetMCEntryNumber() - 1, mc_point_branch_id, i), detector_id, readout_id,
                              readout_id * 3 + 3 - pixel_info.column_on_grid, pixel_info.pixel_number, pixel_info.row_on_grid, hit_z, tdc_time_ns, absolute_time, classifier);

    // buffer is responsible to delete digit:
    // writeout_buffer->FillNewData(digit, absolute_time, absolute_time+dead_time);

    digit = new ((*array)[array->GetEntriesFast()]) PndDiscDigitizedHit(*digit);
    digit->Print();
  }

  if ((!is_run_mixed || input_file_id > 0) && particle_types.size() > 0) {
    PndDiscParticleMCPoint *particle_mc_point;
    n_mc_points = tclarr_particle_tracks_in->GetEntries();
    for (i = 0; i < n_mc_points; i++) {
      particle_mc_point = (PndDiscParticleMCPoint *)tclarr_particle_tracks_in->At(i);
      // if(particle_types.count(abs(particle_mc_point->pdgCode)) == 0) continue;
      if (particle_mc_point->GetPosInInside() != 0)
        continue; // check if created inside
      if (!particle_mc_point->GetIsPrimary())
        continue; // check if particle is primary
      particle_mc_point = new ((*tclarr_particle_tracks_out)[tclarr_particle_tracks_out->GetEntriesFast()]) PndDiscParticleMCPoint(*particle_mc_point);
      Double_t particle_tof = particle_mc_point->GetTime();
      particle_mc_point->SetTime(particle_tof + FairRootManager::Instance()->GetEventTime());
    }
  }
}

void PndDiscTaskDigitization::FinishEvent()
{
  tclarr_particle_tracks_out->Clear();
  FinishEvents();
}

void PndDiscTaskDigitization::FinishTask() {}

void PndDiscTaskDigitization::FilterParticleSpecies(Int_t pdg_code)
{
  particle_types.insert(abs(pdg_code));
}

ClassImp(PndDiscTaskDigitization)
