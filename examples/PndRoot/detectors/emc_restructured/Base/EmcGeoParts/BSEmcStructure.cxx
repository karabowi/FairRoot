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

//======================================================================
// Description:
//       Class BSEmcStructure
//
//======================================================================

#include <algorithm>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <utility>
#include <vector>

#include "TGeoBBox.h"
#include "TGeoShape.h"
#include "TGeoVolume.h"

#include "fairlogger/Logger.h"

//-----------------------
// This Class's Header --
//-----------------------
#include "TGeoArb8.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoNode.h"
#include "TMath.h"
#include "TRotation.h"
#include "TString.h"
#include "TVector3.h"

#include "PndGeoHandling.h"

#include "BSEmcDetectorID.h"
#include "BSEmcStructure.h"
#include "BSEmcXtal.h"

#include "stdlib.h"

template <typename T>
Int_t getsign(const T &a)
{
  return (a > 0 ? 1 : a < 0 ? -1 : 0);
}

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
BSEmcStructure *BSEmcStructure::fInstance = nullptr;

BSEmcStructure::~BSEmcStructure() {}

BSEmcStructure *BSEmcStructure::Instance(TGeoManager *t_geoMan)
{
  if (fInstance == nullptr) {
    if (t_geoMan == nullptr) {
      LOG(error) << "Empty geometry passed to BSEmcStructure";
      abort();
    } else {
      fInstance = new BSEmcStructure(t_geoMan);
    }
  }
  return fInstance;
}

// If BSEmcStructure is called without input parameters
// it means that it should have been instantiated with TGeoManager before or use gGeoManager

BSEmcStructure *BSEmcStructure::Instance()
{
  if (fInstance == nullptr) {
    TGeoManager *geoMan = gGeoManager;
    if (geoMan == nullptr) {
      LOG(error) << "BSEmcStructure::Instance() - gGeoManager does not exist";
      abort();
    } else {
      fInstance = new BSEmcStructure(geoMan);
    }
  }
  return fInstance;
}

BSEmcStructure::BSEmcStructure(TGeoManager * /*t_geoMan*/) : fRescaleFactor(1.0), fDetectorIdXtalMap() {}

void BSEmcStructure::ParseVolumeTree()
{
  LOG(debug) << "BSEmcStructure::BSEmcStructure() Instance being constructed.";

  TString node_path;
  const TGeoMatrix *crystal_matrix = nullptr;
  const Double_t *trans = nullptr;
  TGeoRotation geoRot;
  TRotation rot;
  Int_t detId = 0;

  TGeoIterator next{gGeoManager->GetTopVolume()};
  TGeoNode *node = nullptr;
  std::vector<Int_t> seenDetIds;
  while ((node = next()) != nullptr) {
    next.GetPath(node_path);
    node_path = node_path.ReplaceAll("cave/", "/cave_1/");

    detId = PndGeoHandling::Instance()->GetShortID(node_path);
    const Int_t lowestEmcDetId = 100000000;
    const Int_t highestEmcDetId = 600000000;
    if (detId > lowestEmcDetId && detId < highestEmcDetId) {
      LOG(debug3) << "BSEmcStructure::BSEmcStructure()"
                  << " Found detId " << detId << " for " << node_path;

      auto p = std::find(seenDetIds.begin(), seenDetIds.end(), detId);
      if (p == seenDetIds.end()) {
        seenDetIds.push_back(detId);
      } else {
        LOG(warn) << "BSEmcStructure::BSEmcStructure: Found " << detId << " twice! Continuing.";
        continue;
      }
      // Obtaine TGeoMatrix matrix for the current node, later it is factorised to translation and rotation
      crystal_matrix = next.GetCurrentMatrix();

      trans = crystal_matrix->GetTranslation();
      TVector3 pos{trans[0], trans[1], trans[2]};
      geoRot.SetMatrix(crystal_matrix->GetRotationMatrix());
      // Check the geometry type and convert to TGeoTrap.
      std::unique_ptr<TGeoTrap> trap{nullptr};
      TString shapeType = node->GetVolume()->GetShape()->ClassName();
      if (shapeType == "TGeoTrap") {
        TGeoTrap *tmp = dynamic_cast<TGeoTrap *>(node->GetVolume()->GetShape());
        fDetectorIdXtalMap[detId] = std::unique_ptr<BSEmcXtal>(new BSEmcXtal(detId, *tmp, pos, geoRot));
      } else if (shapeType == "TGeoArb8") {
        trap.reset(CreateFromArb8(node->GetVolume()->GetShape()));
        fDetectorIdXtalMap[detId] = std::unique_ptr<BSEmcXtal>(new BSEmcXtal(detId, *trap, pos, geoRot));
      } else if (shapeType == "TGeoBBox" || shapeType == "TGeoScaledShape") {
        trap.reset(CreateFromTGeoBBox(node->GetVolume()->GetShape()));
        fDetectorIdXtalMap[detId] = std::unique_ptr<BSEmcXtal>(new BSEmcXtal(detId, *trap, pos, geoRot));
      } else {
        LOG(error) << "BSEmcStructure::BSEmcStructure: Unknown geometry type " << shapeType << " for node: " << node_path;
        // abort();
      }
    }
  }
}

TGeoTrap *BSEmcStructure::CreateFromArb8(TGeoShape *t_shape) const
{
  TGeoArb8 *arb8 = dynamic_cast<TGeoArb8 *>(t_shape);

  // Approximate the shape with a TGeoTrap.

  Double_t *verts = arb8->GetVertices();

  Double_t dz = arb8->GetDz();
  Double_t tx =
    (verts[4 * 2 + 0] + verts[5 * 2 + 0] + verts[6 * 2 + 0] + verts[7 * 2 + 0] - verts[0 * 2 + 0] - verts[1 * 2 + 0] - verts[2 * 2 + 0] - verts[3 * 2 + 0]) / (2 * 4 * dz);
  Double_t ty =
    (verts[4 * 2 + 1] + verts[5 * 2 + 1] + verts[6 * 2 + 1] + verts[7 * 2 + 1] - verts[0 * 2 + 1] - verts[1 * 2 + 1] - verts[2 * 2 + 1] - verts[3 * 2 + 1]) / (2 * 4 * dz);

  Double_t thetac = TMath::ATan(TMath::Sqrt(tx * tx + ty * ty)) * TMath::RadToDeg();
  Double_t phic = TMath::ATan2(ty, tx) * TMath::RadToDeg();

  Double_t h1 = (verts[1 * 2 + 1] + verts[2 * 2 + 1] - verts[0 * 2 + 1] - verts[3 * 2 + 1]) / (2 * 2);
  Double_t bl1 = (verts[3 * 2 + 0] - verts[0 * 2 + 0]) / 2;
  Double_t tl1 = (verts[2 * 2 + 0] - verts[1 * 2 + 0]) / 2;
  Double_t alpha1 = TMath::ATan((verts[1 * 2 + 0] + verts[2 * 2 + 0] - verts[0 * 2 + 0] - verts[3 * 2 + 0]) / (2 * 2 * h1)) * TMath::RadToDeg();

  Double_t h2 = (verts[5 * 2 + 1] + verts[6 * 2 + 1] - verts[4 * 2 + 1] - verts[7 * 2 + 1]) / (2 * 2);
  Double_t bl2 = (verts[7 * 2 + 0] - verts[4 * 2 + 0]) / 2;
  Double_t tl2 = (verts[6 * 2 + 0] - verts[5 * 2 + 0]) / 2;
  Double_t alpha2 = TMath::ATan((verts[5 * 2 + 0] + verts[6 * 2 + 0] - verts[4 * 2 + 0] - verts[7 * 2 + 0]) / (2 * 2 * h2)) * TMath::RadToDeg();
  LOG(trace) << "Creating a TGeoTrap() from a TGeoArb8: \n"
             << "dz: " << dz << "\nthetac: " << thetac << "\nphic: " << phic << "\nh1: " << h1 << "\nbl1: " << bl1 << "\ntl1: " << tl1 << "\nalpha1: " << alpha1 << "\nh2: " << h2
             << "\nbl2: " << bl2 << "\ntl2: " << tl2 << "\nalpha2: " << alpha2;
  return new TGeoTrap(dz, thetac, phic, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2);
}

TGeoTrap *BSEmcStructure::CreateFromTGeoBBox(const TGeoShape *t_shape) const
{
  LOG(trace) << "Creating a TGeoTrap() from a TGeoBBox";
  const TGeoBBox *box = dynamic_cast<const TGeoBBox *>(t_shape);

  // Convert to TGeoTrap.
  return new TGeoTrap(box->GetDZ(), 0, 0, box->GetDY(), box->GetDX(), box->GetDX(), 0, box->GetDY(), box->GetDX(), box->GetDX(), 0);
}

// Print information on angular position for all the crystall into the file
void BSEmcStructure::PrintCrystals(std::string t_filename, Int_t t_option) const
{
  std::ofstream f{t_filename.c_str()};
  // different option corresponds to different details level of output
  if (t_option == 1) {
    f << "detID"
      << "\t"
      << "ThetaInd"
      << "\t"
      << "PhiInd"
      << "\t"
      << "theta_centre"
      << "\t"
      << "theta_frontface"
      << "\t"
      << "dTheta"
      << "\t"
      << "phi_centre"
      << "\t"
      << "phi_frontface"
      << "\t"
      << "dPhi"
      << "\tx\ty\tz" << std::endl;
    TVector3 centre, front_centre;
    Double_t theta_c = NAN, theta_f = NAN, phi_c = NAN, phi_f = NAN;
    Double_t dTheta = NAN, dPhi = NAN;
    BSEmcDetectorID detId = BSEmcDetectorID(0);
    for (const auto &crystal : fDetectorIdXtalMap) {
      detId = BSEmcDetectorID(crystal.first);
      centre = (crystal.second)->GetCentre();
      front_centre = (crystal.second)->GetFrontCentre();
      theta_c = centre.Theta() * TMath::RadToDeg();
      phi_c = centre.Phi() * TMath::RadToDeg();
      theta_f = front_centre.Theta() * TMath::RadToDeg();
      phi_f = front_centre.Phi() * TMath::RadToDeg();

      dTheta = theta_c - theta_f;
      dPhi = phi_c - phi_f;

      f << detId.GetDetectorId() << "\t" << detId.GetXPad() << "\t" << detId.GetYPad() << "\t" << theta_c << "\t" << theta_f << "\t" << dTheta << "\t" << phi_c << "\t" << phi_f
        << "\t" << dPhi << "\t" << front_centre.X() << "\t" << front_centre.Y() << "\t" << front_centre.Z() << "\t" << std::endl;
    }
  } else if (t_option == 2) {
    f << "detID"
      << "\t"
      << "ThetaInd"
      << "\t"
      << "PhiInd"
      << "\t"
      << "X"
      << "\t"
      << "Y"
      << "\t"
      << "Z" << std::endl;
    TVector3 front_centre;
    TVector3 centre;
    Double_t x = NAN, y = NAN, z = NAN;
    BSEmcDetectorID detId;
    for (const auto &crystal : fDetectorIdXtalMap) {
      detId = BSEmcDetectorID(crystal.first);
      front_centre = (crystal.second)->GetFrontCentre();
      centre = (crystal.second)->GetCentre();
      x = centre.X();
      y = centre.Y();
      z = centre.Z();

      f << detId.GetDetectorId() << "\t" << detId.GetColumn() << "\t" << detId.GetRow() << "\t" << x << "\t" << y << "\t" << z << std::endl;
    }
  }

  f.close();
  LOG(debug) << "write emc structure to log file";
}
