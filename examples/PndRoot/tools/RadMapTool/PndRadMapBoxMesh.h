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

#ifndef __PNDRADMAPBOXMESH_HH__
#define __PNDRADMAPBOXMESH_HH__
#include <TH2D.h>
#include <TROOT.h>
#include <TFormula.h>
#include <TMatrixD.h>
#include <FairRadMapPoint.h>
#include <PndRadMapPlane.h>
#include <TDatabasePDG.h>

enum quantity {
  Edep = 1,
  Dose = 2,
  Fluence = 3,
  SimpleFluence = 9, // -> 1/m^2
  Flux = 5,
  Kerma = 6,
  EnergyFluence = 4, // not mplemented
  Density = 7,
  Mass = 8,
  Twos = 10
}; /// debug
//

struct Corner {
  TVector3 corner1;
  TVector3 corner2;
  TVector3 corner3;
};

/* class PndRadMapPlane { */
/*  public: */
/*   PndRadMapPlane(TVector3 _corner1, */
/*         TVector3 _corner2, */
/*         TVector3 _corner3, */
/*         double dist = 0); */
/*   PndRadMapPlane(); */
/*   TVector3 LineIntersection(TVector3 begline, */
/*                             TVector3 endline); */
/*   void SetNormal(TVector3 n){normal = n;}; */
/*   void SetDistance(double d){distance = d;}; */
/*   TVector3 GetCorner(int i); */
/*   TVector3 Corner1(){return corner1;}; */
/*   TVector3 Corner2(){return corner2;}; */
/*   TVector3 Corner3(){return corner3;}; */
/*   double Distance(){return distance;}; */
/*   TVector3 Normal(){return normal;}; */
/*  private: */
/*   TVector3 normal; */
/*   TVector3 corner1; */
/*   TVector3 corner2; */
/*   TVector3 corner3; */
/*   double distance; */
/* }; */

class PndRadMapBoxMesh {
 public:
  PndRadMapBoxMesh();
  PndRadMapBoxMesh(PndRadMapBoxMesh &m); // copy cons
  PndRadMapBoxMesh(const char *Name, int Xbins, Double_t Xlow, Double_t Xhigh, int Ybins, Double_t Ylow, Double_t Yhigh, int Zbins, Double_t Zlow, Double_t Zhigh);
  PndRadMapBoxMesh(const char *name, int xbins, Double_t xlow, Double_t xhigh, int ybins, Double_t ylow, Double_t yhigh, Double_t zlow, Double_t zhigh, orientation plane = ZX,
                   quantity Quantity = Edep);
  ~PndRadMapBoxMesh();

  void SetFilter(const char *filter);
  void SetQuantity(quantity Quantity = Edep);
  void SetOrientation(orientation plane, Double_t rotate = 99999, axis Ax = Xx);
  void SetOrientation(Double_t rotate = 99999, axis Ax = Xx);
  void SetVerbosityLevel(int verbose = 0);
  void Fill(FairRadMapPoint *p);
  void Transform(Double_t X, Double_t Y, Double_t Z);
  void Transform(Double_t X, Double_t Y, Double_t Z, Double_t &X0, Double_t &Y0, Double_t &Z0);
  void Transform(TVector3 InV, TVector3 &OutV);

  void Scale(Double_t sca);
  void Save(TFile *fout);
  void Save();
  TH2D *GetHisto();
  PndRadMapPlane *GetPlane();
  Double_t CalcFluence(FairRadMapPoint *p);

 protected:
  bool IsInside(Double_t X, Double_t Y, Double_t Z);
  bool IsInside(FairRadMapPoint *p);
  bool IsInside();
  void Fill(Int_t gBin, Double_t val);
  void Fill(Double_t X, Double_t Y, Double_t Z, Double_t we = 1);
  void makeHisto(const char *Orient, Double_t rotate, int Hbins, Double_t Hlow, Double_t Hhigh, int Vbins, Double_t Vlow, Double_t Vhigh,
                 Double_t dlow /*, Double_t dhigh --unused parameter */);

  TH2D *_MeshHisto;
  TH2I *_StatHisto;
  TH2D *_EnergyHisto;
  TString _Name;

  orientation _orientation;
  Double_t _rotate;
  axis _axis;
  Double_t _volume;
  quantity _quantity;

  int _Xbins;
  Double_t _Xlow, _Xhigh;
  int _Ybins;
  Double_t _Ylow, _Yhigh;
  int _Zbins;
  Double_t _Zlow, _Zhigh;

  Double_t _X, _Y, _Z;
  Double_t _tX, _tY, _tZ; // back-shifted, back-rotated coordinate

  TFormula _filter;
  int _verbose;

  PndRadMapPlane *_plane;
  TVector3 InterSection;

 private:
  bool _isSurfaceQuantity;
  TDatabasePDG *pdg;
  TParticlePDG *pdgpart;
};

void InvMatVecProd(TMatrixD mat, TVector3 vec, TVector3 &res);

#endif
