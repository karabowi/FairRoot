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

// -------------------------------------------------------------------------
// -----                      PndRichCalDb source file                  -----
// -----         Created 12/01/04  by M. Al/Turany (FairField.cxx)      -----
// -------------------------------------------------------------------------

#include <iomanip>
#include <iostream>
#include <fstream>
#include "stdlib.h"

#include "TArrayF.h"
#include "TFile.h"
#include "TMath.h"
#include "TObjArray.h"
#include "FairLogger.h"

#include "PndRichCalDb.h"
#include "PndRichCalDbData.h"
#include "PndRichCalDbPar.h"

using namespace std;

// -------------   Default constructor  ----------------------------------
PndRichCalDb::PndRichCalDb()
  : FairField(), fFileName(""), fPmin(0), fPmax(0), fPstep(0), fXmin(0), fXmax(0), fXstep(0), fYmin(0), fYmax(0), fYstep(0), fTmin(0), fTmax(0), fTstep(0), fFmin(0), fFmax(0),
    fFstep(0), fNp(0), fNx(0), fNy(0), fNt(0), fNf(0), fBetaMean(nullptr), fBetaSig(nullptr), fBetaEff(nullptr)
{
  SetName("");
  fType = 0;
  fPmin = 1;
  fPmax = 11;
  fPstep = 10;
  fXmin = 2;
  fXmax = 12;
  fXstep = 10;
  fYmin = 3;
  fYmax = 13;
  fYstep = 10;
  fTmin = 4;
  fTmax = 14;
  fTstep = 10;
  fFmin = 5;
  fFmax = 15;
  fFstep = 10;
  fNp = 2;
  fNx = 2;
  fNy = 2;
  fNt = 2;
  fNf = 2;
  Int_t size = fNp * fNx * fNy * fNt * fNf;
  fBetaSig = new TArrayF(size);
  fBetaMean = new TArrayF(size);
  fBetaEff = new TArrayF(size);
}
// ------------------------------------------------------------------------

// -------------   Standard constructor   ---------------------------------
PndRichCalDb::PndRichCalDb(const char *mapName, const char *fileType)
  : FairField(mapName), fFileName(""), fPmin(0), fPmax(0), fPstep(0), fXmin(0), fXmax(0), fXstep(0), fYmin(0), fYmax(0), fYstep(0), fTmin(0), fTmax(0), fTstep(0), fFmin(0),
    fFmax(0), fFstep(0), fNp(0), fNx(0), fNy(0), fNt(0), fNf(0), fBetaMean(nullptr), fBetaSig(nullptr), fBetaEff(nullptr)
{
  SetName(mapName);
  TString dir = getenv("VMCWORKDIR");
  fFileName = dir + "/detectors/rich/" + mapName;
  if (fileType[0] == 'R')
    fFileName += ".root";
  else
    fFileName += ".dat";
  fType = 1;
}
// ------------------------------------------------------------------------

// ------------   Constructor from PndRichCalDbPar   --------------------------
PndRichCalDb::PndRichCalDb(PndRichCalDbPar *calDbPar)
  : FairField(), fFileName(""), fPmin(0), fPmax(0), fPstep(0), fXmin(0), fXmax(0), fXstep(0), fYmin(0), fYmax(0), fYstep(0), fTmin(0), fTmax(0), fTstep(0), fFmin(0), fFmax(0),
    fFstep(0), fNp(0), fNx(0), fNy(0), fNt(0), fNf(0), fBetaMean(nullptr), fBetaSig(nullptr), fBetaEff(nullptr)
{
  fType = 1;
  if (!calDbPar) {
    LOG(error) << "PndRichCalDb::PndRichCalDb: empty parameter container!";
    SetName("");
    fType = -1;
  } else {
    TString Name = GetName();
    calDbPar->MapName(Name);
    TString dir = getenv("VMCWORKDIR");
    fFileName = dir + "/detectors/rich/" + Name + ".root";
    fFileName = Name + ".root";
    fType = calDbPar->GetType();
  }
}
// ------------------------------------------------------------------------

// -------------   ----Copy constructor   ---------------------------------
PndRichCalDb::PndRichCalDb(const PndRichCalDb &L)
  : FairField(), fFileName(L.fFileName), fPmin(L.fPmin), fPmax(L.fPmax), fPstep(L.fPstep), fXmin(L.fXmin), fXmax(L.fXmax), fXstep(L.fXstep), fYmin(L.fYmin), fYmax(L.fYmax),
    fYstep(L.fYstep), fTmin(L.fTmin), fTmax(L.fTmax), fTstep(L.fTstep), fFmin(L.fFmin), fFmax(L.fFmax), fFstep(L.fFstep), fNp(L.fNp), fNx(L.fNx), fNy(L.fNy), fNt(L.fNt),
    fNf(L.fNf), fBetaMean(L.fBetaMean), fBetaSig(L.fBetaSig), fBetaEff(L.fBetaEff)
{
  fType = L.fType;
}
// ------------------------------------------------------------------------

// ------------   Destructor   --------------------------------------------
PndRichCalDb::~PndRichCalDb()
{
  if (fBetaMean)
    delete fBetaMean;
  if (fBetaSig)
    delete fBetaSig;
  if (fBetaEff)
    delete fBetaEff;
}
// ------------------------------------------------------------------------

// -----------   Intialisation   ------------------------------------------
void PndRichCalDb::Init()
{
  if (fFileName.EndsWith(".root"))
    ReadRootFile(fFileName, fName);
  else if (fFileName.EndsWith(".dat"))
    ReadAsciiFile(fFileName);
  else {
    LOG(error) << "-E- PndRichCalDb::Init: No proper file name defined! (" << fFileName.Data() << ")";
    Fatal("Init", "No proper file name");
  }
}
// ------------------------------------------------------------------------

// -----------   Get beta mean   ---------------------------
Double_t PndRichCalDb::GetBetaMean(dbpoint pnt)
{

  Double_t p = pnt.beta;
  Double_t x = pnt.x;
  Double_t y = pnt.y;
  Double_t t = pnt.theta * 180 / 3.1415927;
  Double_t f = pnt.phi * 180 / 3.1415927;
  Int_t ip = 0;
  Int_t ix = 0;
  Int_t iy = 0;
  Int_t it = 0;
  Int_t iq = 0;
  Double_t dp = 0.;
  Double_t dx = 0.;
  Double_t dy = 0.;
  Double_t dt = 0.;
  Double_t df = 0.;

  if (x >= 0 && y < 0) {
    f = 360 - f;
    y = -y;
  }
  if (x < 0 && y >= 0) {
    f = 180 - f;
    x = -x;
  }
  if (x < 0 && y < 0) {
    f = 180 + f;
    x = -x;
    y = -y;
  }
  if (f > 360)
    f -= 360;
  if (f < 0)
    f += 360;

  if (IsInside(p, x, y, t, f, ip, ix, iy, it, iq, dp, dx, dy, dt, df)) {

    Double_t pq[5][2];
    pq[0][1] = dp;
    pq[0][0] = 1 - pq[0][1];
    pq[1][1] = dx;
    pq[1][0] = 1 - pq[1][1];
    pq[2][1] = dy;
    pq[2][0] = 1 - pq[2][1];
    pq[3][1] = dt;
    pq[3][0] = 1 - pq[3][1];
    pq[4][1] = df;
    pq[4][0] = 1 - pq[4][1];

    Double_t value = 0;
    Int_t index;
    for (Int_t bp = 0; bp < 2; bp++) {
      for (Int_t bx = 0; bx < 2; bx++) {
        for (Int_t by = 0; by < 2; by++) {
          for (Int_t bt = 0; bt < 2; bt++) {
            for (Int_t bf = 0; bf < 2; bf++) {
              index = (ip + bp) * fNx * fNy * fNt * fNf + (ix + bx) * fNy * fNt * fNf + (iy + by) * fNt * fNf + (it + bt) * fNf + (iq + bf);
              value += fBetaMean->At(index) * pq[0][bp] * pq[1][bx] * pq[2][by] * pq[3][bt] * pq[4][bf];
            }
          }
        }
      }
    }

    // Return interpolated field value
    return value;
  }

  return 0.;
}
// ------------------------------------------------------------------------

// -----------   Get beta sigma   ---------------------------
Double_t PndRichCalDb::GetBetaSig(dbpoint pnt)
{

  Double_t p = pnt.beta;
  Double_t x = pnt.x;
  Double_t y = pnt.y;
  Double_t t = pnt.theta * 180 / 3.1415927;
  Double_t f = pnt.phi * 180 / 3.1415927;
  Int_t ip = 0;
  Int_t ix = 0;
  Int_t iy = 0;
  Int_t it = 0;
  Int_t iq = 0;
  Double_t dp = 0.;
  Double_t dx = 0.;
  Double_t dy = 0.;
  Double_t dt = 0.;
  Double_t df = 0.;

  if (x >= 0 && y < 0) {
    f = 360 - f;
    y = -y;
  }
  if (x < 0 && y >= 0) {
    f = 180 - f;
    x = -x;
  }
  if (x < 0 && y < 0) {
    f = 180 + f;
    x = -x;
    y = -y;
  }
  if (f > 360)
    f -= 360;
  if (f < 0)
    f += 360;

  if (IsInside(p, x, y, t, f, ip, ix, iy, it, iq, dp, dx, dy, dt, df)) {

    Double_t pq[5][2];
    pq[0][1] = dp;
    pq[0][0] = 1 - pq[0][1];
    pq[1][1] = dx;
    pq[1][0] = 1 - pq[1][1];
    pq[2][1] = dy;
    pq[2][0] = 1 - pq[2][1];
    pq[3][1] = dt;
    pq[3][0] = 1 - pq[3][1];
    pq[4][1] = df;
    pq[4][0] = 1 - pq[4][1];

    Double_t value = 0;
    Int_t index;
    for (Int_t bp = 0; bp < 2; bp++) {
      for (Int_t bx = 0; bx < 2; bx++) {
        for (Int_t by = 0; by < 2; by++) {
          for (Int_t bt = 0; bt < 2; bt++) {
            for (Int_t bf = 0; bf < 2; bf++) {
              index = (ip + bp) * fNx * fNy * fNt * fNf + (ix + bx) * fNy * fNt * fNf + (iy + by) * fNt * fNf + (it + bt) * fNf + (iq + bf);
              value += fBetaSig->At(index) * pq[0][bp] * pq[1][bx] * pq[2][by] * pq[3][bt] * pq[4][bf];
            }
          }
        }
      }
    }

    // Return interpolated field value
    return value;
  }

  return 0.;
}
// ------------------------------------------------------------------------

// -----------   Get beta sigma   ---------------------------
Double_t PndRichCalDb::GetBetaEff(dbpoint pnt)
{

  Double_t p = pnt.beta;
  Double_t x = pnt.x;
  Double_t y = pnt.y;
  Double_t t = pnt.theta * 180 / 3.1415927;
  Double_t f = pnt.phi * 180 / 3.1415927;
  Int_t ip = 0;
  Int_t ix = 0;
  Int_t iy = 0;
  Int_t it = 0;
  Int_t iq = 0;
  Double_t dp = 0.;
  Double_t dx = 0.;
  Double_t dy = 0.;
  Double_t dt = 0.;
  Double_t df = 0.;

  if (x >= 0 && y < 0) {
    f = 360 - f;
    y = -y;
  }
  if (x < 0 && y >= 0) {
    f = 180 - f;
    x = -x;
  }
  if (x < 0 && y < 0) {
    f = 180 + f;
    x = -x;
    y = -y;
  }
  if (f > 360)
    f -= 360;
  if (f < 0)
    f += 360;

  if (IsInside(p, x, y, t, f, ip, ix, iy, it, iq, dp, dx, dy, dt, df)) {

    Double_t pq[5][2];
    pq[0][1] = dp;
    pq[0][0] = 1 - pq[0][1];
    pq[1][1] = dx;
    pq[1][0] = 1 - pq[1][1];
    pq[2][1] = dy;
    pq[2][0] = 1 - pq[2][1];
    pq[3][1] = dt;
    pq[3][0] = 1 - pq[3][1];
    pq[4][1] = df;
    pq[4][0] = 1 - pq[4][1];

    Double_t value = 0;
    Int_t index;
    for (Int_t bp = 0; bp < 2; bp++) {
      for (Int_t bx = 0; bx < 2; bx++) {
        for (Int_t by = 0; by < 2; by++) {
          for (Int_t bt = 0; bt < 2; bt++) {
            for (Int_t bf = 0; bf < 2; bf++) {
              index = (ip + bp) * fNx * fNy * fNt * fNf + (ix + bx) * fNy * fNt * fNf + (iy + by) * fNt * fNf + (it + bt) * fNf + (iq + bf);
              value += fBetaEff->At(index) * pq[0][bp] * pq[1][bx] * pq[2][by] * pq[3][bt] * pq[4][bf];
            }
          }
        }
      }
    }

    // Return interpolated field value
    return value;
  }

  return 0.;
}
// ------------------------------------------------------------------------

// -----------   Check whether a point is inside the map   ----------------
Bool_t PndRichCalDb::IsInside(Double_t p, Double_t x, Double_t y, Double_t t, Double_t f, Int_t &ip, Int_t &ix, Int_t &iy, Int_t &it, Int_t &iq, Double_t &dp, Double_t &dx,
                              Double_t &dy, Double_t &dt, Double_t &df)
{

  // ---  Check for being outside the map range
  p = p < fPmax ? p : fPmax - 0.00001;
  t = t < fTmax ? t : fTmax - 0.01;
  if (!(p >= fPmin && p < fPmax && x >= fXmin && x < fXmax && y >= fYmin && y < fYmax && t >= fTmin && t < fTmax && f >= fFmin && f < fFmax)) {
    ip = ix = iy = it = iq = 0;
    dp = dx = dy = dt = df = 0.;
    return kFALSE;
  }

  dp = (p - fPmin) / fPstep;
  dx = (x - fXmin) / fXstep;
  dy = (y - fYmin) / fYstep;
  dt = (t - fTmin) / fTstep;
  df = (f - fFmin) / fFstep;

  // --- Determine grid cell
  ip = Int_t(dp);
  ix = Int_t(dx);
  iy = Int_t(dy);
  it = Int_t(dt);
  iq = Int_t(df);

  // Relative distance from grid point (in units of cell size)
  dp -= Double_t(ip);
  dx -= Double_t(ix);
  dy -= Double_t(iy);
  dt -= Double_t(it);
  df -= Double_t(iq);

  return kTRUE;
}
// ------------------------------------------------------------------------

// ----------   Write the map to an ASCII file   --------------------------
void PndRichCalDb::WriteAsciiFile(const char *fileName)
{

  // Open file
  LOG(info) << "PndRichCalDb: Writing field map to ASCII file " << fileName;
  ofstream mapFile(fileName);
  if (!mapFile.is_open()) {
    LOG(error) << "PndRichCalDb:ReadAsciiFile: Could not open file! ";
    return;
  }

  // Write field map grid parameters
  mapFile.precision(4);
  mapFile << showpoint;

  mapFile << fPmin << " " << fPmax << " " << fNp << endl;
  mapFile << fXmin << " " << fXmax << " " << fNx << endl;
  mapFile << fYmin << " " << fYmax << " " << fNy << endl;
  mapFile << fTmin << " " << fTmax << " " << fNt << endl;
  mapFile << fFmin << " " << fFmax << " " << fNf << endl;

  // Write field values
  cout << right;
  // Int_t nTot = fNp * fNx * fNy * fNt * fNf; //[R.K. 01/2017] unused variable?
  cout << "-I- PndRichCalDb: " << fNp * fNx * fNy * fNt * fNf << " entries to write... " << setw(3) << 0 << " % ";
  Int_t index = 0;
  //  div_t modul;
  //  Int_t iDiv = TMath::Nint(nTot/100.);
  for (Int_t ip = 0; ip < fNp; ip++) {
    for (Int_t ix = 0; ix < fNx; ix++) {
      for (Int_t iy = 0; iy < fNy; iy++) {
        for (Int_t it = 0; it < fNt; it++) {
          for (Int_t iq = 0; iq < fNf; iq++) {
            index = ip * fNx * fNy * fNt * fNf + ix * fNy * fNt * fNf + iy * fNt * fNf + it * fNf + iq;
            /*                 if (iDiv!=0)
                             {
                                modul = div(index,iDiv);
                                if ( modul.rem == 0 ) {
                                   Double_t perc = TMath::Nint(100.*index/nTot);
                                   cout << "\b\b\b\b\b\b" << setw(3) << perc << " % " << flush;
                                }
                             }*/
            mapFile << fBetaMean->At(index) << " " << fBetaSig->At(index) << " " << fBetaEff->At(index) << endl;
          } // f-Loop
        }   // t-Loop
      }     // y-Loop
    }       // x-Loop
  }         // p-Loop
  mapFile.close();
}
// ------------------------------------------------------------------------

// -------   Write field map to a ROOT file   -----------------------------
void PndRichCalDb::WriteRootFile(const char *fileName, const char *mapName)
{

  PndRichCalDbData *data = new PndRichCalDbData(mapName, *this);
  TFile *oldFile = gFile;
  TFile *file = new TFile(fileName, "RECREATE");
  data->Write();
  file->Close();
  if (oldFile)
    oldFile->cd();
}
// ------------------------------------------------------------------------

// ---------   Screen output   --------------------------------------------
void PndRichCalDb::Print()
{
  TString type = "Map";
  if (fType == 2)
    type = "Soleniod Map ";
  if (fType == 3)
    type = "Dipole Map ";
  if (fType == 4)
    type = "Trans Map ";
  cout << "======================================================" << endl;
  cout.precision(4);
  cout << showpoint;
  cout << "----  " << fTitle << " : " << fName << endl;
  cout << "----" << endl;
  cout << "----  Field type     : " << type << endl;
  cout << "----" << endl;
  cout << "----  Field map grid : " << endl;
  cout << "----  p = " << setw(4) << fPmin << " to " << setw(4) << fPmax << "   , " << fNp << " grid points, dp = " << fPstep << "   " << endl;
  cout << "----  x = " << setw(4) << fXmin << " to " << setw(4) << fXmax << " cm, " << fNx << " grid points, dx = " << fXstep << " cm" << endl;
  cout << "----  y = " << setw(4) << fYmin << " to " << setw(4) << fYmax << " cm, " << fNy << " grid points, dy = " << fYstep << " cm" << endl;
  cout << "----  t = " << setw(4) << fTmin << " to " << setw(4) << fTmax << "   , " << fNt << " grid points, dt = " << fTstep << "   " << endl;
  cout << "----  f = " << setw(4) << fFmin << " to " << setw(4) << fFmax << "   , " << fNf << " grid points, df = " << fFstep << "   " << endl;
  cout << endl;
  /*  cout << "----  Field scaling factor: " << fScale << endl;
    Double_t bx = GetBx(0.,0.,0.);
    Double_t by = GetBy(0.,0.,0.);
    Double_t bz = GetBz(0.,0.,0.);
    cout << "----" << endl;
    cout << "----  Field at origin is ( " << setw(6) << bx << ", " << setw(6)
         << by << ", " << setw(6) << bz << ") kG" << endl;*/
  cout << "======================================================" << endl;
}
// ------------------------------------------------------------------------

// ---------    Reset parameters and data (private)  ----------------------
void PndRichCalDb::Reset()
{
  fPmin = fXmin = fYmin = fTmin = fFmin = 0;
  fPmax = fXmax = fYmax = fTmax = fFmax = 0;
  fPstep = fXstep = fYstep = fTstep = fFstep = 0;
  fNp = fNx = fNy = fNt = fNf = 0;
  if (fBetaMean) {
    delete fBetaMean;
    fBetaMean = nullptr;
  }
  if (fBetaSig) {
    delete fBetaSig;
    fBetaSig = nullptr;
  }
  if (fBetaEff) {
    delete fBetaEff;
    fBetaEff = nullptr;
  }
}
// ------------------------------------------------------------------------

// -----   Read field map from ASCII file (private)   ---------------------
void PndRichCalDb::ReadAsciiFile(const char *fileName)
{

  // Open file
  LOG(info) << " PndRichCalDb: Reading field map from ASCII file " << fileName;
  ifstream mapFile(fileName);
  if (!mapFile.is_open()) {
    cerr << "-E- PndRichCalDb:ReadAsciiFile: Could not open file! " << endl;
    Fatal("ReadAsciiFile", "Could not open file");
  }

  // Read grid parameters

  mapFile >> fPmin >> fPmax >> fNp;
  mapFile >> fXmin >> fXmax >> fNx;
  mapFile >> fYmin >> fYmax >> fNy;
  mapFile >> fTmin >> fTmax >> fNt;
  mapFile >> fFmin >> fFmax >> fNf;
  fPstep = (fPmax - fPmin) / Double_t(fNp - 1);
  fXstep = (fXmax - fXmin) / Double_t(fNx - 1);
  fYstep = (fYmax - fYmin) / Double_t(fNy - 1);
  fTstep = (fTmax - fTmin) / Double_t(fNt - 1);
  fFstep = (fFmax - fFmin) / Double_t(fNf - 1);

  // Create field arrays
  fBetaMean = new TArrayF(fNp * fNx * fNy * fNt * fNf);
  fBetaSig = new TArrayF(fNp * fNx * fNy * fNt * fNf);
  fBetaEff = new TArrayF(fNp * fNx * fNy * fNt * fNf);

  // Read the field values
  cout << right;
  Int_t nTot = fNp * fNx * fNy * fNt * fNf;
  cout << "-I- PndRichCalDb: " << nTot << " entries to read... " << setw(3) << 0 << " % ";
  Int_t index = 0;
  //  div_t modul;
  //  Int_t iDiv = TMath::Nint(nTot/100.);
  Double_t m, s, e;
  for (Int_t ip = 0; ip < fNp; ip++) {
    for (Int_t ix = 0; ix < fNx; ix++) {
      for (Int_t iy = 0; iy < fNy; iy++) {
        for (Int_t it = 0; it < fNt; it++) {
          for (Int_t iq = 0; iq < fNf; iq++) {
            if (!mapFile.good())
              cerr << "-E- PndRichCalDb::ReadAsciiFile: "
                   << "I/O Error at " << ip << " " << ix << " " << iy << " " << it << " " << iq << endl;
            index = ip * fNx * fNy * fNt * fNf + ix * fNy * fNt * fNf + iy * fNt * fNf + it * fNf + iq;
            /*                 if (iDiv!=0)
                             {
                                modul = div(index,iDiv);
                                if ( modul.rem == 0 ) {
                                   Double_t perc = TMath::Nint(100.*index/nTot);
                                   cout << "\b\b\b\b\b\b" << setw(3) << perc << " % " << flush;
                                }
                             }*/
            mapFile >> m >> s >> e;
            if (s < 0 || e < 0 || e > 1)
              cout << index << " " << m << " " << s << " " << e << endl;
            fBetaMean->AddAt(m, index);
            fBetaSig->AddAt(s, index);
            fBetaEff->AddAt(e, index);
            if (mapFile.eof()) {
              cerr << endl
                   << "-E- PndRichCalDb::ReadAsciiFile: EOF"
                   << " reached at " << ip << " " << ix << " " << iy << " " << it << " " << iq << endl;
              mapFile.close();
              break;
            }
          } // f-Loop
        }   // t-Loop
      }     // y-Loop
    }       // x-Loop
  }         // p-Loop

  cout << "   " << index + 1 << " read" << endl;

  mapFile.close();
}
// ------------------------------------------------------------------------

// -------------   Read field map from ROOT file (private)  ---------------
void PndRichCalDb::ReadRootFile(const char *fileName, const char *mapName)
{

  // Store gFile pointer
  TFile *oldFile = gFile;

  // Open root file
  LOG(info) << "PndRichCalDb: Reading field map from ROOT file  " << fileName;
  TFile *file = new TFile(fileName, "READ");
  if (file->IsZombie()) {
    LOG(error) << "-E- PndRichCalDb::ReadRootfile: Cannot read from file! (" << fileName << ")";
    Fatal("ReadRootFile", "Cannot read from file");
  }

  // Get the field data object
  PndRichCalDbData *data = nullptr;
  file->GetObject(mapName, data);
  if (!data) {
    LOG(error) << "PndRichCalDb::ReadRootFile: data object " << fileName << " not found in file! ";
    exit(-1);
  }

  // Get the field parameters
  SetCalDb(data);

  // Close the root file and delete the data object
  file->Close();
  delete data;
  delete file;
  if (oldFile)
    oldFile->cd();
}
// ------------------------------------------------------------------------

// ------------   Set field parameters and data (private)  ----------------
void PndRichCalDb::SetCalDb(const PndRichCalDbData *data)
{

  // Check compatibility
  std::cout << "fType = " << fType << " " << data->GetType() << std::endl;
  fType = data->GetType();
  if (data->GetType() != fType) {
    LOG(error) << "PndRichCalDb::SetField: Incompatible map types Field map is of type " << fType << " \n but map on file is of type " << data->GetType();
    Fatal("SetField", "Incompatible map types");
  }

  fPmin = data->GetPmin();
  fXmin = data->GetXmin();
  fYmin = data->GetYmin();
  fTmin = data->GetTmin();
  fFmin = data->GetFmin();
  fPmax = data->GetPmax();
  fXmax = data->GetXmax();
  fYmax = data->GetYmax();
  fTmax = data->GetTmax();
  fFmax = data->GetFmax();
  fNp = data->GetNp();
  fNx = data->GetNx();
  fNy = data->GetNy();
  fNt = data->GetNt();
  fNf = data->GetNf();
  fPstep = (fPmax - fPmin) / Double_t(fNp - 1);
  fXstep = (fXmax - fXmin) / Double_t(fNx - 1);
  fYstep = (fYmax - fYmin) / Double_t(fNy - 1);
  fTstep = (fTmax - fTmin) / Double_t(fNt - 1);
  fFstep = (fFmax - fFmin) / Double_t(fNf - 1);
  if (fBetaMean)
    delete fBetaMean;
  if (fBetaSig)
    delete fBetaSig;
  if (fBetaEff)
    delete fBetaEff;
  fBetaMean = new TArrayF(*(data->GetBetaMean()));
  fBetaSig = new TArrayF(*(data->GetBetaSig()));
  fBetaEff = new TArrayF(*(data->GetBetaEff()));
}
// ------------------------------------------------------------------------

ClassImp(PndRichCalDb)
