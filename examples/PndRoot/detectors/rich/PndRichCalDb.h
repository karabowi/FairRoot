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
// -----                      PndFieldMap header file                  -----
// -----          Created 12/01/04  by M. Al/Turany                    -----
// -------------------------------------------------------------------------

/** PndFieldMap.h
 ** @author M.Al/Turany <m.al-turany@gsi.de>
 ** Magnetic field map on a 3-D grid.
 ** Field values are hold and returned in kG.
 **/

#ifndef PNDRICHCALDB_H
#define PNDRICHCALDB_H 1

#include "FairField.h"

class TArrayF;
class PndRichCalDbData;
class PndRichCalDbPar;

struct dbpoint {
  Double_t mass;
  Double_t beta;
  Double_t x;
  Double_t y;
  Double_t theta;
  Double_t phi;
};

class PndRichCalDb : public FairField {

 public:
  /** Default constructor **/
  PndRichCalDb();

  /** Standard constructor
   ** @param name       Name of field map
   ** @param fileType   R = ROOT file, A = ASCII
   **/
  PndRichCalDb(const char *mapName, const char *fileType = "R");

  /** Constructor from PndRichCalDbPar **/
  PndRichCalDb(PndRichCalDbPar *CaldbPar);

  /** Destructor **/
  virtual ~PndRichCalDb();

  /** Initialisation (read map from file) **/
  virtual void Init();

  /** Get the field components at a certain point
   ** @param x,y,z     Point coordinates (global) [cm]
   ** @value Bx,By,Bz  Field components [kG]
   **/
  virtual Double_t GetBetaMean(dbpoint pnt);
  virtual Double_t GetBetaSig(dbpoint pnt);
  virtual Double_t GetBetaEff(dbpoint pnt);

  /** Determine whether a point is inside the field map
   ** @param x,y,z              Point coordinates (global) [cm]
   ** @param ix,iy,iz (return)  Grid cell
   ** @param dx,dy,dz (return)  Distance from grid point [cm] if inside
   ** @value kTRUE if inside map, else kFALSE
   **/
  virtual Bool_t IsInside(Double_t p, Double_t x, Double_t y, Double_t t, Double_t f, Int_t &ip, Int_t &ix, Int_t &iy, Int_t &it, Int_t &iq, Double_t &dp, Double_t &dx,
                          Double_t &dy, Double_t &dt, Double_t &df);

  /** Write the field map to an ASCII file **/
  void WriteAsciiFile(const char *fileName);

  /** Write field map data to a ROOT file **/
  void WriteRootFile(const char *fileName, const char *mapName);

  /** Accessors to field parameters in local coordinate system **/
  Double_t GetPmin() const { return fPmin; }
  Double_t GetXmin() const { return fXmin; }
  Double_t GetYmin() const { return fYmin; }
  Double_t GetTmin() const { return fTmin; }
  Double_t GetFmin() const { return fFmin; }
  Double_t GetPmax() const { return fPmax; }
  Double_t GetXmax() const { return fXmax; }
  Double_t GetYmax() const { return fYmax; }
  Double_t GetTmax() const { return fTmax; }
  Double_t GetFmax() const { return fFmax; }
  Double_t GetPstep() const { return fPstep; }
  Double_t GetXstep() const { return fXstep; }
  Double_t GetYstep() const { return fYstep; }
  Double_t GetTstep() const { return fTstep; }
  Double_t GetFstep() const { return fFstep; }
  Int_t GetNp() const { return fNp; }
  Int_t GetNx() const { return fNx; }
  Int_t GetNy() const { return fNy; }
  Int_t GetNt() const { return fNt; }
  Int_t GetNf() const { return fNf; }

  /** Accessors to the field value arrays **/
  TArrayF *GetBetaMean() const { return fBetaMean; }
  TArrayF *GetBetaSig() const { return fBetaSig; }
  TArrayF *GetBetaEff() const { return fBetaEff; }

  /** Accessor to field map file **/
  const char *GetFileName() { return fFileName.Data(); }

  /** Screen output **/
  virtual void Print();

 private:
  PndRichCalDb(const PndRichCalDb &L);
  PndRichCalDb &operator=(const PndRichCalDb &) { return *this; };

 protected:
  /** Reset the field parameters and data **/
  void Reset();

  /** Read the field map from an ASCII file **/
  void ReadAsciiFile(const char *fileName);

  /** Read field map from a ROOT file **/
  void ReadRootFile(const char *fileName, const char *mapName);

  /** Set field parameters and data **/
  void SetCalDb(const PndRichCalDbData *data);

  /** Map file name **/
  TString fFileName;

  /** Field limits in local coordinate system **/
  Double_t fPmin, fPmax, fPstep;
  Double_t fXmin, fXmax, fXstep;
  Double_t fYmin, fYmax, fYstep;
  Double_t fTmin, fTmax, fTstep;
  Double_t fFmin, fFmax, fFstep;

  /** Number of grid points  **/
  Int_t fNp, fNx, fNy, fNt, fNf;

  /** Arrays with the field values  **/
  TArrayF *fBetaMean;
  TArrayF *fBetaSig;
  TArrayF *fBetaEff;

  ClassDef(PndRichCalDb, 1)
};

#endif
