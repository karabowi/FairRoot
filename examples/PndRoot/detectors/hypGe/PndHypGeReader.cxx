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
//  PndHypGeReader
//
//  Reader of HypGe geometry file (ASCII)
//
//  roginal Author: S.Spataro
//  modified 15/05/07  by A.Sanchez
//
///////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include "TString.h"
#include <algorithm>

#include "PndHypGeReader.h"

using namespace std;

PndHypGeReader::PndHypGeReader(TString name)
{
  sName = name;
  PndHypGeReader::fill_vector();
}

PndHypGeReader::~PndHypGeReader()
{
  g4data.clear();
}

void PndHypGeReader::fill_vector()
{
  fstream f;
  string str;
  DataG4 data;
  int line_nr;

  f.open(sName.Data(), ios::in);
  // f.open(sName, ios::in);
  cout << sName << endl;
  if (!f) {
    cout << "PndHypGeReader Error: Data file cannot be open!" << endl;
    exit(1);
  } else {
    // line_nr = 1;
    // while(line_nr<=3) {getline(f,str,'\n'); line_nr++;};
    while (f) {
      f >> data.crystal >> data.posX >> data.posY >> data.posZ;
      PndHypGeReader::g4data.push_back(data);
    };
    f.close();
  };
  f.clear();

  return;
}

DataG4 PndHypGeReader::GetData(int crystal)
{
  DataG4 data;

  for (unsigned int i = 0; i < PndHypGeReader::g4data.size(); i++) {
    if (crystal == PndHypGeReader::g4data[i].crystal) {

      data.crystal = PndHypGeReader::g4data[i].crystal;

      data.posX = PndHypGeReader::g4data[i].posX;
      data.posY = PndHypGeReader::g4data[i].posY;
      data.posZ = PndHypGeReader::g4data[i].posZ;
      // pos.SetXYZ(data.posX, data.posY, data.posZ);
    };
  };

  return data;
  // return pos;
}
