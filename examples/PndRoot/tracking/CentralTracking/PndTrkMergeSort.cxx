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

#include "PndTrkMergeSort.h"
#include <iostream>
#include <cmath>

// Root includes
#include "TROOT.h"

using namespace std;

//----------begin of function PndTrkMergeSort::Merge

void PndTrkMergeSort::Merge(Short_t nl, Double_t *left, Int_t *ind_left, Short_t nr, Double_t *right, Int_t *ind_right, Double_t *result, Int_t *ind)
{
  Short_t i = 0, j, nl_curr = 0, nr_curr = 0;

  while (nl > 0 && nr > 0) {
    if (left[nl_curr] <= right[nr_curr]) {
      result[i] = left[nl_curr];
      ind[i] = ind_left[nl_curr];
      nl--;
      nl_curr++;
    } else {
      result[i] = right[nr_curr];
      ind[i] = ind_right[nr_curr];
      nr--;
      nr_curr++;
    }
    i++;
  }
  //--------------------
  if (nl == 0) {
    for (j = 0; j < nr; j++) {
      result[i + j] = right[nr_curr + j];
      ind[i + j] = ind_right[nr_curr + j];
    }
  } else {
    for (j = 0; j < nl; j++) {
      result[i + j] = left[nl_curr + j];
      ind[i + j] = ind_left[nl_curr + j];
    }
  }
}

//----------end of function PndTrkMergeSort::Merge

//----------begin of function PndTrkMergeSort::Merge2
// the only difference with Merge is that in Merge2 only Short_t are used (NO Int_t);

void PndTrkMergeSort::Merge2(Short_t nl, Double_t *left, Short_t *ind_left, Short_t nr, Double_t *right, Short_t *ind_right, Double_t *result, Short_t *ind)
{
  Short_t i = 0, j, nl_curr = 0, nr_curr = 0;

  while (nl > 0 && nr > 0) {
    if (left[nl_curr] <= right[nr_curr]) {
      result[i] = left[nl_curr];
      ind[i] = ind_left[nl_curr];
      nl--;
      nl_curr++;
    } else {
      result[i] = right[nr_curr];
      ind[i] = ind_right[nr_curr];
      nr--;
      nr_curr++;
    }
    i++;
  }
  //--------------------
  if (nl == 0) {
    for (j = 0; j < nr; j++) {
      result[i + j] = right[nr_curr + j];
      ind[i + j] = ind_right[nr_curr + j];
    }
  } else {
    for (j = 0; j < nl; j++) {
      result[i + j] = left[nl_curr + j];
      ind[i + j] = ind_left[nl_curr + j];
    }
  }
}

//----------end of function PndTrkMergeSort::Merge2

//----------begin of function PndTrkMergeSort::Merge3
// the only difference with Merge is that in Merge3 only Short_t are used (NO Int_t); and instead
// of Double_t *left, Double_t *right, Double_t *result, [the arrays on which the ordering is performed]
// rhere are Int_t *right, *left, *result;  namely the ordering is performed over arrays of Int_t;

void PndTrkMergeSort::Merge3(Short_t nl, Int_t *left, Short_t *ind_left, Short_t nr, Int_t *right, Short_t *ind_right, Int_t *result, Short_t *ind)
{
  Short_t i = 0, j, nl_curr = 0, nr_curr = 0;

  while (nl > 0 && nr > 0) {
    if (left[nl_curr] <= right[nr_curr]) {
      result[i] = left[nl_curr];
      ind[i] = ind_left[nl_curr];
      nl--;
      nl_curr++;
    } else {
      result[i] = right[nr_curr];
      ind[i] = ind_right[nr_curr];
      nr--;
      nr_curr++;
    }
    i++;
  }
  //--------------------
  if (nl == 0) {
    for (j = 0; j < nr; j++) {
      result[i + j] = right[nr_curr + j];
      ind[i + j] = ind_right[nr_curr + j];
    }
  } else {
    for (j = 0; j < nl; j++) {
      result[i + j] = left[nl_curr + j];
      ind[i + j] = ind_left[nl_curr + j];
    }
  }
}

//----------end of function PndTrkMergeSort::Merge3

//----------begin of function PndTrkMergeSort::Merge_Sort

void PndTrkMergeSort::Merge_Sort(Short_t n_ele, Double_t *array, Int_t *ind)
{

  Int_t middle, i, // nr,  nl, //[R.K. 01/2017] unused variable?
    ind_left[n_ele], ind_right[n_ele];

  Double_t left[n_ele], right[n_ele];

  if (n_ele <= 1)
    return;

  middle = n_ele / 2;
  for (i = 0; i < middle; i++) {
    left[i] = array[i];
    ind_left[i] = ind[i];
  }
  for (i = middle; i < n_ele; i++) {
    right[i - middle] = array[i];
    ind_right[i - middle] = ind[i];
  }

  Merge_Sort(middle, left, ind_left);
  Merge_Sort(n_ele - middle, right, ind_right);

  if (left[middle - 1] > right[0]) {
    Merge(middle, left, ind_left, n_ele - middle, right, ind_right, array, ind);
  } else {
    //  do the appending
    for (i = 0; i < middle; i++) {
      array[i] = left[i];
      ind[i] = ind_left[i];
    }
    for (i = middle; i < n_ele; i++) {
      array[i] = right[i - middle];
      ind[i] = ind_right[i - middle];
    }
  }
}

//----------end of function PndTrkMergeSort::Merge_Sort

//----------begin of function PndTrkMergeSort::Merge_Sort2

// the only difference with Merge_Sort is that   *ind is a Short_t (and NOT an Int_t);

void PndTrkMergeSort::Merge_Sort2(Short_t n_ele, Double_t *array, Short_t *ind)
{

  Short_t middle, i, // nr, nl,  //[R.K. 01/2017] unused variable?
    ind_left[n_ele], ind_right[n_ele];

  Double_t left[n_ele], right[n_ele];

  if (n_ele <= 1)
    return;

  middle = n_ele / 2;
  for (i = 0; i < middle; i++) {
    left[i] = array[i];
    ind_left[i] = ind[i];
  }
  for (i = middle; i < n_ele; i++) {
    right[i - middle] = array[i];
    ind_right[i - middle] = ind[i];
  }

  Merge_Sort2(middle, left, ind_left);
  Merge_Sort2(n_ele - middle, right, ind_right);

  if (left[middle - 1] > right[0]) {
    Merge2(middle, left, ind_left, n_ele - middle, right, ind_right, array, ind);
  } else {
    //  do the appending
    for (i = 0; i < middle; i++) {
      array[i] = left[i];
      ind[i] = ind_left[i];
    }
    for (i = middle; i < n_ele; i++) {
      array[i] = right[i - middle];
      ind[i] = ind_right[i - middle];
    }
  }
}

//----------end of function PndTrkMergeSort::Merge_Sort2

//----------begin of function PndTrkMergeSort::Merge_Sort3

// the only difference with Merge_Sort is that   *ind is a Short_t (and NOT an Int_t) and the *array is
// an Int_t (and not a Double_t), namely the merge-sorting is based on an array of Int_t;

void PndTrkMergeSort::Merge_Sort3(Short_t n_ele,
                                  Int_t *array, // the array to be ordered;
                                  Short_t *ind)
{

  Short_t middle, i, // nr, nl, //[R.K. 01/2017] unused variable?
    ind_left[n_ele], ind_right[n_ele];

  Int_t left[n_ele], right[n_ele];

  if (n_ele <= 1)
    return;

  middle = n_ele / 2;
  for (i = 0; i < middle; i++) {
    left[i] = array[i];
    ind_left[i] = ind[i];
  }
  for (i = middle; i < n_ele; i++) {
    right[i - middle] = array[i];
    ind_right[i - middle] = ind[i];
  }

  Merge_Sort3(middle, left, ind_left);
  Merge_Sort3(n_ele - middle, right, ind_right);

  if (left[middle - 1] > right[0]) {
    Merge3(middle, left, ind_left, n_ele - middle, right, ind_right, array, ind);
  } else {
    //  do the appending
    for (i = 0; i < middle; i++) {
      array[i] = left[i];
      ind[i] = ind_left[i];
    }
    for (i = middle; i < n_ele; i++) {
      array[i] = right[i - middle];
      ind[i] = ind_right[i - middle];
    }
  }
}

//----------end of function PndTrkMergeSort::Merge_Sort3

ClassImp(PndTrkMergeSort);
