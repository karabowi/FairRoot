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

//===============================================================
// PndStt2GeoHandler (singleton class)
//===============================================================
// Class to provide STT geometry info (for geoType 1 and 3):
// usage: add to your task Init function:
// PndStt2GeoHandler *geoH = PndStt2GeoHandler::Instance(par); // par [PndGeoSttPar*] will be used on first access only
// PndSttTube *tube = geoH->GetTube(id);
// map<int, PndSttTube*> fTubeMap = geoH->GetTubeMap();
// TClonesArray *fTubeArray = geohandler->GetTubeArray();
//===============================================================
// contact: K. Goetzen - GSI Darmstadt (k.goetzen@gsi.de)
//===============================================================

#include "PndStt2GeoHandler.h"
#include "PndGeoStt.h"
#include "PndSttTube.h"
#include "PndSttHit.h"
#include "PndSttMapCreator.h"

#include "FairLogger.h"

#include "TVector3.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
#include "TGeoVolume.h"
#include "TGeoTube.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TRandom3.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "TIterator.h"

#include <iostream>

using namespace std;

PndStt2GeoHandler* PndStt2GeoHandler::fInstance = nullptr;

// -----------------------------------------
// constructor
// -----------------------------------------
PndStt2GeoHandler::PndStt2GeoHandler(PndGeoSttPar *par) : fGeoType(-1), fMaxTubeID(-1), fSttGeoPar(par), fTubeArray(nullptr), fStrawMap(nullptr), fGeoMap(nullptr)
{
  // ##### instance first requested with parameters, create geometry 
  if (par!=nullptr) {
    fSttGeoPar = par;
    fGeoType = fSttGeoPar->GetGeometryType();
    
    switch (fGeoType) {
      // ##### old geometry using PndSttMapCreator, PndSttStrawMap, PndSttGeometryMap
      case 1:
        LOG(info) << "PndStt2GeoHandler: Creating singleton instance using OLD geometry (geotype = 1)";
        FillTubeMapOld();
        break;
      
      // ##### new geometry using PndStt2GeoHandler
      case 3:
        LOG(info) << "PndStt2GeoHandler: Creating singleton instance using NEW geometry (geotype = 3)";
        FillTubeMap();
        break;
      
      // ##### ROOT geometry not supported yet
      default: 
        Fatal("PndStt2GeoHandler", Form("Geometry mapping for STT with geoType=%d not supported!", fGeoType));
      break;
    }
  }
  else
    LOG(info) << "PndStt2GeoHandler: Creating singleton instance WITHOUT geometry";

  int neicnt = 0;
  for (auto tb:fTubeMap) {
    neicnt += tb.second->GetNeighborings().GetSize();
  }
  printf("[INFO] PndStt2GeoHandler - n_tubes = %d, n_neighbors = %d\n", (int)fTubeMap.size(), neicnt);
}

// -----------------------------------------
// return singleton instance
// -----------------------------------------
PndStt2GeoHandler* PndStt2GeoHandler::Instance(PndGeoSttPar *par)
{
  if (fInstance==nullptr)
    fInstance = new PndStt2GeoHandler(par);
  
  return fInstance;
}


// -----------------------------------------
//  get tubeID from name or path string
// -----------------------------------------
Int_t PndStt2GeoHandler::GetTubeIdFromStr(TString path)
{
  // two possibilities:
  // copy : /cave_1/stt01assembly_0/stt01tube_YYY/stt01gas_1     --> YYY
  // solo : /cave_1/stt01assembly_0/stt01tubeYYY_0/stt01gasYYY_0 --> YYY
  
  if (fPath2TubeID[path]==0) {
    TString pathcp = path;
    
    pathcp.ReplaceAll("/cave_1/stt01assembly_0/",""); // remove path if present
    pathcp.ReplaceAll("stt01tube","");                // remove tube name prefix
    pathcp.ReplaceAll("/stt01gas_1","");              // remove '/stt01gas_1'
    pathcp.ReplaceAll("_0","");                       // remove _0 in solo tubes 
    pathcp.ReplaceAll("_","");                        // remove _ in copy tubes (tube path)
    pathcp.ReplaceAll("#","");                        // remove # in copy tubes (tube name)
    
    fPath2TubeID[path] = pathcp.Atoi();
  }
  
  return fPath2TubeID[path];
}

// -----------------------------------------
//  get volume path from tube Id
// -----------------------------------------
TString PndStt2GeoHandler::GetPathFromTubeId(Int_t id, bool copy=true) 
{
  // two possibilities:
  // copy : XXX -> /cave_1/stt01assembly_0/stt01tube_XXX
  // solo : XXX -> /cave_1/stt01assembly_0/stt01tubeXXX_0
  
  TString path = Form(copy ? "stt01tube_%d" : "stt01tube%d_0", id);
  path = "/cave_1/stt01assembly_0/" + path;
  return path;
}

// -----------------------------------------
//  Create tube assigned to tubeid
// -----------------------------------------
PndSttTube* PndStt2GeoHandler::CreateTube(Int_t tubeid)
{
  // ##### find path of tube (copy or solo)
  TString pathcopy = GetPathFromTubeId(tubeid);
  TString pathsolo = GetPathFromTubeId(tubeid, false);

  bool iscopy = gGeoManager->CheckPath(pathcopy);
  bool issolo = iscopy ? false : gGeoManager->CheckPath(pathsolo);
  
  if (!iscopy && !issolo) {
    cout << "PndStt2GeoHandler::CreateTube(tubeid): Tube " << tubeid << " not found (nor as a copy)" << endl;
    return nullptr;
  }
  
  TString path = iscopy ? pathcopy : pathsolo;

  // ##### transformation (trans+rot) of the assembly
  gGeoManager->cd("/cave_1/stt01assembly_0");
  TGeoNode *assembly_node = gGeoManager->GetCurrentNode();
  double local[3] = {0., 0., 0.}, master[3];
  assembly_node->LocalToMaster(local, master);
  TVector3 assembly_position(master[0], master[1], master[2]);

  // ##### transformation (trans+rot) of the tube
  gGeoManager->cd(path);
  TGeoNode *tube_node = gGeoManager->GetCurrentNode();
  TGeoTube *tube_vol = (TGeoTube*) tube_node->GetVolume()->GetShape();
  Double_t l2 = tube_vol->GetDz();    // tube half length
  Double_t ri = tube_vol->GetRmin();  // tube inner radius
  Double_t ra = tube_vol->GetRmax();  // tube outer radius
  
  // ##### global wire positions
  double lcnt[3] = {0.,0.,0.}, loc1[3] = {0.,0.,-l2}, loc2[3] = {0.,0.,l2}, mcnt[3], mpu[3], mpd[3];
  tube_node->LocalToMaster(lcnt, mcnt); // local center to global
  tube_node->LocalToMaster(loc1, mpu);  // local upstream end to global 
  tube_node->LocalToMaster(loc2, mpd);  // local downstream end to global
  
  TVector3 pcnt(mcnt[0], mcnt[1], mcnt[2]);
  TVector3 pup(mpu[0], mpu[1], mpu[2]);
  TVector3 pdown(mpd[0], mpd[1], mpd[2]);

  pcnt  += assembly_position;
  pup   += assembly_position;
  pdown += assembly_position;
  
  // ##### rotation matrix
  TGeoMatrix *mat = tube_node->GetMatrix();
  Double_t const *mr = mat->GetRotationMatrix();

  // ##### create tube object
  return new PndSttTube(tubeid, pcnt, pup, pdown, mr, l2, ri, ra);  
}

// -----------------------------------------
//  Fill Tube Map
// -----------------------------------------
void PndStt2GeoHandler::FillTubeMap()
{
  fMaxTubeID = fSttGeoPar->GetMaxTubeId();
  TString geoFileName = fSttGeoPar->GetGeoFileName(); 
  TString geoInfoName = geoFileName;
  geoInfoName.ReplaceAll(".geo","_geoinfo.root");
  
  cout <<"[INFO] PndStt2GeoHandler - PndGeoSttPar::MaxTubeId()   = " << fMaxTubeID << endl;
  cout <<"[INFO] PndStt2GeoHandler - PndGeoSttPar::GeoFileName() = " << geoFileName << endl;
  cout <<"[INFO] PndStt2GeoHandler - GeoInfoName                 = " << geoInfoName << endl;
  
  // ##### scan geometry (via gGeoManager) for tube coordinates and shape infos
  Double_t maxTubeLen = 0;  // determine full tube length
  for (int itube = 1; itube<=fMaxTubeID; ++itube) {
    PndSttTube *tube = CreateTube(itube);
    if (maxTubeLen < tube->GetHalfLength()*2) 
      maxTubeLen = tube->GetHalfLength()*2;
    fTubeMap[itube] = tube;
    if (itube==1) 
      cout <<"[INFO] PndStt2GeoHandler - RadIn, RadOut               = " << tube->GetRadIn()<<", "<< tube->GetRadOut() << endl;
  }
    
  // ##### set info whether tube is a short tube
  for (auto tube:fTubeMap)
    if ((maxTubeLen - tube.second->GetHalfLength()*2)>0.1)
      tube.second->SetShortTube();
  
  // ##### read neighbours, sector/layer/index, connected tube and tube type from geoinfo file
  TString workdir = gSystem->Getenv("VMCWORKDIR");
  geoInfoName = workdir+"/macro/params/"+geoInfoName;
  TFile f(geoInfoName);
  if (f.IsZombie()) 
    Fatal("PndStt2GeoHandler", Form("Geo info file '%s' not found.", geoInfoName.Data()));
  
  TTree *tgeo = (TTree*) f.Get("geo");
  if (tgeo==nullptr) 
    Fatal("PndStt2GeoHandler", Form("No tree 'geo' found in geo file '%s' not found.", geoInfoName.Data())); 
    
  // ##### read infos from TTree
  Int_t id, sec, lay, idx, dir, conn, nnei;
  Double_t skew;
  Int_t nei[100];
  tgeo->SetBranchAddress("id",   &id);   // tube ID
  tgeo->SetBranchAddress("seg",  &sec);  // segment / sector 
  tgeo->SetBranchAddress("lay",  &lay);  // layer 
  tgeo->SetBranchAddress("idx",  &idx);  // index in layer
  tgeo->SetBranchAddress("dir",  &dir);  // tube direction:  -1 (left), 0 (axial), 1 (right)
  tgeo->SetBranchAddress("skew", &skew); // skew angle[deg]: <0 (left), 0 (axial), >0 (right)
  tgeo->SetBranchAddress("conn", &conn); // connected short tube; 0 = no connection
  tgeo->SetBranchAddress("nnei", &nnei); // number of neighbours
  tgeo->SetBranchAddress("nei",  nei);   // list of neighbouring tube IDs

  // ##### loop TTree
  int lastsec = -1; // save last sector to detect sector change in TTree for layer limit flag  
  Double_t skewAng = -1;     // skew angle
  for (Long64_t i = 0; i < tgeo->GetEntries(); i++) {
    tgeo->GetEntry(i);
    if (!fTubeMap.count(id)) {
      cout <<"[ERROR] - PndStt2GeoHandler - Invalid tubeID "<<id<<" in geoinfo file "<<geoInfoName<<endl;
      continue;
    }
    
    // ##### add geo infos to tube
    PndSttTube *tube = fTubeMap[id];
    tube->SetSectorID(sec);
    tube->SetLayerID(lay);
    tube->SetIndex(idx);
    tube->SetTubeType(dir);
    tube->SetSkewAngle(skew*TMath::DegToRad()); // skew angle deg -> rad
    tube->SetConnected(conn);
    tube->SetNeighborings(TArrayI(nnei, nei));
    
    // ##### store stereo angle
    if (dir!=0 && skewAng<0) 
      skewAng = fabs(skew);
    
    // ##### tube at layer edge? (relies on tubes sorted by segment and layer in TTree)
    if (sec!=lastsec || id==fMaxTubeID) {
      tube->SetLayerLimitFlag(); // current tube
      if (id>1) 
        fTubeMap[id-1]->SetLayerLimitFlag(); // previous tube
    }
    
    // ##### add tube index lookup tables (LUT)
    fTubesInLayer[{sec,lay}].push_back(id);    // layer indices LUT
    fTubeLookup[GetPosID(sec, lay, idx)] = id; // tube ID LUT
    
    lastsec = sec;
  }
  
  // ##### set sector limit flags (-1,0,1)
  Double_t tubeDiam = fTubeMap[1]->GetRadOut()*2; // tube diameter
  int ntt = sin(skewAng)*maxTubeLen/tubeDiam;     // compute number of tube length types (skewed tubes)
  printf("[INFO] PndStt2GeoHandler::FillTubeMap -- d_tube = %.3f, l_tube = %.3f, ang = %.4f =>  n_len = %d\n", tubeDiam, maxTubeLen, skewAng, ntt); 
  for (auto vrow:fTubesInLayer) {
    //int sec = vrow.first.first;
    //int row = vrow.first.second;
    TvInt vtubes = vrow.second;
    bool axial = fTubeMap[vtubes[0]]->IsParallel();
    // ##### axial tube: first and last tube in layer are sector edges
    if (axial) {
      fTubeMap[vtubes.front()]->SetSectorLimitFlag(-1);
      fTubeMap[vtubes.back()]->SetSectorLimitFlag(1);
    } 
    // ##### skewed tube: first 7 and last 7 tubes in layer are sector edges
    else {
      int ntubes = vtubes.size();
      for (int i=0; i<min(ntt,ntubes); ++i) {
        fTubeMap[vtubes[i]]->SetSectorLimitFlag(-1);
        fTubeMap[vtubes[ntubes-i-1]]->SetSectorLimitFlag(1);
      }
    }
  }
  
  f.Close();
}

// -----------------------------------------
//  Fill Tube Map (old) and setup mappers
// -----------------------------------------
void PndStt2GeoHandler::FillTubeMapOld()
{
  // ##### fill fTubeArray (TClonesArray)
  PndSttMapCreator mapper(fSttGeoPar);
  fTubeArray = mapper.FillTubeArray();
  fStrawMap  = new PndSttStrawMap(fTubeArray);
  fGeoMap    = new PndSttGeometryMap(fTubeArray, 1);
  //fGeoMap->FillStrawNeighborsMap();
  fMaxTubeID = fTubeArray->GetEntries();
  
  // ##### fill fTubeMap
  TIterator* tubeIter = fTubeArray->MakeIterator();
  PndSttTube* tube=nullptr;
  while ((tube = (PndSttTube*)tubeIter->Next())) 
    fTubeMap[tube->GetTubeID()] = tube;
}

//-------------------------------------------
//  return tube ID at position (sec,lay,idx)
//-------------------------------------------
int PndStt2GeoHandler::GetTubeAt(int sec, int lay, int idx)
{
  int posid = GetPosID(sec, lay, idx);
  if (fTubeLookup.find(posid) != fTubeLookup.end())
    return fTubeLookup[posid];
  
  return -1;
}

// -----------------------------------------
//  Fill and return TClonesArray of Tubes
// -----------------------------------------
TClonesArray* PndStt2GeoHandler::GetTubeArray()
{
  // ##### if not filled, fill tube array
  if (fTubeArray==nullptr) {
    fTubeArray = new TClonesArray("PndSttTube");
    
    for (auto tube:fTubeMap) 
      new ((*fTubeArray)[tube.first]) PndSttTube(*(tube.second));
  }
  
  return fTubeArray;
}


// -----------------------------------------
//  Correctness checks printout
// -----------------------------------------
void PndStt2GeoHandler::CheckTubeMap(int nlay, int ndist)
{
  // ##### print all layer limit tubes
  cout <<"Layer limit tube IDs: ";
  for (auto tube:fTubeMap) {
    if (tube.second->IsLayerLimit()) cout <<tube.second->GetTubeID()<<" ";
  }
  cout <<endl;

  // ##### print sector limits (S0-1/L8-10)
  cout <<"Sector limit flags: "<<endl;
  for (int is=0; is<=1; ++is) {
    for (int il=8; il<=10; ++il) {
      TvInt vrow = fTubesInLayer[{is,il}];
      printf("S%2d/L%2d (n=%2d): ", is, il, (int)vrow.size());
      for (auto tubeid:vrow) printf("%+2d ", fTubeMap[tubeid]->GetSectorLimit());
      cout <<endl;
    }
  }
 
  // ##### print some layers
  for (int i=0; i<nlay; ++i) {
    int sec = int(gRandom->Uniform(0,6)+1);
    int lay = int(gRandom->Uniform(0,27)+1);
    printf("Tubes in (Sector,Lay) = (%d, %2d): ", sec, lay); 
    if (IsRow(sec, lay))
      for (auto tubeid:fTubesInLayer[{sec, lay}]) printf("%d ", tubeid);
    cout <<endl;
  }
  
  // ##### print some distances
  for (int i=0; i<ndist; ++i) {
    int id1 = 0, id2 = 0;
    while (!IsTube(id1) || !IsTube(id2)) {
      id1=0;
      id2=0;
      int sec = gRandom->Uniform(0,6)+1;
      int row = gRandom->Uniform(0,GetMaxRow())+1;
      if (!IsRow(sec, row)) continue;
      TvInt vrow = GetStrawRow(sec,row);
      int idx = gRandom->Uniform(vrow.front(), vrow.back())+1;
      
      id1 = GetTubeAt(sec, row, idx);
      id2 = GetTubeAt(sec,gRandom->Uniform(row-2, row+3), gRandom->Uniform(idx-5, idx+5));
    }
    printf("d(%d, %d) = %.3f\n", id1, id2, GetDistance(id1, id2));
  }  
}

// -----------------------------------------
//  Angle between tubes (in xy plane?) (see PndSttGeometryMap::GetAngleBetweenTubes)
// -----------------------------------------
double PndStt2GeoHandler::GetAngleBetweenTubes(int tube1, int tube2)
{
  // ##### check whether both tubes exist 
  if (!IsTube(tube1) || !IsTube(tube2)) return 0.;
  
  // ##### compute phi of connection vector of tube centers 
  TVector3 connection = fTubeMap[tube2]->GetPosition() - fTubeMap[tube1]->GetPosition();
  double phi = connection.Phi();
  
  while (phi < 0)
    phi += TMath::TwoPi();

  return phi;
}

// -----------------------------------------
//  Check whether 3 tubes are in straight line (in xy-plane; see PndSttGeometryMap::InStraightLine)
// -----------------------------------------
bool PndStt2GeoHandler::InStraightLine(int tube1, int tube2, int tube3)
{
  double diff = abs(GetAngleBetweenTubes(tube1, tube2) - GetAngleBetweenTubes(tube1, tube3));

  // accept a deviation of 1 percent
  // if ((TMath::Pi() * 0.99) < diff & diff < (TMath::Pi() * 1.01)) {
  if (((TMath::Pi() * 0.99) < diff) && (diff < (TMath::Pi() * 1.01))) { //[R.K. 01/2017] more explicit parenthesis logic
    // tube1 is in the middle
    return true;
  } else if (diff < 0.01) {
    // tube1 is at the edge of the line
    return true;
  } else
    return false;
}

// -----------------------------------------
//  Check if edge straw (at the center gap)
// -----------------------------------------
bool PndStt2GeoHandler::IsEdgeStraw(int tubeId)
{
  // sector layout: (*) mark edges at gap
  
  //   0* | *5 
  //  1   |   4
  //   2* | *3
    
  if (!IsTube(tubeId)) return false;
  PndSttTube *tube = fTubeMap[tubeId];
  int sec = tube->GetSectorID();
  int row = tube->GetLayerID();
  int idx = tube->GetIndex();
  int uid = -1;
  
  switch (sec) {
    // ##### sector 0, 3: check whether tube exists proceeding (idx-1) the current one in this sector/row
    case 0: case 3:
      uid = GetPosID(sec, row, idx-1);
      break;
      
    // ##### sector 2, 5:  check whether tube exists succeeding (idx+1) the current one in this sector/row
    case 2: case 5:
      uid = GetPosID(sec, row, idx+1);
      break;
      
    // ##### sector 1, 4: no edge tubes
    default: return false;
  }

  // ##### tube before/after current one does not exist
  return (fTubeLookup.find(uid) == fTubeLookup.end());
}

// -----------------------------------------
//  Compute distance and POCA of 2 straws (by tube id)
// -----------------------------------------
double PndStt2GeoHandler::CalculateStrawPoca(int tubeid0, int tubeid1, TVector3 &poca)
{  
  if (!IsTube(tubeid0) || !IsTube(tubeid1)) return -1.0;
  
  PndSttTube *tube0 = fTubeMap[tubeid0], *tube1 = fTubeMap[tubeid1];
  
  // ##### wire end points upstream (p0, p1) and downstream (p0e, p1e) and direction vectors (d0, d1)
  TVector3 p0c = tube0->GetPosition(), p0 = tube0->GetWireUpStr(), p0e = tube0->GetWireDownStr(), d0 = p0e - p0; // wire 0
  TVector3 p1c = tube1->GetPosition(), p1 = tube1->GetWireUpStr(), p1e = tube1->GetWireDownStr(), d1 = p1e - p1; // wire 1
  
  double dist = -1.0;
  poca = 0.5*(p0c+p1c);
  
  // ----------------------
  // ##### tubes identical
  // ----------------------
  if (tubeid0 == tubeid1) {
    dist = 0.0;
    poca = p0c;
  }
  
  // -----------------------------------
  // ##### parallel wires: |d0 x d1| = 0
  // -----------------------------------
  else if (d0.Cross(d1).Mag()<0.1) {
    // ##### determine line parameter (p1e -> p0 + s0*d0 or p0e -> p1 + s1*d1) of projection of downstream ends to other wire
    double s0 = d1*(p0e-p1)/d1.Mag2();
    double s1 = d0*(p1e-p0)/d0.Mag2();
    
    // ##### if either 0 < s_0,1 < 1 (wires 'overlap'), use distance formular for parallel lines: |(p0-p1) x d1| / |d1|
    if ((s0>=0 && s0<=1) || (s1>=0 && s1<=1)) {
      dist = ((p0-p1).Cross(d1)).Mag()/d1.Mag();
      poca = (s0>=0 && s0<=1) ? 0.5*(p0 + s0*d0 + p1e) : 0.5*(p1 + s1*d1 + p0e);
    }
    // ##### else take minimum of distances |p0 - p1e| and |p1 - p0e| as distance
    else {
      dist = min((p0-p1e).Mag(), (p1-p0e).Mag());
      poca = ((p0-p1e).Mag()<(p1-p0e).Mag()) ? 0.5*(p0+p1e) : 0.5*(p0e+p1);
    }
  }
  
  // -------------------------
  // ##### non-parallel wires 
  // -------------------------
  else
  { 
    // ##### determine parameters s,t for perpendicular intersecting line between wire 0 and wire 1
    // ##### formulas based on equations (p0 + s*d0 - p1 -t*d1)*d0 = 0 and (p0 + s*d0 - p1 -t*d1)*d1 = 0 (conn. perpendicular to both lines)
    double s = (p0-p1)*((d0*d1)*d1-d1.Mag2()*d0)/(d0.Mag2()*d1.Mag2()-pow(d0*d1,2));
    double t = (s*d0*d1+(p0-p1)*d1)/d1.Mag2();
    
    // ##### dx intersects both wires (i.e.: 0 < s < 1 AND 0 < t < 1)
    if (s>=0 && s<=1 && t>=0 && t<=1) {   
      TVector3 xp0 = p0 + s * d0; // intersection point along wire 0
      TVector3 xp1 = p1 + t * d1; // intersection point along wire 1
      TVector3 dx  = xp1 - xp0;   // connection line between these points
      
      dist = dx.Mag();            // distance = length of connection
      poca = 0.5*(xp0 + xp1);     // poca = center of connection line
    }
    // ##### dx does not intersect both wires 
    // ##### => compute all projection of the end points to the respective other wire
    // #####    and the distances of the end points themselves and take the minimum
    else {
      // ##### distances of the end points
      vector<double> vdist = { (p0-p1).Mag(), (p0-p1e).Mag(), (p1-p0e).Mag(), (p0e-p1e).Mag() };
      vector<TVector3> vpoca = { 0.5*(p0+p1), 0.5*(p0+p1e), 0.5*(p0e+p1), 0.5*(p0e+p1e) };

      double s0 = d0*(p1e-p0)/d0.Mag2();  // projection of p1e to wire 0
      double s1 = d0*(p1-p0)/d0.Mag2();   // projection of p1  to wire 0
      double t0 = d1*(p0e-p1)/d1.Mag2();  // projection of p0e to wire 1
      double t1 = d1*(p0-p1)/d1.Mag2();   // projection of p0  to wire 1
      
      // ##### only consider projections inside wire range
      if (s0>=0 && s0<=1) {
        vdist.push_back(((p1e-p0).Cross(d0)).Mag()/d0.Mag());
        vpoca.push_back(0.5*(p1e + p0+s0*d0));
      }
      if (s1>=0 && s1<=1) { 
        vdist.push_back(((p1-p0).Cross(d0)).Mag()/d0.Mag());
        vpoca.push_back(0.5*(p1  + p0+s1*d0));
      }
      if (t0>=0 && t0<=1) { 
        vdist.push_back(((p0e-p1).Cross(d1)).Mag()/d1.Mag());
        vpoca.push_back(0.5*(p0e + p1+t0*d1));
      }
      if (t1>=0 && t1<=1) { 
        vdist.push_back(((p0-p1).Cross(d1)).Mag()/d1.Mag());
        vpoca.push_back(0.5*(p0  + p1+t1*d1));
      }
      
      int minidx = TMath::LocMin(vdist.size(), &vdist[0]);
      dist = vdist[minidx];
      poca = vpoca[minidx];
      //dist = TMath::MinElement(vdist.size(), &vdist[0]);
    }
  }
  
  return dist;
}


// -----------------------------------------
//  Compute distance and POCA of 2 straws; Legacy PndSttGeometryMap code
// -----------------------------------------
Double_t PndStt2GeoHandler::CalculateStrawPocaLegacy(PndSttHit *hit1, PndSttHit *hit2, TVector3 &poca)
{
  TVector3 p1(hit1->GetX(), hit1->GetY(), hit1->GetZ());
  TVector3 p2(hit2->GetX(), hit2->GetY(), hit2->GetZ());

  PndSttTube *tube1 = fTubeMap[hit1->GetTubeID()];
  PndSttTube *tube2 = fTubeMap[hit2->GetTubeID()];

  TVector3 u1(tube1->GetWireDirection());
  TVector3 u2(tube2->GetWireDirection());

  // ##### same tube (p1 = p2)
  TVector3 p21 = p2 - p1;
  if (p21.Mag() < 0.000001) {
    poca = p1;
    return 0;
  }
  
  // ##### parallel tubes (u1 x u2 = 0)
  TVector3 m = u2.Cross(u1);
  if (m.Mag() < 0.000001) {
    poca = p1 + 0.5 * p21;
    return p21.Mag();
  }

  // ##### find crossing of straight lines
  TVector3 mnorm = (1.0 / m.Mag2()) * m;
  TVector3 r = p21.Cross(mnorm);
  Double_t t1 = r.Dot(u2);
  Double_t t2 = r.Dot(u1);

  // ##### if beyond limits (+- half len), set to limits
  if (t1 > tube1->GetHalfLength())
    t1 = tube1->GetHalfLength();
  else if (t1 < -tube1->GetHalfLength()) 
    t1 = -tube1->GetHalfLength();

  if (t2 > tube2->GetHalfLength())
    t2 = tube2->GetHalfLength();
  else if (t2 < -tube2->GetHalfLength())
    t2 = -tube1->GetHalfLength();

  // ##### q1, q2 = projections to lines
  TVector3 q1 = p1 + t1 * u1;
  TVector3 q2 = p2 + t2 * u2;
  TVector3 q21 = q2 - q1;
  
  // ##### poca = center of connection line (q1,q2)
  poca = q1 + 0.5 * q21;

  return (TMath::Abs(p21.Dot(m)) / m.Mag());
}


// -----------------------------------------
//  Compute distance and POCA of 2 straws (by hits)
// -----------------------------------------
double PndStt2GeoHandler::CalculateStrawPoca(PndSttHit *hit0, PndSttHit *hit1, TVector3 &poca)
{
  // ##### call legacy routine for old geometry
  if (fGeoMap!=nullptr)
    return fGeoMap->CalculateStrawPoca(hit0, hit1, poca);
  
  //// ##### NEW geometry
  //int tubeid0 = hit0->GetTubeID();
  //int tubeid1 = hit1->GetTubeID();
  //return CalculateStrawPoca(tubeid0, tubeid1, poca);

  return CalculateStrawPocaLegacy(hit0, hit1, poca);
}


//-------------------------------------------
//  Compute DOCA of two tube wires
//-------------------------------------------
Double_t PndStt2GeoHandler::GetDistance(int tubeid0, int tubeid1)
{
  TVector3 dummy;
  //return CalculateStrawPocaLegacy(tubeid0, tubeid1, dummy);
  return CalculateStrawPoca(tubeid0, tubeid1, dummy);
}

//-------------------------------------------
//  Find sector number for angle phi (in xy-plane)
//-------------------------------------------
int PndStt2GeoHandler::FindPhiSector(double phi)
{
  //    phi range     sector    phi range
  // (   90 .. 150)   0 | 5   (  30 ..  90)
  // (+-150 .. 180)  1  |  4  ( -30 ..  30)
  // ( -150 .. -90)   2 | 3   ( -90 .. -30)

  // #### transform in the way, that 
  // [-180;-150] + 270 -> [ 90; 120]/60 % 6 = 1
  // [-150;- 90] + 270 -> [120; 180]/60 % 6 = 2
  // [- 90;- 30] + 270 -> [180; 240]/60 % 6 = 3
  // [- 30;  30] + 270 -> [240; 300]/60 % 6 = 4
  // [  30;  90] + 270 -> [300; 360]/60 % 6 = 5
  // [  90; 150] + 270 -> [360; 420]/60 % 6 = 0
  // [ 150; 180] + 270 -> [420; 480]/60 % 6 = 1
  
  int sec = (phi*TMath::RadToDeg()+270)/60;
  return sec % 6;
}


//-------------------------------------------
//  return vector with tube ids in row
//-------------------------------------------
std::vector<int> PndStt2GeoHandler::GetStrawRow(int sec, int row)
{ 
  if (fStrawMap!=nullptr) 
    return fStrawMap->GetStrawRow(sec,row);
    
  std::vector<int> empty;
  if (IsRow(sec,row)) 
    return fTubesInLayer[{sec,row}]; 
  else 
    return empty; 
}

//-------------------------------------------
//  Check whether row is skewed
//-------------------------------------------
bool PndStt2GeoHandler::IsSkewedRow(int row)
{ 
  if (fGeoMap!=nullptr) 
    return fGeoMap->IsSkewedRow(row);
    
  return IsRow(1,row) ? fTubeMap[fTubesInLayer[{1,row}][0]]->IsSkew() : false;
}

//-------------------------------------------
//  Check whether tubes are parallel
//-------------------------------------------
bool PndStt2GeoHandler::ParallelTubes(int tubeid0, int tubeid1)
{
  if (!IsTube(tubeid0) || !IsTube(tubeid1)) return false;
  
  if (fGeoMap!=nullptr) {
    TVector3 m = fTubeMap[tubeid0]->GetWireDirection().Cross(fTubeMap[tubeid1]->GetWireDirection());
    return (m.Mag() < 0.000001);
  }
    
  return fTubeMap[tubeid0]->GetTubeType()==fTubeMap[tubeid1]->GetTubeType();
}

// -----------------------------------------
// -----------------------------------------

ClassImp(PndStt2GeoHandler);
