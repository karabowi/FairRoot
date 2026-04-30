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
//
//  PndEmcReader
//
//  Reader of Emc geometry file (ASCII)
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include "TString.h"
#include <algorithm>

#include "PndEmcReader.h"

using namespace std;

PndEmcReader::PndEmcReader(TString name) : g4data(), sName(name)
{
  g4data.clear();
  PndEmcReader::fill_vector();
}

PndEmcReader::~PndEmcReader()
{
  g4data.clear();
}

void PndEmcReader::fill_vector()
{
  fstream f;
  string str;
  DataG4 data;
  int line_nr;

  f.open(sName.Data(), ios::in);
  cout << "PndEmcReader: EMC geometry file == " << sName.Data() << endl;
  if (!f) {
    cout << "PndEmcReader Error: Data file cannot be open!" << endl;
    exit(1);
  } else {
    line_nr = 1;
    while (line_nr <= 3) {
      getline(f, str, '\n');
      line_nr++;
    };
    while (f) {

      f >> data.module >> data.row >> data.crystal >> data.theta >> data.phi >> data.tau >> data.posX >> data.posY >> data.posZ >> data.pDz >> data.pTheta >> data.pPhi >>
        data.pDy1 >> data.pDx1 >> data.pDx2 >> data.pAlp1 >> data.pDy2 >> data.pDx3 >> data.pDx4 >> data.pAlp2;
      PndEmcReader::g4data.push_back(data);
    };
    f.close();
  };
  f.clear();

  return;
}

int PndEmcReader::GetMaxModules()
{
  int modules = 0;

  for (unsigned int i = 0; i < PndEmcReader::g4data.size(); i++)
    if (modules < PndEmcReader::g4data[i].module)
      modules = PndEmcReader::g4data[i].module;

  return modules;
}

int PndEmcReader::GetMaxRows(int module)
{
  int rows = 0;

  for (unsigned int i = 0; i < PndEmcReader::g4data.size(); i++)
    if (module == PndEmcReader::g4data[i].module)
      if (rows < PndEmcReader::g4data[i].row)
        rows = PndEmcReader::g4data[i].row;

  return rows;
}

int PndEmcReader::GetMaxCrystals(int module, int row)
{
  int crystals = 0;

  for (unsigned int i = 0; i < PndEmcReader::g4data.size(); i++)
    if ((module == PndEmcReader::g4data[i].module) && (row == PndEmcReader::g4data[i].row))
      if (crystals < PndEmcReader::g4data[i].crystal)
        crystals = PndEmcReader::g4data[i].crystal;

  return crystals;
}

int PndEmcReader::GetMinModules()
{
  int modules = 1000000;

  for (unsigned int i = 0; i < PndEmcReader::g4data.size(); i++)
    if (modules > PndEmcReader::g4data[i].module)
      modules = PndEmcReader::g4data[i].module;

  return modules;
}

int PndEmcReader::GetMinRows(int module)
{
  int rows = 1000000;

  for (unsigned int i = 0; i < PndEmcReader::g4data.size(); i++)
    if (module == PndEmcReader::g4data[i].module)
      if (rows > PndEmcReader::g4data[i].row)
        rows = PndEmcReader::g4data[i].row;

  return rows;
}

int PndEmcReader::GetMinCrystals(int module, int row)
{
  int crystals = 1000000;

  for (unsigned int i = 0; i < PndEmcReader::g4data.size(); i++)
    if ((module == PndEmcReader::g4data[i].module) && (row == PndEmcReader::g4data[i].row))
      if (crystals > PndEmcReader::g4data[i].crystal)
        crystals = PndEmcReader::g4data[i].crystal;

  return crystals;
}

DataG4 PndEmcReader::GetData(int module, int row, int crystal)
{
  DataG4 data;
  data.module = -1; // Module not present

  for (unsigned int i = 0; i < PndEmcReader::g4data.size(); i++) {
    if ((module == PndEmcReader::g4data[i].module) && (row == PndEmcReader::g4data[i].row) && (crystal == PndEmcReader::g4data[i].crystal)) {
      data.module = PndEmcReader::g4data[i].module;
      data.row = PndEmcReader::g4data[i].row;
      data.crystal = PndEmcReader::g4data[i].crystal;
      data.theta = PndEmcReader::g4data[i].theta;
      data.phi = PndEmcReader::g4data[i].phi;
      data.tau = PndEmcReader::g4data[i].tau;
      data.posX = PndEmcReader::g4data[i].posX;
      data.posY = PndEmcReader::g4data[i].posY;
      data.posZ = PndEmcReader::g4data[i].posZ;
      data.pDz = PndEmcReader::g4data[i].pDz;
      data.pTheta = PndEmcReader::g4data[i].pTheta;
      data.pPhi = PndEmcReader::g4data[i].pPhi;
      data.pDy1 = PndEmcReader::g4data[i].pDy1;
      data.pDx1 = PndEmcReader::g4data[i].pDx1;
      data.pDx2 = PndEmcReader::g4data[i].pDx2;
      data.pAlp1 = PndEmcReader::g4data[i].pAlp1;
      data.pDy2 = PndEmcReader::g4data[i].pDy2;
      data.pDx3 = PndEmcReader::g4data[i].pDx3;
      data.pDx4 = PndEmcReader::g4data[i].pDx4;
      data.pAlp2 = PndEmcReader::g4data[i].pAlp2;
    };
  };

  return data;
}
