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

#ifndef GeCluster_h
#define GeCluster_h

/* #include "geant4/globals.hh" */
/* #include "geant4/G4LogicalVolume.hh" */
#include "TList.h"

class TGeoVolume;
class TGeoSphere;
class TGeoTube;
class TGeoPgon;
class TGeoCompositeShape;
class CbmHypGe;

// Dummy declarations to get rid of warnings

class GeCluster : public TObject {

 public:
  GeCluster();
  virtual ~GeCluster();

  Double_t X_Crystal[105];
  Double_t Y_Crystal[105];
  Double_t Z_Crystal[105];

  int Id_Crystal[105];

  int Id_Cluster[105];
  int Id_Crystal_In_Cluster[105];

  void SetPathGeo(TString path) { fpath = path; };

  TList *CreateCluster(TGeoPgon *logicCrystal_test, TGeoPgon *lay_sci, TGeoCompositeShape *lay_cap, TGeoVolume &sphere, double distance, double theta, double phi,
                       double depth_first_interaction, int id_start);

  /*  TList  *CreateCluster(TGeoCompositeShape* logicCrystal_test, */
  /* 		    TGeoVolume& sphere, */
  /* 		    double distance,  */
  /* 		    double theta,  */
  /* 		    double phi,  */
  /* 		    double depth_first_interaction,  */
  /* 		    int id_start);  */

  /*  void CreateCluster(TGeoCompositeShape* logicCrystal_test, */
  /* 		    TGeoVolume& sphere, */
  /* 		    double distance,  */
  /* 		    double theta,  */
  /* 		    double phi,  */
  /* 		    double depth_first_interaction,  */
  /* 		    int id_start);  */

  ClassDef(GeCluster, 2);

 private:
  int fnum_crystal;
  int fnum_cluster;
  TString fpath;

  // TGeoVolume*   sphere;    // pointer to the logical envelope

  // GeCrystal
  // TGeoTube*            Crystal_Tube;   // pointer to the solid Target
  // TGeoPgon*       Crystal_Polyhedra;   // pointer to the solid Target
  // TGeoCompositeShape*  logicCrystal_test;   //

  TGeoVolume *crystal_centre[105]; // pointer to the physical Target
  TGeoVolume *crystal[105];        // pointer to the physical Target
  TGeoVolume *laySci[105];         // pointer to the physical Target
  TGeoVolume *laySci_peri[105];    // periferical physical Target
  TGeoVolume *Cap[105];            // pointer to the physical Target
  TGeoVolume *Cap_peri[105];       // periferical physical Target
  TList *SenVolList;
};

#endif
