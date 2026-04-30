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
#include "PndRichCalDb.h"
#include "PndRichCalDbData.h"

// -------------   Default constructor  ----------------------------------
PndRichCalDbData::PndRichCalDbData()
  : fType(1), fPmin(0), fPmax(0), fXmin(0), fXmax(0), fYmin(0), fYmax(0), fTmin(0), fTmax(0), fFmin(0), fFmax(0), fNp(0), fNx(0), fNy(0), fNt(0), fNf(0), fBetaMean(nullptr),
    fBetaSig(nullptr), fBetaEff(nullptr)
{
}
// ------------------------------------------------------------------------

// -------------   Standard constructor   ---------------------------------
PndRichCalDbData::PndRichCalDbData(const char *mapName)
  : TNamed(mapName, "PND Rich Cal db"), fPmin(0), fPmax(0), fXmin(0), fXmax(0), fYmin(0), fYmax(0), fTmin(0), fTmax(0), fFmin(0), fFmax(0), fNp(0), fNx(0), fNy(0), fNt(0), fNf(0),
    fBetaMean(nullptr), fBetaSig(nullptr), fBetaEff(nullptr)
{
}
// ------------------------------------------------------------------------

// -----   Constructor from PndRichCalDb   ------------------------------
PndRichCalDbData::PndRichCalDbData(const char *name, const PndRichCalDb &cal)
  : TNamed(name, "PND Rich Cal db"), fType(cal.GetType()), fPmin(cal.GetPmin()), fPmax(cal.GetPmax()), fXmin(cal.GetXmin()), fXmax(cal.GetXmax()), fYmin(cal.GetYmin()),
    fYmax(cal.GetYmax()), fTmin(cal.GetTmin()), fTmax(cal.GetTmax()), fFmin(cal.GetFmin()), fFmax(cal.GetFmax()), fNp(cal.GetNp()), fNx(cal.GetNx()), fNy(cal.GetNy()),
    fNt(cal.GetNt()), fNf(cal.GetNf()), fBetaMean(new TArrayF(*(cal.GetBetaMean()))), fBetaSig(new TArrayF(*(cal.GetBetaSig()))), fBetaEff(new TArrayF(*(cal.GetBetaEff())))
{
}
// ------------------------------------------------------------------------

// -----   Copy Constructor from PndRichCalDb   ----------------------------
PndRichCalDbData::PndRichCalDbData(const PndRichCalDbData &L)
  : TNamed(L), fType(L.GetType()), fPmin(L.GetPmin()), fPmax(L.GetPmax()), fXmin(L.GetXmin()), fXmax(L.GetXmax()), fYmin(L.GetYmin()), fYmax(L.GetYmax()), fTmin(L.GetTmin()),
    fTmax(L.GetTmax()), fFmin(L.GetFmin()), fFmax(L.GetFmax()), fNp(L.GetNp()), fNx(L.GetNx()), fNy(L.GetNy()), fNt(L.GetNt()), fNf(L.GetNf()),
    fBetaMean(new TArrayF(*(L.GetBetaMean()))), fBetaSig(new TArrayF(*(L.GetBetaSig()))), fBetaEff(new TArrayF(*(L.GetBetaEff())))

{
}
// ------------------------------------------------------------------------

// ------------   Define size of PndRichCalDbData   --------------------------------------------
Int_t PndRichCalDbData::SizeOf()
{
  Int_t size = this->Sizeof();
  size += 6 * sizeof(Int_t);
  size += 10 * sizeof(Double_t);
  size += fBetaMean->GetSize() * sizeof(Float_t);
  size += fBetaSig->GetSize() * sizeof(Float_t);
  size += fBetaEff->GetSize() * sizeof(Float_t);
  return size;
}
// ------------------------------------------------------------------------

// ------------   Destructor   --------------------------------------------
PndRichCalDbData::~PndRichCalDbData()
{
  if (fBetaMean)
    delete fBetaMean;
  if (fBetaSig)
    delete fBetaSig;
  if (fBetaEff)
    delete fBetaEff;
}
// ------------------------------------------------------------------------

ClassImp(PndRichCalDbData)
