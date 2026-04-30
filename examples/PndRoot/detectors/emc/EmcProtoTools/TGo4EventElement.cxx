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

// $Id: TGo4EventElement.cxx 760 2011-06-22 12:03:48Z linev $
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4EventElement.h"

#include "TTree.h"
#include <iostream>
#include "FairLogger.h"

#if ROOT_VERSION_CODE <= ROOT_VERSION(5, 34, 19)
R__EXTERN TTree *gTree;
#endif

TGo4EventElement::TGo4EventElement()
  : TNamed("Go4Element", "This is a Go4 EventElement"), fbIsValid(kTRUE), fxParent(nullptr), fxEventSource(nullptr), fIdentifier(-1), fDebug(kFALSE)
{
}

TGo4EventElement::TGo4EventElement(const char *name)
  : TNamed(name, "This is a Go4 EventElement"), fbIsValid(kTRUE), fxParent(nullptr), fxEventSource(nullptr), fIdentifier(-1), fDebug(kFALSE)
{
}

TGo4EventElement::TGo4EventElement(const char *aName, const char *aTitle, Short_t aBaseCat)
  : TNamed(aName, aTitle), fbIsValid(kTRUE), fxParent(nullptr), fxEventSource(nullptr), fIdentifier(aBaseCat), fDebug(kFALSE)
{
}

TGo4EventElement::~TGo4EventElement() {}

Bool_t TGo4EventElement::CheckEventSource(const char *) // classname  //[R.K.03/2017] unused variable(s)
{
  return kFALSE;
}

void TGo4EventElement::PrintEvent() {}

void TGo4EventElement::Print(Option_t *) const
{
  ((TGo4EventElement *)this)->PrintEvent();
}

void TGo4EventElement::makeBranch(TBranch *) // parent //[R.K.03/2017] unused variable(s)
{
  // method for recursive branching algorithm
}

void TGo4EventElement::synchronizeWithTree(TTree *tree, TGo4EventElement **var_ptr)
{

  if (tree == nullptr)
    return;

  TBranch *topb = nullptr;
  TString searchname = GetName();
  if (searchname.Length() > 0) {
    searchname += ".";
    topb = tree->FindBranch(searchname.Data());
  }

  // if no appropriate branches found, use first branch for the event
  // TODO: should we check class name of the branch?
  if (topb == 0)
    topb = (TBranch *)tree->GetListOfBranches()->First();

  Int_t index = tree->GetListOfBranches()->IndexOf(topb);

  // FIXME SL: should we activate other branches, reading not working when all branches disabled in the beginning
  // note: only deactivate subleafs _after_ address of top branch is set!
  // tree->SetBranchStatus("*",0);

  activateBranch(topb, index, var_ptr);
}

/** THIS WAS OLD CODE FROM TGo4FileSource

void TGo4EventElement::synchronizeWithTree(TTree *tree, TGo4EventElement** var_ptr)

TString topname;
Bool_t masterbranch=kFALSE;
TString branchName = dest->GetName();
if(!fxBranchName.Contains("."))  {
   fxBranchName+="."; // for master branch, add dot. Subbranch names with dot separators do not get final dot
   masterbranch=kTRUE;
}
TObjArray* blist = tree->GetListOfBranches();
TBranch* topb = (TBranch*) blist->At(0);
if(topb) {
   topname = topb->GetName();
   //cout <<"Activating top branch "<<topname.Data() << endl;
   tree->SetBranchAddress(topname.Data(),(void**) var_ptr);
   //topb->SetAddress(&fxTopEvent); // this will not set address of possible cloned tree. we use the set address of the tree
}
tree->SetBranchStatus("*",0); // note: only deactivate subleafs _after_ address of top branch is set!
tree->SetBranchStatus(topname.Data(),1); // required to process any of the subbranches!
TString wildbranch = branchName;
wildbranch += "*";
tree->SetBranchStatus(wildbranch.Data(),1);
//cout <<"Build event activates: "<<wildbranch.Data() << endl;
wildbranch = branchName;
if(!masterbranch) wildbranch+=".";
wildbranch+="*";
tree->SetBranchStatus(wildbranch.Data(),1);
//cout <<"Build event activates: "<<wildbranch.Data() << endl;
fbActivated = kTRUE;

*/

Int_t TGo4EventElement::activateBranch(TBranch *branch, Int_t, TGo4EventElement **var_ptr) // init //[R.K.03/2017] unused variable(s)
{
  if (branch == nullptr)
    return 0;

  TString cad = branch->GetName();

  TTree *tree = branch->GetTree();

  if (var_ptr != 0)
    tree->SetBranchAddress(cad.Data(), (void **)var_ptr);

  tree->SetBranchStatus(cad.Data(), 1);
  cad += "*";
  tree->SetBranchStatus(cad.Data(), 1);

  return 0;
}

void TGo4EventElement::deactivate()
{
  TString name = GetName();
#if ROOT_VERSION_CODE <= ROOT_VERSION(5, 34, 19)
  name += ".";
  gTree->SetBranchStatus(name.Data(), 0);
  name += "*";
  gTree->SetBranchStatus(name.Data(), 0);
#else
  LOG(warn) << " Could not deactivate() event element %s in this ROOT Version, do not use!" << name.Data();
#endif
}

void TGo4EventElement::activate()
{
  TString name = GetName();
#if ROOT_VERSION_CODE <= ROOT_VERSION(5, 34, 19)
  name += ".";
  gTree->SetBranchStatus(name.Data(), 1);
  name += "*";
  gTree->SetBranchStatus(name.Data(), 1);
#else
  LOG(warn) << " Could not activate() element %s in this ROOT Version, do not use!" << name.Data();
#endif
}

void TGo4EventElement::Clear(Option_t *) {}

Int_t TGo4EventElement::Init()
{
  Int_t res(0);
  Clear();
  SetValid(kTRUE);
  if (fxEventSource) {
  } else {
    res = 1;
  }
  return res;
}

Int_t TGo4EventElement::Fill()
{
  Clear();

  if (fxEventSource == nullptr) {
    SetValid(kFALSE);
    return 1;
  }

  Int_t res = 0;
  SetValid(kFALSE);

  return res == 0 ? 1 : res;
}
