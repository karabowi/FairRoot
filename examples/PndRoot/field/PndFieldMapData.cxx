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

#include "TArrayF.h"
#include "PndFieldMap.h"
#include "PndFieldMapData.h"

// -------------   Default constructor  ----------------------------------
PndFieldMapData::PndFieldMapData()
  : fType(1), fXmin(0), fXmax(0), fYmin(0), fYmax(0), fZmin(0), fZmax(0), fUnit(0), fNx(0), fNy(0), fNz(0), fBx(nullptr), fBy(nullptr), fBz(nullptr)
{
}
// ------------------------------------------------------------------------

// -------------   Standard constructor   ---------------------------------
PndFieldMapData::PndFieldMapData(const char *mapName)
  : TNamed(mapName, "PND Field Map Data"), fType(1), fXmin(0), fXmax(0), fYmin(0), fYmax(0), fZmin(0), fZmax(0), fUnit(0), fNx(0), fNy(0), fNz(0), fBx(nullptr), fBy(nullptr),
    fBz(nullptr)
{
}
// ------------------------------------------------------------------------

// -----   Constructor from PndFieldMap   ------------------------------
PndFieldMapData::PndFieldMapData(const char *name, const PndFieldMap &map)
  : TNamed(name, "PND Field Map Data"), fType(map.GetType()), fXmin(map.GetXmin()), fXmax(map.GetXmax()), fYmin(map.GetYmin()), fYmax(map.GetYmax()), fZmin(map.GetZmin()),
    fZmax(map.GetZmax()), fUnit(map.GetUnit()), fNx(map.GetNx()), fNy(map.GetNy()), fNz(map.GetNz()), fBx(new TArrayF(*(map.GetBx()))), fBy(new TArrayF(*(map.GetBy()))),
    fBz(new TArrayF(*(map.GetBz())))

{

  // Take out scaling factor and convert from kG to T
  Double_t factor = map.GetScale() * 10.;
  Int_t index = 0;
  for (Int_t ix = 0; ix < fNx; ix++) {
    for (Int_t iy = 0; iy < fNy; iy++) {
      for (Int_t iz = 0; iz < fNz; iz++) {
        index = ix * fNy * fNz + iy * fNz + iz;
        if (fBx)
          (*fBx)[index] = (*fBx)[index] / factor;
        if (fBy)
          (*fBy)[index] = (*fBy)[index] / factor;
        if (fBz)
          (*fBz)[index] = (*fBz)[index] / factor;
      } // z loop
    }   // y loop
  }     // x loop
}
// ------------------------------------------------------------------------

// -----   Copy Constructor from PndFieldMap   ----------------------------
PndFieldMapData::PndFieldMapData(const PndFieldMapData &L)
  : TNamed(L), fType(L.GetType()), fXmin(L.GetXmin()), fXmax(L.GetXmax()), fYmin(L.GetYmin()), fYmax(L.GetYmax()), fZmin(L.GetZmin()), fZmax(L.GetZmax()), fUnit(L.fUnit),
    fNx(L.GetNx()), fNy(L.GetNy()), fNz(L.GetNz()), fBx(new TArrayF(*(L.GetBx()))), fBy(new TArrayF(*(L.GetBy()))), fBz(new TArrayF(*(L.GetBz())))

{
}
// ------------------------------------------------------------------------

// ------------   Destructor   --------------------------------------------
PndFieldMapData::~PndFieldMapData()
{
  if (fBx)
    delete fBx;
  if (fBy)
    delete fBy;
  if (fBz)
    delete fBz;
}
// ------------------------------------------------------------------------

ClassImp(PndFieldMapData)
