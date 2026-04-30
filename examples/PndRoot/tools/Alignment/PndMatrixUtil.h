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

/*
    matrix helper macro

    NOTE: we're always using column-major notation for vectors and matrices.
    Even though ROOT uses column major notation for TGeoMatrices and TGeoHMatrices
    (sometimes), the actual implementation is wrong. Inside the homogenous matrix,
    the rotation matrix is stored colum-major, but the translation matrix is stored
    row-major. Therefore, the matrix elements must be picked individually.

 */

#ifndef PNDMATRIXUTIL_H
#define PNDMATRIXUTIL_H

#include <TFile.h>
#include <TGeoMatrix.h>
#include <TROOT.h>
#include <TGeoManager.h>

#include <nlohmann/json.hpp>

#include <iomanip> // for setw
#include <iostream>
#include <fstream>
#include <ostream>
#include <map>
#include <string>

using std::cerr;
using std::cout;
using std::map;
using std::string;

using json = nlohmann::json;

class PndMatrixUtil {

 private:
  static bool saveMatricesToJson(std::map<std::string, TGeoHMatrix> *matrices, std::string &outfilename);
  static bool saveMatricesToRootFile(std::map<std::string, TGeoHMatrix> *matrices, std::string &fileName);

  static std::map<string, TGeoHMatrix> *readMatricesFromRootFile(string &fileName);
  static std::map<std::string, TGeoHMatrix> *readMatricesFromJSON(string &filename);

  static bool saveGeoManagerMatricesToDisk(std::vector<string> &paths, string &fileName);
  static bool hasEnding(std::string const &fullString, std::string const &ending);
  static std::vector<string> getPathsFromMatrixFile(string &fileName);

 public:
  PndMatrixUtil();
  static bool saveMatrices(std::map<std::string, TGeoHMatrix> *matrices, std::string &outfilename);
  static std::map<string, TGeoHMatrix> *readMatrices(std::string &fileName);

  ClassDef(PndMatrixUtil, 0);
};

#endif