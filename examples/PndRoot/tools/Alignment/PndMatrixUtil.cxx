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

#include "PndMatrixUtil.h"

ClassImp(PndMatrixUtil);

PndMatrixUtil::PndMatrixUtil(){
  std::cout << "Only intended as static library." << std::endl;
}

bool PndMatrixUtil::hasEnding(std::string const &fullString, std::string const &ending)
{
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
  } else {
    return false;
  }
}

//! don't pass a reference to matrices here, root requires the address of a pointer
bool PndMatrixUtil::saveMatricesToRootFile(std::map<std::string, TGeoHMatrix> *matrices, std::string &fileName)
{
  TFile *misalignmentMatrixRootfile = new TFile(fileName.c_str(), "NEW");
  if (misalignmentMatrixRootfile->IsOpen()) {
    printf("File opened successfully\n");
    gDirectory->WriteObject(matrices, "PndLmdMisalignMatrices");
    misalignmentMatrixRootfile->Write();
    misalignmentMatrixRootfile->Close();
    cout << "All matrices written to file " << fileName << "!\n";
    return true;
  }
  cerr << "WARNING! Could not write to " << fileName << "!\n";
  return false;
}

bool PndMatrixUtil::saveMatricesToJson(std::map<std::string, TGeoHMatrix> *matrices, std::string &outfilename)
{
  // matrices is a map of string->TGeoHMatrix
  json j;
  double thisMatrixDoubles[16];
  TGeoHMatrix thisMatrix;

  for (auto &path : *matrices) {
    thisMatrix = path.second;
    thisMatrix.GetHomogenousMatrix(thisMatrixDoubles);

    // ROOT stores matrices in a way thats both unusual and not correctly documented (https://root.cern.ch/doc/master/classTGeoMatrix.html#a863edc5b96ee163a77ac9fedf7f78415). 
    j[path.first] = {thisMatrixDoubles[0], thisMatrixDoubles[1],  thisMatrixDoubles[2],  thisMatrixDoubles[12], thisMatrixDoubles[4],  thisMatrixDoubles[5],
                     thisMatrixDoubles[6], thisMatrixDoubles[13], thisMatrixDoubles[8],  thisMatrixDoubles[9],  thisMatrixDoubles[10], thisMatrixDoubles[14],
                     thisMatrixDoubles[3], thisMatrixDoubles[7],  thisMatrixDoubles[11], thisMatrixDoubles[15]};
  }

  // save!
  cout << "save " << j.size() << " matrices to json!\n";
  std::ofstream o(outfilename.c_str());
  o << std::setw(2) << j << std::endl;
  return true;
}

// read matrices from file and return a pointer to new
std::map<string, TGeoHMatrix> *PndMatrixUtil::readMatricesFromRootFile(string &fileName)
{
  TFile *misalignmentMatrixRootfile = new TFile(fileName.c_str(), "READ");
  if (misalignmentMatrixRootfile->IsOpen()) {
    std::map<string, TGeoHMatrix> *matrices;
    gDirectory->GetObject("PndLmdMisalignMatrices", matrices);
    misalignmentMatrixRootfile->Close();
    cout << matrices->size() << " matrices successfully read from file.\n";
    return matrices;
  }
  cerr << "WARNING! Could not read from " << fileName << "!\n";
  return nullptr;
}

std::map<std::string, TGeoHMatrix> *PndMatrixUtil::readMatricesFromJSON(string &filename)
{
  std::map<std::string, TGeoHMatrix> *rootMatrices = new std::map<std::string, TGeoHMatrix>;
  TGeoHMatrix thisMatrix;

  double rot[9];
  double trans[3];
  string path;

  // read JSON
  std::ifstream iStream(filename);
  json j;
  iStream >> j;
  iStream.close();

  for (auto i : j.items()) {
    path = i.key();
    rot[0] = i.value()[0];
    rot[1] = i.value()[1];
    rot[2] = i.value()[2];
    rot[3] = i.value()[4];
    rot[4] = i.value()[5];
    rot[5] = i.value()[6];
    rot[6] = i.value()[8];
    rot[7] = i.value()[9];
    rot[8] = i.value()[10];

    trans[0] = i.value()[3];
    trans[1] = i.value()[7];
    trans[2] = i.value()[11];

    thisMatrix.SetRotation(rot);
    thisMatrix.SetTranslation(trans);

    (*rootMatrices)[path] = thisMatrix;
  }
  return rootMatrices;
}

// reads all matrices for "path in paths" from gGeoManager and writes them to "fileName"
bool PndMatrixUtil::saveGeoManagerMatricesToDisk(std::vector<string> &paths, string &fileName)
{
  std::map<string, TGeoHMatrix> totalmatrices;

  // go over geoManager, get all matrices
  for (auto &path : paths) {
    gGeoManager->cd(path.c_str());
    TGeoHMatrix currentmatrix = *(gGeoManager->GetCurrentMatrix());
    totalmatrices[path] = currentmatrix;
  }
  return saveMatricesToRootFile(&totalmatrices, fileName);
}

bool PndMatrixUtil::saveMatrices(std::map<std::string, TGeoHMatrix> *matrices, std::string &outfilename)
{
  // TODO: actually, I want to save in both formats
  cout << "attempting to save " << outfilename << " ...\n";
  if (hasEnding(outfilename, ".root")) {
    return saveMatricesToRootFile(matrices, outfilename);
  } else if (hasEnding(outfilename, ".json")) {
    return saveMatricesToJson(matrices, outfilename);
  }
  cout << "Error! Unsupported file extension!"
       << "\n";
  return false;
}

std::map<string, TGeoHMatrix> *PndMatrixUtil::readMatrices(std::string &fileName)
{
  // check for file existence
  // https://stackoverflow.com/a/19841704/5264039
  std::ifstream infile(fileName);
  if (!infile.good()) {
    cout << "\n\n======================\n";
    cout << "matrix file not found!\n";
    cout << "this macro will likely fail!\n";
    cout << "======================\n\n";
    infile.close();
  } else {

    if (hasEnding(fileName, ".root")) {
      return readMatricesFromRootFile(fileName);
    } else if (hasEnding(fileName, ".json")) {
      return readMatricesFromJSON(fileName);
    }

    cout << "Error! Unsupported file extension!"
         << "\n";
  }
  return nullptr;
}

std::vector<string> PndMatrixUtil::getPathsFromMatrixFile(string &fileName)
{
  std::map<string, TGeoHMatrix> *matrices = readMatrices(fileName);
  std::vector<string> result;
  for (auto &i : *matrices) {
    result.push_back(i.first);
  }
  return result;
}
