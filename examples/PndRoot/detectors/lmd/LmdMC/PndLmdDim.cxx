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

/*
 * Pndcpp
 *
 * this class gives you methods to retrieve parameters of the geometry
 *
 *  Created on: Oct 5, 2012
 *      Author: promme
 *		as of 2015 maintained by Roman Klasen, klasen@kph.uni-mainz.de
 */

#include "PndLmdDim.h"
#include <TGeoMatrix.h>
#include <stdlib.h>
#include <TGeoPhysicalNode.h>
// //work with DB
// #include<PndLmdContFact.h>
// #include<TList.h>
// #include<PndLmdAlignPar.h>
PndLmdDim *PndLmdDim::pinstance = nullptr;

#include <cmath>

// version 3: navigation path names changed
int PndLmdDim::geometry_version = 3;

#include <TROOT.h>
PndLmdDim::PndLmdDim()
{
  sensIDoffset = 0;
  double test_mult_fact = 1.; // 100.; // should be 1 when not debugging code
  pi = 3.141592654;
  // number of detector planes
  n_planes = 4;
  // number of modules per plane half
  nmodules = 5;
  // position of planes where the first plane defines the origin
  plane_pos_z = new double[4];
  plane_pos_z[0] = 0.0;
  plane_pos_z[1] = 20.0;
  plane_pos_z[2] = 30.0;
  plane_pos_z[3] = 40.0;
  half_offset_x = 0.005 * test_mult_fact; // 50 mum
  half_offset_y = 0.005 * test_mult_fact;
  half_offset_z = 0.005 * test_mult_fact;
  half_tilt_phi = 0.; // negligible
  half_tilt_theta = 0.;
  half_tilt_psi = 0.;
  plane_half_offset_x = 0.005 * test_mult_fact;
  plane_half_offset_y = 0.005 * test_mult_fact;
  plane_half_offset_z = 0.005 * test_mult_fact;
  plane_half_tilt_phi = 0.0; // negligible
  plane_half_tilt_theta = 0.0;
  plane_half_tilt_psi = 0.0;
  cvd_offset_x = 0.005 * test_mult_fact;
  cvd_offset_y = 0.005 * test_mult_fact;
  cvd_offset_z = 0.0;
  cvd_tilt_phi = 0.001 * test_mult_fact;
  cvd_tilt_theta = 0.;
  cvd_tilt_psi = 0.;
  side_offset_x = 0.005 * test_mult_fact;
  side_offset_y = 0.005 * test_mult_fact;
  side_offset_z = 0.; // do not use! -> clashing volumes
  side_tilt_phi = 0.; // do not use! -> clashing volumes
  side_tilt_theta = 0.;
  side_tilt_psi = 0.;                             // do not use! -> clashing volumes
  sensor_offset_x = 0 * 100e-4 * test_mult_fact;  // 100 mum
  sensor_offset_y = 0 * 100e-4 * test_mult_fact;  // 100 mum
  sensor_offset_z = 0.;                           // do not use! -> clashing volumes
  sensor_tilt_phi = 0 * 2500e-6 * test_mult_fact; // 150 murad
  sensor_tilt_theta = 0.;                         // do not use! -> clashing volumes
  sensor_tilt_psi = 0.;                           // do not use! -> clashing volumes

  // cvd_diamond is cut out of 79.5 mm discs of 200 micron thickness
  // inner min. radius due to beam pipe + a safety margin
  inner_rad = 3.7;
  // not used yet but should be the outer acceptance
  outer_rad = 12.;
  // number of CVD diamond discs per plane
  n_cvd_discs = 10;
  // radius of a CVD diamond disc
  cvd_disc_rad = 7.95 / 2.;
  // the half of the diamond thickness
  cvd_disc_thick_half = 0.01;
  // even and odd discs in a plane will be shifted in z in order to prevent
  // mechanical damage during assembly
  cvd_disc_even_odd_offset = 0.25;
  // angle from the division of a circle into n_cvd_discs
  delta_phi = 2. * pi / ((double)(n_cvd_discs));
  // a polygon of n_cvd_discs sides fitting a radius of inner_rad
  // has a side length pol_side_lg of
  pol_side_lg_half = inner_rad * sin(delta_phi / 2.);
  // the minimum distance to the center of the polygone is given by
  pol_side_dist_min = inner_rad * cos(delta_phi / 2.);
  // the cvd disc has to be placed such that the disc crosses
  // the inner ring at an angle of 0 and delta_phi
  // this defines the distance to the center according to pythagoras
  cvd_disc_dist = pol_side_dist_min + sqrt(cvd_disc_rad * cvd_disc_rad - pol_side_lg_half * pol_side_lg_half);

  kapton_disc_thick_half = 0.004454 / 2.; // Using aluminum in equivalent thickness to cover flex cable and gluing at once

  maps_n_col = 3;
  maps_n_row = 2;
  // enabled [row][col]
  enabled = new bool *[maps_n_row];
  enabled[0] = new bool[maps_n_col];
  enabled[1] = new bool[maps_n_col];
  enabled[0][0] = true;
  enabled[0][1] = true;
  enabled[0][2] = true;
  enabled[1][0] = false;
  enabled[1][1] = true;
  enabled[1][2] = true;
  n_sensors = 5;
  // NOTE: MOST of the following VARIABLES are HALF of it
  // due to geometry construction in GEANT
  maps_thickness = 0.005 / 2.;
  maps_passive_top = 0.01 / 2.; //*12000.;
  maps_passive_bottom = 0.05 / 2.;
  maps_passive_left = 0.01 / 2.;  //*6000.;
  maps_passive_right = 0.01 / 2.; //*3000.;
  maps_active_width = 2.0 / 2. - maps_passive_left - maps_passive_right;
  maps_active_height = 2.0 / 2. - maps_passive_top - maps_passive_bottom;

  maps_active_pixel_size = 0.008; // 80µm pixel size

  maps_width = maps_passive_left + maps_active_width + maps_passive_right;
  maps_height = maps_passive_top + maps_active_height + maps_passive_bottom;

  maps_active_offset_x = (maps_passive_left - maps_passive_right) / 2.;
  maps_active_offset_y = (maps_passive_top - maps_passive_bottom) / 2.;

  die_gap = 0.05; // (cm)

  maps_die_width = maps_width * maps_n_col;
  maps_die_height = maps_height * maps_n_row + die_gap * maps_n_row - 1;

  die_offset_x = 0.;
  die_offset_y = 0.;
  die_offset_z = 0.;
  die_tilt_phi = 0.;
  die_tilt_theta = 0.;
  die_tilt_psi = 0.;
  //*********************************** lumi box parameters ***********************************
  // see CAD files for details
  // https://edms.cern.ch/nav/P:FAIR-000000719:V0/P:FAIR-000000726:V0/TAB3
  // width
  box_size_x = 36. / 2.;
  // height
  box_size_y = 60.6 / 2.;
  // length
  box_size_z = 95.6 / 2.;
  // thickness of the V2A steel plates
  box_thickness = 0.5;
  // position of the inner rib
  pos_rib = 36.4;
  // beam pipe radius at entrance
  rad_entrance = 9. + 1.;
  // beam pipe radius at exit
  rad_exit = 5.;
  // beam pipe separating non interacting paricles
  rad_pipe = 3.5;
  // beam pipe thickness;
  // pipe_thickness = 0.1;
  // cone height of the transition region
  // length_transision = 36.4;
  // length of the inner pipe
  // length_pipe = 50.;
  //*********************************** global parameters *************************************
  // where bending starts with
  end_seg_upstream = 361;
  // the bending radius
  r_bend = 5700.;
  // and the angle of the circle path
  phi_bend = 40.0e-3;
  ;
  // the point where both tangents of the straight beam pipe tubes meet is
  pos_rot_z = end_seg_upstream + tan(phi_bend / 2.) * r_bend;
  // z position of the lmd box
  pos_z = 1050.; //(cm)
  // position of the first detector plane
  pos_plane_0 = 74.1;
  end_seg_bend = end_seg_upstream + sin(phi_bend) * r_bend;
  // x position of the lmd
  pos_x = (pos_z - end_seg_upstream - tan(phi_bend / 2.) * r_bend) * tan(phi_bend);
  // y position of the lmd
  pos_y = 0.;
  rot_x = 0.;
  rot_y = phi_bend;
  rot_z = 0.;

  // later transformations will require a relative
  // navigation structure based on strings
  // the path to individual volumes is stored here

  // luminosity detector top box
  nav_paths.push_back("lmd_vol_vac");
  // luminosity detector reference system
  nav_paths.push_back("lmd_vol_ref_sys");
  // luminosity detector halfs with the
  // number 0 for top and 1 for bottom
  nav_paths.push_back("lmd_vol_half");
  // luminosity detector plane 0 to 3
  nav_paths.push_back("lmd_vol_plane");
  // luminosity detector module 0 to 5
  // clockwise around direction upstream (z)
  nav_paths.push_back("lmd_vol_module");
  // luminosity detector front side (upstream = 0)
  // and backside (downstream = 1)
  nav_paths.push_back("lmd_vol_side");
  // luminosity detector die
  // ( 0 for the 1x3 sensors and 1 for the 1x2 sensors )
  nav_paths.push_back("lmd_vol_die");
  // luminosity detector sensor
  // nav_paths.push_back("lmd_vol_sensor_"); misalignment of individual sensors is not supportet yet
  // luminosity detector active sensor
  // 0 is the most inner sensor
  nav_paths.push_back("LumActivePixelRect"); // the _ comes automatically with the copy number

  fgGeoMan = (TGeoManager *)gROOT->FindObject("FAIRGeom");
  if (!fgGeoMan)
    cout << "Error: could not find the geometry manager!" << endl;
}

PndLmdDim::PndLmdDim(const PndLmdDim &instance)
{
  if (!instance.box_thickness)
    cout << " fix for pedantic compiler will never appear on cout " << endl;
}

PndLmdDim::~PndLmdDim()
{
  cout << endl;
  cout << " Cleaning up PndLmdDim " << endl;
  cout << " If you see that message several times please check the performance of your code! " << endl;
  Cleanup();
  delete pinstance;
}

PndLmdDim &PndLmdDim::Get_instance()
{
  if (!pinstance) {
    pinstance = new PndLmdDim();
  }
  return (*pinstance);
}

PndLmdDim *PndLmdDim::Instance()
{
  if (!pinstance) {
    pinstance = new PndLmdDim();
  }
  return (pinstance);
}

/*
 void PndLmdDim::transform_sensor_local_to_lmd_local(const int sensor_id, double & x, double & y, double & z, bool misaligned)
 {
 }



 void PndLmdDim::transform_local_lmd_to_global(const int sensor_id, double & x, double & y, double & z, bool misaligned)
 {
 }



 void PndLmdDim::transform_local_sensor()
 {
 }*/

#include <FairGeoLoader.h>
#include <FairGeoInterface.h>
#include <FairGeoMedia.h>
#include <FairGeoBuilder.h>
#include <TGeoTorus.h>
#include <TGeoSphere.h>
#include <TGeoTube.h>
#include <TGeoCompositeShape.h>
#include <TGeoCone.h>
#include <TGeoBBox.h>
#include <TGeoPcon.h>

#include <TPad.h>
bool PndLmdDim::Retrieve_version_number()
{
  bool result = false;
  // FairRootManager* ioman = FairRootManager::Instance();
  TGeoManager *gGeoMan = (TGeoManager *)gROOT->FindObject("FAIRGeom");
  if (!gGeoMan) {
    cout << " Info: no FAIRGeom found, using gGeoManager " << endl;
    gGeoMan = gGeoManager;
  }
  if (gGeoMan) {
    // gGeoMan->Draw("ogl");
    // gPad->Update();
    // gPad->Print("test.pdf");
    TGeoVolume *vol = gGeoMan->FindVolumeFast(nav_paths[0].c_str());
    // cout << gGeoMan->GetCurrentNode()->GetName() << endl;
    if (vol) {
      string sversion = vol->GetTitle();
      if (sversion.compare(0, 8, "version ") != 0) {
        cout << " Warning from PndLmdDim::Retrieve_version_number: no version number encoded in the node title. Setting it to 0. " << endl;
        geometry_version = 0;
      } else {
        // take the number behind "version " string
        geometry_version = atoi(&(vol->GetTitle()[8]));
      }
      cout << " Info from PndLmdDim::Retrieve_version_number: The geometry version was set to " << geometry_version << endl;
      // cout << vol->GetName() << endl;
      result = true;
    } else {
      cout << " *** Error in PndLmdDim::Retrieve_version_number:" << endl;
      cout << " Could not find the top volume " << nav_paths[0].c_str() << " to retrieve the version number of the luminosity detector! Is the geometry already loaded? " << endl;
    }
  } else {
    cout << " *** Error in PndLmdDim::Retrieve_version_number:" << endl;
    cout << " Could not find a GeoManager to retrieve the version number of the luminosity detector! " << endl;
  }
  return result;
}

void PndLmdDim::Generate_rootgeom(TGeoVolume &mothervol, bool misaligned)
{
  Cleanup();
  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  if (!geoLoad) {
    geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    cout << " creating FairGeoLoader instance " << endl;
  }
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  string dir = getenv("VMCWORKDIR");
  geoFace->setMediaFile((dir + "/geometry/media_pnd.geo").c_str()); //("${VMCWORKDIR}/geometry/media_pnd.geo");
  geoFace->readMedia();
  // geoFace->print();

  FairGeoMedia *Media = geoFace->getMedia();
  FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();

  // retrieve available media
  FairGeoMedium *FairMediumAir = Media->getMedium("air");
  FairGeoMedium *FairMediumSteel = Media->getMedium("steel");
  FairGeoMedium *FairMediumAl = Media->getMedium("Aluminum");
  FairGeoMedium *FairMediumSilicon = Media->getMedium("silicon");
  FairGeoMedium *FairMediumDiamond = Media->getMedium("HYPdiamond");
  FairGeoMedium *FairMediumVacuum = Media->getMedium("vacuum7");
  FairGeoMedium *FairMediumKapton = Media->getMedium("kapton");
  FairGeoMedium *FairMediumCopper = Media->getMedium("copper");

  if (!FairMediumAir || !FairMediumSteel || !FairMediumAl || !FairMediumKapton || !FairMediumSilicon || !FairMediumVacuum || !FairMediumCopper) {
    std::cout << " Error: not all media found " << std::endl;
    return;
  }

  int nmed;
  nmed = geobuild->createMedium(FairMediumAir);
  nmed = geobuild->createMedium(FairMediumSteel);
  nmed = geobuild->createMedium(FairMediumAl);
  nmed = geobuild->createMedium(FairMediumKapton);
  nmed = geobuild->createMedium(FairMediumSilicon);
  nmed = geobuild->createMedium(FairMediumDiamond);
  nmed = geobuild->createMedium(FairMediumVacuum);
  if (!nmed)
    cout << " fix for pedantic compiler line " << endl;

  // no translation nor rotation
  TGeoRotation *rot_no = new TGeoRotation("rot_no", 0., 0., 0.);           // no rotation
  TGeoTranslation *trans_no = new TGeoTranslation("trans_no", 0., 0., 0.); // no translation
  TGeoCombiTrans *rottrans_no = new TGeoCombiTrans("rottrans_no", 0., 0., 0., rot_no);
  rottrans_no->RegisterYourself();

  // ************ create the luminosity monitor box ***********
  // create the bounding box
  double tube_upstream_length = 33.3 / 2.;
  double tube_upstream_rad_in = 10.;
  double tube_upstream_rad_out = tube_upstream_rad_in + 0.2;
  double tube_dostream_length = 12. / 2.; // length with flange

  // create a vacuum around the luminosity detector
  double lmd_total_length = box_size_z + tube_upstream_length + tube_dostream_length;

  double origin[3] = {0., 0., lmd_total_length};
  TGeoBBox *lmd_box_vac = new TGeoBBox("lmd_box_vac", box_size_x, box_size_y, lmd_total_length, origin);
  TGeoVolume *lmd_vol_vac = new TGeoVolume(nav_paths[0].c_str(), lmd_box_vac, fgGeoMan->GetMedium("vacuum7"));
  // lmd_vol_vac->SetTransparency(20);
  lmd_vol_vac->SetLineColor(3);
  double x, y, z, rottheta, rotphi, rotpsi;
  Get_pos_lmd_global(x, y, z, rottheta, rotphi, rotpsi);
  TGeoRotation *lmd_rot = new TGeoRotation("lmd_rot");
  lmd_rot->RotateX(rottheta / pi * 180.);
  lmd_rot->RotateY(rotphi / pi * 180.);
  lmd_rot->RotateZ(rotpsi / pi * 180.);
  TGeoCombiTrans *lmd_transrot = new TGeoCombiTrans(x, y, z, lmd_rot);
  lmd_transrot->SetName("lmd_transrot");
  lmd_transrot->RegisterYourself();
  // pumping station upstream
  TGeoTube *pipe_upstream = new TGeoTube("pipe_upstream", tube_upstream_rad_in, tube_upstream_rad_out, tube_upstream_length);
  TGeoCombiTrans *comb_trans_pipe_upstream = new TGeoCombiTrans("comb_trans_pipe_upstream", 0., 0., tube_upstream_length, rot_no);
  // comb_trans_pipe_upstream->RegisterYourself();
  TGeoVolume *lmd_vol_pipe_up = new TGeoVolume("lmd_vol_pipe_up", pipe_upstream, fgGeoMan->GetMedium("steel"));
  lmd_vol_pipe_up->SetLineColor(11);
  lmd_vol_vac->AddNode(lmd_vol_pipe_up, 0, comb_trans_pipe_upstream);
  // the lmd box
  TGeoBBox *lmd_box_outer = new TGeoBBox("lmd_box_outer", box_size_x, box_size_y, box_size_z);
  TGeoBBox *lmd_box_inner = new TGeoBBox("lmd_box_inner", box_size_x - box_thickness, box_size_y - box_thickness, box_size_z - box_thickness);
  TGeoBBox *lmd_box_rib = new TGeoBBox("lmd_box_rib", box_size_x - box_thickness, box_size_y - box_thickness, box_thickness / 2.);
  TGeoTube *box_hole_upstream = new TGeoTube("box_hole_upstream", 0.0, rad_entrance, box_thickness);
  // move the cut pipe upstream
  TGeoCombiTrans *comb_trans_cut_pipe_upstream = new TGeoCombiTrans("comb_trans_cut_pipe_upstream", 0., 0., -box_size_z + box_thickness / 2., rot_no);
  comb_trans_cut_pipe_upstream->RegisterYourself();
  TGeoTube *box_hole_downstream = new TGeoTube("box_hole_downstream", 0.0, rad_exit, box_thickness);
  // move the cut pipe downstream
  TGeoCombiTrans *comb_trans_cut_pipe_downstream = new TGeoCombiTrans("comb_trans_cut_pipe_downstream", 0., 0., +box_size_z - box_thickness / 2., rot_no);
  comb_trans_cut_pipe_downstream->RegisterYourself();
  // move rib upstream
  TGeoCombiTrans *comb_trans_rib = new TGeoCombiTrans("comb_trans_rib", 0., 0., -box_size_z + pos_rib, rot_no);
  comb_trans_rib->RegisterYourself();
  // inner clash protection rods
  double clash_rod_x = 3.;
  double clash_rod_y = 0.5;
  double clash_rod_z = 29.75;
  double origin_left[3] = {-box_size_x + clash_rod_x + box_thickness, 0., 18.15};
  new TGeoBBox("lmd_box_clash_rod_left", clash_rod_x, clash_rod_y, clash_rod_z, origin_left); // TGeoBBox* lmd_box_clash_rod_left =  //[R.K.03/2017] unused variable
  double origin_right[3] = {+box_size_x - clash_rod_x - box_thickness, 0., 18.15};
  new TGeoBBox("lmd_box_clash_rod_right", clash_rod_x, clash_rod_y, clash_rod_z, origin_right); // TGeoBBox* lmd_box_clash_rod_right =  //[R.K.03/2017] unused variable
  // compose all the parts into one luminosity vacuum box
  TGeoCompositeShape *shape_lmd_box = new TGeoCompositeShape("shape_lmd_box", "(lmd_box_outer-lmd_box_inner + ((lmd_box_rib-box_hole_upstream):comb_trans_rib))"
                                                                              "-box_hole_upstream:comb_trans_cut_pipe_upstream"
                                                                              "-box_hole_downstream:comb_trans_cut_pipe_downstream"
                                                                              "+lmd_box_clash_rod_left+lmd_box_clash_rod_right");
  TGeoVolume *lmd_vol_box = new TGeoVolume("lmd_vol_box", shape_lmd_box, fgGeoMan->GetMedium("steel"));
  lmd_vol_box->SetLineColor(11);
  // lmd_vol_box->SetVisibility(false);//TEST
  // lmd_vol_box->SetTransparency(20);
  TGeoCombiTrans *comb_trans_lmd_box = new TGeoCombiTrans("comb_trans_lmd_box", 0., 0., 2 * tube_upstream_length + box_size_z, rot_no);
  // comb_trans_pipe_upstream->RegisterYourself();
  lmd_vol_vac->AddNode(lmd_vol_box, 0, comb_trans_lmd_box);

  // pipe downstream of the box
  double lmd_pipe_box_do[18];
  lmd_pipe_box_do[0] = 0.;
  lmd_pipe_box_do[1] = 360.;
  lmd_pipe_box_do[2] = 4.;

  lmd_pipe_box_do[3] = box_size_z * 2. + tube_upstream_length * 2.;
  lmd_pipe_box_do[4] = rad_exit - 0.2;
  lmd_pipe_box_do[5] = rad_exit;

  lmd_pipe_box_do[6] = lmd_pipe_box_do[3] + tube_dostream_length * 2. - 4.;
  lmd_pipe_box_do[7] = lmd_pipe_box_do[4];
  lmd_pipe_box_do[8] = lmd_pipe_box_do[5];

  lmd_pipe_box_do[9] = lmd_pipe_box_do[6];
  lmd_pipe_box_do[10] = lmd_pipe_box_do[4];
  lmd_pipe_box_do[11] = 7.;

  lmd_pipe_box_do[12] = lmd_pipe_box_do[3] + tube_dostream_length * 2.;
  lmd_pipe_box_do[13] = lmd_pipe_box_do[10];
  lmd_pipe_box_do[14] = lmd_pipe_box_do[11];

  // lmd_pipe_box_do[15] = lmd_pipe_box_do[12];
  // lmd_pipe_box_do[16] = lmd_pipe_box_do[4];
  // lmd_pipe_box_do[17] = lmd_pipe_box_do[14];

  TGeoPcon *shape_pipe_box_do = new TGeoPcon(lmd_pipe_box_do);
  TGeoVolume *vlum_pipe_box_do = new TGeoVolume("vlum_pipe_box_do", shape_pipe_box_do, fgGeoMan->GetMedium("steel"));
  vlum_pipe_box_do->SetLineColor(kGray); // 39);
  TGeoCombiTrans *lmd_trans_pipe_box_do = new TGeoCombiTrans("lmd_trans_pipe_box_do", 0., 0., 0., rot_no);
  lmd_trans_pipe_box_do->RegisterYourself();
  lmd_vol_vac->AddNode(vlum_pipe_box_do, 0, lmd_trans_pipe_box_do);
  // end of pipe downstream of the box

  //	TGeoTube* lmd_flange_upstr = new TGeoTube(
  //			"lmd_flange_upstr", 9.2, 25.3/2., 1.2);
  //	TGeoCombiTrans* lmd_trans_fl_up = new TGeoCombiTrans("lmd_trans_fl_up", 0., 0., 1.2+delta, r1);
  //	lmd_trans_fl_up->RegisterYourself();
  // upstream flange holding the kapton cone
  //	TGeoTube* lmd_cone_flange_upstr = new TGeoTube(
  //			"lmd_cone_flange_upstr", 9.2, 25.3/2., 1.5);
  //	TGeoCombiTrans* lmd_trans_co_fl_up = new TGeoCombiTrans("lmd_trans_co_fl_up", 0., 0., -1.5+50.-delta, r1);
  //	lmd_trans_co_fl_up->RegisterYourself();

  // ********************* enhanced beam pipe construct in the box ********************

  double debug_multiplier = 1.;
  // define where the inner region of the box starts
  double box_inner_up_z = 2 * tube_upstream_length + box_thickness;

  // flange holding the kapton cone upstream
  double lmd_cone_clamp_length = 13.;
  double lmd_cone_clamp_params[15];
  lmd_cone_clamp_params[0] = 0.;
  lmd_cone_clamp_params[1] = 360.;
  lmd_cone_clamp_params[2] = 4.;

  // values taken from drawings starting upstream at the box
  // pipe extension as well as female part of the clamp are included as well
  lmd_cone_clamp_params[3] = 0.;
  lmd_cone_clamp_params[4] = 20. / 2.;
  lmd_cone_clamp_params[5] = 20.4 / 2.;

  lmd_cone_clamp_params[6] = 10.;
  lmd_cone_clamp_params[7] = 20. / 2.;
  lmd_cone_clamp_params[8] = 20.4 / 2.;

  lmd_cone_clamp_params[9] = 10.;
  lmd_cone_clamp_params[10] = 20. / 2.;
  lmd_cone_clamp_params[11] = 26. / 2.;

  lmd_cone_clamp_params[12] = lmd_cone_clamp_length;
  lmd_cone_clamp_params[13] = 20. / 2.;
  lmd_cone_clamp_params[14] = 26. / 2.;

  TGeoPcon *lmd_cone_clamp = new TGeoPcon(lmd_cone_clamp_params);
  TGeoVolume *vlum_cone_clamp = new TGeoVolume("vlum_cone_clamp", lmd_cone_clamp, fgGeoMan->GetMedium("steel"));
  vlum_cone_clamp->SetLineColor(kGray); // 39);
  TGeoCombiTrans *lmd_trans_lmd_cone_clamp = new TGeoCombiTrans("lmd_trans_lmd_cone_clamp", 0., 0., box_inner_up_z, rot_no);
  lmd_trans_lmd_cone_clamp->RegisterYourself();
  lmd_vol_vac->AddNode(vlum_cone_clamp, 0, lmd_trans_lmd_cone_clamp);

  /*
   * polymide      |\
   * aluminum      |\\
   *                \\\|\
   *                 \\\ \
   *                  \\\ \
   *                   \\| \ ________
   *                   |\|\__________| V2A tube
   *                    \____| brass
   *
   *                    y [cm]
   *                    ^
   *                    |
   *                    |-------> z [cm]
   *
   *
   * nomenclature:
   * transistion region consists of an aluminum (AL) / polymide (pol) laminate cone (c)
   * with an inner (i) and outer (o) diameter upstream (u) and downstream (d)
   * cpoluo = cone polymide upstream outer
   * The tube (t) is made of V2A (A2) and starts with a cone to glue the transistion cone to it
   * To conduct the EM fields of the non interacting beam a brass (br) ring is inserted
   * The opening angle of the cone is 15 degree as determined by HESR
   * the tube must have a diameter of 70 mm due to beam acceptance restrictions
   */

  // 10µm aluminum
  double cALthick = 0.001 * debug_multiplier;
  // 20µm polimide
  double cpolthick = 0.002 * debug_multiplier;
  // 500µm brass ring
  double tbrthick = 0.05;
  // 500µm inner beam pipe
  double tA2thick = 0.05;
  double cone_angle = 15. / 180. * 3.1416;

  double cALdiy = 7. / 2.;
  double cALdiz = 40.;
  double cALdoy = cALdiy + cALthick / cos(cone_angle);
  double cALdoz = cALdiz;
  double cpoldiy = cALdoy;
  double cpoldiz = cALdoz;
  double cpoldoy = cpoldiy + cpolthick / cos(cone_angle);
  double cpoldoz = cpoldiz;
  double cbrdoy = cALdiy;
  double cbrdoz = cALdiz;
  double cbrdiy = cbrdoy - tbrthick; // /cos(cone_angle) does not apply here for simplicity;
  double cbrdiz = cbrdoz;

  double cALuiz = lmd_cone_clamp_length;
  double cALuiy = cALdiy + (cALdiz - cALuiz) * tan(cone_angle);
  double cALuoy = cALuiy + cALthick / cos(cone_angle);
  double cALuoz = cALuiz;
  double cpoluiy = cALuoy;
  double cpoluiz = cALuoz;
  double cpoluoy = cpoluiy + cpolthick / cos(cone_angle);
  double cpoluoz = cpoluiz;

  double cbruoz = cbrdiz - 0.5;
  double cbruoy = cbrdoy + 0.5 * tan(cone_angle);
  double cbruiz = cbruoz;
  double cbruiy = cbruoy - tbrthick; // /cos(cone_angle) does not apply here for simplicity;

  double cA2diy = cALdiy;
  double cA2diz = cpoldoz + (cpoldoy - cALdiy) / tan(cone_angle);
  double cA2uiz = cA2diz - 0.1 / tan(cone_angle);
  double cA2uiy = cA2diy + 0.1;
  double cA2uoy = cA2uiy + tA2thick;
  // double cA2uoz = cA2uiz; //[R.K. 01/2017] unused variable?
  double cA2doy = cA2diy + tA2thick;
  // double cA2doz = cA2diz; //[R.K.02/2017] Unused variable?

  double tbruiz = cbrdiz;
  double tbruiy = cbrdiy;
  // double tbruoz = cbrdoz; //[R.K. 01/2017] unused variable?
  double tbruoy = cbrdoy;

  double tbrdiz = tbruiz + 2.;
  double tbrdiy = tbruiy;
  // double tbrdoz = tbrdiz; //[R.K. 01/2017] unused variable?
  double tbrdoy = tbruoy;

  // double tA2uoz = cA2doz; //[R.K. 01/2017] unused variable?
  double tA2uoy = cA2doy;
  double tA2uiz = cA2diz;
  double tA2uiy = cA2diy;

  double tA2diz = tA2uiz + 44.;
  double tA2diy = tA2uiy;
  // double tA2doz = tA2uiz; //[R.K. 01/2017] unused variable?
  double tA2doy = tA2uoy;

  TGeoCone *lmd_capton_cone = new TGeoCone("lmd_capton_cone", (cpoldoz - cpoluoz) / 2., cpoluiy, cpoluoy, cpoldiy, cpoldoy);
  TGeoCombiTrans *lmd_trans_cap_co = new TGeoCombiTrans("lmd_trans_cap_co", 0., 0., box_inner_up_z + (cpoldoz - cpoluoz) / 2. + cpoluoz, rot_no);
  lmd_trans_cap_co->RegisterYourself();
  TGeoVolume *vlum_CaptonCone = new TGeoVolume("vlum_CaptonCone", lmd_capton_cone, fgGeoMan->GetMedium("kapton"));
  vlum_CaptonCone->SetLineColor(kRed);                        // 39);
  lmd_vol_vac->AddNode(vlum_CaptonCone, 0, lmd_trans_cap_co); // TEST with/without cone!!!

  // 10 mu thick kapton foil aluminum coating
  TGeoCone *lmd_al_cone = new TGeoCone("lmd_al_cone", (cALdiz - cALuiz) / 2., cALuiy, cALuoy, cALdiy, cALdoy);
  TGeoVolume *vlum_AlCone = new TGeoVolume("vlum_AlCone", lmd_al_cone, fgGeoMan->GetMedium("Aluminum"));
  vlum_AlCone->SetLineColor(kGray);                       // 39);
  lmd_vol_vac->AddNode(vlum_AlCone, 0, lmd_trans_cap_co); // TEST with/without cone!!!

  double lmd_pipe_params[36];
  lmd_pipe_params[0] = 0.;
  lmd_pipe_params[1] = 360.;
  lmd_pipe_params[2] = 11.;

  lmd_pipe_params[3] = cA2uiz;
  lmd_pipe_params[4] = cA2uiy;
  lmd_pipe_params[5] = cA2uoy;

  lmd_pipe_params[6] = cA2diz;
  lmd_pipe_params[7] = cA2diy;
  lmd_pipe_params[8] = cA2doy;

  lmd_pipe_params[9] = tA2diz;
  lmd_pipe_params[10] = tA2diy;
  lmd_pipe_params[11] = tA2doy;

  lmd_pipe_params[12] = 0 + lmd_pipe_params[9];
  lmd_pipe_params[13] = 3.5;
  lmd_pipe_params[14] = 4.05;

  lmd_pipe_params[15] = 1.5 + lmd_pipe_params[9];
  lmd_pipe_params[16] = 3.5;
  lmd_pipe_params[17] = 4.05;

  lmd_pipe_params[18] = 3.5 + lmd_pipe_params[9];
  lmd_pipe_params[19] = 4.2;
  lmd_pipe_params[20] = 6.;

  lmd_pipe_params[21] = 10. + lmd_pipe_params[9];
  lmd_pipe_params[22] = 4.2;
  lmd_pipe_params[23] = 6.;

  lmd_pipe_params[24] = 10. + lmd_pipe_params[9];
  lmd_pipe_params[25] = 4.2;
  lmd_pipe_params[26] = 4.55;

  lmd_pipe_params[27] = 20. + lmd_pipe_params[9];
  lmd_pipe_params[28] = 4.2;
  lmd_pipe_params[29] = 4.55;

  lmd_pipe_params[30] = 20. + lmd_pipe_params[9];
  lmd_pipe_params[31] = 4.2;
  lmd_pipe_params[32] = 4.55; // 7.;

  lmd_pipe_params[33] = 22 + lmd_pipe_params[9];
  lmd_pipe_params[34] = 4.2;
  lmd_pipe_params[35] = 4.55; // 7.;

  TGeoPcon *lmd_V2_pipe = new TGeoPcon(lmd_pipe_params);
  TGeoVolume *vlum_V2_pipe = new TGeoVolume("vlum_V2_pipe", lmd_V2_pipe, fgGeoMan->GetMedium("steel"));
  vlum_V2_pipe->SetLineColor(kGray); // 39);
  TGeoCombiTrans *lmd_trans_lmd_V2_pipe = new TGeoCombiTrans("lmd_trans_lmd_V2_pipe", 0., 0., box_inner_up_z, rot_no);
  lmd_trans_lmd_V2_pipe->RegisterYourself();
  lmd_vol_vac->AddNode(vlum_V2_pipe, 0, lmd_trans_lmd_V2_pipe);

  double lmd_conductor_params[12];
  lmd_conductor_params[0] = 0.;
  lmd_conductor_params[1] = 360.;
  lmd_conductor_params[2] = 3.;

  lmd_conductor_params[3] = cbruiz;
  lmd_conductor_params[4] = cbruiy;
  lmd_conductor_params[5] = cbruoy;

  lmd_conductor_params[6] = cbrdiz;
  lmd_conductor_params[7] = cbrdiy;
  lmd_conductor_params[8] = cbrdoy;

  lmd_conductor_params[9] = tbrdiz;
  lmd_conductor_params[10] = tbrdiy;
  lmd_conductor_params[11] = tbrdoy;

  TGeoPcon *lmd_cond_ring = new TGeoPcon(lmd_conductor_params);
  TGeoVolume *vlum_cond_ring = new TGeoVolume("vlum_cond_ring", lmd_cond_ring,
                                              fgGeoMan->GetMedium("steel")); // should be brass
  vlum_cond_ring->SetLineColor(kYellow);                                     // 39);
  TGeoCombiTrans *lmd_trans_lmd_cond_ring = new TGeoCombiTrans("lmd_trans_lmd_cond_ring", 0., 0., box_inner_up_z, rot_no);
  lmd_trans_lmd_cond_ring->RegisterYourself();
  lmd_vol_vac->AddNode(vlum_cond_ring, 0, lmd_trans_lmd_cond_ring);
  // beam pipe to shield the sensors
  //	TGeoTube* lmd_beam_pipe = new TGeoTube("lmd_beam_pipe", 3.5, 3.6, 50./2.);
  //	TGeoCombiTrans* lmd_trans_p = new TGeoCombiTrans("lmd_trans_p", 0., 0., 50.+23.386+50./2., r1);
  //	lmd_trans_p->RegisterYourself();
  // beam pipe cone downstream
  //	TGeoCone* lmd_cone_downstr = new TGeoCone("lmd_cone_downstr", 20./2., 3.5, 3.7, 9./2., 9.2/2.);
  //	TGeoCombiTrans* lmd_trans_co_do = new TGeoCombiTrans("lmd_trans_co_do", 0., 0., 50.+23.386+50.+20./2., r1);
  //	lmd_trans_co_do->RegisterYourself();
  // beam pipe downstream
  //	TGeoTube* lmd_beam_pipe_downstream = new TGeoTube("lmd_beam_pipe_downstream", 9./2., 9.2/2., 56./2.);
  //	TGeoCombiTrans* lmd_trans_p_down = new TGeoCombiTrans("lmd_trans_p_down", 0., 0., 50.+23.386+50.+20.+56./2., r1);
  //	lmd_trans_p_down->RegisterYourself();*/

  // ****************************** luminosity detector reference system ************************
  // definition of the luminosity detector local system
  TGeoCombiTrans *rottrans_lmd_in_box = new TGeoCombiTrans("rottrans_lmd_in_box", 0., 0., pos_plane_0, rot_no);
  TGeoVolumeAssembly *lmd_vol_ref_sys = new TGeoVolumeAssembly(nav_paths[1].c_str());
  // definition of the retractable luminosity detector halves

  // ****************************** cvd cooling support structure ********************

  // According to Heinrich's drawings from 07.03.14
  double lmd_cool_sup_inner_rad = 10.6;
  double lmd_cool_sup_outer_rad = 16.;
  double lmd_cool_sup_outer_cut = 14.;
  double lmd_cool_sup_thick = 1.; // half of it

  // construct first a tube
  new TGeoTube("shape_cool_sup_tube", lmd_cool_sup_inner_rad, lmd_cool_sup_outer_rad, lmd_cool_sup_thick); // TGeoTube* shape_cool_sup_tube =  //[R.K.03/2017] unused variable
  // to cut off a half + a little bit
  new TGeoBBox("shape_cool_sup_cut", lmd_cool_sup_outer_rad, lmd_cool_sup_outer_rad, lmd_cool_sup_thick + 0.1); // TGeoBBox* shape_cool_sup_cut =  //[R.K.03/2017] unused variable
  // set the position for the cut off
  TGeoTranslation *trans_shape_cool_sup_cut_low = new TGeoTranslation("trans_shape_cool_sup_cut_low", 0., -lmd_cool_sup_outer_rad + 0.5,
                                                                      0.); // 0.5 should be 0.3 but for the sake of simplicity not to clash to simple rod description
  TGeoCombiTrans *combtrans_shape_cool_sup_cut_low = new TGeoCombiTrans(*trans_shape_cool_sup_cut_low, *rot_no);
  combtrans_shape_cool_sup_cut_low->SetName("combtrans_shape_cool_sup_cut_low");
  combtrans_shape_cool_sup_cut_low->RegisterYourself();

  TGeoTranslation *trans_shape_cool_sup_cut_high = new TGeoTranslation("trans_shape_cool_sup_cut_high", 0., +lmd_cool_sup_outer_rad - 0.5,
                                                                       0.); // 0.5 should be 0.3 but for the sake of simplicity not to clash to simple rod description
  TGeoCombiTrans *combtrans_shape_cool_sup_cut_high = new TGeoCombiTrans(*trans_shape_cool_sup_cut_high, *rot_no);
  combtrans_shape_cool_sup_cut_high->SetName("combtrans_shape_cool_sup_cut_high");
  combtrans_shape_cool_sup_cut_high->RegisterYourself();

  // We need some cut outs for the modules and the outer structure
  // we give them a little bit more space for misalignment studies without clashing volumes
  new TGeoTube("shape_module_cutout", 0., cvd_disc_rad + 0.05,
               cvd_disc_thick_half + 2 * kapton_disc_thick_half + 0.01); // TGeoTube* shape_module_cutout =  //[R.K.03/2017] unused variable
  for (size_t imodule = 0; imodule < nmodules * 2 /*upper and lower half*/; imodule++) {
    double angle = delta_phi / 2. + imodule * delta_phi;
    double add_z = cvd_disc_even_odd_offset;
    // the offset of the modules in the upper and lower halfs
    // are opposite
    if (((imodule) % 2) == 0)
      add_z = -add_z;
    double _x = cos(angle) * cvd_disc_dist;
    double _y = sin(angle) * cvd_disc_dist;
    TGeoTranslation *trans_shape_module_cutout = new TGeoTranslation(_x, _y, add_z);
    TGeoCombiTrans *combtrans_shape_module_cutout = new TGeoCombiTrans(*trans_shape_module_cutout, *rot_no);
    stringstream _cutout_name;
    _cutout_name << "rottrans_cutout_" << imodule;
    combtrans_shape_module_cutout->SetName(_cutout_name.str().c_str());
    combtrans_shape_module_cutout->RegisterYourself();

    _x = cos(angle) * (lmd_cool_sup_outer_cut + lmd_cool_sup_outer_rad);
    _y = sin(angle) * (lmd_cool_sup_outer_cut + lmd_cool_sup_outer_rad);
    TGeoTranslation *trans_cool_sup_cut_outer = new TGeoTranslation(_x, _y, 0.);
    stringstream _cutshape_rot_name;
    _cutshape_rot_name << "cutshaperot_" << imodule;
    TGeoRotation *rot_cool_sup_cut_outer = new TGeoRotation(_cutshape_rot_name.str().c_str(), angle / pi * 180., 0., 0.);
    TGeoCombiTrans *combtrans_cool_sup_cut_outer = new TGeoCombiTrans(*trans_cool_sup_cut_outer, *rot_cool_sup_cut_outer);
    stringstream _cutshape_name;
    _cutshape_name << "cutshape_" << imodule;
    combtrans_cool_sup_cut_outer->SetName(_cutshape_name.str().c_str());
    combtrans_cool_sup_cut_outer->RegisterYourself();
  }

  // construct the support from basic shape and it's cut outs
  TGeoCompositeShape *shape_cool_sup_up = new TGeoCompositeShape("shape_cool_sup_up", "shape_cool_sup_tube-shape_cool_sup_cut:combtrans_shape_cool_sup_cut_low"
                                                                                      "-shape_module_cutout:rottrans_cutout_0"
                                                                                      "-shape_module_cutout:rottrans_cutout_1"
                                                                                      "-shape_module_cutout:rottrans_cutout_2"
                                                                                      "-shape_module_cutout:rottrans_cutout_3"
                                                                                      "-shape_module_cutout:rottrans_cutout_4"
                                                                                      "-shape_cool_sup_cut:cutshape_0"
                                                                                      "-shape_cool_sup_cut:cutshape_1"
                                                                                      "-shape_cool_sup_cut:cutshape_2"
                                                                                      "-shape_cool_sup_cut:cutshape_3"
                                                                                      "-shape_cool_sup_cut:cutshape_4");

  TGeoVolume *lmd_vol_cool_sup_up = new TGeoVolume("lmd_vol_cool_sup_up", shape_cool_sup_up, fgGeoMan->GetMedium("Aluminum"));
  lmd_vol_cool_sup_up->SetLineColor(17);

  // the lower cooling support has to be rotated, we create simply a new volume for it
  TGeoRotation *rot_shape_cool_sup_down = new TGeoRotation("rot_shape_cool_sup_down", 180., 180., 0.);
  TGeoCombiTrans *combtrans_shape_cool_sup_down = new TGeoCombiTrans(*trans_no, *rot_shape_cool_sup_down);
  combtrans_shape_cool_sup_down->SetName("combtrans_shape_cool_sup_down");
  combtrans_shape_cool_sup_down->RegisterYourself();
  // construct the support from basic shape and it's cut outs
  TGeoCompositeShape *shape_cool_sup_down = new TGeoCompositeShape("shape_cool_sup_down", "shape_cool_sup_tube-shape_cool_sup_cut:combtrans_shape_cool_sup_cut_high"
                                                                                          "-shape_module_cutout:rottrans_cutout_5"
                                                                                          "-shape_module_cutout:rottrans_cutout_6"
                                                                                          "-shape_module_cutout:rottrans_cutout_7"
                                                                                          "-shape_module_cutout:rottrans_cutout_8"
                                                                                          "-shape_module_cutout:rottrans_cutout_9"
                                                                                          "-shape_cool_sup_cut:cutshape_5"
                                                                                          "-shape_cool_sup_cut:cutshape_6"
                                                                                          "-shape_cool_sup_cut:cutshape_7"
                                                                                          "-shape_cool_sup_cut:cutshape_8"
                                                                                          "-shape_cool_sup_cut:cutshape_9");

  TGeoVolume *lmd_vol_cool_sup_down = new TGeoVolume("lmd_vol_cool_sup_down", shape_cool_sup_down, fgGeoMan->GetMedium("Aluminum"));
  lmd_vol_cool_sup_down->SetLineColor(17);

  // ****************************** cvd cooling support discs ************************
  // the cvd disc shape
  double gap_between_disc_and_support_structure(0.025); // 250 mu gap
  TGeoTubeSeg *shape_cvd_disc = new TGeoTubeSeg("shape_cvd_disc", inner_rad, lmd_cool_sup_inner_rad - gap_between_disc_and_support_structure, cvd_disc_thick_half,
                                                -delta_phi / 2. / pi * 180., +delta_phi / 2. / pi * 180.);

  TGeoRotation *cvd_rotation = new TGeoRotation("cvd_rotation", 0, 0, 0);
  TGeoTranslation *cvd_translation = new TGeoTranslation("cvd_translation", -cvd_disc_dist, 0, 0);
  TGeoCombiTrans *cvd_combtrans = new TGeoCombiTrans(*cvd_translation, *cvd_rotation);
  cvd_combtrans->SetName("cvd_combtrans");
  cvd_combtrans->RegisterYourself();

  // this next line is pretty stupid but it made the work for the better geometry minimal
  // otherwise I would have to do some deeper digging and reworking...
  TGeoCompositeShape *shape_cvd_support = new TGeoCompositeShape("shape_cvd_support", "(shape_cvd_disc:cvd_combtrans+shape_cvd_disc:cvd_combtrans)");

  TGeoVolume *lmd_vol_cvd_disc = new TGeoVolume("lmd_vol_cvd_disc", shape_cvd_support, fgGeoMan->GetMedium("HYPdiamond"));
  lmd_vol_cvd_disc->SetLineColor(9);
  // ****************************** kapton flexible circuits to the sensors ************************
  // the cvd disc shape
  TGeoTubeSeg *shape_kapton_disc = new TGeoTubeSeg("shape_kapton_disc", inner_rad, lmd_cool_sup_inner_rad - gap_between_disc_and_support_structure, kapton_disc_thick_half,
                                                   -delta_phi / 2. / pi * 180., +delta_phi / 2. / pi * 180.);

  TGeoRotation *kapton_rotation = new TGeoRotation("kapton_rotation", 0, 0, 0);
  TGeoTranslation *kapton_translation = new TGeoTranslation("kapton_translation", -cvd_disc_dist, 0, 0);
  TGeoCombiTrans *kapton_combtrans = new TGeoCombiTrans(*kapton_translation, *kapton_rotation);
  kapton_combtrans->SetName("kapton_combtrans");
  kapton_combtrans->RegisterYourself();

  // this next line is pretty stupid but it made the work for the better geometry minimal
  // otherwise I would have to do some deeper digging and reworking...
  TGeoCompositeShape *shape_kapton_support = new TGeoCompositeShape("shape_kapton_support", "(shape_kapton_disc:kapton_combtrans+shape_kapton_disc:kapton_combtrans)");

  TGeoVolume *lmd_vol_kapton_disc =
    new TGeoVolume("lmd_vol_kapton_disc", shape_kapton_support, fgGeoMan->GetMedium("Aluminum")); // kapton")); // changed to equivalent for glue/flex cable etc.
  lmd_vol_kapton_disc->SetLineColor(kRed);
  // lmd_vol_kapton_disc->SetTransparency(50);
  lmd_vol_kapton_disc->SetVisibility(false);
  // *********************************** HV-MAPS *************************************

  // create basic shapes and their positions
  TGeoBBox *shape_maps_active_centered = new TGeoBBox("shape_maps_active_centered", maps_active_width, maps_active_height, maps_thickness);
  TGeoCombiTrans *combtrans_maps_active =
    new TGeoCombiTrans("combtrans_maps_active", -maps_width + maps_passive_left * 2. + maps_active_width, -maps_height + maps_passive_bottom * 2. + maps_active_height, 0., rot_no);
  combtrans_maps_active->RegisterYourself();
  TGeoBBox *shape_maps_passive_left = new TGeoBBox("shape_maps_passive_left", maps_passive_left, maps_height, maps_thickness);
  TGeoCombiTrans *combtrans_maps_passive_left = new TGeoCombiTrans("combtrans_maps_passive_left", -maps_width + maps_passive_left, 0., 0., rot_no);
  combtrans_maps_passive_left->RegisterYourself();
  TGeoBBox *shape_maps_passive_right = new TGeoBBox("shape_maps_passive_right", maps_passive_right, maps_height, maps_thickness);
  TGeoCombiTrans *combtrans_maps_passive_right = new TGeoCombiTrans("combtrans_maps_passive_right", maps_width - maps_passive_right, 0., 0., rot_no);
  combtrans_maps_passive_right->RegisterYourself();
  TGeoBBox *shape_maps_passive_top = new TGeoBBox("shape_maps_passive_top", maps_width, maps_passive_top, maps_thickness);
  TGeoCombiTrans *combtrans_maps_passive_top = new TGeoCombiTrans("combtrans_maps_passive_top", 0., maps_height - maps_passive_top, 0., rot_no);
  combtrans_maps_passive_top->RegisterYourself();
  TGeoBBox *shape_maps_passive_bottom = new TGeoBBox("shape_maps_passive_bottom", maps_width, maps_passive_bottom, maps_thickness);
  TGeoCombiTrans *combtrans_maps_passive_bottom = new TGeoCombiTrans("combtrans_maps_passive_bottom", 0., -maps_height + maps_passive_bottom, 0., rot_no);
  combtrans_maps_passive_bottom->RegisterYourself();

  if (!lmd_box_outer || !lmd_box_inner || !lmd_box_rib || !box_hole_upstream || !box_hole_downstream || !shape_cvd_disc || !shape_kapton_disc || !shape_maps_active_centered ||
      !shape_maps_passive_left || !shape_maps_passive_right || !shape_maps_passive_top || !shape_maps_passive_bottom)
    cout << " pedantic compiler together with root geometries sucks " << endl;

  TGeoCompositeShape *shape_maps_passive =
    new TGeoCompositeShape("shape_maps_passive", "(shape_maps_passive_top:combtrans_maps_passive_top+shape_maps_passive_right:combtrans_maps_passive_right+shape_maps_passive_"
                                                 "bottom:combtrans_maps_passive_bottom+shape_maps_passive_left:combtrans_maps_passive_left)");

  TGeoCompositeShape *shape_maps_active = new TGeoCompositeShape("shape_maps_active", "(shape_maps_active_centered:combtrans_maps_active-shape_maps_passive)");

  TGeoVolume *_vol_passive = new TGeoVolume("LumPassiveRect", shape_maps_passive, fgGeoMan->GetMedium("silicon"));
  _vol_passive->SetLineColor(30);

  TGeoVolume *_vol_active = new TGeoVolume(nav_paths[7].c_str(), shape_maps_active, fgGeoMan->GetMedium("silicon"));
  _vol_active->SetLineColor(36);
  // **************************************************************

  // ****************************** loops in the luminosity detector ************************
  stringstream name;
  stringstream uniqueid; // seems pandaroot has problems when volumes are not uniquely named
  double _x(0), _y(0), _z(0), _rotphi(0), _rottheta(0), _rotpsi(0);
  double _offset_x(0), _offset_y(0), _offset_z(0), _offset_phi(0), _offset_theta(0), _offset_psi(0);
  unsigned int sensor_id(0);
  unsigned int module_id(0);
  for (unsigned int ihalf = 0; ihalf < 2; ihalf++) { // loop over detector halves
    // in order do be able to displace the detector halves those are introduced as
    // separate volume assemblies
    name.str("");
    name << nav_paths[2]; // << ihalf;
    uniqueid.str("");
    // uniqueid << "_" << ihalf;
    TGeoVolumeAssembly *lmd_vol_half_ = new TGeoVolumeAssembly(name.str().c_str());
    // mothervol.AddNode(lmd_vol_half_, 0, rottrans_no);
    for (unsigned int iplane = 0; iplane < n_planes; iplane++) { // loop over planes
      name.str("");
      name << nav_paths[3]; // << iplane;
      uniqueid.str("");
      // uniqueid << "_" << ihalf << iplane;
      TGeoVolumeAssembly *lmd_vol_plane_ = new TGeoVolumeAssembly((name.str() + uniqueid.str()).c_str());
      // move to the position of the corresponding plane
      TGeoMatrix *rottrans_plane = new TGeoCombiTrans(0., 0., plane_pos_z[iplane], rot_no);
      if (misaligned) {
        Get_offset(ihalf, iplane, -1, -1, -1, -1, _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi, true);
        TGeoRotation *rot_plane_offset = new TGeoRotation("rot_plane_offset", _offset_phi / pi * 180., _offset_theta / pi * 180., _offset_psi / pi * 180.);
        TGeoCombiTrans *rottrans_plane_offset = new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_plane_offset);
        //	rottrans_plane = new TGeoHMatrix(*rottrans_plane * *rottrans_plane_offset);
        rottrans_plane = new TGeoHMatrix(*rottrans_plane_offset * *rottrans_plane);
      }
      for (unsigned int imodule = 0; imodule < nmodules; imodule++) { // loop over modules
        name.str("");
        name << nav_paths[4]; // << imodule;
        uniqueid.str("");
        // uniqueid << "_" << ihalf << iplane << imodule;
        TGeoVolumeAssembly *lmd_vol_module_ = new TGeoVolumeAssembly((name.str() + uniqueid.str()).c_str());
        double angle = delta_phi / 2. + ihalf * pi + imodule * delta_phi;
        double add_z = cvd_disc_even_odd_offset;
        // the offset of the modules in the upper and lower halfs
        // are opposite
        if (((imodule + ihalf) % 2) == 0)
          add_z = -add_z;
        _x = cos(angle) * cvd_disc_dist;
        _y = sin(angle) * cvd_disc_dist;
        _z = add_z;
        _rotphi = 0.;
        _rottheta = 0.;
        _rotpsi = angle / pi * 180.;
        TGeoRotation *rot_module = new TGeoRotation("rot_module", _rotphi, _rottheta, _rotpsi);
        TGeoMatrix *rottrans_module = new TGeoCombiTrans(_x, _y, _z, rot_module);
        if (misaligned) {
          Get_offset(ihalf, iplane, imodule, -1, -1, -1, _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi, true);
          TGeoRotation *rot_module_offset = new TGeoRotation("rot_module_offset", _offset_phi / pi * 180., _offset_theta / pi * 180., _offset_psi / pi * 180.);
          TGeoCombiTrans *rottrans_module_offset = new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_module_offset);
          rottrans_module = new TGeoHMatrix(*rottrans_module_offset * *rottrans_module);
        }
        /*
         if (ihalf == 0 && iplane == 2 && imodule == 3){
         _x += 2.;
         _y += 1.;
         _z += -5.;
         _rotphi += 20.;
         _rottheta += 70.;
         }*/
        // add the cvd disc into that assembly
        // TGeoVolume* lmd_vol_cvd_disc = new TGeoVolume("lmd_vol_cvd_disc",
        //						shape_cvd_support, fgGeoMan->GetMedium("HYPdiamond"));
        lmd_vol_module_->AddNode(lmd_vol_cvd_disc, module_id, rottrans_no);
        module_id++;
        for (unsigned int iside = 0; iside < 2; iside++) { // loop over the two sides of the modules
          name.str("");
          name << nav_paths[5]; // << iside;
          uniqueid.str("");
          // uniqueid << "_" << ihalf << iplane << imodule << iside;
          TGeoVolumeAssembly *lmd_vol_side_ = new TGeoVolumeAssembly((name.str() + uniqueid.str()).c_str());
          // rotation around the y axis for the upstream side
          // side 0 is downstream! what may be not so obvious
          _x = 0.;
          _y = 0.;
          _z = 0.;
          if (iside == 0) {
            _rotphi = 0.;
            _rottheta = 0.;
            _rotpsi = 0.;
          } else {
            _rotphi = 0.;
            _rottheta = 180.;
            _rotpsi = 0.;
          }
          TGeoRotation *rot_side = new TGeoRotation("rot_side", _rotphi, _rottheta, _rotpsi);
          TGeoMatrix *rottrans_side = new TGeoCombiTrans(_x, _y, _z, rot_side);
          if (misaligned) {
            Get_offset(ihalf, iplane, imodule, iside, -1, -1, _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi, true);
            TGeoRotation *rot_side_offset = new TGeoRotation("rot_side_offset", _offset_phi / pi * 180., _offset_theta / pi * 180., _offset_psi / pi * 180.);
            TGeoCombiTrans *rottrans_side_offset = new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_side_offset);
            rottrans_side = new TGeoHMatrix(*rottrans_side_offset * *rottrans_side);
          }
          // glue the HV-MAPS to the cvd surface
          for (unsigned int idie = 0; idie < 2; idie++) { // loop over dies
            // it seems we will have only 2 sensors on a die and only a few of them
            // -> term die will stay as it is, but may be different from the reality
            name.str("");
            name << nav_paths[6]; // << idie;
            uniqueid.str("");
            // uniqueid << "_" << ihalf << iplane << imodule << iside << idie;
            TGeoVolumeAssembly *lmd_vol_die_ = new TGeoVolumeAssembly((name.str() + uniqueid.str()).c_str());
            // rotation to the cut side of the cvd_disc
            // the origin is the inner edge
            const double _sinhalf = sin(delta_phi / 2.);
            // const double _coshalf = cos (delta_phi/2.);
            const double _edge_y = +_sinhalf * inner_rad;
            // angle between the edge and the half of the cvd disc
            const double _edgeangle = asin(-_edge_y / cvd_disc_rad);
            const double _edge_x = -cvd_disc_rad * cos(_edgeangle);
            _x = _edge_x;
            _y = _edge_y;
            _z = +cvd_disc_thick_half + maps_thickness; // move to the surface
            _rotphi = +delta_phi / 2. / pi * 180.;
            _rottheta = 0.;
            _rotpsi = 0.; // rotate to the cut edge
            TGeoRotation *rot_die = new TGeoRotation("rot_die", _rotphi, _rottheta, _rotpsi);
            TGeoCombiTrans *rottrans_die = new TGeoCombiTrans(_x, _y, _z, rot_die);
            // construct now the sensors on the two dies
            for (unsigned int isensor = 0; isensor < 3; isensor++) { // loop over sensors
              // the 0th die is oriented at the inner edge
              _x = +maps_width + maps_width * 2. * isensor;
              _y = -maps_height;
              _z = 0.;
              // only the inner two sensors will possibly on one physical die
              // so take an offset into account
              if (isensor == 2)
                _x = _x + die_gap;
              // next row
              if (idie == 1) {
                if (isensor == 0)
                  continue;
                _y -= die_gap + 2. * maps_height;
                _rotphi = 0.;
              } else {
                _rotphi = 0.;
              }
              _rottheta = 0.;
              _rotpsi = 0.;
              //"LumActiveRect" is the keyword for digitization of hits
              /*
               name.str("");
               name << nav_paths[7] << isensor;
               uniqueid.str("");
               uniqueid << "_" << ihalf << iplane << imodule << iside << idie << isensor;
               TGeoVolume* _vol_active =
               new TGeoVolume(
               (name.str()+uniqueid.str()).c_str(),
               shape_maps_active,
               fgGeoMan->GetMedium("silicon"));
               _vol_active->SetLineColor(36);
               */
              /*
               name.str("");
               name << "LumPassiveRect_" << isensor;
               TGeoVolume* _vol_passive =
               new TGeoVolume(
               (name.str()+uniqueid.str()).c_str(),
               shape_maps_passive,
               fgGeoMan->GetMedium("silicon"));
               _vol_passive->SetLineColor(30);
               */
              TGeoRotation *rot_sensor = new TGeoRotation("rot_sensor", _rotphi, _rottheta, _rotpsi);
              TGeoMatrix *rottrans_sensor = new TGeoCombiTrans(_x, _y, _z, rot_sensor);
              if (misaligned) {
                Get_offset(ihalf, iplane, imodule, iside, idie, isensor, _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi, true);
                TGeoRotation *rot_sensor_offset = new TGeoRotation("rot_sensor_offset", _offset_phi / pi * 180., _offset_theta / pi * 180., _offset_psi / pi * 180.);
                TGeoCombiTrans *rottrans_sensor_offset = new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_sensor_offset);
                rottrans_sensor = new TGeoHMatrix(*rottrans_sensor_offset * *rottrans_sensor);
              }
              lmd_vol_die_->AddNode(_vol_active, sensor_id, rottrans_sensor);
              lmd_vol_die_->AddNode(_vol_passive, sensor_id, rottrans_sensor);
              // cout << sensor_id << " " << _vol_active->GetName() << endl << endl;
              // save the transformation from the cvd_side reference frame
              // into the local frame of the sensors
              transformation_matrices[Tkey(ihalf, iplane, imodule, iside, idie, isensor)] = new TGeoHMatrix((*rottrans_die) * (*rottrans_sensor));

              if (0) { // some tests for debugging
                unsigned int _sensor_id = Get_sensor_id(ihalf, iplane, imodule, iside, idie, isensor);
                if (sensor_id != _sensor_id) {
                  cout << " wrong sensor id " << _sensor_id << " != " << sensor_id << endl;
                }
                int _ihalf, _iplane, _imodule, _iside, _idie, _isensor;
                Get_sensor_by_id(sensor_id, _ihalf, _iplane, _imodule, _iside, _idie, _isensor);
                if ((signed)ihalf != _ihalf)
                  cout << " wrong half " << _ihalf << endl;
                if ((signed)iplane != _iplane)
                  cout << " wrong plane " << _iplane << endl;
                if ((signed)imodule != _imodule)
                  cout << " wrong module " << _imodule << endl;
                if ((signed)iside != _iside)
                  cout << " wrong side " << _iside << endl;
                if ((signed)idie != _idie)
                  cout << " wrong die " << _idie << endl;
                if ((signed)isensor != _isensor)
                  cout << " wrong sensor " << _isensor << endl;
              }
              sensor_id++;
            } // loop over sensors
            lmd_vol_side_->AddNode(lmd_vol_die_, idie, rottrans_die);
          } // loop over dies
          _x = 0;
          _y = 0;
          _z = +cvd_disc_thick_half + maps_thickness * 2. + kapton_disc_thick_half; // move to the surface
          _rotphi = 0.;
          _rottheta = 0.;
          _rotpsi = 0.; // rotate to the cut edge
          TGeoRotation *rot_kapton = new TGeoRotation("rot_kapton", _rotphi, _rottheta, _rotpsi);
          TGeoCombiTrans *rottrans_kapton = new TGeoCombiTrans(_x, _y, _z, rot_kapton);
          lmd_vol_side_->AddNode(lmd_vol_kapton_disc, 0, rottrans_kapton); // GGenerate_Tkeyumber(ihalf,iplane, imodule, iside)
          lmd_vol_module_->AddNode(lmd_vol_side_, iside, rottrans_side);
          // save the transformation from the lumi reference frame
          // into the local cvd side reference frame
          transformation_matrices[Tkey(ihalf, iplane, imodule, iside, -1, -1)] = new TGeoHMatrix((*rottrans_plane) * (*rottrans_module) * (*rottrans_side));
        } // loop over the two sides of the modules
        lmd_vol_plane_->AddNode(lmd_vol_module_, imodule, rottrans_module);
        // transformation_matrices[Generate_Tkey(ihalf, iplane, imodule, -1, -1, -1)] =
        //   new TGeoHMatrix((*rottrans_plane) * (*rottrans_module));
      } // loop over modules
      if (ihalf == 0)
        lmd_vol_plane_->AddNode(lmd_vol_cool_sup_up, iplane, rottrans_no);
      else
        lmd_vol_plane_->AddNode(lmd_vol_cool_sup_down, iplane, rottrans_no);
      lmd_vol_half_->AddNode(lmd_vol_plane_, iplane, rottrans_plane);
      // // save the transformation from the lumi reference frame
      // // into the plane reference frame
      // transformation_matrices[Generate_Tkey(ihalf, iplane, -1, -1, -1, -1)] =
      //   new TGeoHMatrix((*rottrans_plane));
    } // loop over planes
    lmd_vol_ref_sys->AddNode(lmd_vol_half_, ihalf, rottrans_no);
    // save the transformation into the lumi reference frame
    transformation_matrices[Tkey(-1, -1, -1, -1, -1, -1)] = new TGeoHMatrix((*lmd_transrot) * (*rottrans_lmd_in_box));
  } // loop over detector halves
  // code geometry version in the node title
  stringstream nodetitle;
  nodetitle << "version " << geometry_version << endl;
  lmd_vol_vac->SetTitle(nodetitle.str().c_str());
  lmd_vol_vac->AddNode(lmd_vol_ref_sys, 0, rottrans_lmd_in_box);
  mothervol.AddNode(lmd_vol_vac, geometry_version, lmd_transrot);

  /*
   gGeoMan->CloseGeometry();
   //gGeoMan->Get
   //cout << gGeoMan->InitTrack(0,30,1050,0,0,1)->GetName() << endl;
   const double* current_point = gGeoMan->GetCurrentPoint();
   cout << current_point[0] << " " << current_point[1] << " " << current_point[2] << endl;
   double new_point[3] = {8.7, 1., 29.7375};
   gGeoMan->SetCurrentPoint(new_point);
   const double* new_current_point = gGeoMan->GetCurrentPoint();
   cout << new_current_point[0] << " " << new_current_point[1] << " " << new_current_point[2] << endl;
   cout << gGeoMan->GetCurrentNode()->GetName() << endl;
   gGeoMan->FindNode();
   cout << gGeoMan->GetCurrentNode()->GetName() << endl;

   string path = "/lmd_HV_MAPS_1/vol_lmd_vac_0/Lumi_HV-MAPS_0/vol_LumPassive_cvd_disc_plane_1_disc_1_0";
   gGeoMan->cd(path.c_str());///LumActivePixelRect_plane_0_disc_0_side_0_col_0_row_0_0");
   cout << gGeoMan->GetPath()<< endl;
   TGeoPhysicalNode* node =
   gGeoMan->MakePhysicalNode((path + "/LumActivePixelRect_plane_0_disc_0_side_0_col_0_row_0_0").c_str());///Lumi_HV-MAPS/LumPassive_cvd_disc_plane_2_disc_3");
   if (node){
   //TGeoCombiTrans* align_trans = new TGeoCombiTrans(1.,0.,0.,georot_no);
   //node->Align(align_trans);
   //gGeoMan->CloseGeometry();
   TGeoHMatrix* matrix = node -> GetMatrix();
   matrix -> Print();
   }
   //gGeoMan->Set
   */
}

// void PndLmdDim::Create_transformation_matrices_aligned(string filename_in, string filename_out){
void PndLmdDim::reCreate_transformation_matrices()
{
  // no translation nor rotation
  TGeoRotation *rot_no = new TGeoRotation("rot_no", 0., 0., 0.); // no rotation
  // TGeoCombiTrans* rottrans_no = new TGeoCombiTrans("rottrans_no", 0., 0.,
  //			0., rot_no);

  double _x(0), _y(0), _z(0), _rotphi(0), _rottheta(0), _rotpsi(0);
  double _offset_x(0), _offset_y(0), _offset_z(0), _offset_phi(0), _offset_theta(0), _offset_psi(0);

  for (unsigned int ihalf = 0; ihalf < 2; ihalf++) {             // loop over detector halves
    for (unsigned int iplane = 0; iplane < n_planes; iplane++) { // loop over planes
      // move to the position of the corresponding plane
      TGeoMatrix *rottrans_plane = new TGeoCombiTrans(0., 0., plane_pos_z[iplane], rot_no);

      // Get_offset(ihalf, iplane, -1, -1, -1, -1,
      // 	   _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
      // TGeoRotation* rot_plane_offset = new TGeoRotation("rot_plane_offset",
      // 						  _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
      // TGeoCombiTrans* rottrans_plane_offset =
      //   new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_plane_offset);
      // //	rottrans_plane = new TGeoHMatrix(*rottrans_plane * *rottrans_plane_offset);
      // rottrans_plane = new TGeoHMatrix(*rottrans_plane_offset * *rottrans_plane);

      for (unsigned int imodule = 0; imodule < nmodules; imodule++) { // loop over modules
        double angle = delta_phi / 2. + ihalf * pi + imodule * delta_phi;
        double add_z = cvd_disc_even_odd_offset;
        // the offset of the modules in the upper and lower halfs
        // are opposite
        if (((imodule + ihalf) % 2) == 0)
          add_z = -add_z;
        _x = cos(angle) * cvd_disc_dist;
        _y = sin(angle) * cvd_disc_dist;
        _z = add_z;
        _rotphi = 0.;
        _rottheta = 0.;
        _rotpsi = angle / pi * 180.;
        TGeoRotation *rot_module = new TGeoRotation("rot_module", _rotphi, _rottheta, _rotpsi);
        TGeoMatrix *rottrans_module = new TGeoCombiTrans(_x, _y, _z, rot_module);
        // Set_offset(ihalf, iplane, imodule, -1, -1, -1,
        // 	   _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
        Get_offset(ihalf, iplane, imodule, -1, -1, -1, _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
        // TGeoRotation* rot_module_offset = new TGeoRotation("rot_module_offset",
        // 						   _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
        //	cout<<"_offset_x: "<<_offset_x<<" cm"<<endl;
        TGeoRotation *rot_module_offset = new TGeoRotation("rot_module_offset", 0, 0, 0);
        // // ///TEST
        // rot_module_offset->RotateZ(-_offset_psi/pi*180.);
        // rot_module_offset->RotateY(_offset_theta/pi*180.);
        // rot_module_offset->RotateX(-_offset_phi/pi*180.);

        rot_module_offset->RotateZ(_offset_psi / pi * 180.);
        rot_module_offset->RotateY(_offset_theta / pi * 180.);
        rot_module_offset->RotateX(_offset_phi / pi * 180.);
        //	rot_module_offset->Print();

        //	cout<<"_rot_x_theta: "<<_offset_phi/pi*180.<<" degree"<<endl;
        TGeoCombiTrans *rottrans_module_offset = new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_module_offset);
        //	rottrans_module = new TGeoHMatrix(*rottrans_module_offset * *rottrans_module);
        rottrans_module = new TGeoHMatrix(*rottrans_module * *rottrans_module_offset);
        for (int iside = 0; iside < 2; iside++) { // loop over the two sides of the modules
          // rotation around the y axis for the downstream side
          _x = 0.;
          _y = 0.;
          _z = 0.;
          if (iside == 0) {
            _rotphi = 0.;
            _rottheta = 0.;
            _rotpsi = 0.;
          } else {
            _rotphi = 0.;
            _rottheta = 180.;
            _rotpsi = 0.;
          }
          TGeoRotation *rot_side = new TGeoRotation("rot_side", _rotphi, _rottheta, _rotpsi);
          TGeoMatrix *rottrans_side = new TGeoCombiTrans(_x, _y, _z, rot_side);

          // Get_offset(ihalf, iplane, imodule, iside, -1, -1,
          // 	       _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
          // TGeoRotation* rot_side_offset = new TGeoRotation("rot_side_offset",
          // 						     _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
          // TGeoCombiTrans* rottrans_side_offset =
          //   new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_side_offset);
          // rottrans_side = new TGeoHMatrix(*rottrans_side_offset * *rottrans_side);
          // save the transformation from the lumi reference frame
          // into the local cvd side reference frame
          transformation_matrices[Tkey(ihalf, iplane, imodule, iside, -1, -1)] = new TGeoHMatrix((*rottrans_plane) * (*rottrans_module) * (*rottrans_side));
          // transformation_matrices_aligned[Generate_Tkey(ihalf, iplane, imodule, iside, -1, -1)] =
          //  new TGeoHMatrix((*rottrans_plane) * (*rottrans_module) * (*rottrans_side)); //TEST

        } // loop over the two sides of the modules
          // transformation_matrices[Generate_Tkey(ihalf, iplane, imodule, -1, -1, -1)] =
          //       new TGeoHMatrix((*rottrans_plane) * (*rottrans_module));
      }   // loop over modules
    }     // loop over planes
          // // save the transformation into the lumi reference frame
          // transformation_matrices[Generate_Tkey(-1, -1, -1, -1, -1, -1)] = new TGeoHMatrix((*lmd_transrot) * (*rottrans_lmd_in_box));
  }       // loop over detector halves
}

void PndLmdDim::Correct_transformation_matrices()
{
  // Read_transformation_matrices(filename_in,true);//read initial matrices

  // ****************************** loops in the luminosity detector ************************
  double _offset_x(0), _offset_y(0), _offset_z(0), _offset_phi(0), _offset_theta(0), _offset_psi(0);
  for (unsigned int ihalf = 0; ihalf < 2; ihalf++) { // loop over detector halves
    // TODO: "Transformation matrix not existent!" =
    // TGeoMatrix* rottrans_half = Get_matrix(ihalf, -1, -1 , -1, -1, -1,  false);
    // Get_offset(ihalf, -1, -1, -1, -1, -1,
    // 	     _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
    // TGeoRotation* rot_half_offset = new TGeoRotation("rot_half_offset",
    // 						     _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
    // TGeoCombiTrans* rottrans_half_offset =
    //   new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_half_offset);
    // rottrans_half = new TGeoHMatrix(*rottrans_half_offset * *rottrans_half);

    for (unsigned int iplane = 0; iplane < n_planes; iplane++) { // loop over planes
      // TODO: "Transformation matrix not existent!" =
      //   cout<<"Correct matrix plane #"<<iplane<<endl;
      //   TGeoMatrix* rottrans_plane = Get_matrix(ihalf, iplane, -1 , -1, -1, -1,  false);
      // Get_offset(ihalf, iplane, -1, -1, -1, -1,
      // 	       _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
      // TGeoRotation* rot_plane_offset = new TGeoRotation("rot_plane_offset",
      // 						     _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
      // TGeoCombiTrans* rottrans_plane_offset =
      //   new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_plane_offset);
      // rottrans_plane = new TGeoHMatrix(*rottrans_plane_offset * *rottrans_plane);

      for (unsigned int imodule = 0; imodule < nmodules; imodule++) { // loop over modules
        // TODO: "Transformation matrix not existent!" =
        // cout<<"Correct matrix module #"<<imodule<<" "<<ihalf<<iplane<<imodule<<"-1-1-1"<<endl;
        // TGeoMatrix* rottrans_module = Get_matrix(ihalf, iplane, imodule, -1, -1, -1,  false);
        // Get_offset(ihalf, iplane, imodule, -1, -1, -1,
        // 		 _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
        // cout<<"offsets:"<<_offset_x<<" "<<_offset_y<<" "<<_offset_z<<" "<<_offset_phi<<" "<<_offset_theta<<" "<<_offset_psi<<endl;
        // TGeoRotation* rot_module_offset = new TGeoRotation("rot_module_offset",
        // 							 _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
        // TGeoCombiTrans* rottrans_module_offset =
        // 	new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_module_offset);
        // rottrans_module = new TGeoHMatrix(*rottrans_module_offset * *rottrans_module);
        // //   rottrans_module = new TGeoHMatrix(*rottrans_module * *rottrans_module_offset);
        for (unsigned int iside = 0; iside < 2; iside++) { // loop over the two sides of the modules
          TGeoMatrix *rottrans_side = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, false);
          Get_offset(ihalf, iplane, imodule, iside, -1, -1, _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
          // cout<<"offsets:"<<_offset_x<<" "<<_offset_y<<" "<<_offset_z<<" "<<_offset_phi<<" "<<_offset_theta<<" "<<_offset_psi<<endl;
          TGeoRotation *rot_side_offset = new TGeoRotation("rot_side_offset", _offset_phi / pi * 180., _offset_theta / pi * 180., _offset_psi / pi * 180.);
          TGeoCombiTrans *rottrans_side_offset = new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_side_offset);
          rottrans_side = new TGeoHMatrix(*rottrans_side_offset * *rottrans_side);
          //	  rottrans_side = new TGeoHMatrix(*rottrans_side * *rottrans_side_offset);

          // // save the transformation from the lumi reference frame
          // // into the local cvd side reference frame
          // transformation_matrices[Generate_Tkey(ihalf, iplane, imodule, iside, -1, -1)] =
          //   new TGeoHMatrix((*rottrans_plane) * (*rottrans_module) * (*rottrans_side));

          // save the transformation from the lumi reference frame
          // into the local cvd side reference frame
          // transformation_matrices[Generate_Tkey(ihalf, iplane, imodule, iside, -1, -1)] =
          //   new TGeoHMatrix((*rottrans_side)); //TODO: ????
          transformation_matrices_aligned[Tkey(ihalf, iplane, imodule, iside, -1, -1)] = new TGeoHMatrix((*rottrans_side)); // TEST

        } // loop over the two sides of the modules
          // transformation_matrices[Generate_Tkey(ihalf, iplane, imodule, -1, -1, -1)] =
          // 	new TGeoHMatrix((*rottrans_plane) * (*rottrans_module)); //TODO: ????
      }   // loop over modules
    }     // loop over planes
          // // save the transformation into the lumi reference frame
          // transformation_matrices[Generate_Tkey(-1, -1, -1, -1, -1, -1)] = new TGeoHMatrix((*lmd_transrot) * (*rottrans_lmd_in_box));
  }       // loop over detector halves
}

#include <fstream>

void PndLmdDim::Read_transformation_matrices(string filename, bool aligned, int version_number)
{
  Retrieve_version_number();
  map<Tkey, TGeoMatrix *> *matrices = nullptr;
  if (aligned) {
    matrices = &transformation_matrices_aligned;
  } else {
    matrices = &transformation_matrices;
  }
  // kill existing matrices
  for (it_transformation_matrices = matrices->begin(); it_transformation_matrices != matrices->end(); it_transformation_matrices++) {
    delete (it_transformation_matrices->second);
  }
  matrices->clear();

  string dir = getenv("VMCWORKDIR");
  if (filename == "") {
    filename = dir + "/geometry/trafo_matrices_lmd.dat";
  } else {
    filename = dir + filename;
  }

  ifstream file(filename.c_str());
  int matrices_counter(0);
  if (file.is_open()) {
    // read the first line to get additional information
    // in case of no (backward compatibility) seek to the first line
    string line0;
    getline(file, line0);
    // first the version info is expected by "version " followed by an integer
    string keyword = "version ";
    if (line0.compare(0, 8, keyword) != 0) {
      // jump to beginning
      cout << " **** Warning in PndLmdDim: the input file " << filename << " has no version! **** " << endl;
      file.seekg(0, file.beg);
    } else {
      int _version = 0;
      istringstream inputstream0(&line0[8]);
      inputstream0 >> _version;
      if (_version != version_number) {
        cout << " **** Warning in PndLmdDim: the version " << _version << "  in " << filename << " does not match the geometry version " << version_number << "! **** " << endl;
      }
    }
    while (1) {
      // read the key
      string key;
      getline(file, key);
      // cout << "next key " << key << endl;
      // char newline;
      if (file.good()) {
        // generate rotation and translation matrices with it
        // error treatment is not foreseen yet
        string line1;
        getline(file, line1);
        istringstream inputstream1(line1);
        double translation[3];
        for (int i = 0; i < 3; i++) {
          inputstream1 >> translation[i];
        }
        string line2;
        getline(file, line2);
        istringstream inputstream2(line2);
        double rotation[9];
        for (int i = 0; i < 9; i++) {
          inputstream2 >> rotation[i];
        }
        TGeoRotation *rot = new TGeoRotation("rotation");
        rot->SetMatrix(rotation);
        TGeoCombiTrans *rottrans = new TGeoCombiTrans(translation[0], translation[1], translation[2], rot);
        (*matrices)[Tkey(key)] = rottrans;
        matrices_counter++;
      } else {
        file.close();
        break;
      }
    }
    file.close();
    cout << " Read " << matrices_counter << " matrices from " << filename << endl;
  } else {
    cout << " Error in PndLmdDim::Read_transformation_matrices: could not read from " << filename << endl;
  }
}

TGeoHMatrix *PndLmdDim::Get_matrix(string path, bool aligned, int ihalf, int iplane, int imodule, int iside, int idie, int isensor)
{
  TGeoHMatrix *result = nullptr;
  if (!gGeoManager->CheckPath(path.c_str())) {
    cout << " Error in PndLmdDim::Get_matrix: path " << path << " is not valid " << endl;
    return result;
  }
  if (!aligned) { // try to obtain the original matrix, if available
    TGeoPNEntry *pne;
    // TGeoHMatrix *ide;  // ideal
    // TGeoHMatrix *mis;  // misaligned
    string uname = Generate_key(ihalf, iplane, imodule, iside, idie, isensor);
    pne = gGeoManager->GetAlignableEntry(uname.c_str());
    if (pne) {
      TGeoPhysicalNode *node = pne->GetPhysicalNode(); // new TGeoPhysicalNode(path.c_str());//
      if (node) {
        result = new TGeoHMatrix(*node->GetOriginalMatrix());
        // if (aligned) result = new TGeoHMatrix(*node->GetNode(node->GetLevel())->GetMatrix());
        return result;
      } else {
        cout << " no node found in pn entry at " << path << endl;
        cout << " obtaining default matrix " << endl;
      }
    } else {
      cout << " no pn entry (alignable node) found in " << path << endl;
      cout << " obtaining default matrix " << endl;
    }
  }
  // I don't care if it was aligned or not, use the last one
  gGeoManager->cd(path.c_str());
  TGeoMatrix *matrix = gGeoManager->GetCurrentNode()->GetMatrix();
  if (!matrix)
    return nullptr;
  result = new TGeoHMatrix(*(matrix));
  // result->Print();
  // if (result){
  //	cout << " found a transformation matrix for " << path << endl;
  //	result->Print();
  //	return result;
  //}

  //
  // if (!pne){
  //	cout << " no pn entry found in " << path << endl;
  //	return 0;
  //}
  // TGeoPhysicalNode *node = new TGeoPhysicalNode(path.c_str());//pne->GetPhysicalNode();
  // if (!node){
  //	cout << " no node found in pn entry at " << path << endl;
  //	return 0;
  //}
  // result = new TGeoHMatrix(*node->GetOriginalMatrix());
  // if (aligned) result = new TGeoHMatrix(*node->GetNode(node->GetLevel())->GetMatrix());
  // else result = new TGeoHMatrix(*node->GetOriginalMatrix());
  // if (!result){
  //	cout << " warning: no matrix found for the path " << path << endl;
  //}
  // delete node;
  return result;
}

bool PndLmdDim::Set_matrix(string path, TGeoHMatrix *matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor)
{
  if (!matrix)
    return false;
  if (!gGeoManager->CheckPath(path.c_str())) {
    cout << " Error in PndLmdDim::Set_matrix: path " << path << " is not valid " << endl;
    return false;
  }
  TGeoPNEntry *pne;
  TGeoPhysicalNode *node;
  string uname = Generate_key(ihalf, iplane, imodule, iside, idie, isensor);
  pne = gGeoManager->GetAlignableEntry(uname.c_str());
  if (!pne) {
    cout << " creating alignable entry for " << path << endl;
    if (isensor > 0) {
      int sensID = Get_sensor_id(ihalf, iplane, imodule, iside, idie, isensor);
      pne = gGeoManager->SetAlignableEntry(uname.c_str(), path.c_str(), sensID);
    } else {
      pne = gGeoManager->SetAlignableEntry(uname.c_str(), path.c_str());
    }
    if (pne) {
      node = new TGeoPhysicalNode(path.c_str());
      pne->SetPhysicalNode(node);
    }
  }
  if (!pne) {
    cout << " Error: no pn entry (alignable node) at " << path << " created " << endl;
    return false;
  }
  node = pne->GetPhysicalNode();
  if (!node) {
    cout << " no node found for pn entry (alignable node) at " << path << endl;
    return false;
  }
  node->Align(matrix); // GetNode(node->GetLevel())->Align();
  // cout << " matrix at " << path << " successfully aligned " << endl;
  /*
   if (aligned) return new TGeoHMatrix(*node->GetNode(node->GetLevel())->GetMatrix());
   else return new TGeoHMatrix(*node->GetOriginalMatrix());	*/
  return true;
}

bool PndLmdDim::Read_transformation_matrices_from_geometry(bool aligned)
{
  if (!Retrieve_version_number()) {
    return false;
  }
  if (geometry_version < 3) {
    cout << " *** Error in PndLmdDim::Read_transformation_matrices_from_geometry:" << endl;
    cout << " geometry version " << geometry_version << " is not compatible with this method! " << endl;
    return false;
  }
  bool result = false;
  // FairRootManager* ioman = FairRootManager::Instance();
  TGeoManager *gGeoMan = (TGeoManager *)gROOT->FindObject("FAIRGeom");
  if (!gGeoMan) {
    cout << " Info: no FAIRGeom found, using gGeoManager " << endl;
    gGeoMan = gGeoManager;
  }
  if (gGeoMan) {
    vector<string> list_of_sensors;
    string path_to_top = Get_List_of_Sensors(list_of_sensors);
    int offset;
    if (!Test_List_of_Sensors(list_of_sensors, offset)) {
      cout << " *** Error in PndLmdDim::Read_transformation_matrices_from_geometry:" << endl;
      cout << " could not retrieve list of sensors from geometry " << endl;
      return false;
    }

    map<Tkey, TGeoMatrix *> *matrices = nullptr;
    if (aligned) {
      matrices = &transformation_matrices_aligned;
    } else {
      matrices = &transformation_matrices;
    }
    // kill existing matrices
    for (it_transformation_matrices = matrices->begin(); it_transformation_matrices != matrices->end(); it_transformation_matrices++) {
      delete (it_transformation_matrices->second);
    }
    matrices->clear();

    // int matrices_counter(0); //[R.K. 01/2017] unused variable?
    // TGeoHMatrix* matrix; //[R.K. 01/2017] unused variable?

    TGeoVolume *vol = gGeoMan->FindVolumeFast(nav_paths[0].c_str());
    if (vol) { // check only the existence of the top node, everything else will be not checked, so earlier simulations will fail here
      /*
       string sversion = vol->GetTitle();
       if (sversion.compare(0,8,"version ")!=0){
       cout << " Warning from PndLmdDim::Retrieve_version_number: no version number encoded in the node title. Setting it to 0. " << endl;
       geometry_version = 0;
       } else {
       // take the number behind "version " string
       geometry_version = atoi(&(vol->GetTitle()[8]));
       }
       cout << " Info from PndLmdDim::Retrieve_version_number: The geometry version was set to " << geometry_version << endl;
       //cout << vol->GetName() << endl;*/
      int sensor_id = offset;
      stringstream path_to_ref;
      path_to_ref << path_to_top << "/" << nav_paths[1] << "_0";
      gGeoMan->cd(path_to_ref.str().c_str());
      TGeoHMatrix *top_matrix = new TGeoHMatrix(*gGeoMan->GetCurrentMatrix());
      // top_matrix->Print();
      if (top_matrix)
        (*matrices)[Tkey(-1, -1, -1, -1, -1, -1)] = new TGeoHMatrix(*top_matrix);
      for (unsigned int ihalf = 0; ihalf < 2; ihalf++) { // loop over detector halves
        stringstream path_to_half;
        path_to_half << path_to_ref.str() << "/" << nav_paths[2] << "_" << ihalf;
        // gGeoMan->cd(path_to_half.str().c_str());
        TGeoHMatrix *half_matrix = Get_matrix(path_to_half.str(), aligned, ihalf, -1, -1, -1, -1, -1); // gGeoMan->GetCurrentNode()->GetMatrix();
        // here is some mess, sorry for that!
        // if (half_matrix && top_matrix) (*matrices)[Tkey(-1, -1, -1, -1, -1, -1)] =
        //		new TGeoHMatrix(*half_matrix * *top_matrix);
        for (unsigned int iplane = 0; iplane < n_planes; iplane++) { // loop over planes
          stringstream path_to_plane;
          path_to_plane << path_to_half.str() << "/" << nav_paths[3] << "_" << iplane;
          // gGeoMan->cd(path_to_plane.str().c_str());
          TGeoHMatrix *plane_matrix = Get_matrix(path_to_plane.str(), aligned, ihalf, iplane, -1, -1, -1, -1);
          // if (matrix) (*matrices)[Tkey(ihalf, iplane, imodule, iside, idie, isensor)] = new TGeoHMatrix(*matrix);
          for (unsigned int imodule = 0; imodule < nmodules; imodule++) { // loop over modules
            stringstream path_to_module;
            path_to_module << path_to_plane.str() << "/" << nav_paths[4] << "_" << imodule;
            // gGeoMan->cd(path_to_module.str().c_str());
            TGeoHMatrix *module_matrix = Get_matrix(path_to_module.str(), aligned, ihalf, iplane, imodule, -1, -1, -1);
            // if (matrix) (*matrices)[Tkey(ihalf, iplane, imodule, iside, idie, isensor)] = new TGeoHMatrix(*matrix);
            for (unsigned int iside = 0; iside < 2; iside++) { // loop over the two sides of the modules
              stringstream path_to_side;
              path_to_side << path_to_module.str() << "/" << nav_paths[5] << "_" << iside;
              // gGeoMan->cd(path_to_side.str().c_str());
              TGeoHMatrix *side_matrix = Get_matrix(path_to_side.str(), aligned, ihalf, iplane, imodule, iside, -1, -1); // gGeoMan->GetCurrentNode()->GetMatrix();
              if (plane_matrix && module_matrix && side_matrix)
                (*matrices)[Tkey(ihalf, iplane, imodule, iside, -1, -1)] = new TGeoHMatrix(*plane_matrix * *module_matrix * *side_matrix);
              for (unsigned int idie = 0; idie < 2; idie++) { // loop over dies
                stringstream path_to_die;
                path_to_die << path_to_side.str() << "/" << nav_paths[6] << "_" << idie;
                TGeoHMatrix *die_matrix = Get_matrix(path_to_die.str(), aligned, ihalf, iplane, imodule, iside, idie, -1);
                for (unsigned int isensor = 0; isensor < 3; isensor++) { // loop over sensors
                  if (idie == 1) {
                    if (isensor == 0)
                      continue;
                  }
                  stringstream path_to_sensor;
                  path_to_sensor << path_to_die.str() << "/" << nav_paths[7] << "_" << sensor_id;
                  TGeoHMatrix *sensor_matrix = Get_matrix(path_to_sensor.str(), aligned, ihalf, iplane, imodule, iside, idie, isensor); // gGeoMan->GetCurrentNode()->GetMatrix();
                  if (sensor_matrix && die_matrix)
                    (*matrices)[Tkey(ihalf, iplane, imodule, iside, idie,
                                     isensor)] = new TGeoHMatrix(*die_matrix * *sensor_matrix); // do I have to delete it, or should I copy it?
                  // new TGeoHMatrix((*rottrans_die) * (*rottrans_sensor));
                  sensor_id++;
                  delete sensor_matrix;
                } // loop over sensors
                delete die_matrix;
              } // loop over dies
              delete side_matrix;
            } // loop over the two sides of the modules
            delete module_matrix;
          } // loop over modules
          delete plane_matrix;
        } // loop over planes
        delete half_matrix;
      } // loop over detector halves
      delete top_matrix;
      gGeoManager->RefreshPhysicalNodes(false);
      cout << " read " << (*matrices).size() << " matrices from root geometry " << endl;
      result = true;
    } else {
      cout << " *** Error in PndLmdDim::Read_transformation_matrices_from_geometry:" << endl;
      cout << " Could not find the top volume " << nav_paths[0].c_str() << " to retrieve the transformation matrix for the luminosity detector! Is the geometry already loaded? "
           << endl;
    }
  } else {
    cout << " *** Error in PndLmdDim::Read_transformation_matrices_from_geometry:" << endl;
    cout << " Could not find a GeoManager to load the luminosity detector matrices from it! " << endl;
  }
  return result;
}

bool PndLmdDim::Write_transformation_matrices_to_geometry(bool aligned)
{
  if (!Retrieve_version_number()) {
    return false;
  }
  if (geometry_version < 3) {
    cout << " *** Error in PndLmdDim::Write_transformation_matrices_to_geometry:" << endl;
    cout << " geometry version " << geometry_version << " is not compatible with this method! " << endl;
    return false;
  }
  bool result = false;
  // FairRootManager* ioman = FairRootManager::Instance();
  TGeoManager *gGeoMan = (TGeoManager *)gROOT->FindObject("FAIRGeom");
  if (!gGeoMan) {
    cout << " Info: no FAIRGeom found, using gGeoManager " << endl;
    gGeoMan = gGeoManager;
  }
  if (gGeoMan) {
    vector<string> list_of_sensors;
    string path_to_top = Get_List_of_Sensors(list_of_sensors);
    int offset;
    if (!Test_List_of_Sensors(list_of_sensors, offset)) {
      cout << " *** Error in PndLmdDim::Write_transformation_matrices_to_geometry:" << endl;
      cout << " could not retrieve list of sensors from geometry " << endl;
      return false;
    }

    map<Tkey, TGeoMatrix *> *matrices = nullptr;
    if (aligned) {
      matrices = &transformation_matrices_aligned;
    } else {
      matrices = &transformation_matrices;
    }
    // check matrix existence
    if (matrices->size() == 0) {
      cout << " *** Error in PndLmdDim::Write_transformation_matrices_to_geometry:" << endl;
      cout << " no matrices to apply to the geometry! " << endl;
      return false;
    }

    // int matrices_counter(0); //[R.K. 01/2017] unused variable?
    TGeoHMatrix *matrix;
    TGeoHMatrix *matrix_mother;

    TGeoVolume *vol = gGeoMan->FindVolumeFast(nav_paths[0].c_str());
    if (vol) { // check only the existence of the top node, everything else will be not checked, so earlier simulations will fail here
      /*
       string sversion = vol->GetTitle();
       if (sversion.compare(0,8,"version ")!=0){
       cout << " Warning from PndLmdDim::Retrieve_version_number: no version number encoded in the node title. Setting it to 0. " << endl;
       geometry_version = 0;
       } else {
       // take the number behind "version " string
       geometry_version = atoi(&(vol->GetTitle()[8]));
       }
       cout << " Info from PndLmdDim::Retrieve_version_number: The geometry version was set to " << geometry_version << endl;
       //cout << vol->GetName() << endl;*/
      int sensor_id = offset;
      stringstream path_to_ref;
      path_to_ref << path_to_top << "/" << nav_paths[1] << "_0";
      matrix = (TGeoHMatrix *)Get_matrix(-1, -1, -1, -1, -1, -1, aligned);
      // obtain matrix up to the mother volume to subtract it
      // from the total one in the search tree
      gGeoMan->cd(path_to_top.c_str());
      matrix_mother = (TGeoHMatrix *)gGeoMan->GetCurrentMatrix();
      if (matrix && matrix_mother) {
        TGeoHMatrix *_matrix = new TGeoHMatrix(matrix_mother->Inverse() * *matrix);
        Set_matrix(path_to_ref.str(), _matrix, -1, -1, -1, -1, -1, -1);
      }
      for (unsigned int ihalf = 0; ihalf < 2; ihalf++) { // loop over detector halves
        stringstream path_to_half;
        path_to_half << path_to_ref.str() << "/" << nav_paths[2] << "_" << ihalf;
        gGeoMan->cd(path_to_half.str().c_str());
        // get the original matrix to subtract it
        // I'm not sure if it is always correct
        // TGeoHMatrix* half_matrix = Get_matrix(path_to_half.str(), false, ihalf,
        //-1, -1, -1, -1, -1); //[R.K. 01/2017] unused variable?
        // matrix = (TGeoHMatrix*) Get_matrix(ihalf,-1,-1,-1,-1,-1, aligned);
        // if (matrix) Set_matrix(path_to_half.str(), matrix,
        //		ihalf, -1, -1, -1, -1, -1);
        for (unsigned int iplane = 0; iplane < n_planes; iplane++) { // loop over planes
          stringstream path_to_plane;
          path_to_plane << path_to_half.str() << "/" << nav_paths[3] << "_" << iplane;
          // gGeoMan->cd(path_to_plane.str().c_str());
          TGeoHMatrix *plane_matrix = Get_matrix(path_to_plane.str(), false, ihalf, iplane, -1, -1, -1, -1);
          // matrix = (TGeoHMatrix*) Get_matrix(ihalf,iplane,-1,-1,-1,-1, aligned);
          // if (matrix) Set_matrix(path_to_plane.str(), matrix,
          //		ihalf, iplane, -1, -1, -1, -1);
          for (unsigned int imodule = 0; imodule < nmodules; imodule++) { // loop over modules
            stringstream path_to_module;
            path_to_module << path_to_plane.str() << "/" << nav_paths[4] << "_" << imodule;
            TGeoHMatrix *module_matrix = Get_matrix(path_to_module.str(), false, ihalf, iplane, imodule, -1, -1, -1);
            // gGeoMan->cd(path_to_module.str().c_str());
            // matrix = (TGeoHMatrix*) Get_matrix(ihalf,iplane,imodule,-1,-1,-1, aligned);
            // if (matrix) Set_matrix(path_to_module.str(), matrix,
            //		ihalf, iplane, imodule, -1, -1, -1);
            for (unsigned int iside = 0; iside < 2; iside++) { // loop over the two sides of the modules
              stringstream path_to_side;
              path_to_side << path_to_module.str() << "/" << nav_paths[5] << "_" << iside;
              // gGeoMan->cd(path_to_side.str().c_str());
              TGeoHMatrix *side_matrix = Get_matrix(path_to_side.str(), false, ihalf, iplane, imodule, iside, -1, -1);
              matrix = (TGeoHMatrix *)Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
              if (matrix && plane_matrix && module_matrix && side_matrix) {
                TGeoHMatrix *_matrix = new TGeoHMatrix((*plane_matrix * *module_matrix).Inverse() * *matrix);
                Set_matrix(path_to_side.str(), _matrix, ihalf, iplane, imodule, iside, -1, -1);
              }
              for (unsigned int idie = 0; idie < 2; idie++) { // loop over dies
                stringstream path_to_die;
                path_to_die << path_to_side.str() << "/" << nav_paths[6] << "_" << idie;
                // gGeoMan->cd(path_to_die.str().c_str());
                // matrix = (TGeoHMatrix*) Get_matrix(ihalf,iplane,imodule,iside,idie,-1, aligned);
                TGeoHMatrix *die_matrix = Get_matrix(path_to_die.str(), aligned, ihalf, iplane, imodule, iside, idie, -1);
                // if (matrix) Set_matrix(path_to_die.str(), matrix,
                //		ihalf, iplane, imodule, iside, idie, -1);
                for (unsigned int isensor = 0; isensor < 3; isensor++) { // loop over sensors
                  if (idie == 1) {
                    if (isensor == 0)
                      continue;
                  }
                  stringstream path_to_sensor;
                  stringstream path_to_sensor_passive;
                  path_to_sensor << path_to_die.str() << "/" << nav_paths[7] << "_" << sensor_id;
                  // here the passive part of the sensor must be also shifted
                  // this was not reflected in the original idea to store the path in the nav_paths
                  path_to_sensor_passive << path_to_die.str() << "/"
                                         << "LumPassiveRect"
                                         << "_" << sensor_id - sensIDoffset;
                  // gGeoMan->cd(path_to_sensor.str().c_str());
                  TGeoHMatrix *sensor_matrix = Get_matrix(path_to_sensor.str(), aligned, ihalf, iplane, imodule, iside, idie, isensor);
                  matrix = (TGeoHMatrix *)Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
                  if (sensor_matrix && die_matrix && matrix) {
                    TGeoHMatrix *_matrix = new TGeoHMatrix(die_matrix->Inverse() * *matrix);
                    // TGeoHMatrix* _matrix_passive = new TGeoHMatrix(die_matrix->Inverse() * *matrix);
                    Set_matrix(path_to_sensor.str(), _matrix, ihalf, iplane, imodule, iside, idie, isensor);
                    Set_matrix(path_to_sensor_passive.str(), _matrix, ihalf, iplane, imodule, iside, idie, isensor + 1000);
                  }
                  sensor_id++;
                } // loop over sensors
                  // lmd_vol_side_->AddNode(lmd_vol_die_, 0, rottrans_die);
              }   // loop over dies
            }     // loop over the two sides of the modules
                  // lmd_vol_plane_->AddNode(lmd_vol_module_, 0, rottrans_module);
          }       // loop over modules
        }         // loop over planes
                  // lmd_vol_ref_sys->AddNode(lmd_vol_half_, 0, rottrans_no);
        // transformation_matrices[Tkey(-1, -1, -1, -1, -1, -1)] = new TGeoHMatrix((*lmd_transrot) * (*rottrans_lmd_in_box));
      } // loop over detector halves
      cout << " set " << (*matrices).size() << " matrices to root geometry " << endl;
      result = true;
    } else {
      cout << " *** Error in PndLmdDim::Write_transformation_matrices_to_geometry:" << endl;
      cout << " Could not find the top volume " << nav_paths[0].c_str() << " to retrieve the transformation matrix for the luminosity detector! Is the geometry already loaded? "
           << endl;
    }
    gGeoMan->RefreshPhysicalNodes();
  } else {
    cout << " *** Error in PndLmdDim::Write_transformation_matrices_to_geometry:" << endl;
    cout << " Could not find a GeoManager to load the luminosity detector matrices from it! " << endl;
  }

  return result;
}

string PndLmdDim::Get_List_of_Sensors(vector<string> &list_of_sensors, bool found_lmd, bool first_call)
{
  string result("");
  if (first_call) {
    gGeoManager->CdTop();
    first_call = false;
  }
  // cout << gGeoManager->GetPath() << " ";
  string nodename(gGeoManager->GetCurrentNavigator()->GetCurrentNode()->GetName());
  // cout << "current node is " << nodename << endl;
  if (nodename.compare(0, nav_paths[7].size(), nav_paths[7]) == 0) {
    // cout << " found a sensor " << nav_paths[7] << endl;
    list_of_sensors.push_back(gGeoManager->GetPath());
  }
  int nnodes = gGeoManager->GetCurrentNode()->GetNdaughters();
  for (int i = 0; i < nnodes; i++) {
    // cout << " navigating into node " << i << endl;
    gGeoManager->CdDown(i);
    result = Get_List_of_Sensors(list_of_sensors, found_lmd, first_call);
    if (nodename == nav_paths[1] || found_lmd) {
      cout << " found the lmd node! Aborting recursive search. " << endl;
      gGeoManager->CdUp();
      break;
    }
    gGeoManager->CdUp();
  }
  if (nodename.compare(0, nav_paths[0].size(), nav_paths[0]) == 0) {
    result = gGeoManager->GetPath();
    cout << " top volume is " << result << endl;
  }
  return result;
}

bool PndLmdDim::Test_List_of_Sensors(vector<string> list_of_sensors, int &offset)
{
  bool result = true;
  offset = 0;
  int nsensorstotal = n_sensors * 2 * nmodules * 2 * n_planes;
  int max_sensID = nsensorstotal - 1;
  int checksum = 0;
  int sumtocheck = 0;
  if ((int)list_of_sensors.size() != nsensorstotal) {
    cout << " PndLmdDim::Test_List_of_Sensors: number of sensors is wrong " << list_of_sensors.size() << " != " << nsensorstotal << endl;
    return false;
  }
  for (int isensor = 0; isensor < nsensorstotal; isensor++) {
    gGeoManager->cd(list_of_sensors[isensor].c_str());
    string path(gGeoManager->GetCurrentNavigator()->GetPath());
    if (path != list_of_sensors[isensor]) {
      cout << " PndLmdDim::Test_List_of_Sensors: Could not navigate to " << list_of_sensors[isensor] << endl;
      return false;
    }
    // strip the copy number in the path
    char sensID_char[5] = {'0', '0', '0', '\0'};
    sensID_char[4] = '\0';
    for (unsigned ichar = 3; ichar > 0; ichar--) { // path.size()-1; ichar > path.size()-5; ichar--){
      char digit = path[path.size() - 4 + ichar];
      if (digit == '_')
        break;
      sensID_char[ichar] = digit;
    }
    int sensID = atoi(sensID_char);
    if (sensID > max_sensID) {
      max_sensID = sensID;
    }
    checksum += isensor;
    sumtocheck += sensID;
  }
  offset = max_sensID - (nsensorstotal - 1);
  if (offset != 0) {
    cout << " PndLmdDim::Test_List_of_Sensors: Found an offset in the sensorIDs of " << offset << endl;
    checksum += (nsensorstotal * offset);
  }
  if (checksum != sumtocheck) {
    cout << " PndLmdDim::Test_List_of_Sensors: check sum of sensor id's does not match " << checksum << " != " << sumtocheck << endl;
    return false;
  }
  return result;
}

bool PndLmdDim::Set_sensIDoffset(int offset)
{
  if (offset >= 0) {
    sensIDoffset = (int)offset;
    return true;
  }
  vector<string> sensors;
  Get_List_of_Sensors(sensors);
  if (Test_List_of_Sensors(sensors, offset)) {
    sensIDoffset = offset;
    return true;
  } else {
    cout << " Could not set sensIDoffset from root geometry " << endl;
    return false;
  }
}

#include <iomanip>

void PndLmdDim::Write_transformation_matrices(string filename, bool aligned, int version_number)
{
  map<Tkey, TGeoMatrix *> *matrices = nullptr;
  if (aligned) {
    matrices = &transformation_matrices_aligned;
  } else {
    matrices = &transformation_matrices;
  }
  int matrices_counter(0);
  string dir = getenv("VMCWORKDIR");
  if (filename == "")
    filename = dir + "/geometry/trafo_matrices_lmd.dat";
  ofstream file(filename.c_str());
  if (file.is_open()) {
    // write the first line to add additional information
    file << "version " << version_number << "\n";
    for (it_transformation_matrices = matrices->begin(); it_transformation_matrices != matrices->end(); it_transformation_matrices++) {
      // write the key
      Tkey key = it_transformation_matrices->first;
      file << Generate_key(key.half, key.plane, key.module, key.side, key.die, key.sensor) << '\n';
      const double *translation = it_transformation_matrices->second->GetTranslation();
      const double *rotation = it_transformation_matrices->second->GetRotationMatrix();
      // write the numbers for the translation
      for (int i = 0; i < 3; i++) {
        file << setw(14) << setprecision(14) << scientific << translation[i];
        if (i == 2)
          file << '\n';
        else
          file << ' ';
      }
      // write the numbers for the rotation
      for (int i = 0; i < 9; i++) {
        file << setw(14) << setprecision(14) << scientific << rotation[i];
        if (i == 8)
          file << '\n';
        else
          file << ' ';
      }
      matrices_counter++;
    }
    file.close();
    cout << matrices_counter << " Transformation matrices were written to " << filename << endl;
  } else
    cout << " Error in PndLmdDim::Write_transformation_matrices: could not write to " << filename << endl;
}

void PndLmdDim::Cleanup()
{
  for (it_transformation_matrices = transformation_matrices.begin(); it_transformation_matrices != transformation_matrices.end(); it_transformation_matrices++) {
    delete (it_transformation_matrices->second);
  }
  transformation_matrices.clear();
  for (it_transformation_matrices = transformation_matrices_aligned.begin(); it_transformation_matrices != transformation_matrices_aligned.end(); it_transformation_matrices++) {
    delete (it_transformation_matrices->second);
  }
  transformation_matrices_aligned.clear();
}

void PndLmdDim::Read_DB_offsets(PndLmdAlignPar *lmdalignpar)
{
  // ana = FairRun::Instance();
  // rtdb=ana->GetRuntimeDb();
  // TList* fAlignParamList = new TList();// alignment params fom DB
  double fShiftX[40], fShiftY[40], fShiftZ[40];
  double fRotateX[40], fRotateY[40], fRotateZ[40];
  // //read params for lumi alignment
  // PndLmdContFact* thelmdcontfact = (PndLmdContFact*)rtdb->getContFactory("PndLmdContFact");
  // TList* theAlignLMDContNames = thelmdcontfact->GetAlignParNames();
  // Info("SetParContainers()","AlignLMD The container names list contains %i entries",theAlignLMDContNames->GetEntries());
  // TIter cfAlIter(theAlignLMDContNames);
  // while (TObjString* contname = (TObjString*)cfAlIter()) {
  //   TString parsetname = contname->String();
  //   Info("SetParContainers()",parsetname.Data());
  //   PndLmdAlignPar *lmdalignpar = (PndLmdAlignPar*)(rtdb->getContainer(parsetname.Data()));
  //   if(!lmdalignpar) Fatal("SetParContainers","No ALIGN parameter found: %s",parsetname.Data());
  //   fAlignParamList->Add(lmdalignpar);
  // }

  // TIter alignparams(fAlignParamList);
  // PndLmdAlignPar* lmdalignpar=(PndLmdAlignPar*)alignparams();
  // //  lmdalignpar->getParams(alignparams);
  //  lmdalignpar->Print();
  if (0 == lmdalignpar) {
    Error("PndLmdStripClusterTask::SetCalculators()", "A ALIGN Parameter Set does not exist properly.");
  } else {
    // cout<<"&&&&& it's from LmdDim &&&&&"<<endl;
    // lmdalignpar->Print();
    for (int ik = 0; ik < 40; ik++) {
      //      cout<<"ik = "<<ik<<endl;
      fShiftX[ik] = lmdalignpar->GetShiftX(ik);
      //   cout<<"fShiftX[ik] = "<<fShiftX[ik]<<endl;
      fShiftY[ik] = lmdalignpar->GetShiftY(ik);
      fShiftZ[ik] = lmdalignpar->GetShiftZ(ik);
      fRotateX[ik] = lmdalignpar->GetRotateX(ik);
      fRotateY[ik] = lmdalignpar->GetRotateY(ik);
      fRotateZ[ik] = lmdalignpar->GetRotateZ(ik);
      //      if (fVerbose > 2) cout<<"fShiftX["<<ik<<"]="<<fShiftX[ik]<<" fRotateX["<<ik<<"]="<<fRotateX[ik]
      //		    <<" fRotateY["<<ik<<"]="<<fRotateY[ik]<<" fRotateZ["<<ik<<"]="<<fRotateZ[ik]<<endl;
    }
  }
  //  lmdalignpar->Print();

  // so far for modules alignment only
  // TODO: individual sensor and\or die alignment???
  for (unsigned int ihalf = 0; ihalf < 2; ihalf++) {                  // loop over detector halves
    for (unsigned int iplane = 0; iplane < n_planes; iplane++) {      // loop over planes
      for (unsigned int imodule = 0; imodule < nmodules; imodule++) { // loop over modules
        Tkey key(ihalf, iplane, imodule, -1, -1, -1);
        int ikey = (ihalf * n_planes * nmodules) + (iplane * nmodules) + imodule;
        //	cout<<"for: "<<ihalf<<iplane<<imodule<<": ikey="<<ikey<<endl;
        offsets[key].push_back(fShiftX[ikey]);
        offsets[key].push_back(fShiftY[ikey]);
        offsets[key].push_back(fShiftZ[ikey]);
        offsets[key].push_back(fRotateX[ikey]);
        offsets[key].push_back(fRotateY[ikey]);
        offsets[key].push_back(fRotateZ[ikey]);

        Tkey key2(ihalf, iplane, imodule, 0, -1, -1); // top side
        //	cout<<"for: "<<ihalf<<iplane<<imodule<<": ikey="<<ikey<<endl;
        offsets[key2].push_back(fShiftX[ikey]);
        offsets[key2].push_back(fShiftY[ikey]);
        offsets[key2].push_back(fShiftZ[ikey]);
        offsets[key2].push_back(fRotateX[ikey]);
        offsets[key2].push_back(fRotateY[ikey]);
        offsets[key2].push_back(fRotateZ[ikey]);

        // //TODO: is it correct???
        Tkey key3(ihalf, iplane, imodule, 1, -1, -1); // bottom side
        offsets[key3].push_back(fShiftX[ikey]);
        offsets[key3].push_back(fShiftY[ikey]);
        offsets[key3].push_back(fShiftZ[ikey]);
        offsets[key3].push_back(fRotateX[ikey]);
        offsets[key3].push_back(fRotateY[ikey]);
        offsets[key3].push_back(fRotateZ[ikey]);
      }
    }
  }
}

void PndLmdDim::Set_offset(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, double x, double y, double z, double rotphi, double rottheta, double rotpsi)
{
  Tkey key = Tkey(ihalf, iplane, imodule, iside, idie, isensor);
  itoffset = offsets.find(key);
  if (itoffset != offsets.end()) {
    cout << " **** Warning in PndLmdDim::Set_offset: offset exists already! Replacing it! *** " << endl;
  } else {
    offsets[key].clear();
    offsets[key].push_back(x);
    offsets[key].push_back(y);
    offsets[key].push_back(z);
    offsets[key].push_back(rotphi);
    offsets[key].push_back(rottheta);
    offsets[key].push_back(rotpsi);
  }
}

void PndLmdDim::Get_offset(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, double &x, double &y, double &z, double &rotphi, double &rottheta, double &rotpsi,
                           bool random)
{
  Tkey key = Tkey(ihalf, iplane, imodule, iside, idie, isensor);
  // cout<<"setting Offset for: "<<ihalf<<", "<<iplane<<", "<<imodule<<", "<<iside<<", "<<idie<<", "<<isensor<<endl;
  itoffset = offsets.find(key);
  if (itoffset != offsets.end()) {
    x = itoffset->second[0];
    y = itoffset->second[1];
    z = itoffset->second[2];
    rotphi = itoffset->second[3];
    rottheta = itoffset->second[4];
    rotpsi = itoffset->second[5];
  } else {
    if (random) {
      cout << " generating offset for";
      if (ihalf >= 0 && iplane >= 0 && imodule >= 0 && iside >= 0 && idie >= 0 && isensor >= 0) {
        cout << " one sensor ";
        x = gRandom->Gaus(0, sensor_offset_x);
        y = gRandom->Gaus(0, sensor_offset_y);
        z = gRandom->Gaus(0, sensor_offset_z);
        rotphi = gRandom->Gaus(0, sensor_tilt_phi);
        rottheta = gRandom->Gaus(0, sensor_tilt_theta);
        rotpsi = gRandom->Gaus(0, sensor_tilt_psi);
        // check for clashing sensors starting from the inner
        // sensor and correct for it
        // check is only performed in the glueing plane
        // check along x
        // calculate the own additional space required by an offset in x and the rotation
        /*
         double dx = maps_height * (sqrt(2.)*sin(fabs(rotphi)+3.1416/4.)-1);
         if ((isensor > 0 && idie == 0) || (isensor > 1 && idie == 1)){
         // check the space required by the preceding sensor
         double _x, _y, _z, _rotphi, _rottheta, _rotpsi;
         Get_offset(ihalf, iplane, imodule, iside, idie, isensor-1, _x, _y, _z, _rotphi, _rottheta, _rotpsi);
         double _dx = maps_height * (sqrt(2.)*sin(fabs(_rotphi)+3.1416/4.)-1);
         // check if there is space left
         if ( (x+_x) < (_dx+dx) ){
         x = (_dx+dx)-(x+_x);
         }
         }
         // same for y neighbors
         if (isensor > 0 && idie > 0){
         // check the space required by the preceding sensor
         double _x, _y, _z, _rotphi, _rottheta, _rotpsi;
         Get_offset(ihalf, iplane, imodule, iside, idie-1, isensor, _x, _y, _z, _rotphi, _rottheta, _rotpsi);
         double _dy = maps_height * (sqrt(2.)*sin(fabs(_rotphi)+3.1416/4.)-1);
         // check if there is space left
         if ( (y+_y) < (_dy+dx) ){
         y = (_dy+dx)-(y+_y);
         }
         }*/
        // x = (cvd_offset_x);
        // y = (cvd_offset_y);
        // z = (cvd_offset_z);
        // rotphi = (cvd_tilt_phi);
        // rottheta = (cvd_tilt_theta);
        // rotpsi = (cvd_tilt_psi);
      }
      // the precision of dies containing sensors is requested
      // when requested offset applies to all sensors
      // on one side
      if (ihalf >= 0 && iplane >= 0 && imodule >= 0 && iside >= 0 && idie < 0 && isensor < 0) {
        cout << " one module side ";
        x = gRandom->Gaus(0, side_offset_x);
        y = gRandom->Gaus(0, side_offset_y);
        z = gRandom->Gaus(0, side_offset_z);
        rottheta = gRandom->Gaus(0, side_tilt_theta);
        rotphi = gRandom->Gaus(0, side_tilt_phi);
        rotpsi = gRandom->Gaus(0, side_tilt_psi);
      }
      // the precision of cvd discs is requested
      // when requested offset applies also to the sensors
      // on both sides
      if (ihalf >= 0 && iplane >= 0 && imodule >= 0 && iside < 0 && idie < 0 && isensor < 0) {
        cout << " one module ";
        x = gRandom->Gaus(0, cvd_offset_x);
        y = gRandom->Gaus(0, cvd_offset_y);
        z = gRandom->Gaus(0, cvd_offset_z);
        rotphi = gRandom->Gaus(0, cvd_tilt_phi);
        rottheta = gRandom->Gaus(0, cvd_tilt_theta);
        rotpsi = gRandom->Gaus(0, cvd_tilt_psi);
        // x = (cvd_offset_x);
        // y = (cvd_offset_y);
        // z = (cvd_offset_z);
        // rotphi = (cvd_tilt_phi);
        // rottheta = (cvd_tilt_theta);
        // rotpsi = (cvd_tilt_psi);
      }
      // the precision of plane halves containing sensors is requested
      // when requested offset applies to all modules
      if (ihalf >= 0 && iplane >= 0 && imodule < 0 && iside < 0 && idie < 0 && isensor < 0) {
        cout << " one plane half ";
        x = gRandom->Gaus(0, plane_half_offset_x);
        y = gRandom->Gaus(0, plane_half_offset_y);
        z = gRandom->Gaus(0, plane_half_offset_z);
        rottheta = gRandom->Gaus(0, plane_half_tilt_theta);
        rotphi = gRandom->Gaus(0, plane_half_tilt_phi);
        rotpsi = gRandom->Gaus(0, plane_half_tilt_psi);
      }
      // the precision of halves of planes is requested
      // when requested offset applies to module supports
      if (ihalf >= 0 && iplane < 0 && imodule < 0 && iside < 0 && idie < 0 && isensor < 0) {
        cout << " one half ";
        x = gRandom->Gaus(0, half_offset_x);
        y = gRandom->Gaus(0, half_offset_y);
        z = gRandom->Gaus(0, half_offset_z);
        rottheta = gRandom->Gaus(0, half_tilt_theta);
        cout << " wtf ? " << half_tilt_phi << endl;
        rotphi = gRandom->Gaus(0, half_tilt_phi);
        rotpsi = gRandom->Gaus(0, half_tilt_psi);
      }
      // 	// the precision of the luminosity detector is requested
      // 	// when no degrees of freedom are available to the rest
      if (ihalf < 0 && imodule < 0 && iplane < 0 && iside < 0 && idie < 0 && isensor < 0) {
        cout << " the luminosity detector ";
        // not implemented yet
        x = gRandom->Gaus(0, 0);
        y = gRandom->Gaus(0, 0);
        z = gRandom->Gaus(0, 0);
        rottheta = gRandom->Gaus(0, 0);
        rotphi = gRandom->Gaus(0, 0);
        rotpsi = gRandom->Gaus(0, 0);
      }
      cout << " of x = " << x << " cm y = " << y << " cm z = " << z;
      cout << " cm theta = " << rottheta << " rad phi = " << rotphi;
      cout << " rad psi = " << rotpsi << " rad " << endl;
    } else {
      x = 0;
      y = 0;
      z = 0;
      rotphi = 0;
      rottheta = 0;
      rotpsi = 0;
    }
    offsets[key].push_back(x);
    offsets[key].push_back(y);
    offsets[key].push_back(z);
    offsets[key].push_back(rotphi);
    offsets[key].push_back(rottheta);
    offsets[key].push_back(rotpsi);
  }
  //	  cout<<"x,y,z,rotphi,rottheta,rotpsi: "<<x<<", "<<y<<", "<<z<<", "<<rotphi<<", "<<rottheta<<", "<<rotpsi<<endl;
}

TVector3 PndLmdDim::Decode_hit(const int sensorID, const double column, const double row, const bool aligned, bool newVersion)
{

  if (newVersion) {

    double x, y;
    x = column;
    y = row;

    // correct for pixel corner to center
    x += 0.5;
    y += 0.5;

    // shift from corner to center (this considers inactive area!)
    x -= (247.5 / 2.0);
    y -= (242.5 / 2.0);

    // scale for pixel size
    x *= 80e-4;
    y *= 80e-4;

    TVector3 hit(x, y, 0.);
    int ihalf, iplane, imodule, iside, idie, isensor;
    Get_sensor_by_id(sensorID, ihalf, iplane, imodule, iside, idie, isensor);
    return Transform_sensor_to_global(hit, ihalf, iplane, imodule, iside, idie, isensor, false, aligned);
  }

  // an additional offset of about 0.5 pixels in x and 2 pixels in y
  // seems to appear during digitization via SDS class
  // this is taken here into account
  double x = (column - 250. / 2. + .5) * maps_active_pixel_size + maps_passive_left * 2.;
  double y = (row - 250. / 2. + 0.5 - 2.5) * maps_active_pixel_size + maps_passive_bottom * 2.;
  TVector3 hit(x, y, 0.);
  int ihalf, iplane, imodule, iside, idie, isensor;
  Get_sensor_by_id(sensorID, ihalf, iplane, imodule, iside, idie, isensor);
  // hit = Transform_sensor_to_lmd_local(hit, ihalf, iplane, imodule, iside, idie, isensor, aligned);
  // hit =
  return Transform_sensor_to_global(hit, ihalf, iplane, imodule, iside, idie, isensor, false, aligned);
}

void PndLmdDim::Propagate_fast_ip_to_lmd(TVector3 &pos, TVector3 &mom, double pbeam)
{
  static bool first_call(true);
  if (geometry_version != 2 && !first_call) {
    first_call = false;
    cout << " Warning in PndLmdDim::Propagate_fast_ip_to_lmd: Wrong geometry version " << geometry_version << endl;
    cout << " Transformation matrix was fit to data with geometry version 2 ! " << endl;
  }
  if (pbeam != 1.5 && pbeam != 15) {
    cout << " Error in PndLmdDim::Propagate_fast_ip_to_lmd: no matrix fit for " << pbeam << " GeV/c beam momentum";
    cout << " nor interpolation is implemented yet " << endl;
    return;
  }
  // check if to create and initialize a matrix first
  if (propagation_matrices.find(pbeam) == propagation_matrices.end()) {
    PndLmdDimPropMat propmat(pbeam);
    propagation_matrices[pbeam] = propmat;
  }
  propagation_matrices[pbeam].Propagate(pos, mom);
}

bool PndLmdDim::Get_overlapping_sensor(const TVector3 &point, int &ihalf, int &iplane, int &imodule, int &iside, int &idie, int &isensor, bool aligned)
{
  bool result = false;
  int _ihalf = ihalf;
  ihalf = -1;
  int _iplane = iplane;
  iplane = -1;
  int _imodule = imodule;
  imodule = -1;
  int _iside = iside;
  iside = -1;
  int _idie = idie;
  idie = -1;
  int _isensor = isensor;
  isensor = -1;
  // check if point lies on the proposed sensor
  if (!Is_on_Sensor(point, _ihalf, _iplane, _imodule, _iside, _idie, _isensor, aligned))
    return false;
  // check all other sensors on the opposite side
  if (_iside == 0)
    _iside = 1;
  else
    _iside = 0;
  // make it faster by the knowledge, which sensors can only overlap
  vector<int> jdie;
  vector<int> jsensor;
  int nchecks = Get_overlapping_sensor(_idie, _isensor, jdie, jsensor);
  // cout << " nchecks " << nchecks << endl;
  if (nchecks <= 0)
    return false;
  for (int icheck = 0; icheck < nchecks; icheck++) {
    if (Is_on_Sensor(point, _ihalf, _iplane, _imodule, _iside, jdie[icheck], jsensor[icheck], aligned)) {
      ihalf = _ihalf;
      iplane = _iplane;
      imodule = _imodule;
      iside = _iside;
      idie = jdie[icheck];
      isensor = jsensor[icheck];
      // cout << " true " << endl;
      return true;
    }
  }
  return result;
}

int PndLmdDim::Get_overlapping_sensor(int idie, int isensor, vector<int> &jdie, vector<int> &jsensor)
{
  jdie.empty();
  jsensor.empty();
  if (idie == 0 && isensor == 0) {
    jdie.push_back(0);
    jsensor.push_back(0);
    jdie.push_back(0);
    jsensor.push_back(1);
    return 2;
  }
  if (idie == 0 && isensor == 1) {
    jdie.push_back(1);
    jsensor.push_back(1);
    jdie.push_back(0);
    jsensor.push_back(1);
    jdie.push_back(0);
    jsensor.push_back(0);
    return 3;
  }
  if (idie == 0 && isensor == 2) {
    jdie.push_back(1);
    jsensor.push_back(2);
    jdie.push_back(1);
    jsensor.push_back(1);
    return 2;
  }
  if (idie == 1 && isensor == 1) {
    jdie.push_back(0);
    jsensor.push_back(1);
    jdie.push_back(0);
    jsensor.push_back(2);
    jdie.push_back(1);
    jsensor.push_back(1);
    jdie.push_back(1);
    jsensor.push_back(2);
    return 4;
  }
  if (idie == 1 && isensor == 2) {
    jdie.push_back(1);
    jsensor.push_back(2);
    jdie.push_back(0);
    jsensor.push_back(2);
    jdie.push_back(1);
    jsensor.push_back(1);
    return 3;
  }
  return 0;
}

bool PndLmdDim::Is_on_Sensor(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  bool result_x = false;
  bool result_y = false;
  const TVector3 &point_on_sensor = Transform_global_to_sensor(point, ihalf, iplane, imodule, iside, idie, isensor, false, aligned);
  // take only the projection
  double x = point_on_sensor.X();
  double y = point_on_sensor.Y();
  // cout << " checking x " << x << " and  y " << y << " on sensor " << ihalf << " " << iplane << " " << imodule << " " << iside << " " << idie << " " << isensor << endl;
  if (x <= 0) { // check left border
    if ((x + maps_width - maps_passive_left * 2.) >= 0)
      result_x = true;
  } else { // check right border
    if ((x - maps_width + maps_passive_right * 2.) <= 0)
      result_x = true;
  }
  if (y <= 0) { // check bottom border
    if ((y + maps_height - maps_passive_bottom * 2.) >= 0)
      result_y = true;
  } else { // check top border
    if ((y - maps_height + maps_passive_top * 2.) <= 0)
      result_y = true;
  }
  // cout << " result is " << result_x << " " << result_y << " therefore " << (result_x && result_y) << endl << endl;
  // both must be set to true to be true
  return (result_x && result_y);
}

void PndLmdDim::Transform_global_to_lmd_local(double &x, double &y, double &z, bool aligned)
{
  const TGeoHMatrix &matrix = Get_transformation_global_to_lmd_local(aligned);
  double from[3] = {x, y, z};
  // cout << x << " " << y << " " << z << endl;
  double to[3];
  matrix.MasterToLocal(from, to);
  x = to[0];
  y = to[1];
  z = to[2];
  // cout << x << " " << y << " " << z << endl << endl;
}

void PndLmdDim::Transform_global_to_lmd_local_vect(double &x, double &y, double &z, bool aligned)
{
  const TGeoHMatrix &matrix = Get_transformation_global_to_lmd_local(aligned);
  double from[3] = {x, y, z};
  // cout << x << " " << y << " " << z << endl;
  double to[3];
  matrix.MasterToLocalVect(from, to);
  x = to[0];
  y = to[1];
  z = to[2];
  // cout << x << " " << y << " " << z << endl << endl;
}

map<Tkey, TGeoMatrix *> *PndLmdDim::Get_matrices(bool aligned)
{
  map<Tkey, TGeoMatrix *> *matrices = nullptr;
  if (aligned) {
    matrices = &transformation_matrices_aligned;
  } else {
    matrices = &transformation_matrices;
  }
  if (matrices->size() == 0) {
    cout << " Warning in PndLmdDim::Get_matrices: No transformation matrices loaded! => trying to load default ones. " << endl;
    Read_transformation_matrices("", aligned);
  }
  return matrices;
}

TGeoMatrix *PndLmdDim::Get_matrix(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  map<Tkey, TGeoMatrix *> *matrices = Get_matrices(aligned);
  if (!matrices)
    return nullptr;
  it_transformation_matrices = matrices->find(Tkey(ihalf, iplane, imodule, iside, idie, isensor));
  if (it_transformation_matrices == matrices->end()) {
    // cout << " Warning in PndLmdDim::Get_matrix: Transformation matrix not existent! " << endl;
    // cout << ihalf << " " << iplane << " " << imodule << " " << iside << " " << idie << " " << isensor << endl;
    return nullptr;
  } else {
    return it_transformation_matrices->second;
  }
}

bool PndLmdDim::Get_matrix_difference(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, double &dx, double &dy, double &dz, double &dphi, double &dtheta,
                                      double &dpsi)
{
  bool result = true;
  dx = 0;
  dy = 0;
  dz = 0;
  dphi = 0;
  dtheta = 0;
  dpsi = 0;
  // try to retrieve the matrix
  TGeoMatrix *matrix = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, false);
  TGeoMatrix *matrix_aligned = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, true);
  if (!matrix || !matrix_aligned)
    return false;
  TGeoCombiTrans combi_trans_matrix(*matrix);
  TGeoCombiTrans combi_trans_matrix_aligned(*matrix_aligned);
  if (combi_trans_matrix.IsTranslation() && combi_trans_matrix_aligned.IsTranslation()) {
    const double *pos = combi_trans_matrix.GetTranslation();
    const double *pos_aligned = combi_trans_matrix_aligned.GetTranslation();
    dx = pos[0] - pos_aligned[0];
    dy = pos[1] - pos_aligned[1];
    dz = pos[2] - pos_aligned[2];
  }
  if (combi_trans_matrix.IsRotation() && combi_trans_matrix_aligned.IsRotation()) {
    double rot[3], rot_aligned[3];
    combi_trans_matrix.GetRotation()->GetAngles(rot[0], rot[1], rot[2]);
    combi_trans_matrix_aligned.GetRotation()->GetAngles(rot_aligned[0], rot_aligned[1], rot_aligned[2]);
    dphi = rot[0] - rot_aligned[0];
    dtheta = rot[1] - rot_aligned[1];
    dpsi = rot[2] - rot_aligned[2];
  }
  /*if (ihalf==-1){
   dx = pos[0];
   dy = pos[1];
   dz = pos[2];
   dphi = rot[0];
   dtheta = rot[1];
   dpsi = rot[2];
   }*/
  // format the values to something meaning full for the lumi
  dx *= 10.e3; // cm -> µm
  dy *= 10.e3; // cm -> µm
  dz *= 10.e3; // cm -> µm

  dphi = dphi / 180. * pi * 1.e6;     // deg -> µrad
  dtheta = dtheta / 180. * pi * 1.e6; // deg -> µrad
  dpsi = dpsi / 180. * pi * 1.e6;     // deg -> µrad
  return result;
}

#include <TCanvas.h>
void PndLmdDim::Calc_matrix_offsets()
{
  // go through all available matrices and calculate the differences between in terms of
  // displacement and rotation
  int ihalf(-1), iplane(-1), imodule(-1), iside(-1), idie(-1), isensor(-1);
  double dx(0), dy(0), dz(0), dphi(0), dtheta(0), dpsi(0);
  TH1D hist_dx_sensors_local("hist_dx_sensors_local", "ref local;#Deltax [#mum]; entries", 100, -1e3, 1e3);
  TH1D hist_dx_sensors_global("hist_dx_sensors_lmd ", "ref lmd;#Deltax [#mum]; entries", 100, -1e3, 1e3);
  TH1D hist_dy_sensors_local("hist_dy_sensors_local", "ref local;#Deltay [#mum]; entries", 100, -1e3, 1e3);
  TH1D hist_dy_sensors_global("hist_dy_sensors_lmd", "ref lmd;#Deltay [#mum]; entries", 100, -1e3, 1e3);
  TH1D hist_dz_sensors_local("hist_dz_sensors_local", "ref local;#Deltaz [#mum]; entries", 100, -1e3, 1e3);
  TH1D hist_dz_sensors_global("hist_dz_sensors_lmd", "ref lmd;#Deltaz [#mum]; entries", 100, -1e3, 1e3);
  TH1D hist_dphi_sensors_local("hist_dphi_sensors_local", "ref local;#Delta#phi [#murad]; entries", 100, -1e3, 1e3);
  TH1D hist_dphi_sensors_global("hist_dphi_sensors_lmd", "ref lmd;#Delta#phi [#murad]; entries", 100, -1e3, 1e3);
  TH1D hist_dtheta_sensors_local("hist_dtheta_sensors_local", "ref local;#Delta#theta [#murad]; entries", 100, -1e3, 1e3);
  TH1D hist_dtheta_sensors_global("hist_dtheta_sensors_lmd", "ref lmd;#Delta#theta [#murad]; entries", 100, -1e3, 1e3);
  TH1D hist_dpsi_sensors_local("hist_dpsi_sensors_local", "ref local;#Delta#psi [#murad]; entries", 100, -1e3, 1e3);
  TH1D hist_dpsi_sensors_global("hist_dpsi_sensors_lmd", "ref lmd;#Delta#psi [#murad]; entries", 100, -1e3, 1e3);

  ofstream outfile("offsets.txt");
  if (outfile.is_open()) {
    outfile.setf(std::ios::fixed, std::ios::floatfield); // floatfield not set
    outfile.precision(7);

    if (Get_matrix_difference(ihalf, iplane, imodule, iside, idie, isensor, dx, dy, dz, dphi, dtheta, dpsi)) {
      outfile << " coordinate offset of the lmd reference system \n";
      outfile << " \u0394x [µm] \t \u0394y [µm] \t \u0394z [µm] \t \u0394\u03C6 [µrad] \t \u0394\u03D1 [µrad] \t \u0394\u03C8 [µrad] \t \n";
      outfile << dx << "\t" << dy << "\t" << dz << "\t" << dphi << "\t" << dtheta << "\t" << dpsi << "\n";
      outfile << endl;
    }
    for (ihalf = 0; ihalf < 2; ihalf++) {
      iplane = -1;
      imodule = -1;
      iside = -1;
      idie = -1;
      isensor = -1;
      if (Get_matrix_difference(ihalf, iplane, imodule, iside, idie, isensor, dx, dy, dz, dphi, dtheta, dpsi)) {
        outfile << "\t coordinate offset of the lmd half " << ihalf << " \n";
        outfile << "\t \u0394x [µm] \t \u0394y [µm] \t \u0394z [µm] \t \u0394\u03C6 [µrad] \t \u0394\u03D1 [µrad] \t \u0394\u03C8 [µrad] \t \n";
        outfile << "\t" << dx << "\t" << dy << "\t" << dz << "\t" << dphi << "\t" << dtheta << "\t" << dpsi << "\n";
        outfile << endl;
      }
      for (iplane = 0; iplane < (int)n_planes; iplane++) {
        imodule = -1;
        iside = -1;
        idie = -1;
        isensor = -1;
        if (Get_matrix_difference(ihalf, iplane, imodule, iside, idie, isensor, dx, dy, dz, dphi, dtheta, dpsi)) {
          outfile << "\t\t coordinate offset of the lmd plane " << iplane << " \n";
          outfile << "\t\t \u0394x [µm] \t \u0394y [µm] \t \u0394z [µm] \t \u0394\u03C6 [µrad] \t \u0394\u03D1 [µrad] \t \u0394\u03C8 [µrad] \t \n";
          outfile << "\t\t" << dx << "\t" << dy << "\t" << dz << "\t" << dphi << "\t" << dtheta << "\t" << dpsi << "\n";
          outfile << endl;
        }
        for (imodule = 0; imodule < n_cvd_discs / 2; imodule++) {
          iside = -1;
          idie = -1;
          isensor = -1;
          if (Get_matrix_difference(ihalf, iplane, imodule, iside, idie, isensor, dx, dy, dz, dphi, dtheta, dpsi)) {
            outfile << "\t\t\t coordinate offset of the lmd module " << imodule << " \n";
            outfile << "\t\t\t \u0394x [µm] \t \u0394y [µm] \t \u0394z [µm] \t \u0394\u03C6 [µrad] \t \u0394\u03D1 [µrad] \t \u0394\u03C8 [µrad] \t \n";
            outfile << "\t\t\t" << dx << "\t" << dy << "\t" << dz << "\t" << dphi << "\t" << dtheta << "\t" << dpsi << "\n";
            outfile << endl;
          }
          for (iside = 0; iside < 2; iside++) {
            idie = -1;
            isensor = -1;
            if (Get_matrix_difference(ihalf, iplane, imodule, iside, idie, isensor, dx, dy, dz, dphi, dtheta, dpsi)) {
              outfile << "\t\t\t\t coordinate offset of the lmd module side " << iside << " \n";
              outfile << "\t\t\t\t \u0394x [µm] \t \u0394y [µm] \t \u0394z [µm] \t \u0394\u03C6 [µrad] \t \u0394\u03D1 [µrad] \t \u0394\u03C8 [µrad] \t \n";
              outfile << "\t\t\t\t" << dx << "\t" << dy << "\t" << dz << "\t" << dphi << "\t" << dtheta << "\t" << dpsi << "\n";
              outfile << endl;
            }
            for (idie = 0; idie < 2; idie++) {
              isensor = -1;
              if (Get_matrix_difference(ihalf, iplane, imodule, iside, idie, isensor, dx, dy, dz, dphi, dtheta, dpsi)) {
                outfile << "\t\t\t\t\t coordinate offset of the lmd die " << idie << " \n";
                outfile << "\t\t\t\t\t \u0394x [µm] \t \u0394y [µm] \t \u0394z [µm] \t \u0394\u03C6 [µrad] \t \u0394\u03D1 [µrad] \t \u0394\u03C8 [µrad] \t \n";
                outfile << "\t\t\t\t\t" << dx << "\t" << dy << "\t" << dz << "\t" << dphi << "\t" << dtheta << "\t" << dpsi << "\n";
                outfile << endl;
              }
              for (isensor = 0; isensor < 3; isensor++) {
                if (Get_matrix_difference(ihalf, iplane, imodule, iside, idie, isensor, dx, dy, dz, dphi, dtheta, dpsi)) {
                  outfile << "\t\t\t\t\t\t coordinate offset of the lmd sensor " << isensor << " \n";
                  outfile << "\t\t\t\t\t\t \u0394x [µm] \t \u0394y [µm] \t \u0394z [µm] \t \u0394\u03C6 [µrad] \t \u0394\u03D1 [µrad] \t \u0394\u03C8 [µrad] \t \n";
                  outfile << "\t\t\t\t\t\t" << dx << "\t" << dy << "\t" << dz << "\t" << dphi << "\t" << dtheta << "\t" << dpsi << "\n";
                  outfile << endl;
                  hist_dx_sensors_local.Fill(dx);
                  hist_dy_sensors_local.Fill(dy);
                  hist_dz_sensors_local.Fill(dz);
                  hist_dphi_sensors_local.Fill(dphi);
                  hist_dtheta_sensors_local.Fill(dtheta);
                  hist_dpsi_sensors_local.Fill(dpsi);
                }
              } // loop over the sensor of one die
            }   // loop over the die of one side module
          }     // loop over the sides of one module
        }       // loop over the modules of one plane half
      }         // loop over the planes of the detector
    }           // loop over the halves of the detector
    outfile.close();
    TCanvas outcanvas("canvas", "canvas", 600, 600);
    hist_dx_sensors_local.Draw();
    outcanvas.Print("offsets.pdf(");
    hist_dy_sensors_local.Draw();
    outcanvas.Print("offsets.pdf(");
    hist_dz_sensors_local.Draw();
    outcanvas.Print("offsets.pdf(");
    hist_dphi_sensors_local.Draw();
    outcanvas.Print("offsets.pdf(");
    hist_dtheta_sensors_local.Draw();
    outcanvas.Print("offsets.pdf(");
    hist_dpsi_sensors_local.Draw();
    outcanvas.Print("offsets.pdf)");
  } else {
    cout << " sorry, could not write file into the current directory. " << endl;
    // if file is open
  }
}

TGeoMatrix *PndLmdDim::Get_matrix_global_to_lmd_local(bool aligned)
{
  return Get_matrix(-1, -1, -1, -1, -1, -1, aligned);
}

// TGeoMatrix* PndLmdDim::Get_matrix_global_to_sector_local(int ihalf, int iplane, int imodule, bool aligned){
//   return Get_matrix(ihalf, iplane, imodule,-1,-1,-1, aligned);
// }

TGeoMatrix *PndLmdDim::Get_matrix_lmd_local_to_module_side(int ihalf, int iplane, int imodule, int iside, bool aligned)
{
  return Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
}

TGeoMatrix *PndLmdDim::Get_matrix_module_side_to_sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  return Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
}

TGeoHMatrix PndLmdDim::Get_transformation_global_to_lmd_local(bool aligned)
{
  TGeoMatrix *matrix = Get_matrix(-1, -1, -1, -1, -1, -1, aligned);
  if (!matrix)
    return TGeoHMatrix();
  return TGeoHMatrix(*matrix);
}

TGeoHMatrix PndLmdDim::Get_transformation_lmd_local_to_module_side(int ihalf, int iplane, int imodule, int iside, bool aligned)
{
  TGeoMatrix *matrix = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
  if (!matrix)
    return TGeoHMatrix();
  return TGeoHMatrix(*matrix);
}

TGeoHMatrix PndLmdDim::Get_transformation_module_side_to_sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  TGeoMatrix *matrix = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
  if (!matrix)
    TGeoHMatrix();
  return TGeoHMatrix(*matrix);
}

TGeoHMatrix PndLmdDim::Get_transformation_global_to_sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  // instead of using the 3 methods to transform between the reference frames
  // pointers to the matrices are retrieved directly in order to gain a little
  // bit of performance (no construction and deletion of new matrices needed)
  TGeoMatrix *matrix1 = Get_matrix(-1, -1, -1, -1, -1, -1, aligned);
  TGeoMatrix *matrix2 = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
  TGeoMatrix *matrix3 = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
  if (!matrix1 || !matrix2 || !matrix3)
    return TGeoHMatrix();
  // return TGeoHMatrix(((*matrix1) * (*matrix2)) * (*matrix3));  //FIXME: I removed that line
  return TGeoHMatrix((*matrix1) * (*matrix2) * (*matrix3));
}

TGeoHMatrix PndLmdDim::Get_transformation_lmd_local_to_sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  TGeoMatrix *matrix2 = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
  TGeoMatrix *matrix3 = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
  if (!matrix2 || !matrix3)
    return TGeoHMatrix();
  return TGeoHMatrix((*matrix2) * (*matrix3));
}

TGeoHMatrix PndLmdDim::Get_transformation_lmd_local_to_global(bool aligned)
{
  TGeoMatrix *matrix = Get_matrix(-1, -1, -1, -1, -1, -1, aligned);
  if (!matrix)
    return TGeoHMatrix();
  return TGeoHMatrix(matrix->Inverse());
}

TGeoHMatrix PndLmdDim::Get_transformation_module_side_to_lmd_local(int ihalf, int iplane, int imodule, int iside, bool aligned)
{
  TGeoMatrix *matrix = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
  if (!matrix)
    TGeoHMatrix();
  return TGeoHMatrix(matrix->Inverse());
}

TGeoHMatrix PndLmdDim::Get_transformation_sensor_to_module_side(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  TGeoMatrix *matrix = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
  if (!matrix)
    return TGeoHMatrix();
  return TGeoHMatrix(matrix->Inverse());
}

TGeoHMatrix PndLmdDim::Get_transformation_sensor_to_global(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  // instead of using the 3 methods to transform between the reference frames
  // pointers to the matrices are retrieved directly in order to gain a little
  // bit of performance (no construction and deletion of new matrices needed)
  TGeoMatrix *matrix1 = Get_matrix(-1, -1, -1, -1, -1, -1, aligned);
  TGeoMatrix *matrix2 = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
  TGeoMatrix *matrix3 = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
  if (!matrix1 || !matrix2 || !matrix3)
    return TGeoHMatrix();
  return TGeoHMatrix(((*matrix1) * (*matrix2) * (*matrix3)).Inverse());
}

// FIXME: above and below a return is missing and was added;

TGeoHMatrix PndLmdDim::Get_transformation_sensor_to_lmd_local(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  TGeoMatrix *matrix2 = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
  TGeoMatrix *matrix3 = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
  if (!matrix2 || !matrix3)
    return TGeoHMatrix();
  return TGeoHMatrix(((*matrix2) * (*matrix3)).Inverse());
}

TGeoHMatrix PndLmdDim::Get_transformation_sensor_to_sensor_aligned(int ihalf, int iplane, int imodule, int iside, int idie, int isensor)
{
  // to do: optimize like in Get_transformation_sensor_to_global
  const TGeoHMatrix &matrix = Get_transformation_sensor_to_global(ihalf, iplane, imodule, iside, idie, isensor, false);
  const TGeoHMatrix &matrix_aligned = Get_transformation_global_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, true);
  return TGeoHMatrix(matrix * matrix_aligned);
}

TGeoHMatrix PndLmdDim::Get_transformation_sensor_aligned_to_sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor)
{
  // to do: optimize like in Get_transformation_sensor_to_global
  const TGeoHMatrix &matrix_aligned = Get_transformation_sensor_to_global(ihalf, iplane, imodule, iside, idie, isensor, true);
  const TGeoHMatrix &matrix = Get_transformation_global_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, false);
  return TGeoHMatrix(matrix_aligned * matrix);
}

TVector3 PndLmdDim::Transform_global_to_lmd_local(const TVector3 &point, bool isvector, bool aligned)
{
  const TGeoHMatrix &matrix = Get_transformation_global_to_lmd_local(aligned);
  double master[3];
  point.GetXYZ(master);
  double local[3];
  if (isvector)
    matrix.MasterToLocalVect(master, local);
  else
    matrix.MasterToLocal(master, local);
  return TVector3(local);
}

TVector3 PndLmdDim::Transform_lmd_local_to_module_side(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, bool isvector, bool aligned)
{
  const TGeoHMatrix &matrix = Get_transformation_lmd_local_to_module_side(ihalf, iplane, imodule, iside, aligned);
  double master[3];
  point.GetXYZ(master);
  double local[3];
  if (isvector)
    matrix.MasterToLocalVect(master, local);
  else
    matrix.MasterToLocal(master, local);
  return TVector3(local);
}

TVector3 PndLmdDim::Transform_module_side_to_sensor(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector, bool aligned)
{
  const TGeoHMatrix &matrix = Get_transformation_module_side_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned);
  double master[3];
  point.GetXYZ(master);
  double local[3];
  if (isvector)
    matrix.MasterToLocalVect(master, local);
  else
    matrix.MasterToLocal(master, local);
  return TVector3(local);
}

TVector3 PndLmdDim::Transform_global_to_sensor(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector, bool aligned)
{
  const TGeoHMatrix &matrix = Get_transformation_global_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned);
  double master[3];
  point.GetXYZ(master);
  double local[3];
  if (isvector)
    matrix.MasterToLocalVect(master, local);
  else
    matrix.MasterToLocal(master, local);
  return TVector3(local);
}

TVector3 PndLmdDim::Transform_lmd_local_to_global(const TVector3 &point, bool isvector, bool aligned)
{
  // I think Local to Master calculation is faster than the getter of the inverse matrix
  const TGeoHMatrix &matrix = Get_transformation_global_to_lmd_local(aligned);
  double local[3];
  point.GetXYZ(local);
  double master[3];
  if (isvector)
    matrix.LocalToMasterVect(local, master);
  else
    matrix.LocalToMaster(local, master);
  return TVector3(master);
}

TVector3 PndLmdDim::Transform_lmd_local_to_sensor(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector, bool aligned)
{
  const TGeoHMatrix &matrix = Get_transformation_lmd_local_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned);
  double master[3];
  point.GetXYZ(master);
  double local[3];
  if (isvector)
    matrix.MasterToLocalVect(master, local);
  else
    matrix.MasterToLocal(master, local);
  return TVector3(local);
}

TVector3 PndLmdDim::Transform_module_side_to_lmd_local(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, bool isvector, bool aligned)
{
  // I think Local to Master calculation is faster than the getter of the inverse matrix
  const TGeoHMatrix &matrix = Get_transformation_lmd_local_to_module_side(ihalf, iplane, imodule, iside, aligned);
  double local[3];
  point.GetXYZ(local);
  double master[3];
  if (isvector)
    matrix.LocalToMasterVect(local, master);
  else
    matrix.LocalToMaster(local, master);
  return TVector3(master);
}

TVector3 PndLmdDim::Transform_sensor_to_module_side(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector, bool aligned)
{
  // I think Local to Master calculation is faster than the getter of the inverse matrix
  const TGeoHMatrix &matrix = Get_transformation_module_side_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned);
  double local[3];
  point.GetXYZ(local);
  double master[3];
  if (isvector)
    matrix.LocalToMasterVect(local, master);
  else
    matrix.LocalToMaster(local, master);
  return TVector3(master);
}

TVector3 PndLmdDim::Transform_sensor_to_global(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector, bool aligned)
{
  // I think Local to Master calculation is faster than the getter of the inverse matrix
  const TGeoHMatrix &matrix = Get_transformation_global_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned);
  double local[3];
  point.GetXYZ(local);
  double master[3];
  if (isvector)
    matrix.LocalToMasterVect(local, master);
  else
    matrix.LocalToMaster(local, master);
  return TVector3(master);
}

TVector3 PndLmdDim::Transform_sensor_to_lmd_local(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector, bool aligned)
{
  const TGeoHMatrix &matrix = Get_transformation_lmd_local_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned);
  double local[3];
  point.GetXYZ(local);
  double master[3];
  if (isvector)
    matrix.LocalToMasterVect(local, master);
  else
    matrix.LocalToMaster(local, master);
  return TVector3(master);
}

TVector3 PndLmdDim::Transform_sensor_to_sensor_aligned(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector)
{
  const TGeoHMatrix &matrix = Get_transformation_sensor_to_sensor_aligned(ihalf, iplane, imodule, iside, idie, isensor);
  double master[3];
  point.GetXYZ(master);
  double local[3];
  if (isvector)
    matrix.MasterToLocalVect(master, local);
  else
    matrix.MasterToLocal(master, local);
  return TVector3(local);
}

TVector3 PndLmdDim::Transform_sensor_aligned_to_sensor(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector)
{
  const TGeoHMatrix &matrix = Get_transformation_sensor_aligned_to_sensor(ihalf, iplane, imodule, iside, idie, isensor);
  double master[3];
  point.GetXYZ(master);
  double local[3];
  if (isvector)
    matrix.MasterToLocalVect(master, local);
  else
    matrix.MasterToLocal(master, local);
  return TVector3(local);
}

TVector3 PndLmdDim::Transform_sensor_to_sensor(const TVector3 &point, int ihalf_from, int iplane_from, int imodule_from, int iside_from, int idie_from, int isensor_from,
                                               int ihalf_to, int iplane_to, int imodule_to, int iside_to, int idie_to, int isensor_to, bool isvector, bool aligned)
{
  const TGeoHMatrix &matrix_from = Get_transformation_sensor_to_lmd_local(ihalf_from, iplane_from, imodule_from, iside_from, idie_from, isensor_from, aligned);
  const TGeoHMatrix &matrix_to = Get_transformation_lmd_local_to_sensor(ihalf_to, iplane_to, imodule_to, iside_to, idie_to, isensor_to, aligned);
  TGeoHMatrix matrix(matrix_from * matrix_to);
  double master[3];
  point.GetXYZ(master);
  double local[3];
  if (isvector)
    matrix.MasterToLocalVect(master, local);
  else
    matrix.MasterToLocal(master, local);
  return TVector3(local);
}

TMatrixD PndLmdDim::Transform_global_to_lmd_local(const TMatrixD &matrix, bool aligned)
{
  TMatrixD rotmatrix(3, 3, Get_transformation_global_to_lmd_local(aligned).GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

TMatrixD PndLmdDim::Transform_lmd_local_to_module_side(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, bool aligned)
{
  TMatrixD rotmatrix(3, 3, Get_transformation_lmd_local_to_module_side(ihalf, iplane, imodule, iside, aligned).GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

TMatrixD PndLmdDim::Transform_module_side_to_sensor(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  TMatrixD rotmatrix(3, 3, Get_transformation_module_side_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned).GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

TMatrixD PndLmdDim::Transform_global_to_sensor(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  TMatrixD rotmatrix(3, 3, Get_transformation_global_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned).GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

TMatrixD PndLmdDim::Transform_lmd_local_to_sensor(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  TMatrixD rotmatrix(3, 3, Get_transformation_lmd_local_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned).GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

TMatrixD PndLmdDim::Transform_lmd_local_to_global(const TMatrixD &matrix, bool aligned)
{
  TMatrixD rotmatrix(3, 3, Get_transformation_lmd_local_to_global(aligned).GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

TMatrixD PndLmdDim::Transform_module_side_to_lmd_local(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, bool aligned)
{
  TMatrixD rotmatrix(3, 3, Get_transformation_module_side_to_lmd_local(ihalf, iplane, imodule, iside, aligned).GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

TMatrixD PndLmdDim::Transform_sensor_to_module_side(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  TMatrixD rotmatrix(3, 3, Get_transformation_sensor_to_module_side(ihalf, iplane, imodule, iside, idie, isensor, aligned).GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

TMatrixD PndLmdDim::Transform_sensor_to_global(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  TMatrixD rotmatrix(3, 3, Get_transformation_sensor_to_global(ihalf, iplane, imodule, iside, idie, isensor, aligned).GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

TMatrixD PndLmdDim::Transform_sensor_to_lmd_local(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned)
{
  TMatrixD rotmatrix(3, 3, Get_transformation_sensor_to_lmd_local(ihalf, iplane, imodule, iside, idie, isensor, aligned).GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

TMatrixD PndLmdDim::Transform_sensor_to_sensor_aligned(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor)
{
  TMatrixD rotmatrix(3, 3, Get_transformation_sensor_to_sensor_aligned(ihalf, iplane, imodule, iside, idie, isensor).GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

TMatrixD PndLmdDim::Transform_sensor_aligned_to_sensor(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor)
{
  TMatrixD rotmatrix(3, 3, Get_transformation_sensor_aligned_to_sensor(ihalf, iplane, imodule, iside, idie, isensor).GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

TMatrixD PndLmdDim::Transform_sensor_to_sensor(const TMatrixD &matrix, int ihalf_from, int iplane_from, int imodule_from, int iside_from, int idie_from, int isensor_from,
                                               int ihalf_to, int iplane_to, int imodule_to, int iside_to, int idie_to, int isensor_to, bool aligned)
{
  TMatrixD rotmatrix(3, 3,
                     (Get_transformation_sensor_to_lmd_local(ihalf_from, iplane_from, imodule_from, iside_from, idie_from, isensor_from, aligned) *
                      Get_transformation_lmd_local_to_sensor(ihalf_to, iplane_to, imodule_to, iside_to, idie_to, isensor_to, aligned))
                       .GetRotationMatrix());
  return TMatrixD(rotmatrix * TMatrixD(matrix, TMatrixD::kMultTranspose, rotmatrix));
}

void PndLmdDim::Test_matrices()
{
  // TVector3 from(1.,2.,3.);
  // TVector3 to = Transform_global_to_lmd_local(from);
  // to = Transform_lmd_local_to_global(to);
  // if ((from-to).Mag() > 1e7) cout << " error " << endl;
  for (unsigned int ihalf = 0; ihalf < 2; ihalf++)
    for (unsigned int iplane = 0; iplane < n_planes; iplane++)
      for (unsigned int imodule = 0; imodule < nmodules; imodule++)
        for (unsigned int iside = 0; iside < 2; iside++)
          for (unsigned int idie = 0; idie < 2; idie++)
            for (unsigned int isensor = 0; isensor < 3; isensor++) {
              if (idie == 1 && isensor == 0)
                continue;
              TVector3 from_sens(1., 2., 3.);
              TVector3 to_sens = Transform_global_to_sensor(from_sens, ihalf, iplane, imodule, iside, idie, isensor);
              to_sens = Transform_sensor_to_global(to_sens, ihalf, iplane, imodule, iside, idie, isensor);
              if ((from_sens - to_sens).Mag() > 1e7)
                cout << " error " << endl;
            }
}

void PndLmdDim::Draw_Sensors(int iplane, bool aligned, bool lmd_frame, int glside)
{

  for (unsigned int ihalf = 0; ihalf < 2; ihalf++)
    // for (unsigned int iplane = 0; iplane < n_planes; iplane++)
    for (unsigned int imodule = 0; imodule < nmodules; imodule++) {
      if (glside > 1) { // default
        for (unsigned int iside = 0; iside < 2; iside++) {
          for (unsigned int idie = 0; idie < 2; idie++) {
            for (unsigned int isensor = 0; isensor < 3; isensor++)
            // for (unsigned int sensorID = 0; sensorID < 400 ; sensorID++)
            {
              // cout << " sensID " << sensorID << endl;
              // int ihalf, _iplane, imodule, iside, idie, isensor;
              // Get_sensor_by_id(sensorID, ihalf, _iplane, imodule, iside, idie, isensor);
              // cout /*<< sensorID*/ << " " << ihalf << " " << iplane << " " << imodule << " " << iside << " " << idie << " " << isensor << endl;
              // if (iplane != _iplane) continue;
              // cout << " " << ihalf << " " << iplane << endl;
              if (idie == 1 && isensor == 0)
                continue;
              TPolyLine *sensor_shape = Get_Sensor_Shape(ihalf, iplane, imodule, iside, idie, isensor, aligned, lmd_frame);
              if (iside == 1)
                sensor_shape->SetLineColor(17);
              else
                sensor_shape->SetLineColor(13);
              sensor_shape->Draw();
            }
          }
        }
      } // default
      else {
        int iside = glside;
        for (unsigned int idie = 0; idie < 2; idie++) {
          for (unsigned int isensor = 0; isensor < 3; isensor++) {
            if (idie == 1 && isensor == 0)
              continue;
            TPolyLine *sensor_shape = Get_Sensor_Shape(ihalf, iplane, imodule, iside, idie, isensor, aligned, lmd_frame);
            if (iside == 1)
              sensor_shape->SetLineColor(17);
            else
              sensor_shape->SetLineColor(13);
            sensor_shape->Draw();
          }
        }
      }
    }
}

TPolyLine *PndLmdDim::Get_Sensor_Shape(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned, bool lmd_frame)
{
  Double_t x[5] = {-maps_width + 2 * maps_passive_left, maps_width - 2 * maps_passive_right, maps_width - 2 * maps_passive_right, -maps_width + 2 * maps_passive_left,
                   -maps_width + 2 * maps_passive_right};
  Double_t y[5] = {-maps_height + 2 * maps_passive_bottom, -maps_height + 2 * maps_passive_bottom, maps_height - 2 * maps_passive_top, maps_height - 2 * maps_passive_top,
                   -maps_height + 2 * maps_passive_bottom};
  for (unsigned int ipoint = 0; ipoint < 5; ipoint++) {
    TVector3 point(x[ipoint], y[ipoint], 0);
    TVector3 point_master;
    if (lmd_frame) {
      point_master = Transform_sensor_to_lmd_local(point, ihalf, iplane, imodule, iside, idie, isensor, false, aligned);
    } else {
      point_master = Transform_sensor_to_global(point, ihalf, iplane, imodule, iside, idie, isensor, false, aligned);
    }
    x[ipoint] = point_master.X();
    y[ipoint] = point_master.Y();
  }
  TPolyLine *pline = new TPolyLine(5, x, y);
  // pline->SetFillColor(38);
  pline->SetLineColor(2);
  pline->SetLineWidth(1);
  // pline->Draw("f");
  // pline->Draw();
  // gPad->Update();
  return pline;
}

vector<TGraph *> PndLmdDim::Get_Sensor_Graph(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned, bool lmd_frame, bool pixel_subdivision)
{
  // stringstream name;
  // name << "multigraph_half_" << ihalf << "_plane_" << iplane << "_module_" << imodule << "_side_" << iside << "_die_" << idie << "_sensor_" << isensor;
  // stringstream title;
  // title << "multi graph for sensor " << Get_sensor_id(ihalf, iplane, imodule, iside, idie, isensor);
  vector<TGraph *> result; // = new TMultiGraph(name.str().c_str(), title.str().c_str());
  vector<double *> xs;
  vector<double *> ys;
  // create the passive area
  // top
  Double_t x_top[5] = {-maps_width, maps_width, maps_width, -maps_width, -maps_width};
  Double_t y_top[5] = {maps_height - maps_passive_top * 2., maps_height - maps_passive_top * 2., maps_height, maps_height, maps_height - maps_passive_top * 2.};
  xs.push_back(x_top);
  ys.push_back(y_top);
  // bottom
  Double_t x_bottom[5] = {-maps_width, maps_width, maps_width, -maps_width, -maps_width};
  Double_t y_bottom[5] = {-maps_height + maps_passive_bottom * 2., -maps_height + maps_passive_bottom * 2., -maps_height, -maps_height, -maps_height + maps_passive_bottom * 2.};
  xs.push_back(x_bottom);
  ys.push_back(y_bottom);
  // left
  Double_t x_left[5] = {-maps_width, -maps_width + maps_passive_left * 2, -maps_width + maps_passive_left * 2, -maps_width, -maps_width};
  Double_t y_left[5] = {-maps_height + maps_passive_bottom * 2., -maps_height + maps_passive_bottom * 2., maps_height - maps_passive_top * 2., maps_height - maps_passive_top * 2.,
                        -maps_height + maps_passive_bottom * 2.};
  xs.push_back(x_left);
  ys.push_back(y_left);
  // right
  Double_t x_right[5] = {+maps_width - maps_passive_right * 2., +maps_width, +maps_width, +maps_width - maps_passive_right * 2., +maps_width - maps_passive_right * 2.};
  Double_t y_right[5] = {-maps_height + maps_passive_bottom * 2., -maps_height + maps_passive_bottom * 2., maps_height - maps_passive_top * 2., maps_height - maps_passive_top * 2.,
                         -maps_height + maps_passive_bottom * 2.};
  xs.push_back(x_right);
  ys.push_back(y_right);
  // the pixels
  int ncols = (int)floor(maps_active_width * 2. / maps_active_pixel_size);
  int nrows = (int)floor(maps_active_height * 2. / maps_active_pixel_size);
  if (!pixel_subdivision) {
    // create only one bin for the active area
    TPolyLine *shape = Get_Sensor_Shape(ihalf, iplane, imodule, iside, idie, isensor, aligned, lmd_frame);
    double *x = shape->GetX();
    double *y = shape->GetY();
    double n = shape->GetN();
    result.push_back(new TGraph(n, x, y));

  } else {
    cout << "creating " << nrows << " rows x " << ncols << " cols of pixels " << endl;
    for (int irow = 0; irow < nrows; irow++) {
      // cout << ".";
      // flush(cout);
      for (int icol = 0; icol < ncols; icol++) {
        // put it on the stack instead of the heap
        // to have it still outside this loop scope
        double *x = new double[5];
        double *y = new double[5];
        x[0] = -maps_width + maps_passive_left * 2. + icol * maps_active_pixel_size;
        x[1] = x[0] + maps_active_pixel_size;
        x[2] = x[1];
        x[3] = x[0];
        x[4] = x[0];
        xs.push_back(x);
        y[0] = -maps_height + maps_passive_bottom * 2. + irow * maps_active_pixel_size;
        y[1] = y[0];
        y[2] = y[0] + maps_active_pixel_size;
        y[3] = y[2];
        y[4] = y[0];
        ys.push_back(y);
      }
    }
  }
  // now transform (if requested) into the corresponding reference frame
  // cout << " transforming "  << endl;
  for (unsigned int ipoints = 0; ipoints < xs.size(); ipoints++) {
    // cout << ".";
    // flush(cout);
    for (unsigned int ipoint = 0; ipoint < 5; ipoint++) {
      TVector3 point(xs[ipoints][ipoint], ys[ipoints][ipoint], 0);
      TVector3 point_master;
      if (lmd_frame) {
        point_master = Transform_sensor_to_lmd_local(point, ihalf, iplane, imodule, iside, idie, isensor, false, aligned);
      } else {
        point_master = Transform_sensor_to_global(point, ihalf, iplane, imodule, iside, idie, isensor, false, aligned);
      }
      xs[ipoints][ipoint] = point_master.X();
      ys[ipoints][ipoint] = point_master.Y();
    }
    result.push_back(new TGraph(5, xs[ipoints], ys[ipoints]));
    // clean up
    if (ipoints > 3) {
      delete[] xs[ipoints];
      delete[] ys[ipoints];
    }
  }
  return result;
}

TH2Poly *PndLmdDim::Get_histogram_Plane(int iplane, int iside, bool aligned, bool lmd_frame, bool pixel_subdivision)
{
  stringstream name;
  name << "histpoly_plane_" << iplane << "_side_" << iside;
  stringstream title;
  title << "plane " << iplane << " side " << iside;
  TH2Poly *result = new TH2Poly();
  result->SetNameTitle(name.str().c_str(), title.str().c_str());
  result->SetFloat();
  result->SetXTitle("x [cm]");
  result->SetYTitle("y [cm]");

  for (unsigned int ihalf = 0; ihalf < 2; ihalf++)
    // for (unsigned int iplane = 0; iplane < n_planes; iplane++)
    for (unsigned int imodule = 0; imodule < nmodules; imodule++)
      // for (unsigned int iside = 0; iside < 2; iside++)
      for (unsigned int idie = 0; idie < 2; idie++)
        for (unsigned int isensor = 0; isensor < 3; isensor++) {
          if (idie == 1 && isensor == 0)
            continue;
          vector<TGraph *> sensor_graph = Get_Sensor_Graph(ihalf, iplane, imodule, iside, idie, isensor, aligned, lmd_frame, pixel_subdivision);
          for (size_t igraph = 0; igraph < sensor_graph.size(); igraph++) {
            result->AddBin(sensor_graph[igraph]);
            // delete sensor_graph[igraph]; is owned by a PolyBin, so don't delete it
          }
        }
  return result;
}

TH2Poly *PndLmdDim::Get_histogram_Moduleside(int ihalf, int iplane, int imodule, int iside, bool aligned, bool lmd_frame, bool pixel_subdivision)
{
  stringstream name;
  name << "histpoly_half_" << ihalf << "_plane_" << iplane << "_module_" << imodule << "_side_" << iside;
  stringstream title;
  title << "half " << ihalf << " plane " << iplane << " module " << imodule << " side " << iside;
  TH2Poly *result = new TH2Poly();
  result->SetNameTitle(name.str().c_str(), title.str().c_str());
  result->SetFloat();
  result->SetXTitle("x [cm]");
  result->SetYTitle("y [cm]");

  // for (unsigned int ihalf = 0; ihalf < 2; ihalf++)
  // for (unsigned int iplane = 0; iplane < n_planes; iplane++)
  // for (unsigned int imodule = 0; imodule < nmodules; imodule++)
  // for (unsigned int iside = 0; iside < 2; iside++)
  for (unsigned int idie = 0; idie < 2; idie++)
    for (unsigned int isensor = 0; isensor < 3; isensor++) {
      if (idie == 1 && isensor == 0)
        continue;
      vector<TGraph *> sensor_graph = Get_Sensor_Graph(ihalf, iplane, imodule, iside, idie, isensor, aligned, lmd_frame, pixel_subdivision);
      for (size_t igraph = 0; igraph < sensor_graph.size(); igraph++) {
        result->AddBin(sensor_graph[igraph]);
        // delete sensor_graph[igraph]; is owned by a PolyBin, so don't delete it
      }
    }
  return result;
}

TH2Poly *PndLmdDim::Get_histogram_Sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned, bool lmd_frame)
{
  stringstream name;
  name << "histpoly_sensor_half_" << ihalf << "_plane_" << iplane << "_module_" << imodule << "_side_" << iside << "_die_" << idie << "_sensor_" << isensor;
  stringstream title;
  title << "sensor " << Get_sensor_id(ihalf, iplane, imodule, iside, idie, isensor);
  TH2Poly *result = new TH2Poly();
  result->SetNameTitle(name.str().c_str(), title.str().c_str());
  result->SetFloat();
  result->SetXTitle("x [cm]");
  result->SetYTitle("y [cm]");

  vector<TGraph *> sensor_graph = Get_Sensor_Graph(ihalf, iplane, imodule, iside, idie, isensor, aligned, lmd_frame, true);
  for (size_t igraph = 0; igraph < sensor_graph.size(); igraph++) {
    // cout << " adding bins " << endl;
    result->AddBin(sensor_graph[igraph]);
    // delete sensor_graph[igraph]; is owned by a PolyBin, so don't delete it
  }
  return result;
}

int PndLmdDim::makeOverlapID(int firstSensorId, int secondSensorId)
{

  int fhalf, fplane, fmodule, fside, fdie, fsensor;
  int bhalf, bplane, bmodule, bside, bdie, bsensor;

  int smalloverlap = 0;

  Get_sensor_by_id(firstSensorId, fhalf, fplane, fmodule, fside, fdie, fsensor);
  Get_sensor_by_id(secondSensorId, bhalf, bplane, bmodule, bside, bdie, bsensor);

  if (fside == bside) {
    return -1;
  }

  // should return the same id for (id1, id2) as for (id2, id1)
  if (bside < fside) {
    std::swap(firstSensorId, secondSensorId);
    Get_sensor_by_id(firstSensorId, fhalf, fplane, fmodule, fside, fdie, fsensor);
    Get_sensor_by_id(secondSensorId, bhalf, bplane, bmodule, bside, bdie, bsensor);
  }

  if (bhalf != fhalf) {
    return -1;
  }
  if (bplane != fplane) {
    return -1;
  }
  if (bmodule != fmodule) {
    return -1;
  }

  // 0to5
  if (fdie == 0 && fsensor == 0 && bdie == 0 && bsensor == 0) {
    smalloverlap = 0;
  }
  // 3to8
  else if (fdie == 1 && fsensor == 1 && bdie == 1 && bsensor == 1) {
    smalloverlap = 1;
  }
  // 4to9
  else if (fdie == 1 && fsensor == 2 && bdie == 1 && bsensor == 2) {
    smalloverlap = 2;
  }
  // 3to6
  else if (fdie == 1 && fsensor == 1 && bdie == 0 && bsensor == 1) {
    smalloverlap = 3;
  }
  // 1to8
  else if (fdie == 0 && fsensor == 1 && bdie == 1 && bsensor == 1) {
    smalloverlap = 4;
  }
  // 2to8
  else if (fdie == 0 && fsensor == 2 && bdie == 1 && bsensor == 1) {
    smalloverlap = 5;
  }
  // 2to9
  else if (fdie == 0 && fsensor == 2 && bdie == 1 && bsensor == 2) {
    smalloverlap = 6;
  }
  // 3to7
  else if (fdie == 1 && fsensor == 1 && bdie == 0 && bsensor == 2) {
    smalloverlap = 7;
  }
  // 4to7
  else if (fdie == 1 && fsensor == 2 && bdie == 0 && bsensor == 2) {
    smalloverlap = 8;
  } else {
    // all other checks are negative? then the sensors don't overlap!
    return -1;
  }
  return 1000 * fhalf + 100 * fplane + 10 * fmodule + smalloverlap;
}

int PndLmdDim::getID1fromOverlapID(int overlapID)
{

  int fhalf, fplane, fmodule, fside, fdie, fsensor;
  // int bhalf, bplane, bmodule, bside, bdie, bsensor; //[R.K.03/2017] unused variable

  // get info from overlapID
  // pad overlapID to 4 digits

  fhalf = std::floor(overlapID / 1000);          // = bhalf //[R.K.03/2017] unused variable
  fplane = std::floor((overlapID % 1000) / 100); // = bplane //[R.K.03/2017] unused variable
  fmodule = std::floor((overlapID % 100) / 10);  // = bmodule //[R.K.03/2017] unused variable
  int smalloverlap = std::floor(overlapID % 10);

  fside = 0;
  // bside = 1; //[R.K.03/2017] unused variable

  if (smalloverlap == 0) {
    fdie = 0;
    fsensor = 0;
    // bdie = 0; //[R.K.03/2017] unused variable
    // bsensor = 0; //[R.K.03/2017] unused variable
  } else if (smalloverlap == 1) {
    fdie = 1;
    fsensor = 1;
    // bdie = 1; //[R.K.03/2017] unused variable
    // bsensor = 1; //[R.K.03/2017] unused variable
  } else if (smalloverlap == 2) {
    fdie = 1;
    fsensor = 2;
    // bdie = 1; //[R.K.03/2017] unused variable
    // bsensor = 2; //[R.K.03/2017] unused variable
  } else if (smalloverlap == 3) {
    fdie = 1;
    fsensor = 1;
    // bdie = 0; //[R.K.03/2017] unused variable
    // bsensor = 1; //[R.K.03/2017] unused variable
  } else if (smalloverlap == 4) {
    fdie = 0;
    fsensor = 1;
    // bdie = 1; //[R.K.03/2017] unused variable
    // bsensor = 1; //[R.K.03/2017] unused variable
  } else if (smalloverlap == 5) {
    fdie = 0;
    fsensor = 2;
    // bdie = 1; //[R.K.03/2017] unused variable
    // bsensor = 1; //[R.K.03/2017] unused variable
  } else if (smalloverlap == 6) {
    fdie = 0;
    fsensor = 2;
    // bdie = 1; //[R.K.03/2017] unused variable
    // bsensor = 2; //[R.K.03/2017] unused variable
  } else if (smalloverlap == 7) {
    fdie = 1;
    fsensor = 1;
    // bdie = 0; //[R.K.03/2017] unused variable
    // bsensor = 2; //[R.K.03/2017] unused variable
  } else if (smalloverlap == 8) {
    fdie = 1;
    fsensor = 2;
    // bdie = 0; //[R.K.03/2017] unused variable
    // bsensor = 2; //[R.K.03/2017] unused variable
  } else {
    return -1;
  }

  int id = Get_sensor_id(fhalf, fplane, fmodule, fside, fdie, fsensor);
  return id;
}

int PndLmdDim::getID2fromOverlapID(int overlapID)
{
  // int fhalf, fplane, fmodule, fside, fdie, fsensor; //[R.K.03/2017] unused variable
  int bhalf, bplane, bmodule, bside, bdie, bsensor;

  // get info from overlapID
  // pad overlapID to 4 digits

  bhalf = std::floor(overlapID / 1000);          // fhalf = //[R.K.03/2017] unused variable
  bplane = std::floor((overlapID % 1000) / 100); // fplane = //[R.K.03/2017] unused variable
  bmodule = std::floor((overlapID % 100) / 10);  // fmodule = //[R.K.03/2017] unused variable
  int smalloverlap = std::floor(overlapID % 10);

  // fside = 0; //[R.K.03/2017] unused variable
  bside = 1;

  if (smalloverlap == 0) {
    // fdie = 0; //[R.K.03/2017] unused variable
    // fsensor = 0; //[R.K.03/2017] unused variable
    bdie = 0;
    bsensor = 0;
  } else if (smalloverlap == 1) {
    // fdie = 1; //[R.K.03/2017] unused variable
    // fsensor = 1; //[R.K.03/2017] unused variable
    bdie = 1;
    bsensor = 1;
  } else if (smalloverlap == 2) {
    // fdie = 1; //[R.K.03/2017] unused variable
    // fsensor = 2; //[R.K.03/2017] unused variable
    bdie = 1;
    bsensor = 2;
  } else if (smalloverlap == 3) {
    // fdie = 1; //[R.K.03/2017] unused variable
    // fsensor = 1; //[R.K.03/2017] unused variable
    bdie = 0;
    bsensor = 1;
  } else if (smalloverlap == 4) {
    // fdie = 0; //[R.K.03/2017] unused variable
    // fsensor = 1; //[R.K.03/2017] unused variable
    bdie = 1;
    bsensor = 1;
  } else if (smalloverlap == 5) {
    // fdie = 0; //[R.K.03/2017] unused variable
    // fsensor = 2; //[R.K.03/2017] unused variable
    bdie = 1;
    bsensor = 1;
  } else if (smalloverlap == 6) {
    // fdie = 0; //[R.K.03/2017] unused variable
    // fsensor = 2; //[R.K.03/2017] unused variable
    bdie = 1;
    bsensor = 2;
  } else if (smalloverlap == 7) {
    // fdie = 1; //[R.K.03/2017] unused variable
    // fsensor = 1; //[R.K.03/2017] unused variable
    bdie = 0;
    bsensor = 2;
  } else if (smalloverlap == 8) {
    // fdie = 1; //[R.K.03/2017] unused variable
    // fsensor = 2; //[R.K.03/2017] unused variable
    bdie = 0;
    bsensor = 2;
  } else {
    return -1;
  }

  int id = Get_sensor_id(bhalf, bplane, bmodule, bside, bdie, bsensor);
  return id;
}

int PndLmdDim::makeModuleID(int overlapID)
{
  int moduleID = std::floor(overlapID / 10.0);
  return moduleID;
}

std::vector<int> PndLmdDim::getAvailableOverlapIDs()
{
  std::vector<int> result;
  int overlapID;

  for (int iHalf = 0; iHalf < 2; iHalf++) {
    for (int iPlane = 0; iPlane < n_planes; iPlane++) {
      for (int iModule = 0; iModule < nmodules; iModule++) {
        for (int iOverlap = 0; iOverlap < 9; iOverlap++) {
          overlapID = 1000 * iHalf + 100 * iPlane + 10 * iModule + iOverlap;
          result.push_back(overlapID);
        }
      }
    }
  }
  return result;
}

//
