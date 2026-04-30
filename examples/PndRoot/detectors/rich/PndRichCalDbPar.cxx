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

#include <iostream>
#include "PndConstField.h"
#include "PndRichCalDb.h"
#include "PndRichCalDbPar.h"
#include "FairParamList.h"
#include "PndMultiField.h"
#include "TObjArray.h"

using namespace std;

// ------   Constructor   --------------------------------------------------
PndRichCalDbPar::PndRichCalDbPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fType(-1), fPmin(0), fPmax(0), fXmin(0), fXmax(0), fYmin(0), fYmax(0), fTmin(0), fTmax(0), fFmin(0), fFmax(0), fBetaMean(0),
    fBetaSig(0), fBetaEff(0), fMapName(TString(""))

{
}
// -------------------------------------------------------------------------

PndRichCalDbPar::PndRichCalDbPar()
  : fType(-1), fPmin(0), fPmax(0), fXmin(0), fXmax(0), fYmin(0), fYmax(0), fTmin(0), fTmax(0), fFmin(0), fFmax(0), fBetaMean(0), fBetaSig(0), fBetaEff(0), fMapName(TString(""))
{
}
// -------------------------------------------------------------------------

// ------   Destructor   ---------------------------------------------------
PndRichCalDbPar::~PndRichCalDbPar() {}
// -------------------------------------------------------------------------

// ------   Put parameters   -----------------------------------------------
void PndRichCalDbPar::putParams(FairParamList *list)
{

  if (!list)
    return;

  list->add("Field Type", fType);

  if (fType == 0) { // constant field
    list->add("Cal db min p", fPmin);
    list->add("Cal db max p", fPmax);
    list->add("Cal db min x", fXmin);
    list->add("Cal db max x", fXmax);
    list->add("Cal db min y", fYmin);
    list->add("Cal db max y", fYmax);
    list->add("Cal db min t", fTmin);
    list->add("Cal db max t", fTmax);
    list->add("Cal db min f", fFmin);
    list->add("Cal db max f", fFmax);
    list->add("Cal db BetaMean", fBetaMean);
    list->add("Cal db BetaSig", fBetaSig);
    list->add("Cal db BetaEff", fBetaEff);
  }
}
// -------------------------------------------------------------------------

// --------   Get parameters   ---------------------------------------------
Bool_t PndRichCalDbPar::getParams(FairParamList *list)
{

  if (!list)
    return kFALSE;

  if (!list->fill("Cal db Type", &fType))
    return kFALSE;

  if (fType == 0) { // constant field
    if (!list->fill("Cal db min p", &fPmin))
      return kFALSE;
    if (!list->fill("Cal db max p", &fPmax))
      return kFALSE;
    if (!list->fill("Cal db min x", &fXmin))
      return kFALSE;
    if (!list->fill("Cal db max x", &fXmax))
      return kFALSE;
    if (!list->fill("Cal db min y", &fYmin))
      return kFALSE;
    if (!list->fill("Cal db max y", &fYmax))
      return kFALSE;
    if (!list->fill("Cal db min t", &fTmin))
      return kFALSE;
    if (!list->fill("Cal db max t", &fTmax))
      return kFALSE;
    if (!list->fill("Cal db min f", &fFmin))
      return kFALSE;
    if (!list->fill("Cal db max f", &fFmax))
      return kFALSE;
    if (!list->fill("Cal db BetaMean", &fBetaMean))
      return kFALSE;
    if (!list->fill("Cal db BetaSig", &fBetaSig))
      return kFALSE;
    if (!list->fill("Cal db BetaEff", &fBetaEff))
      return kFALSE;
  }

  return kTRUE;
}
// -------------------------------------------------------------------------

// ---------   Set parameters from FairField   ------------------------------
void PndRichCalDbPar::SetParameters(FairField *field)
{

  if (!field) {
    cerr << "-W- PndRichCalDbPar::SetParameters: Empty field pointer!" << endl;
    return;
  }

  fType = field->GetType();
  /*
    if ( fType == 0 ) {                                 // constant field
      PndConstField* fieldConst = (PndConstField*) field;
      fBx = fieldConst->GetBx();
      fBy = fieldConst->GetBy();
      fBz = fieldConst->GetBz();
      fXmin = fieldConst->GetXmin();
      fXmax = fieldConst->GetXmax();
      fYmin = fieldConst->GetYmin();
      fYmax = fieldConst->GetYmax();
      fZmin = fieldConst->GetZmin();
      fZmax = fieldConst->GetZmax();
      fMapName = "";
      fPosX = fPosY = fPosZ = fScale = 0.;
    }

    else if ( fType >=1 && fType <= 4 ) {              // field map
      PndRichCalDb* fieldMap = (PndRichCalDb*) field;
      fBx = fBy = fBz = 0.;
      fXmin = fXmax = fYmin = fYmax = fZmin = fZmax = 0.;

      fMapName = field->GetName();
      fPosX   = fieldMap->GetPositionX();
      fPosY   = fieldMap->GetPositionY();
      fPosZ   = fieldMap->GetPositionZ();
      fScale  = fieldMap->GetScale();
    } else if (fType ==5) {

      PndMultiField *fMulti=(PndMultiField *)field;
      TObjArray *fieldlist = fMulti->GetFieldList();
      TIterator* FieldIter = fieldlist->MakeIterator();
      FieldIter->Reset();
      FairField *fi=0;
      while( (fi = (FairField*)FieldIter->Next() ) ) {
           SetParameters(fi);
      }

     delete  FieldIter;
    }else {
      cerr << "-W- PndRichCalDbPar::SetParameters: Unknown field type "
     << fType << "!" << endl;
      fBx = fBy = fBz = 0.;
      fXmin = fXmax = fYmin = fYmax = fZmin = fZmax = 0.;
      fMapName = "";
      fPosX = fPosY = fPosZ = fScale = 0.;
    }
  */
  return;
}
// -------------------------------------------------------------------------

ClassImp(PndRichCalDbPar)
