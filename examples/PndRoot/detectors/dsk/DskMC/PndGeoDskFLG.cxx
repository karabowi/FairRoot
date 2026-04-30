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

/////////////////////////////////////////////////////////////
// PndGeoDskFLG
//
// Class for geometry of DskFLG
//
// Created 29/04/10 by Y. Liang, Giessen University
//
/////////////////////////////////////////////////////////////

#include "PndGeoDskFLG.h"
#include "FairGeoNode.h"
#include "FairLogger.h"

using std::cout;
using std::endl;

ClassImp(PndGeoDskFLG)

  // -----   Default constructor   -------------------------------------------
  PndGeoDskFLG::PndGeoDskFLG()
{
  // Constructor

  fRadius = 1100;              // mm, fix me.  need to retrieve from geo in future
  fPosition_plate = 2000;      // position of plate, fix me.  need to retrieve from geo in future
  fThickness_plate = 20;       // mm, fix me.  need to retrieve from geo in future
  fNumber_focusing_part = 128; // 128 focusing parts, fix me.
  fReflectThreshold = 0.8;

  a0 = 0.;
  a1 = 0.;
  a2 = -14.37217 * 1.0E-4;
  a3 = -2.044957 * 1.0E-6;
  a4 = -1.998331 * 1.0E-8;
  a5 = -1.109226 * 1.0E-10;

  fVerbose = 1;
}
// -------------------------------------------------------------------------

// --------- Propagate cherenkov photon to focusing light guide
void PndGeoDskFLG::Propagate(TVector3 pos, TVector3 dir, Int_t &i_FLG, Int_t &i_Pixel)
{
  TVector3 Cylinder_point(1, 0, 0);
  LineCylinderInteraction(pos, dir, Cylinder_point);
  if (fVerbose > 1)
    LOG(info) << " PndGeoDskFLG::Propagate, Cylinder_point=(" << Cylinder_point.X() << "," << Cylinder_point.Y() << "," << Cylinder_point.Z();

  // now, need to calc how many folder of this line, the position and direction at the edge of cylinder
  Double_t z_inner_edge_plate = fPosition_plate - fThickness_plate / 2;
  Int_t N_reflection = Int_t((Cylinder_point.Z() - z_inner_edge_plate) / fThickness_plate); // from inner side of plate, thickness: 2cm
  Double_t z_plate_edge = (Cylinder_point.Z() - z_inner_edge_plate) - N_reflection * fThickness_plate;
  if (N_reflection % 2 == 1)
    z_plate_edge = -z_plate_edge; // update start point
  if (fVerbose > 1)
    LOG(info) << " PndGeoDskFLG::Propagate, N_reflection: " << N_reflection << "  z of photon, when penetrates plate: " << z_plate_edge;

  Double_t dphi = (TMath::Pi()) * 2 / fNumber_focusing_part;
  Int_t i_phi = Int_t(Cylinder_point.Phi() / dphi);

  if (Cylinder_point.Phi() < 0)
    i_phi = i_phi - 1;

  Double_t phi_foucsing_part = i_phi * dphi;
  if (fVerbose > 1)
    LOG(info) << " PndGeoDskFLG::Propagate, i_phi: " << i_phi << "  " << phi_foucsing_part << " dphi: " << dphi << "   Cylinder_point.Phi(): " << Cylinder_point.Phi();

  // Coordinate transform: global->certain light guide
  TVector3 dir_in_focusing_coor(sin(dir.Theta()) * cos(dir.Phi() - phi_foucsing_part), cos(dir.Theta()), sin(dir.Theta()) * sin(dir.Phi() - phi_foucsing_part));
  if (fVerbose > 1)
    LOG(info) << " PndGeoDskFLG::Propagate,theta, phi in new coordinate: " << dir_in_focusing_coor.Theta() << "  " << dir_in_focusing_coor.Phi();

  // new way, use TVector3::Rotate()

  // now, propagate in light guide
  TVector3 StartPoint(-70, -55 + z_plate_edge, 0); // z = 0 temporary
  Double_t StepLength = 10;                        // mm
  if (fVerbose > 1) {
    LOG(info) << " PndGeoDskFLG::Propagate, start Point: " << StartPoint.X() << " " << StartPoint.Y() << " " << StartPoint.Z();
  }

  TVector3 Direction = dir_in_focusing_coor;
  if (Direction.Phi() < 25. / 180 * (TMath::Pi()) || Direction.Phi() > 50. / 180 * (TMath::Pi()))
    return; // do not deal with this

  Int_t i_loop = 0, max_loop = 2000;
  TVector3 NextPoint = StartPoint;
  do {
    NextPoint = NextPoint + Direction * StepLength;
    i_loop++;
    if (fVerbose > 1)
      cout << "NextPoint: " << NextPoint.X() << " " << NextPoint.Y() << " " << NextPoint.Z() << endl;
  } while (CurveFunction(NextPoint) && i_loop < max_loop);
  NextPoint = NextPoint - Direction * StepLength; // go back one step

  StepLength /= 10; // 0.1mm
  do {
    NextPoint = NextPoint + Direction * StepLength;
    i_loop++;
    if (fVerbose > 1)
      cout << "NextPoint: " << NextPoint.X() << " " << NextPoint.Y() << " " << NextPoint.Z() << endl;
  } while (CurveFunction(NextPoint) && i_loop < max_loop);
  NextPoint = NextPoint - Direction * StepLength; // go back one step

  StepLength /= 10; // 0.01mm
  do {
    NextPoint = NextPoint + Direction * StepLength;
    i_loop++;
    if (fVerbose > 1)
      cout << "NextPoint: " << NextPoint.X() << " " << NextPoint.Y() << " " << NextPoint.Z() << endl;
  } while (CurveFunction(NextPoint) && i_loop < max_loop);
  NextPoint = NextPoint - Direction * StepLength; // go back one step

  Double_t phi_prime = SlopeCurveFunction(NextPoint);
  Direction.SetPhi(2 * phi_prime - Direction.Phi());
  if (fVerbose > 1)
    cout << "slope: " << SlopeCurveFunction(NextPoint) << endl;

  StepLength = 10;
  do {
    NextPoint = NextPoint + Direction * StepLength;
    i_loop++;
    if (fVerbose > 1)
      cout << "NextPoint: " << NextPoint.X() << " " << NextPoint.Y() << " " << NextPoint.Z() << endl;
  } while (LineFunction(NextPoint) && i_loop < max_loop); // while(NextPoint.X()<100);
  NextPoint = NextPoint - Direction * StepLength;         // go back one step

  StepLength /= 10;
  do {
    NextPoint = NextPoint + Direction * StepLength;
    i_loop++;
    if (fVerbose > 1)
      cout << "NextPoint: " << NextPoint.X() << " " << NextPoint.Y() << " " << NextPoint.Z() << endl;
  } while (LineFunction(NextPoint) && i_loop < max_loop); // while(NextPoint.X()<100);
  NextPoint = NextPoint - Direction * StepLength;         // go back one step

  StepLength /= 10;
  do {
    NextPoint = NextPoint + Direction * StepLength;
    i_loop++;
    if (fVerbose > 1)
      cout << "NextPoint: " << NextPoint.X() << " " << NextPoint.Y() << " " << NextPoint.Z() << endl;
  } while (LineFunction(NextPoint) && i_loop < max_loop); // while(NextPoint.X()<100);
  NextPoint = NextPoint - Direction * StepLength;         // go back one step

  // calc pixel ID, use the distance of last point and (67.0518, -100.0000) , or (95.0000  -23.2129)
  Double_t distance_pixel = sqrt((NextPoint.X() - 95) * (NextPoint.X() - 95) + (NextPoint.Y() + 23.2129) * (NextPoint.Y() + 23.2129));

  i_FLG = i_phi;
  i_Pixel = Int_t(distance_pixel); // fixme
}

void PndGeoDskFLG::LineCylinderInteraction(TVector3 pos, TVector3 dir, TVector3 &pos_interaction)
{

  Double_t phi = dir.Phi();
  Double_t slope = tan(phi);
  Double_t A_xx = 1 + slope * slope;
  Double_t B_x = 2 * slope * (pos.Y() - pos.X() * slope);
  Double_t C_const = (pos.Y() - pos.X() * slope) * (pos.Y() - pos.X() * slope) - fRadius * fRadius;

  // cout<<"phi: "<<phi<<"  abc: "<<A_xx<<"  "<<B_x<<" "<<C_const<<endl;
  if ((B_x * B_x - 4 * A_xx * C_const) < 0) {
    cout << " -E- PndGeoDskFLG::LineCylinderInteraction,  no solution? check it" << endl;
    return;
  }
  double x_cylinder, y_cylinder, z_cylinder;
  if ((phi > TMath::Pi() / 2. && phi <= TMath::Pi()) || (phi >= -1 * TMath::Pi() && phi < -1 * TMath::Pi() / 2.)) {
    x_cylinder = (-1 * B_x - sqrt(B_x * B_x - 4 * A_xx * C_const)) / (2 * A_xx);
  } else {
    x_cylinder = (-1 * B_x + sqrt(B_x * B_x - 4 * A_xx * C_const)) / (2 * A_xx);
  }

  y_cylinder = x_cylinder * slope + (pos.Y() - pos.X() * slope);
  z_cylinder = sqrt((y_cylinder - pos.Y()) * (y_cylinder - pos.Y()) + (x_cylinder - pos.X()) * (x_cylinder - pos.X())) / tan(dir.Theta()) + pos.Z();

  pos_interaction.SetXYZ(x_cylinder, y_cylinder, z_cylinder);
}

Bool_t PndGeoDskFLG::CurveFunction(TVector3 Point)
// to judge whether the input point (x1, y1, z2) exceed the curve function
{
  Double_t x1 = Point.X();
  Double_t y1 = Point.Y();

  return (y1 < (a0 + a1 * x1 + a2 * x1 * x1 + a3 * x1 * x1 * x1 + a4 * x1 * x1 * x1 * x1 + a5 * x1 * x1 * x1 * x1 * x1));
}

Bool_t PndGeoDskFLG::LineFunction(TVector3 Point)
{
  Double_t x0 = 95.0000, y0 = -23.2129;
  Double_t x1 = 67.0518, y1 = -100.0000;
  Double_t x = Point.X();
  Double_t y = Point.Y();

  return (y > (y0 + (y1 - y0) / (x1 - x0) * (x - x0)));
}

Double_t PndGeoDskFLG::SlopeCurveFunction(TVector3 Point)
// to judge whether the input point (x1, y1, z2) exceed the curve function
{
  Double_t x1 = Point.X();
  // Double_t y1 = Point.Y(); //[R.K. 01/2017] unused variable?

  return atan(a0 * 0. + a1 + 2 * a2 * x1 + 3 * a3 * x1 * x1 + 4 * a4 * x1 * x1 * x1 + 5 * a5 * x1 * x1 * x1 * x1);
}
