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
// Description: Disc DIRC Implementation for PandaRoot (Giessen)
//-------------------------------------------------------------------------

// Subdetector specific
#include "PndDisc.h"

// ROOT headers
#include <Rtypes.h>
#include <TGeoManager.h>
#include <TVirtualMC.h>
#include <TParticle.h>
#include <TClonesArray.h>
#include <TMath.h>

// Fairroot / PROOT headers
#include <FairRootManager.h>
#include <FairMCApplication.h>
#include <FairPrimaryGenerator.h>
#include <FairMCEventHeader.h>
#include <FairLogger.h>

// cpp
#include <iostream>
#include <set>
#include <string>

namespace {
double n_phase(const double &lambda_um, const double *sellmeier_coefficients)
{
  double lambda_um_sq = lambda_um * lambda_um;
  return sqrt(sellmeier_coefficients[0] / (1.0 - (sellmeier_coefficients[3] / lambda_um_sq)) + sellmeier_coefficients[1] / (1.0 - (sellmeier_coefficients[4] / lambda_um_sq)) +
              sellmeier_coefficients[2] / (1.0 - (sellmeier_coefficients[5] / lambda_um_sq)) + 1.0);
}
} // namespace

///////////////////////////////////
// ctor / dtor
///////////////////////////////////

PndDisc::PndDisc()
  : FairDetector(), clarr_sensor_hits(nullptr), clarr_particle_tracks(nullptr), nextid_clarr_sensor_hits(0), nextid_clarr_particle_tracks(0), ev_header(nullptr), design_id(0),
    wl_min_nm(385.0), wl_max_nm(430.0)
{
}

PndDisc::PndDisc(const char *name, Bool_t active, Int_t det_id)
  : FairDetector(name, active, det_id), store_photon_tracks(kFALSE), clarr_sensor_hits(nullptr), clarr_particle_tracks(nullptr), nextid_clarr_sensor_hits(0),
    nextid_clarr_particle_tracks(0), ev_header(nullptr), design_id(0), wl_min_nm(385.0), wl_max_nm(430.0)
{
}

PndDisc::~PndDisc()
{
  if (clarr_sensor_hits != nullptr) {
    clarr_sensor_hits->Delete();
    delete clarr_sensor_hits;
  }
  if (clarr_particle_tracks != nullptr) {
    clarr_particle_tracks->Delete();
    delete clarr_particle_tracks;
  }
}

//------------------------------------------------
// intialization
//------------------------------------------------

/// Initialize will be called after the Geometry is created

void PndDisc::Initialize()
{
  // create collections (Register is called after Initialize)
  clarr_sensor_hits = new TClonesArray("PndDiscSensorMCPoint");
  clarr_particle_tracks = new TClonesArray("PndDiscParticleMCPoint");
  clarr_particle_tracks->BypassStreamer(kTRUE);

  FairDetector::Initialize(); // call base class implementation
}

//------------------------------------------------
// interface geometry
//------------------------------------------------

void PndDisc::ConstructGeometry()
{

  TString fname = GetGeometryFileName();
  if (!fname.EndsWith(".root")) {
    LOG(error) << "Geometry format not supported.";
    return;
  }

  if (!fname.CompareTo("DIRC_GEO_LIF1.root")) {
    LOG(warning) << "Wrong filename: (%s). DiscDIRC_Detector is expecting geo file DIRC_GEO_LRD.root." << fname.Data();
  }

  names_of_sensitive_volumes.insert(std::string("DiscDIRC_Radiator"));
  names_of_sensitive_volumes.insert(std::string("DiscDIRC_prism_a"));
  names_of_sensitive_volumes.insert(std::string("DiscDIRC_prism_b"));
  names_of_sensitive_volumes.insert(std::string("DiscDIRC_prism_c"));
  names_of_sensitive_volumes.insert(std::string("DiscDIRC_focel_a"));
  names_of_sensitive_volumes.insert(std::string("DiscDIRC_focel_b"));
  names_of_sensitive_volumes.insert(std::string("DiscDIRC_focel_c"));
  // names_of_sensitive_volumes.insert(std::string("DiscDIRC_Filter"));
  names_of_sensitive_volumes.insert(std::string("DiscDIRC_sensor_pmt"));

  // for debugging:
  names_of_sensitive_volumes.insert(std::string("container_volume"));

  ConstructRootGeometry();
}

void PndDisc::ConstructOpGeometry()
{
  int i = 0;
  char str_name[100];
  LOG(info) << "DiscDIRC_Detector::ConstructOpGeometry()";

  //-------------------------------------------------------
  // define the surface properties
  //-------------------------------------------------------

  gMC->DefineOpSurface("DiscDIRC_focel_mirror_surf", kGlisur, kDielectric_metal, kPolished, 0.0);
  {
    Double_t e_photon[] = {1.0e-09, 7.0e-09};
    Double_t r_coeff[] = {0.9, 0.9};
    Double_t efficiency[] = {1., 1.};
    gMC->SetMaterialProperty("DiscDIRC_focel_mirror_surf", "REFLECTIVITY", 2, e_photon, r_coeff);
    gMC->SetMaterialProperty("DiscDIRC_focel_mirror_surf", "EFFICIENCY", 2, e_photon, efficiency);
  }

  gMC->DefineOpSurface("DiscDIRC_filter_a_surf", kGlisur, kDielectric_dielectric, kPolished, 0.0);
  {
    Double_t band_a_min = 1.239841939 * 1.e-06 / wl_max_nm;
    Double_t band_a_max = 1.239841939 * 1.e-06 / wl_min_nm;
    Double_t e_photon_a[] = {1.0e-09, band_a_min, band_a_min + 1E-11, band_a_max, band_a_max + 1E-11, 7.0e-09};
    Double_t transmittance[] = {1E-10, 1E-10, 1., 1., 1E-10, 1E-10};
    gMC->SetMaterialProperty("DiscDIRC_filter_a_surf", "TRANSMITTANCE", 6, e_photon_a, transmittance);
  }

  //-------------------------------------------------------
  // attach surfaces to the geometry setup
  //-------------------------------------------------------

  gMC->SetBorderSurface("rad_interface_0_1", "DiscDIRC_Radiator", 0, "DiscDIRC_Radiator", 1, "DiscDIRC_focel_mirror_surf");
  gMC->SetBorderSurface("rad_interface_1_0", "DiscDIRC_Radiator", 1, "DiscDIRC_Radiator", 0, "DiscDIRC_focel_mirror_surf");

  gMC->SetBorderSurface("rad_interface_1_2", "DiscDIRC_Radiator", 1, "DiscDIRC_Radiator", 2, "DiscDIRC_focel_mirror_surf");
  gMC->SetBorderSurface("rad_interface_2_1", "DiscDIRC_Radiator", 2, "DiscDIRC_Radiator", 1, "DiscDIRC_focel_mirror_surf");

  gMC->SetBorderSurface("rad_interface_2_3", "DiscDIRC_Radiator", 2, "DiscDIRC_Radiator", 3, "DiscDIRC_focel_mirror_surf");
  gMC->SetBorderSurface("rad_interface_3_2", "DiscDIRC_Radiator", 3, "DiscDIRC_Radiator", 2, "DiscDIRC_focel_mirror_surf");

  gMC->SetBorderSurface("rad_interface_3_0", "DiscDIRC_Radiator", 3, "DiscDIRC_Radiator", 0, "DiscDIRC_focel_mirror_surf");
  gMC->SetBorderSurface("rad_interface_0_3", "DiscDIRC_Radiator", 0, "DiscDIRC_Radiator", 3, "DiscDIRC_focel_mirror_surf");

  gMC->SetBorderSurface("rad_hole_interface_0", "DiscDIRC_Radiator", 0, "DiscDIRC_hole", 0, "DiscDIRC_focel_mirror_surf");
  gMC->SetBorderSurface("rad_hole_interface_1", "DiscDIRC_Radiator", 1, "DiscDIRC_hole", 0, "DiscDIRC_focel_mirror_surf");
  gMC->SetBorderSurface("rad_hole_interface_2", "DiscDIRC_Radiator", 2, "DiscDIRC_hole", 0, "DiscDIRC_focel_mirror_surf");
  gMC->SetBorderSurface("rad_hole_interface_3", "DiscDIRC_Radiator", 3, "DiscDIRC_hole", 0, "DiscDIRC_focel_mirror_surf");

  int n_fel = 27;
  for (i = 0; i < 4 * n_fel; i++) {
    // filters:
    sprintf(str_name, "DiscDIRC_focel_a_window_a_%d", i);
    gMC->SetBorderSurface(str_name, "DiscDIRC_sensor_window", i, "DiscDIRC_focel_a", i, "DiscDIRC_filter_a_surf");
    sprintf(str_name, "DiscDIRC_focel_a_window_b_%d", i);
    gMC->SetBorderSurface(str_name, "DiscDIRC_focel_a", i, "DiscDIRC_sensor_window", i, "DiscDIRC_filter_a_surf");

    sprintf(str_name, "DiscDIRC_focel_b_window_a_%d", i);
    gMC->SetBorderSurface(str_name, "DiscDIRC_sensor_window", i, "DiscDIRC_focel_b", i, "DiscDIRC_filter_a_surf");
    sprintf(str_name, "DiscDIRC_focel_b_window_b_%d", i);
    gMC->SetBorderSurface(str_name, "DiscDIRC_focel_b", i, "DiscDIRC_sensor_window", i, "DiscDIRC_filter_a_surf");

    sprintf(str_name, "DiscDIRC_focel_c_window_a_%d", i);
    gMC->SetBorderSurface(str_name, "DiscDIRC_sensor_window", i, "DiscDIRC_focel_c", i, "DiscDIRC_filter_a_surf");
    sprintf(str_name, "DiscDIRC_focel_c_window_b_%d", i);
    gMC->SetBorderSurface(str_name, "DiscDIRC_focel_c", i, "DiscDIRC_sensor_window", i, "DiscDIRC_filter_a_surf");

    // mirror coating
    sprintf(str_name, "DiscDIRC_focel_a_mirror_a_%d", i);
    gMC->SetBorderSurface(str_name, "DiscDIRC_focel_mirror_a", i, "DiscDIRC_focel_a", i, "DiscDIRC_focel_mirror_surf");
    sprintf(str_name, "DiscDIRC_focel_a_mirror_b_%d", i);
    gMC->SetBorderSurface(str_name, "DiscDIRC_focel_a", i, "DiscDIRC_focel_mirror_a", i, "DiscDIRC_focel_mirror_surf");

    sprintf(str_name, "DiscDIRC_focel_b_mirror_a_%d", i);
    gMC->SetBorderSurface(str_name, "DiscDIRC_focel_mirror_b", i, "DiscDIRC_focel_b", i, "DiscDIRC_focel_mirror_surf");
    sprintf(str_name, "DiscDIRC_focel_b_mirror_b_%d", i);
    gMC->SetBorderSurface(str_name, "DiscDIRC_focel_b", i, "DiscDIRC_focel_mirror_b", i, "DiscDIRC_focel_mirror_surf");

    sprintf(str_name, "DiscDIRC_focel_c_mirror_a_%d", i);
    gMC->SetBorderSurface(str_name, "DiscDIRC_focel_mirror_c", i, "DiscDIRC_focel_c", i, "DiscDIRC_focel_mirror_surf");
    sprintf(str_name, "DiscDIRC_focel_c_mirror_b_%d", i);
    gMC->SetBorderSurface(str_name, "DiscDIRC_focel_c", i, "DiscDIRC_focel_mirror_c", i, "DiscDIRC_focel_mirror_surf");
  }

  {
    Double_t sellmeier_coeff[6] = {0.473115591, 0.631038719, 0.906404498, 0.012995717, 0.0041280992, 98.7685322};

    const int n_entries = 800 - 200;
    Double_t photon_momenta[n_entries];
    Double_t rayleigh_length[n_entries];
    Double_t lambda_um;
    for (i = 0, lambda_um = 0.8; i < n_entries; lambda_um -= 0.001, i++) {
      photon_momenta[i] = 1.239841939 * 1E-6 / lambda_um;
      Double_t rindex = n_phase(lambda_um, sellmeier_coeff);
      rayleigh_length[i] = 1E6 / (45962092085.3903 * pow(rindex, 8.) / pow(lambda_um * 1000.0, 4.)) * 10.0;
    }

    gMC->SetMaterialProperty(gMC->MediumId("FusedSilica"), "RAYLEIGH", n_entries, photon_momenta, rayleigh_length);
    gMC->SetMaterialProperty(gMC->MediumId("FusedSilicaB"), "RAYLEIGH", n_entries, photon_momenta, rayleigh_length);
  }
}

//-------------------------------------------------------
// interface transport
//-------------------------------------------------------

void PndDisc::PreTrack()
{
#ifdef DISC_DIRC_VERBOSE
  fLogger->GetLogger()->Info(MESSAGE_ORIGIN, "tracknumber %d", gMC->GetStack()->GetCurrentTrackNumber());
#endif
}

void PndDisc::PostTrack()
{
#ifdef DISC_DIRC_VERBOSE
  fLogger->GetLogger()->Info(MESSAGE_ORIGIN, "tracknumber %d", gMC->GetStack()->GetCurrentTrackNumber());
#endif
}

Bool_t PndDisc::ProcessHits(FairVolume *) // v //[R.K.03/2017] unused variable(s)
{
  static TVector3 pos_in, mom_in;
  static Double_t integrated_energy_deposit;

  const Int_t pid_optical_photon = 50000050;

  static const Int_t sensor_volume_id = gMC->VolId("DiscDIRC_sensor_pmt"); // find a good place for one time initialization
  static const Int_t radiator_volume_id = gMC->VolId("DiscDIRC_Radiator"); // find a good place for one time initialization
  static const Int_t prism_a_volume_id = gMC->VolId("DiscDIRC_prism_a");
  static const Int_t prism_b_volume_id = gMC->VolId("DiscDIRC_prism_b");
  static const Int_t prism_c_volume_id = gMC->VolId("DiscDIRC_prism_c");

  // static const Int_t sensor_volume_uid = // [R.K. 09/2018] unused variable
  gGeoManager->GetUID("DiscDIRC_sensor_pmt"); // find a good place for one time initialization

  static Bool_t entered_inside = kFALSE;
  static Int_t entered_track_id = -1;
  static Int_t continued_track = -1;

  TParticle *track = gMC->GetStack()->GetCurrentTrack();

  // Some Logic Test:
  static Int_t current_track;
  if (gMC->IsTrackEntering()) {
    current_track = gMC->GetStack()->GetCurrentTrackNumber();
  } else {
    if (current_track != gMC->GetStack()->GetCurrentTrackNumber()) {
      LOG(warning) << "LOGIC ERROR: ProcessHits called with track (%d) that has not entered the volume!" << gMC->GetStack()->GetCurrentTrackNumber();
    }
  }

  // get volume information:
  Int_t volume_id, copy_no;
  volume_id = gMC->CurrentVolID(copy_no);

  if (gMC->TrackPid() == pid_optical_photon) {
    //---------------------------------------------------
    // handle optical photons
    //---------------------------------------------------
    TLorentzVector pos, mom;
    gMC->TrackPosition(pos);
    gMC->TrackMomentum(mom);

    Double_t mom_mag = mom.Vect().Mag();

    if (volume_id == radiator_volume_id) {
      std::map<int, double>::const_iterator it = internal_reflection_angle_of_photons.find(current_track);
      if (it == internal_reflection_angle_of_photons.end()) {
        double internal_reflection_angle = TMath::Pi() / 2.0 - mom.Vect().Theta();
        internal_reflection_angle_of_photons.insert(std::pair<int, double>(current_track, internal_reflection_angle));
      }
    } else if (volume_id == prism_a_volume_id || volume_id == prism_b_volume_id || volume_id == prism_c_volume_id) {
      std::map<int, std::pair<TLorentzVector, TLorentzVector>>::iterator it = photons_entering_optics.find(current_track);
      if (it == photons_entering_optics.end()) {
        TLorentzVector pos_local, mom_local;
        TString vol_path(gMC->CurrentVolPath());
        gGeoManager->cd(vol_path.Data());
        TGeoHMatrix *transform = gGeoManager->GetCurrentMatrix();
        Double_t p1[3];
        Double_t p2[3];
        pos.Vect().GetXYZ(p1);
        transform->MasterToLocal(p1, p2);
        pos_local.SetXYZT(p2[0], p2[1], p2[2], 0.0);

        mom.Vect().GetXYZ(p1);
        transform->MasterToLocalVect(p1, p2);
        mom_local.SetXYZT(p2[0], p2[1], p2[2], 0.0);
        photons_entering_optics.insert(std::pair<int, std::pair<TLorentzVector, TLorentzVector>>(current_track, std::pair<TLorentzVector, TLorentzVector>(pos_local, mom_local)));
      }
    }

    if ((mom_mag < 1.239841939 * 1.e-06 / wl_max_nm) || (mom_mag > 1.239841939 * 1.e-06 / wl_min_nm))
      gMC->StopTrack();

    // Photon is in focusing element
    if (gMC->IsTrackEntering() && volume_id == sensor_volume_id) {
      TString vol_path(gMC->CurrentVolPath());
      gGeoManager->cd(vol_path.Data());
      TGeoHMatrix *transform = gGeoManager->GetCurrentMatrix();

      Double_t p1[3];
      Double_t p2[3];
      pos.Vect().GetXYZ(p1);
      transform->MasterToLocal(p1, p2);
      pos.SetXYZT(p2[0], p2[1], p2[2], 0.0);

      mom.Vect().GetXYZ(p1);
      transform->MasterToLocalVect(p1, p2);
      mom.SetXYZT(p2[0], p2[1], p2[2], 0.0);

      // Verbose output
      if (fVerboseLevel > 0)
        LOG(info) << "track->T(): " << track->T() << "\tgMC->TrackTime(): " << gMC->TrackTime() << "\tev_header->GetT(): " << ev_header->GetT() << "\n";

      // double internal_reflection_angle = 0.0; //[R.K. 01/2017] unused variable
      std::map<int, double>::iterator it = internal_reflection_angle_of_photons.find(current_track);

      if (it == internal_reflection_angle_of_photons.end()) {
        LOG(warning) << "No registered total internal reflection angle for photon track id " << current_track << "\n";
      } else {
        // internal_reflection_angle = it->second; //[R.K. 01/2017] unused variable
        internal_reflection_angle_of_photons.erase(it);
      }

      // store the information
      Int_t volume_uid = gGeoManager->GetUID(gMC->VolName(volume_id));

      std::cout << "Sensor number: " << copy_no << ", volume id: " << volume_uid << ", photon position: x = " << pos[0] << " y = " << pos[1] << " z = " << pos[2] << std::endl;

      PndDiscSensorMCPoint *pt = new ((*clarr_sensor_hits)[nextid_clarr_sensor_hits++]) PndDiscSensorMCPoint(
        gMC->GetStack()->GetCurrentTrackNumber(), copy_no, volume_uid, it->second, pos.Vect(), mom.Vect(), gMC->TrackTime() * 1E9, gMC->TrackLength(), gMC->Edep(), track->T());

      std::map<int, std::pair<TLorentzVector, TLorentzVector>>::iterator it_optics = photons_entering_optics.find(current_track);
      if (it_optics == photons_entering_optics.end()) {
        LOG(warning) << "No registered optics entrance data for photon track id " << current_track << "\n";
      } else {
        pt->photon_entering_pos = it_optics->second.first.Vect();
        pt->photon_entering_momentum = it_optics->second.second.Vect();
      }

      gMC->StopTrack();
    }
  } else {
    //-------------------------------------------------------
    // Handle non-optical tracks
    //-------------------------------------------------------
    TLorentzVector pos, mom;
    gMC->TrackPosition(pos);
    gMC->TrackMomentum(mom);

    if (gMC->IsTrackEntering() || (!gMC->IsTrackEntering() && entered_track_id < 0)) {
      if (entered_track_id >= 0)
        LOG(fatal) << "Track entering but processing of preceding track was not finished !!!";

      entered_track_id = gMC->GetStack()->GetCurrentTrackNumber();
      if (entered_track_id < 0)
        LOG(fatal) << "Track entering has neg id !!!";

      std::map<int, std::pair<int, double>>::iterator it = last_track_occurence.find(entered_track_id);

      continued_track = -1;
      if (it != last_track_occurence.end()) {
        PndDiscParticleMCPoint *pt = (PndDiscParticleMCPoint *)clarr_particle_tracks->At(it->second.first);
        if (volume_id == pt->GetVolumeID() || copy_no == pt->GetDetectorID() || gMC->TrackTime() == it->second.second) {
          continued_track = it->second.first;
          integrated_energy_deposit = ((PndDiscParticleMCPoint *)clarr_particle_tracks->At(continued_track))->GetEnergyLoss();

          if (fVerboseLevel > 0)
            LOG(info) << Form("Continued: Track id %d, (vol %d, cpn %d, z=%f, T=%g, pdg %d)", entered_track_id, volume_id, copy_no, pos.Z(), gMC->TrackTime(), gMC->TrackPid());
        }
      }

      if (continued_track < 0) {
        pos_in = pos.Vect();
        mom_in = mom.Vect();
        integrated_energy_deposit = 0.0;
        entered_inside = gMC->IsNewTrack();
        if (fVerboseLevel > 0)
          LOG(info) << Form("Entered: Track id %d, (vol %d, cpn %d, z=%f, T=%g, pdg %d)", entered_track_id, volume_id, copy_no, pos.Z(), gMC->TrackTime(), gMC->TrackPid());
      }
    }

    if (entered_track_id >= 0) {
      if (entered_track_id != gMC->GetStack()->GetCurrentTrackNumber())
        LOG(fatal) << "Track id changed !!! Need a stack !!!";
      integrated_energy_deposit += gMC->Edep();
      if (fVerboseLevel > 0)
        LOG(info) << "Add eloss for: Track id " << entered_track_id;
    }

    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared() || !gMC->IsTrackAlive()) {
      // sanity check:
      if (entered_track_id != gMC->GetStack()->GetCurrentTrackNumber())
        LOG(fatal) << "Track exiting without having entered (entered id = " << entered_track_id << ")";

      if (fVerboseLevel > 0)
        LOG(info) << Form("Exiting: Track id %d (vol %d, cpn %d, Inside = %d, stop = %d, alive = %d, z=%f, T=%g)", entered_track_id, volume_id, copy_no, gMC->IsTrackInside(),
                          gMC->IsTrackStop(), gMC->IsTrackAlive(), pos.Z(), gMC->TrackTime());

      if (gMC->IsTrackStop() || !gMC->IsTrackInside() || gMC->IsTrackDisappeared() || !gMC->IsTrackAlive()) {

        std::map<int, std::pair<int, double>>::iterator it = last_track_occurence.find(entered_track_id);
        if (it == last_track_occurence.end())
          last_track_occurence.insert(std::pair<int, std::pair<int, double>>(entered_track_id, std::pair<int, double>(nextid_clarr_particle_tracks, gMC->TrackTime())));
        else
          it->second.second = gMC->TrackTime();

        if (continued_track >= 0) {
          PndDiscParticleMCPoint *pt = ((PndDiscParticleMCPoint *)clarr_particle_tracks->At(continued_track));
          pt->SetPositionOut(pos.Vect());
          pt->SetMomentumOut(mom.Vect());
          pt->SetTime(gMC->TrackTime() * 1E9);
          pt->SetLength(gMC->TrackLength());
          pt->SetEnergyLoss(integrated_energy_deposit);
          pt->SetIsPrimary(gMC->GetStack()->GetCurrentTrack()->IsPrimary());
        } else {
          Int_t volume_uid = gGeoManager->GetUID(gMC->VolName(volume_id));
          new ((*clarr_particle_tracks)[nextid_clarr_particle_tracks++]) PndDiscParticleMCPoint(
            entered_track_id, copy_no, volume_uid, pos_in, mom_in, pos.Vect(), mom.Vect(), gMC->TrackTime() * 1E9, gMC->TrackLength(), integrated_energy_deposit,
            gMC->TrackCharge(), gMC->TrackMass(), gMC->TrackPid(), entered_inside, gMC->GetStack()->GetCurrentTrack()->IsPrimary());
        }
        entered_track_id = -1;
      } else {
        LOG(info) << Form("Veto: Track id %d (Inside = %d)", entered_track_id, gMC->IsTrackInside());
      }
    }
  }

  return kTRUE;
}

void PndDisc::BeginEvent()
{
  ev_header = FairMCApplication::Instance()->GetGenerator()->GetEvent();
  internal_reflection_angle_of_photons.clear();
  photons_entering_optics.clear();
  last_track_occurence.clear();
}

/// Interface implementation - EndofEvent

void PndDisc::EndOfEvent()
{
  int n_entries = clarr_particle_tracks->GetEntries();
  for (int i = 0; i < n_entries; i++) {
    ((PndDiscParticleMCPoint *)clarr_particle_tracks->At(i))->Print();
  }
  Reset();
}

/// Interface implementation - Register the data collections in FairRootManager.

void PndDisc::Register()
{
  Bool_t serialized = kTRUE; // write arrays to file?
  FairRootManager::Instance()->Register("DiscSensorMCPoint", "DiscPoint", clarr_sensor_hits, serialized);
  FairRootManager::Instance()->Register("DiscParticleMCPoint", "DiscPoint", clarr_particle_tracks, serialized);
}

/// Interface implementation - Get a data collection by index.

/// \param iColl
/// \returns pointer to TClonesArray or nullptr if iColl out of range.

TClonesArray *PndDisc::GetCollection(Int_t iColl) const
{
  switch (iColl) {
  case 0: return clarr_sensor_hits;
  case 1: return clarr_particle_tracks;
  default: return nullptr;
  }
}

/// Interface implementation - reset the 'containers'

/// This function is abstract in FairDetector, but is not called
/// by the underlying framework as one would expect. The user has
/// to call it explicitly.

void PndDisc::Reset()
{
  if (clarr_sensor_hits != nullptr)
    clarr_sensor_hits->Clear();
  if (clarr_particle_tracks != nullptr)
    clarr_particle_tracks->Clear();
  nextid_clarr_sensor_hits = 0;
  nextid_clarr_particle_tracks = 0;
}

/// Interface implementation - Check if a given volume name belongs to a sensitive volume.

/// FAIR geometry construction (FairModule::ExpandNode) querys sensitivity
/// for every single volume name. Volume name format: Name_CopyNr (e.g. Radiator_0).
/// For simplicity, copy numbers are neglected and all instances ('copys') of a
/// specific volume will be flagged sensitive/insensitive.

bool PndDisc::CheckIfSensitive(std::string name)
{
  return (names_of_sensitive_volumes.count(name) != 0);
}

/// Enable/Disable the storage of photon track information.

/// The container for photon track information is
/// created anyway, but it will not be filled if
/// store_photon_tracks == false.

void PndDisc::StorePhotonTracks(Bool_t bval)
{
  store_photon_tracks = bval;
}

/// Set the wavelength range of the bandpass filters.

void PndDisc::SetFilterInterval(Double_t const &wl_min_nm_, Double_t const &wl_max_nm_)
{
  wl_min_nm = wl_min_nm_;
  wl_max_nm = wl_max_nm_;
}

ClassImp(PndDisc)
