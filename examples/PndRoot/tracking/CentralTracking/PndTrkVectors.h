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

#ifndef PndTrkVectors_H
#define PndTrkVectors_H 1
// Root includes
#include "TROOT.h"

#include <stdlib.h>
#include <iostream>

using namespace std;

// this class is useful to mimic the []  operator
// of an array  or the  at() operator of the  std::vector
// but it contains also the boundary check. If the boundary
// are violated an error message is printed and a call
// to the exit  function  with value -1 is performed.
// The argument for the constructor are :
//  p   -->  pointer to an EXISTING array(of ANY type) to be mimiced;
//  dim -->  dimension of the array to be mimiced;
//  nam -->  string containing the  name of the Vec class
//	     that mimics the array.

//  The typical statements for producing the Vec mimicing
//  an existing array of type  MYTYPE is :

/*
  MYTYPE   array[100];      the array must physically exist in memory;
  Vec <MYTYPE> myvec(array, 100, "myvec");
*/

template <class T>
class Vec {

 public:
  int dimension;
  TString name;
  T *array;

  // constructor;
  Vec(T *p, int dim, TString nam)
  {
    if (dim < 0 || dim > 1000000) {
      cout << "PndTrkVectors::Vec  the dimension of the array " << nam << " is " << dim << " and not acceptable; exit(-2) the program.\n";
      exit(-2);
    }
    dimension = dim;
    array = p;
    name = nam;
  };

  ~Vec() { ; };

  // function  at();
  T &at(int index)
  {

    if (index >= 0 && index < dimension) {
      T &alias = array[index];
      return alias;
    }
    cout << "PndTrkVectors::  array " << name << ": index = " << index << " and it is out of bounds [from 0 to " << dimension - 1 << " included]; exiting the process.\n";
    exit(-1);
  };

  // overloaded operator  [] ; the functionality is identical to the  at() function;
  T &operator[](int index)
  {

    T &alias = at(index);
    return alias;
  };
};

#endif
