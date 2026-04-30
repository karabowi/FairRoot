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

#include "PndRadMapBoxMesh.h"
#include <TMath.h>
#include <TFile.h>
#include <TObjArray.h>
#include <TObjString.h>

#include <iostream>
#include <iomanip>
#include <utility>

#include <G4UnitsTable.hh>

#include "PndRadMapPlane.h"

#define verbose true

bool momentumfilter(TVector3 mom, TVector3 diff)
{
  bool back = true;
  for (int i = 0; i < 3; i++)
    if (mom(i) * diff(i) < 0)
      back = false;
  return back;
}

// PndRadMapPlane::PndRadMapPlane(){
//   SetNormal(TVector3(0, 0, 0));
//   SetDistance(0);
//   corner1 = TVector3(0, 0, 0);
//   corner2 = TVector3(0, 0, 0);
//   corner3 = TVector3(0, 0, 0);
// }

// PndRadMapPlane::PndRadMapPlane(TVector3 _corner1, TVector3 _corner2, TVector3 _corner3,
//              double dist){
//   corner1 = _corner1;
//   corner2 = _corner2;
//   corner3 = _corner3;
//   TVector3 dpv1 = corner2-corner1;
//   TVector3 dpv2 = corner3-corner1;
//   TVector3 n = (dpv1.Cross(dpv2)).Unit();
//   SetNormal(n);
//   SetDistance(dist);
// }

// TVector3 PndRadMapPlane::GetCorner(int i){
//   switch(i){
//   case 1:
//     return Corner1();
//   case 2:
//     return Corner2();
//   case 3:
//     return Corner3();
//   default:
//     return TVector3(0, 0, 0);
//   }
// }

// TVector3 intersectLine(TVector3 l0, TVector3 l1,// two points of the line
//                        TVector3 p0, TVector3 p1, TVector3 p2,
//                        float& eff) {//three points of the plane
//   //axes of the plane
//   TVector3 dpv1 = p1-p0;
//   TVector3 dpv2 = p2-p0;
//   TVector3 n = (dpv1.Cross(dpv2)).Unit();

//   // Rotate the points (of the line) to the plane
//   TVector3 vRotRay1 = TVector3 (dpv1*(l0-p1), dpv2*(l0-p1), n*(l0-p1));
//   TVector3 vRotRay2 = TVector3 (dpv1*(l1-p1), dpv2*(l1-p1), n*(l1-p1));

//   float fPercent = vRotRay1.Z() / (vRotRay1.Z()-vRotRay2.Z());

//   eff = fPercent;
//   TVector3 ixp = l0 + fPercent*(l1-l0);
// }

// TVector3 PndRadMapPlane::LineIntersection(TVector3 begline, TVector3 endline){

//   TVector3 diffl = endline-begline;
//   float nDotBeg = normal*(corner1-begline);
//   float nDotDiff = normal*diffl;
//   float tt = nDotBeg/nDotDiff;

//   TVector3 ixp = begline + tt*diffl;

//   return ixp;
// }

PndRadMapBoxMesh::PndRadMapBoxMesh(PndRadMapBoxMesh &m)
{
  _MeshHisto = (TH2D *)m._MeshHisto->Clone();
  _StatHisto = (TH2I *)m._StatHisto->Clone();
  _orientation = m._orientation;
  _rotate = m._rotate;
  _volume = m._volume;
  _Name = m._Name;
  _plane = nullptr;
  _isSurfaceQuantity = false;
  pdg = TDatabasePDG::Instance();
  pdg->ReadPDGTable();
}

PndRadMapBoxMesh::PndRadMapBoxMesh(const char *name, int Xbins, Double_t Xlow, Double_t Xhigh, int Ybins, Double_t Ylow, Double_t Yhigh, int Zbins, Double_t Zlow, Double_t Zhigh)
{ // in cm!!!!
  _verbose = 0;
  _Name = TString(name);
  _Xbins = Xbins;
  _Xlow = Xlow;
  _Xhigh = Xhigh;
  _Ybins = Ybins;
  _Ylow = Ylow;
  _Yhigh = Yhigh;
  _Zbins = Zbins;
  _Zlow = Zlow;
  _Zhigh = Zhigh;
  _filter = TFormula("formula", "1");

  if ((_Xlow == _Xhigh) || (_Ylow == _Yhigh) || (_Zlow == _Zhigh))
    _isSurfaceQuantity = true;
  pdg = TDatabasePDG::Instance();
  pdg->ReadPDGTable();

  // if(_verbose)
  std::cout << "PndRadMapBoxMesh::PndRadMapBoxMesh()\n"
            << _Xlow << ' ' << _Xhigh << ' ' << _Ylow << ' ' << _Yhigh << ' ' << _Zlow << ' ' << _Zhigh << ' ' << _isSurfaceQuantity << std::endl;
}

PndRadMapBoxMesh::PndRadMapBoxMesh(const char *name, int xbins, Double_t xlow, Double_t xhigh, int ybins, Double_t ylow, Double_t yhigh, Double_t zlow, Double_t zhigh,
                                   orientation plane, quantity Quantity)
{ // in cm!!!!

  _verbose = 0;

  _Name = TString(name);

  _orientation = plane;
  SetQuantity(Quantity);

  switch (_orientation) {
  case XY:
    _Xbins = xbins;
    _Xlow = xlow;
    _Xhigh = xhigh;

    _Ybins = ybins;
    _Ylow = ylow;
    _Yhigh = yhigh;

    _Zlow = zlow;
    _Zhigh = zhigh;
    break;
  case YX:
    _Xbins = ybins;
    _Xlow = ylow;
    _Xhigh = yhigh;

    _Ybins = xbins;
    _Ylow = xlow;
    _Yhigh = xhigh;

    _Zlow = zlow;
    _Zhigh = zhigh;
    break;

  case XZ:
    _Xbins = xbins;
    _Xlow = xlow;
    _Xhigh = xhigh;

    _Ylow = zlow;
    _Yhigh = zhigh;

    _Zbins = ybins;
    _Zlow = ylow;
    _Zhigh = yhigh;
    break;
  case ZX:
    _Xbins = ybins;
    _Xlow = ylow;
    _Xhigh = yhigh;

    _Zbins = xbins;
    _Zlow = xlow;
    _Zhigh = xhigh;

    _Ylow = zlow;
    _Yhigh = zhigh;
    break;

  case YZ:
    _Xlow = zlow;
    _Xhigh = zhigh;

    _Ybins = xbins;
    _Ylow = xlow;
    _Yhigh = xhigh;

    _Zbins = ybins;
    _Zlow = ylow;
    _Zhigh = yhigh;
    break;
  case ZY:
    _Xlow = zlow;
    _Xhigh = zhigh;

    _Ybins = ybins;
    _Ylow = ylow;
    _Yhigh = yhigh;

    _Zbins = xbins;
    _Zlow = xlow;
    _Zhigh = xhigh;
    break;
  };

  if ((_Xlow == _Xhigh) || (_Ylow == _Yhigh) || (_Zlow == _Zhigh))
    _isSurfaceQuantity = true;

  pdg = TDatabasePDG::Instance();
  pdg->ReadPDGTable();
}

PndRadMapBoxMesh::~PndRadMapBoxMesh()
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::~PndRadMapBoxMesh()\n";
  delete _StatHisto;
  delete _MeshHisto;
}

void PndRadMapBoxMesh::SetFilter(const char *filter)
{
  // Pid=x, Ch=y, Mom=z,
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::SetFilter(" << filter << ")\n";
  TString temps(filter);
  temps.ReplaceAll("Pid", "x");
  temps.ReplaceAll("Ch", "y");
  temps.ReplaceAll("Mom", "z");

  _filter = TFormula("formula", temps.Data());
}

void PndRadMapBoxMesh::SetQuantity(quantity Quantity)
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::SetQuantity(" << Quantity << ")\n";
  _quantity = Quantity;
}

void PndRadMapBoxMesh::SetOrientation(orientation plane, Double_t rotate, axis Ax)
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::SetOrientation(" << plane << ", " << rotate << ", " << Ax << ")\n";
  _orientation = plane;
  SetOrientation(rotate, Ax);
}

void PndRadMapBoxMesh::SetOrientation(Double_t rotate, axis Ax)
{

  if (_verbose)
    std::cout << "PndRadMapBoxMesh::SetOrientation(" << rotate << ", " << Ax << ")\n";
  _axis = Ax;
  _rotate = (rotate == 99999) ? 0 : rotate;

  if (_verbose)
    std::cout << "Volume[" << _Name.Data() << "][(" << _Xhigh << ", " << _Xlow << ", " << _Xbins << "),(" << _Yhigh << ", " << _Ylow << ", " << _Ybins << "),(" << _Zhigh << ", "
              << _Zlow << ", " << _Zbins << ")]:\n";

  // char C[32];
  switch (_orientation) {
  case XY: makeHisto("XY", _rotate, _Xbins, _Xlow, _Xhigh, _Ybins, _Ylow, _Yhigh, _Zlow /*, _Zhigh --unused parameter*/); break;
  case YX: makeHisto("YX", _rotate, _Ybins, _Ylow, _Yhigh, _Xbins, _Xlow, _Xhigh, _Zlow /*, _Zhigh --unused parameter*/); break;

  case XZ: makeHisto("XZ", _rotate, _Xbins, _Xlow, _Xhigh, _Zbins, _Zlow, _Zhigh, _Ylow /*, _Yhigh --unused parameter*/); break;
  case ZX: makeHisto("ZX", _rotate, _Zbins, _Zlow, _Zhigh, _Xbins, _Xlow, _Xhigh, _Ylow /*, _Yhigh --unused parameter*/); break;

  case YZ: makeHisto("YZ", _rotate, _Ybins, _Ylow, _Yhigh, _Zbins, _Zlow, _Zhigh, _Xlow /*, _Xhigh --unused parameter*/); break;
  case ZY: makeHisto("ZY", _rotate, _Zbins, _Zlow, _Zhigh, _Ybins, _Ylow, _Yhigh, _Xlow /*, _Xhigh --unused parameter*/); break;
  };

  if (!_isSurfaceQuantity)
    _volume = ((_Xhigh - _Xlow) / _Xbins) * ((_Yhigh - _Ylow) / _Ybins) * ((_Zhigh - _Zlow) / _Zbins);
  else
    _volume = 1;

  if (verbose)
    std::cout << "Volume: " << (_Xhigh - _Xlow) / _Xbins << ", " << (_Yhigh - _Ylow) / _Ybins << ", " << (_Zhigh - _Zlow) / _Zbins << " -> " << _volume << "\n";
}

void PndRadMapBoxMesh::SetVerbosityLevel(int Verbose)
{
  _verbose = Verbose;
}

// void PndRadMapBoxMesh::transform(Double_t Vh, Double_t Vv){
void PndRadMapBoxMesh::Transform(Double_t X, Double_t Y, Double_t Z)
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::Transform(" << X << ", " << Y << ", " << Z << ")\n";

  _X = X;
  _Y = Y;
  _Z = Z;
  Transform(X, Y, Z, _X, _Y, _Z);
}

void PndRadMapBoxMesh::Transform(Double_t X, Double_t Y, Double_t Z, Double_t &X0, Double_t &Y0, Double_t &Z0)
{

  if (_verbose) {
    std::cout << "PndRadMapBoxMesh::Transform(" << X << ", " << Y << ", " << Z << ")\n";
    std::cout << "                            " << X0 << ", " << Y0 << ", " << Z0 << ")\n";
  }
  if (_rotate == 99999)
    _rotate = 0;
  if ((_rotate == 0)) {
    X0 = X;
    Y0 = Y;
    Z0 = Z;
  } else {

    Double_t V1 = 0;
    Double_t V2 = 0;

    X0 = X;
    Y0 = Y;
    Z0 = Z;
    switch (_axis) {
    case Xx:
      V1 = Y;
      V2 = Z;
      X0 = X;
      break;
    case Yy:
      V1 = X;
      V2 = Z;
      Y0 = Y;
      break;
    case Zz:
      V1 = X;
      V2 = Y;
      Z0 = Z;
      break;
    };

    Double_t _Vh, _Vv;
    if (_rotate != 0) {
      Double_t R = TMath::Sqrt(V1 * V1 + V2 * V2);
      Double_t rot0 = TMath::ATan(V1 / V2) * TMath::RadToDeg();
      if (V2 < 0)
        rot0 += 180;
      Double_t drot = rot0 - _rotate;
      _Vh = R * TMath::Sin(1 * drot * TMath::DegToRad());
      _Vv = R * TMath::Cos(1 * drot * TMath::DegToRad());
    } else {
      _Vh = V1;
      _Vv = V2;
    }

    switch (_axis) {
    case Xx:
      switch (_orientation) {
      case XY:
      case YX:
      case XZ:
      case ZX:
        Y0 = _Vv;
        Z0 = _Vh;
        break;

      case YZ:
        Y0 = _Vh;
        Z0 = _Vv;
        break;
      case ZY:
        Y0 = _Vv;
        Z0 = _Vh;
        break;
      };
      break;

    case Yy:
      switch (_orientation) {
      case XY:
      case YX:
      case YZ:
      case ZY:
        X0 = _Vh;
        Z0 = _Vv;
        break;

      case XZ:
        X0 = _Vh;
        Z0 = _Vv;
        break;
      case ZX:
        X0 = _Vv;
        Z0 = _Vh;
        break;
      };
      break;

    case Zz:
      switch (_orientation) {
      case XZ:
      case ZX:
      case YZ:
      case ZY:
        X0 = _Vh;
        Y0 = _Vv;
        break;

      case XY:
        X0 = _Vh;
        Y0 = _Vv;
        break;
      case YX:
        X0 = _Vv;
        Y0 = _Vh;
        break;
      };
      break;
    };
  }
  if (_verbose) {
    std::cout << " -> PndRadMapBoxMesh::Transform(" << X << ", " << Y << ", " << Z << ")\n";
    std::cout << "                                " << X0 << ", " << Y0 << ", " << Z0 << ")\n";
  }
}

void PndRadMapBoxMesh::Fill(FairRadMapPoint *p)
{
  if (_verbose)
    std::cout << "void PndRadMapBoxMesh::Fill(FairRadMapPoint *p)\n";

  Int_t pid = p->GetPdg();
  Double_t mom = TMath::Sqrt((p->GetPx() * p->GetPx()) + (p->GetPy() * p->GetPy()) + (p->GetPz() * p->GetPz()));
  pdgpart = pdg->GetParticle(pid);
  Int_t cha = -1000; // to be implemented
  Double_t pdgmass = -1;
  Double_t val = 0;

  Double_t edep = 0;
  Double_t dens = 0;
  Double_t mass = p->GetMass();
  TVector3 poststepv, prestepv;
  Double_t lX = 0, lY = 0, lZ = 0;

  if (pdgpart) {
    pdgmass = pdgpart->Mass();
    cha = pdgpart->Charge();
  }

  if (_filter.Eval(pid, cha, mom)) {

    val = 0;

    edep = p->GetEnergyLoss() * 1.60217657 * 1e-10; // GeV->Joule
    dens = p->GetDensity();
    dens *= 0.001; // g/cm3 -> kg/cm3
    mass = p->GetMass();
    poststepv = TVector3(p->GetXOut(), p->GetYOut(), p->GetZOut());
    prestepv = TVector3(p->GetX(), p->GetY(), p->GetZ());
    lX = 0;
    lY = 0;
    lZ = 0;

    if (_verbose)
      std::cout << "Density: " << dens << ", mass: " << mass << " (" << (dens * _volume) << ", " << _volume << ") " << dens * mass << ' ' << "charge: " << cha << "\n";

    lX = p->GetXOut();
    lY = p->GetYOut();
    lZ = p->GetZOut();

    bool OK = false;
    switch (_quantity) {

    case Edep:
      val = edep;
      if (!std::isnan(val) && val != 0)
        // Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);
        Fill(lX, lY, lZ, val);
      break;

    case Dose:
      val = edep / (dens * _volume); // mass=density*vol,dens[kg/cm^3],vol[cm^3]!
      if (!std::isnan(val) && val != 0) {
        // Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);
        Fill(lX, lY, lZ, val);
      }
      break;

    case Fluence:
      InterSection = _plane->LineIntersection(poststepv, prestepv);
      switch (_orientation) {
      case XY:
      case YX:
        if (((prestepv.Z() <= InterSection.Z()) && (InterSection.Z() <= poststepv.Z())) || ((poststepv.Z() <= InterSection.Z()) && (InterSection.Z() <= prestepv.Z())))
          OK = true;
        break;

      case XZ:
      case ZX:
        if (((prestepv.Y() <= InterSection.Y()) && (InterSection.Y() <= poststepv.Y())) || ((poststepv.Y() <= InterSection.Y()) && (InterSection.Y() <= prestepv.Y())))
          OK = true;
        break;

      case YZ:
      case ZY:
        if (((prestepv.X() <= InterSection.X()) && (InterSection.X() <= poststepv.X())) || ((poststepv.X() <= InterSection.X()) && (InterSection.X() <= prestepv.X())))
          OK = true;
        break;
      };
      // std::cout << "test0\n"
      //           << "[" << prestepv.X()  << "<=" << InterSection.X() << "<=" << poststepv.X() << "] - " << OK << "\n"
      //           << "[" << poststepv.X() << "<=" << InterSection.X() << "<=" << prestepv.X()  << "] - " << OK << "\n"
      //           << "[" << prestepv.Y()  << "<=" << InterSection.Y() << "<=" << poststepv.Y() << "] - " << OK << "\n"
      //           << "[" << poststepv.Y() << "<=" << InterSection.Y() << "<=" << prestepv.Y()  << "] - " << OK << "\n"
      //           << "[" << prestepv.Z()  << "<=" << InterSection.Z() << "<=" << poststepv.Z() << "] - " << OK << "\n"
      //           << "[" << poststepv.Z() << "<=" << InterSection.Z() << "<=" << prestepv.Z()  << "] - " << OK << "\n\n";

      // // if(((prestepv.Z() <= InterSection.Z()) && (InterSection.Z() <= poststepv.Z())) ||
      // //    ((poststepv.Z() <= InterSection.Z()) && (InterSection.Z() <= prestepv.Z()))){
      if (OK) {
        // std::cout << "testing 0...\n";
        // std::cout << "testing 0... "
        //   << _Xlow << "<=" << InterSection.X() << "<=" << _Xhigh << " -> "
        //   << ((_Xlow <= InterSection.X()) && (InterSection.X() <= _Xhigh)) << ' ' << (InterSection.X() <= _Xhigh) << ' ' << (_Xlow <= InterSection.X()) << ' '
        //   << _Ylow << "<=" << InterSection.Y() << "<=" << _Yhigh << " -> "
        //   << ((_Ylow <= InterSection.Y()) && (InterSection.Y() <= _Yhigh)) << ' '
        //   << _Zlow << "<=" << InterSection.Z() << "<=" << _Zhigh << " -> "
        //   << ((_Zlow <= InterSection.Z()) && (InterSection.Z() <= _Zhigh)) << std::endl;

        Fill(InterSection.X(), InterSection.Y(),
             InterSection.Z()); // normalisation to the area ... now it is OK

        Double_t theta = InterSection.Theta() * TMath::RadToDeg();
        Double_t K = TMath::Sqrt(mom * mom + pdgmass * pdgmass) - pdgmass;

        _EnergyHisto->Fill(theta, K); // uncommented by MP, Sep 8
        // val = CalcFluence(p);
        // if(!isnan(val) && val !=0){
        //   Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);
      }
      break;

    case Density:
      val = dens;
      if (!std::isnan(val) && val != 0) {
        Fill(lX, lY, lZ, val);
        // Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);
      }
      break;

    case Mass:
      val = (dens * _volume);
      if (!std::isnan(val) && val != 0) {
        Fill(lX, lY, lZ, val);
        // Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);
      }
      break;

    case SimpleFluence: {
      poststepv = TVector3(p->GetXOut(), p->GetYOut(), p->GetZOut());
      prestepv = TVector3(p->GetX(), p->GetY(), p->GetZ());
      val = (poststepv - prestepv).Mag() / 1e2; // cm->m
      val /= (_volume / 1e6);                   // cm^3->m^3 -> 1/m^2
      if (!std::isnan(val) && val != 0) {
        if (_verbose > 1) {
          std::cout << "Pre: " << prestepv.X() << ", " << prestepv.Y() << ", " << prestepv.Z() << "\n";
          std::cout << "Pre: " << poststepv.X() << ", " << poststepv.Y() << ", " << poststepv.Z() << "\n";
          std::cout << "diff: " << (poststepv - prestepv).Mag() << "; val: " << val << std::endl;
          std::cout << "volume: " << _volume << "\n\n";
        }
        Fill(lX, lY, lZ, val);
        // Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);
      }
    } break;

    case Twos:
      val = 2;
      if (!std::isnan(val) && val != 0) {
        Fill(lX, lY, lZ, val);
        // Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);
      }
      break;
    case EnergyFluence: break;
    case Flux: break;
    case Kerma: break;
    }
  }
}

// void PndRadMapBoxMesh::Fill(FairRadMapPoint *p, Double_t we){
void PndRadMapBoxMesh::Fill(Int_t gBin, Double_t val)
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::Fill(" << gBin << ", " << val << ")\n";

  Int_t binX, binY, binZ;
  _MeshHisto->GetBinXYZ(gBin, binX, binY, binZ);
  Fill(binX, binY, binZ, val);
}

void PndRadMapBoxMesh::Fill(Double_t X, Double_t Y, Double_t Z, Double_t we)
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::Fill(" << X << ", " << Y << ", " << Z << ", " << we << ")\n";

  Transform(X, Y, Z);

  // std::cout << "0 - PndRadMapBoxMesh::Fill(" << X << ", " << Y << ", " << Z << ", " << we << ")\n";
  // std::cout << "1 - PndRadMapBoxMesh::Fill(" << _X << ", " << _Y << ", " << _Z << ", " << we << ")\n";

  // if(IsInside(_X, _Y, _Z)){
  if (IsInside()) {
    // std::cout << "2 - PndRadMapBoxMesh::Fill(" << _X << ", " << _Y << ", " << _Z << ", " << we << ")\n";

    switch (_orientation) {

    case XY:
      _MeshHisto->Fill(_X, _Y, we);
      _StatHisto->Fill(_X, _Y);
      break;
    case YX:
      _MeshHisto->Fill(_Y, _X, we);
      _StatHisto->Fill(_Y, _X);
      break;

    case XZ:
      _MeshHisto->Fill(_X, _Z, we);
      _StatHisto->Fill(_X, _Z);
      break;
    case ZX:
      _MeshHisto->Fill(_Z, _X, we);
      _StatHisto->Fill(_Z, _X);
      break;

    case YZ:
      _MeshHisto->Fill(_Y, _Z, we);
      _StatHisto->Fill(_Y, _Z);
      break;
    case ZY:
      _MeshHisto->Fill(_Z, _Y, we);
      _StatHisto->Fill(_Z, _Y);
      break;
    };
  }
}

void PndRadMapBoxMesh::Scale(Double_t sca)
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::Scale(" << sca << ")" << std::endl;
  _MeshHisto->Scale(sca);
}

TH2D *PndRadMapBoxMesh::GetHisto()
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::GetHisto()\n";
  return _MeshHisto;
}

PndRadMapPlane *PndRadMapBoxMesh::GetPlane()
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::GetPlane()\n";
  return _plane;
}

void PndRadMapBoxMesh::Save(TFile *fout)
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::Save(TFile* fout)\n";

  TObjArray *toa = TString(_MeshHisto->GetName()).Tokenize('_');
  TString dirname = ((TObjString *)toa->At(0))->GetString();
  dirname += '_';
  dirname += ((TObjString *)toa->At(2))->GetString().Data();
  dirname += '_';
  dirname += ((TObjString *)toa->At(3))->GetString().Data();
  std::cout << dirname.Data() << std::endl;

  // fout->mkdir(_MeshHisto->GetName());
  // fout->cd   (_MeshHisto->GetName());
  fout->mkdir(dirname.Data());
  fout->cd(dirname.Data());
  _MeshHisto->Clone()->Write();
  _StatHisto->Clone()->Write();
  if (_isSurfaceQuantity)
    _EnergyHisto->Clone()->Write();
  fout->cd();
}

void PndRadMapBoxMesh::Save()
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::Save()\n";
  _MeshHisto->Write();
  _StatHisto->Write();
  if (_isSurfaceQuantity)
    _EnergyHisto->Clone()->Write();
}

bool PndRadMapBoxMesh::IsInside(Double_t X, Double_t Y, Double_t Z)
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::IsInside(" << X << ", " << Y << ", " << Z << ")\n";

  bool ok = false;

  if (((_Xlow <= X) && (X <= _Xhigh)) && ((_Ylow <= Y) && (Y <= _Yhigh)) && ((_Zlow <= Z) && (Z <= _Zhigh)))
    ok = true;
  return ok;
}

bool PndRadMapBoxMesh::IsInside()
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::IsInside()\n";

  bool ok = false;

  switch (_orientation) {
  case XY:
  case YX:
    if (((_Xlow <= _X) && (_X <= _Xhigh)) && ((_Ylow <= _Y) && (_Y <= _Yhigh)))
      ok = true;
    break;
  case XZ:
  case ZX:
    if (((_Xlow <= _X) && (_X <= _Xhigh)) && ((_Zlow <= _Z) && (_Z <= _Zhigh)))
      ok = true;
    break;
  case YZ:
  case ZY:
    if (((_Ylow <= _Y) && (_Y <= _Yhigh)) && ((_Zlow <= _Z) && (_Z <= _Zhigh)))
      ok = true;
    break;
  };

  // if(((_Xlow <= _X) && (_X <= _Xhigh)) &&
  //    ((_Ylow <= _Y) && (_Y <= _Yhigh)) &&
  //    ((_Zlow <= _Z) && (_Z <= _Zhigh)))
  //   ok = true;
  return ok;
}

bool PndRadMapBoxMesh::IsInside(FairRadMapPoint *p)
{
  if (_verbose)
    std::cout << "PndRadMapBoxMesh::IsInside(FairRadMapPoint *p)\n";

  bool ok = false;
  double xin, yin, zin, xout, yout, zout;
  Transform(p->GetX(), p->GetY(), p->GetZ(), xin, yin, zin);
  Transform(p->GetXOut(), p->GetYOut(), p->GetZOut(), xout, yout, zout);

  if ((((xin <= _Xlow) && (_Xhigh <= xout)) || ((xout <= _Xlow) && (_Xhigh <= xin)) || ((_Xlow <= xout) && (xout <= _Xhigh))) &&
      (((yin <= _Ylow) && (_Yhigh <= yout)) || ((yout <= _Ylow) && (_Yhigh <= yin)) || ((_Ylow <= yout) && (yout <= _Yhigh))) &&
      (((zin <= _Zlow) && (_Zhigh <= zout)) || ((zout <= _Zlow) && (_Zhigh <= zin)) || ((_Zlow <= zout) && (zout <= _Zhigh))))
    ok = true;

  return ok;
}

void PndRadMapBoxMesh::makeHisto(const char *Orient, Double_t rotate, int Hbins, Double_t Hlow, Double_t Hhigh, int Vbins, Double_t Vlow, Double_t Vhigh,
                                 Double_t dlow /*, Double_t dhigh --unused parameter */)
{
  if (_verbose) {
    std::cout << "PndRadMapBoxMesh::makeHisto(" << Orient << ", " << rotate << ",\n";
    std::cout << "                   " << Hbins << ", " << Hlow << ", " << Hhigh << ",\n";
    std::cout << "                   " << Vbins << ", " << Vlow << ", " << Vhigh << ")\n";
  }
  char C[32];
  sprintf(C, "%s_Histo_%s_%g", _Name.Data(), Orient, _rotate);
  _MeshHisto = new TH2D(C, C, Hbins, Hlow, Hhigh, Vbins, Vlow, Vhigh);
  sprintf(C, "%s_StatHisto_%s_%g", _Name.Data(), Orient, _rotate);
  _StatHisto = new TH2I(C, C, Hbins, Hlow, Hhigh, Vbins, Vlow, Vhigh);

  if (_isSurfaceQuantity) {
    sprintf(C, "%s_EnergyHisto_%s_%g", _Name.Data(), Orient, _rotate);
    _EnergyHisto = new TH2D(C, C, 180, 0, 180, 10000, 0, 10);
    _plane = new PndRadMapPlane(dlow, _rotate, _orientation, _axis);
  }
}

Double_t PndRadMapBoxMesh::CalcFluence(FairRadMapPoint *p)
{

  if (_verbose)
    std::cout << "PndRadMapBoxMesh::CalcFluence(FairRadMapPoint *p)" << std::endl;

  Double_t xx, yy, zz, dx = 0, dy = 0, dz = 0;
  // float eff;
  std::vector<Corner> CV;
  std::vector<PndRadMapPlane> PV;
  // The Z should be between zlow and zhigh!!
  Transform(p->GetXOut(), p->GetYOut(), p->GetZOut(), xx, yy, zz);
  std::pair<int, double> minp;
  bool OK = false, OK2 = false;

  if (IsInside(p)) {
    if (IsInside(xx, yy, zz))
      OK = true;
    switch (_orientation) {
    case XY:
    case YX: dz = _Zlow; break;
    case XZ:
    case ZX: dy = _Ylow; break;
    case ZY:
    case YZ: dx = _Xlow; break;
    }

    TVector3 poststepv = TVector3(p->GetXOut(), p->GetYOut(), p->GetZOut());
    TVector3 prestepv = TVector3(p->GetX(), p->GetY(), p->GetZ());
    unsigned int prebin = _MeshHisto->FindBin(prestepv.X(), prestepv.Y());
    unsigned int postbin = _MeshHisto->FindBin(poststepv.X(), poststepv.Y());
    PndRadMapPlane plane;
    int prebinX = 0, prebinY = 0, prebinZ = 0;
    int postbinX = 0, postbinY = 0, postbinZ = 0;
    _MeshHisto->GetBinXYZ(postbin, postbinX, postbinY, postbinZ);
    _MeshHisto->GetBinXYZ(prebin, prebinX, prebinY, prebinZ);
    TVector3 tn(0, 0, 0);

    if (verbose)
      std::cout << "PndRadMapBoxMesh::CalcFluence - 1" << std::endl;

    // 1, only two planes: the narrowest bin lower and higher plane
    // 2, if the resulting two points in the range of the other two axis-limits
    // 3, start the stepping from one of those points

    // -------- 1 -------

    TVector3 isl, lbeg, lend;
    double min_x, min_y, min_z, max_x, max_y, max_z;

    if (!OK) { // if the 'out' is not in the given range
      // the 'in' and the 'out' is outside of the given range, so it can be that it will cross it

      if (verbose)
        std::cout << "PndRadMapBoxMesh::CalcFluence - 2" << std::endl;
      OK2 = false;
      min_x = _Xlow;
      min_y = _Ylow;
      min_z = _Zlow;

      max_x = _Xhigh;
      max_y = _Yhigh;
      max_z = _Zhigh;
      if (verbose)
        std::cout << "min_z: " << min_z << ' ' << max_z << ' ' << dz << std::endl;

      // the 'front' and 'back' XY planes
      plane =
        PndRadMapPlane(TVector3(min_x + dx, min_y + dy, min_z + dz), TVector3(max_x + dx, min_y + dy, min_z + dz), TVector3(min_x + dx, max_y + dy, min_z + dz)); // XY @ z=z_min
      isl = plane.LineIntersection(prestepv, poststepv);
      if (((min_x < isl.X()) && (isl.X() < max_x)) && ((min_y < isl.Y()) && (isl.Y() < max_y))) {
        OK2 = true;
      }
      if (verbose) {
        std::cout << "Xcross1: " << min_x << ' ' << isl.X() << ' ' << max_x << std::endl;
        std::cout << "Ycross1: " << min_y << ' ' << isl.Y() << ' ' << max_y << std::endl;
        std::cout << "=> " << OK2 << std::endl;
      }
      lbeg = isl;

      plane =
        PndRadMapPlane(TVector3(min_x + dx, min_y + dy, max_z + dz), TVector3(max_x + dx, min_y + dy, max_z + dz), TVector3(min_x + dx, max_y + dy, max_z + dz)); // XY @ z-z_max
      isl = plane.LineIntersection(prestepv, poststepv);
      if (((min_x < isl.X()) && (isl.X() < max_x)) && ((min_y < isl.Y()) && (isl.Y() < max_y))) {
        OK2 = OK2 && true;
      }
      if (verbose) {
        std::cout << "Xcross2: " << min_x << ' ' << isl.X() << ' ' << max_x << std::endl;
        std::cout << "Ycross2: " << min_y << ' ' << isl.Y() << ' ' << max_y << std::endl;
        std::cout << "=> " << OK2 << std::endl;
      }
      lend = isl;

      if (OK2) { // the intersections are on the XY planes withing boarders at both z_min and z_max
        if (verbose)
          std::cout << "PndRadMapBoxMesh::CalcFluence - 3" << std::endl;
        prestepv = lbeg;
        poststepv = lend;
        prebin = _MeshHisto->FindBin(prestepv.X(), prestepv.Y());
        postbin = _MeshHisto->FindBin(poststepv.X(), poststepv.Y());
        _MeshHisto->GetBinXYZ(prebin, prebinX, prebinY, prebinZ);
        _MeshHisto->GetBinXYZ(postbin, postbinX, postbinY, postbinZ);
      } else {
        if (verbose)
          std::cout << "PndRadMapBoxMesh::CalcFluence - 4" << std::endl;
      }
    } else {
      // if(prebin != postbin){
      int counter = 0;
      while ((prebin != postbin) && !tn.Z()) {
        counter++;
        PV.clear();

        //       max
        //  /---/      y|
        // /---/|       |   /z
        // ||  ||       |  /
        // |/--|/       | /
        // -----        |/
        // min          +----------
        //                        x

        // .              corner1          corner2          corner3
        // plane1   [(min, min, min), (min, min, max), (max, min, min)] ZX
        // plane2   [(min, min, min), (max, min, min), (min, max, min)] XY
        // plane3   [(min, min, min), (min, min, max), (min, max, min)] ZY

        // plane4   [(min, max, min), (min, max, max), (max, max, min)] ZX
        // plane5   [(min, min, max), (max, min, max), (min, max, max)] XY
        // plane6   [(max, min, min), (max, min, max), (max, max, min)] ZY

        // //plane4   [(max, max, max), (max, max, min), (min, max, max)] XZ
        // //plane5   [(max, max, max), (min, max, max), (max, min, max)]
        // //plane6   [(max, max, max), (max, max, min), (max, min, max)]

        min_x = (_MeshHisto->GetXaxis()->GetBinLowEdge(prebinX) < 0) ? 0 : _MeshHisto->GetXaxis()->GetBinLowEdge(prebinX);
        min_y = (_MeshHisto->GetYaxis()->GetBinLowEdge(prebinY) < 0) ? 0 : _MeshHisto->GetYaxis()->GetBinLowEdge(prebinY);
        min_z = (_MeshHisto->GetZaxis()->GetBinLowEdge(prebinZ) < 0) ? 0 : _MeshHisto->GetZaxis()->GetBinLowEdge(prebinZ);

        max_x = (_MeshHisto->GetXaxis()->GetBinUpEdge(prebinX) < 0) ? 0 : _MeshHisto->GetXaxis()->GetBinUpEdge(prebinX);
        max_y = (_MeshHisto->GetYaxis()->GetBinUpEdge(prebinY) < 0) ? 0 : _MeshHisto->GetYaxis()->GetBinUpEdge(prebinY);
        max_z = (_MeshHisto->GetZaxis()->GetBinUpEdge(prebinZ) < 0) ? 0 : _MeshHisto->GetZaxis()->GetBinUpEdge(prebinZ);
        max_z += _MeshHisto->GetZaxis()->GetBinWidth(1);

        if (verbose) {
          std::cout << counter << " - Corners [[" << min_x << ", " << min_y << ", " << min_z << "] - ";
          std::cout << "[" << max_x << ", " << max_y << ", " << max_z << "]]\n";
        }

        // plane1
        plane = PndRadMapPlane(TVector3(min_x + dx, min_y + dy, min_z + dz), TVector3(min_x + dx, min_y + dy, max_z + dz), TVector3(max_x + dx, min_y + dy, min_z + dz));
        PV.push_back(plane);
        // plane2
        plane = PndRadMapPlane(TVector3(min_x + dx, min_y + dy, min_z + dz), TVector3(max_x + dx, min_y + dy, min_z + dz), TVector3(min_x + dx, max_y + dy, min_z + dz));
        PV.push_back(plane);
        // plane3
        plane = PndRadMapPlane(TVector3(min_x + dx, min_y + dy, min_z + dz), TVector3(min_x + dx, min_y + dy, max_z + dz), TVector3(min_x + dx, max_y + dy, min_z + dz));
        PV.push_back(plane);
        // plane4
        plane = PndRadMapPlane(TVector3(min_x + dx, max_y + dy, min_z + dz), TVector3(min_x + dx, max_y + dy, max_z + dz), TVector3(max_x + dx, max_y + dy, min_z + dz));
        PV.push_back(plane);
        // plane5
        plane = PndRadMapPlane(TVector3(min_x + dx, min_y + dy, max_z + dz), TVector3(max_x + dx, min_y + dy, max_z + dz), TVector3(min_x + dx, max_y + dy, max_z + dz));
        PV.push_back(plane);
        // plane6
        plane = PndRadMapPlane(TVector3(max_x + dx, min_y + dy, min_z + dz), TVector3(max_x + dx, min_y + dy, max_z + dz), TVector3(max_x + dx, max_y + dy, min_z + dz));
        PV.push_back(plane);

        if (verbose)
          std::cout << "Start\n[" << prebin << " {" << prestepv.X() << ' ' << prestepv.Y() << ' ' << prestepv.Z() << "}(" << prebinX << ' ' << prebinY << ' ' << prebinZ << ")\n"
                    << ' ' << postbin << " {" << poststepv.X() << ' ' << poststepv.Y() << ' ' << poststepv.Z() << "}(" << postbinX << ' ' << postbinY << ' ' << postbinZ
                    << ")]\n\n";

        minp = std::make_pair(-1, 111111); //(which plane, what is the disrance)

        // calculate the intersection with every each planes
        isl = PV.at(0).LineIntersection(prestepv, poststepv);
        if (momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl - prestepv)))
          if (minp.second > (isl - prestepv).Mag())
            minp = std::make_pair(0, (isl - prestepv).Mag());
        if (verbose)
          std::cout << "XZ (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") " << (poststepv - prestepv).Mag() << ", "
                    << "(" << (isl - prestepv).X() << ", " << (isl - prestepv).Y() << ", " << (isl - prestepv).Z() << ") " << (isl - prestepv).Mag() << ' '
                    << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl - prestepv)) << ' ' << (minp.second > (isl - prestepv).Mag()) << ' ' << "[" << minp.first
                    << ", " << minp.second << std::endl;

        if (!OK2) { // FIXME: OK2 was not set before!

          isl = PV.at(1).LineIntersection(prestepv, poststepv);
          if (momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl - prestepv)))
            if (minp.second > (isl - prestepv).Mag())
              minp = std::make_pair(1, (isl - prestepv).Mag());
          if (verbose)
            std::cout << "XY (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") " << (poststepv - prestepv).Mag() << ", "
                      << "(" << (isl - prestepv).X() << ", " << (isl - prestepv).Y() << ", " << (isl - prestepv).Z() << ") " << (isl - prestepv).Mag() << ' '
                      << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl - prestepv)) << ' ' << (minp.second > (isl - prestepv).Mag()) << ' ' << "[" << minp.first
                      << ", " << minp.second << "]" << std::endl;
        }

        isl = PV.at(2).LineIntersection(prestepv, poststepv);
        if (momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl - prestepv)))
          if (minp.second > (isl - prestepv).Mag())
            minp = std::make_pair(2, (isl - prestepv).Mag());
        if (verbose)
          std::cout << "YZ (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") " << (poststepv - prestepv).Mag() << ", "
                    << "(" << (isl - prestepv).X() << ", " << (isl - prestepv).Y() << ", " << (isl - prestepv).Z() << ") " << (isl - prestepv).Mag() << ' '
                    << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl - prestepv)) << ' ' << (minp.second > (isl - prestepv).Mag()) << ' ' << "[" << minp.first
                    << ", " << minp.second << "]" << std::endl;

        isl = PV.at(3).LineIntersection(prestepv, poststepv);
        if (momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl - prestepv)))
          if (minp.second > (isl - prestepv).Mag())
            minp = std::make_pair(3, (isl - prestepv).Mag());
        if (verbose)
          std::cout << "XZ (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") " << (poststepv - prestepv).Mag() << ", "
                    << "(" << (isl - prestepv).X() << ", " << (isl - prestepv).Y() << ", " << (isl - prestepv).Z() << ") " << (isl - prestepv).Mag() << ' '
                    << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl - prestepv)) << ' ' << (minp.second > (isl - prestepv).Mag()) << ' ' << "[" << minp.first
                    << ", " << minp.second << "]" << std::endl;

        if (!OK2) { // FIXME: OK2 was not set before!
          isl = PV.at(4).LineIntersection(prestepv, poststepv);
          if (momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl - prestepv)))
            if (minp.second > (isl - prestepv).Mag())
              minp = std::make_pair(4, (isl - prestepv).Mag());
          if (verbose)
            std::cout << "XY (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") " << (poststepv - prestepv).Mag() << ", "
                      << "(" << (isl - prestepv).X() << ", " << (isl - prestepv).Y() << ", " << (isl - prestepv).Z() << ") " << (isl - prestepv).Mag() << ' '
                      << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl - prestepv)) << ' ' << (minp.second > (isl - prestepv).Mag()) << ' ' << "[" << minp.first
                      << ", " << minp.second << "]" << std::endl;
        }

        isl = PV.at(5).LineIntersection(prestepv, poststepv);
        if (momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl - prestepv)))
          if (minp.second > (isl - prestepv).Mag())
            minp = std::make_pair(5, (isl - prestepv).Mag());
        if (verbose)
          std::cout << "YZ (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") " << (poststepv - prestepv).Mag() << ", "
                    << "(" << (isl - prestepv).X() << ", " << (isl - prestepv).Y() << ", " << (isl - prestepv).Z() << ") " << (isl - prestepv).Mag() << ' '
                    << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl - prestepv)) << ' ' << (minp.second > (isl - prestepv).Mag()) << ' ' << "[" << minp.first
                    << ", " << minp.second << "]" << std::endl;

        if (verbose)
          std::cout << "\nmomentum: " << p->GetPx() << ' ' << p->GetPy() << ' ' << p->GetPz() << std::endl;

        // minp stores the smallest possible step
        if (0 <= minp.first) {
          if (verbose) {
            std::cout << "\nmomentum: " << p->GetPx() << ' ' << p->GetPy() << ' ' << p->GetPz() << std::endl;
            std::cout << "[(----)] " << minp.first << ' ' << minp.second << std::endl;
            std::cout << "normal: " << PV.at(minp.first).Normal().X() << ' ' << PV.at(minp.first).Normal().Y() << ' ' << PV.at(minp.first).Normal().Z() << std::endl;
          }

          isl = PV.at(minp.first).LineIntersection(prestepv, poststepv);
          Double_t step = (isl - prestepv).Mag();
          prestepv = isl;

          _MeshHisto->Fill(prestepv.X(), prestepv.Y(), step);
          _StatHisto->Fill(prestepv.X(), prestepv.Y());

          tn = PV.at(minp.first).Normal();
          float ds = 0;
          if (tn.X()) {
            ds = _MeshHisto->GetXaxis()->GetBinWidth(1) / 10.;
            if (verbose)
              std::cout << "Xprebin: " << prebin << " ---> ";
            if (p->GetPx() > 0) {
              prebin = _MeshHisto->FindBin(isl.X() + ds, isl.Y());
              if (verbose)
                std::cout << "(" << isl.X() << " ---> " << isl.X() + ds << ' ' << p->GetPx() << " ) ";
            } else {
              prebin = _MeshHisto->FindBin(isl.X() - ds, isl.Y());
              if (verbose)
                std::cout << "(" << isl.X() << " ---> " << isl.X() - ds << ' ' << p->GetPx() << " ) ";
            }
            if (verbose)
              std::cout << prebin << std::endl;
          }
          if (tn.Y()) {
            ds = _MeshHisto->GetYaxis()->GetBinWidth(1) / 10.;
            if (verbose)
              std::cout << "Yprebin: " << prebin << " ---> ";
            if (p->GetPy() > 0) {
              prebin = _MeshHisto->FindBin(isl.X(), isl.Y() + ds);
              if (verbose)
                std::cout << "1(" << isl.Y() << " ---> " << isl.Y() + ds << " ) ";
            } else {
              prebin = _MeshHisto->FindBin(isl.X(), isl.Y() - ds);
              if (verbose)
                std::cout << "1(" << isl.Y() << " ---> " << isl.Y() - ds << " ) ";
            }
            if (verbose)
              std::cout << prebin << std::endl;
          }

          _MeshHisto->GetBinXYZ(prebin, prebinX, prebinY, prebinZ);
          if (verbose)
            std::cout << "Stop\n[" << prebin << " {" << prestepv.X() << ' ' << prestepv.Y() << ' ' << prestepv.Z() << "}(" << prebinX << ' ' << prebinY << ' ' << prebinZ << ")\n"
                      << ' ' << postbin << " {" << poststepv.X() << ' ' << poststepv.Y() << ' ' << poststepv.Z() << "}(" << postbinX << ' ' << postbinY << ' ' << postbinZ
                      << ")]\n\n";

          if (IsInside(p->GetX(), p->GetY(), p->GetZ())) {
            if (verbose)
              std::cout << " Bingo\n";
          }
        }
      }
    }
  }
  return minp.second;
}

void InvMatVecProd(TMatrixD mat, TVector3 vec, TVector3 &res)
{
  TMatrixD imat = mat.Invert();
  double val;
  double resl[3];
  for (int i = 0; i < 3; i++) {
    val = 0;
    for (int j = 0; j < 3; j++) {
      val += imat[i][j] * vec[j];
    }
    resl[i] = val;
  }
  res = TVector3(resl[0], resl[1], resl[2]);
}
