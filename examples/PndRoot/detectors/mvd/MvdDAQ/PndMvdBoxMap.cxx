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

//  Class PndMvdBoxMap

//  Simone Bianco

#include "PndMvdBoxMap.h"
#include <fstream>
#include <iostream>

using namespace std;

// ----------------------------------------------------------
void PndMvdBoxMap::Print()
{
  Info("Print", "Filename = %s", fFileName.Data());
  return;
}

// ----------------------------------------------------------
Bool_t PndMvdBoxMap::Init()
{
  ifstream mapfile(fFileName.Data(), ifstream::in);

  Int_t fbox = -1, fchanMin = -1, fchanMax = -1;
  nLines = 0;
  TString detPathName;

  if (!mapfile.is_open()) {
    // abort on wrong file
    Error("Init", "Could not open file %s", fFileName.Data());
    return kFALSE;
  }

  /*box = new Int_t();
  fCh = new Int_t();
  lCh = new Int_t();*/
  while (mapfile.good()) {
    mapfile >> fbox >> fchanMin >> fchanMax >> detPathName;
    if (mapfile.eof())
      break;
    cout << "Position: " << mapfile.tellg() << endl;
    Info("Init", "Read line: %i %i %i %s", fbox, fchanMin, fchanMax, detPathName.Data());
    box[nLines] = fbox;
    fCh[nLines] = fchanMin;
    lCh[nLines] = fchanMax;

    fMap[nLines] = detPathName;

    //  TString *name = new TString(detPathName);
    //    name.push_back(detPathName.Data());
    //*name[nLines] = detPathName.Data();

    nLines++;
  }

  cout << nLines << " entries" << endl;

  /*  mapfile.clear();
    mapfile.seekg(ios::beg);


    Int_t buffNum;
    TString buffName;

    for (Int_t zz = 0 ; zz < nLines ; zz++)
    {

      mapfile >> buffNum;
      box[zz] = buffNum;
      mapfile >> buffNum;
      fCh[zz] = buffNum;
      mapfile >> buffNum;
      lCh[zz] = buffNum;
      mapfile >> buffName;
      name[zz] = buffName;

      if(mapfile.eof()) break;
    }
  */

  // std::pair<Int_t,TString> apair(sw,detPathName);
  // fApvNumberMap[rw] = apair;

  mapfile.close();
  return kTRUE;
}

void PndMvdBoxMap::PrintMap()
{

  for (Int_t jj = 0; jj < nLines; jj++) {
    cout << box[jj] << " " << fCh[jj] << " " << lCh[jj] << " " << fMap[jj] << endl;
  }
}

// ----------------------------------------------------------
void PndMvdBoxMap::DoMapping(Int_t nbox, Int_t chan, TString &detpath)
{

  // Int_t sat = -1; //[R.K.02/2017] Unused variable?

  for (Int_t jj = 0; jj < nLines; jj++) {
    if (nbox == box[jj]) {
      if (chan >= fCh[jj] && chan <= lCh[jj]) {
        detpath = fMap[jj];
        // sat = jj; //[R.K.02/2017] Unused variable?
      }
    }
  }

  // for debug...
  //  cout << "Box: " << nbox << ", ch: " << chan << " --> line: " << sat << ", name: " << detpath.Data() << endl;

  return;
}

ClassImp(PndMvdBoxMap);
