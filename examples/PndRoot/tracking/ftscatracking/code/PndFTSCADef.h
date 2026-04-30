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

//-*- Mode: C++ -*-

//* This file is property of and copyright by the ALICE HLT Project        *
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

#ifndef PNDFTSCADEF_H
#define PNDFTSCADEF_H

#include <iostream>
using std::cout;
using std::endl;

#include <Vc/Vc>
#include <Vc/limits>

using ::Vc::double_v;
using ::Vc::float_v;
// using ::Vc::sfloat_v;
using ::Vc::double_m;
using ::Vc::float_m;
using ::Vc::int_v;
using ::Vc::short_v;
using ::Vc::uint_v;
using ::Vc::ushort_v;
using ::Vc::VectorAlignment;
// using ::Vc::sfloat_m;
using ::Vc::int_m;
using ::Vc::uint_m;
// using ::Vc::short_m;
// using ::Vc::ushort_m;

/**
 * Definitions needed for PndFTSCATracker
 *
 */

#if defined(FTSCA_STANDALONE)
typedef unsigned char UChar_t;
typedef UChar_t Byte_t;
typedef int Int_t;
typedef double Double_t;
#else
#include "Rtypes.h"
#endif

// according to http://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template <typename T>
int sign(T val)
{
  return (T(0) < val) - (val < T(0));
}

template <typename T>
T rcp(T val)
{
  return reciprocal(val);
}

#ifdef NDEBUG
#define ASSERT(v, msg)
#else
#define ASSERT(v, msg)                                                                                                   \
  if (v) {                                                                                                               \
  } else {                                                                                                               \
    std::cerr << __FILE__ << ":" << __LINE__ << " assertion failed: " << #v << " = " << (v) << "\n" << msg << std::endl; \
    abort();                                                                                                             \
  }
#endif

struct float2 {
  float x;
  float y;
};
/*
 * Helper for compile-time verification of correct API usage
 */
namespace {
template <bool>
struct FTSCA_STATIC_ASSERT_FAILURE;
template <>
struct FTSCA_STATIC_ASSERT_FAILURE<true> {
};
} // namespace

#define FTSCA_STATIC_ASSERT_CONCAT_HELPER(a, b) a##b
#define FTSCA_STATIC_ASSERT_CONCAT(a, b) FTSCA_STATIC_ASSERT_CONCAT_HELPER(a, b)
#define STATIC_ASSERT(cond, msg)                                                                                  \
  typedef FTSCA_STATIC_ASSERT_FAILURE<cond> FTSCA_STATIC_ASSERT_CONCAT(_STATIC_ASSERTION_FAILED_##msg, __LINE__); \
  FTSCA_STATIC_ASSERT_CONCAT(_STATIC_ASSERTION_FAILED_##msg, __LINE__) Error_##msg;                               \
  (void)Error_##msg

namespace {
template <typename T1>
void UNUSED_PARAM1(const T1 &)
{
}
template <typename T1, typename T2>
void UNUSED_PARAM2(const T1 &, const T2 &)
{
}
template <typename T1, typename T2, typename T3>
void UNUSED_PARAM3(const T1 &, const T2 &, const T3 &)
{
}
template <typename T1, typename T2, typename T3, typename T4>
void UNUSED_PARAM4(const T1 &, const T2 &, const T3 &, const T4 &)
{
}
template <typename T1, typename T2, typename T3, typename T4, typename T5>
void UNUSED_PARAM5(const T1 &, const T2 &, const T3 &, const T4 &, const T5 &)
{
}
template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
void UNUSED_PARAM6(const T1 &, const T2 &, const T3 &, const T4 &, const T5 &, const T6 &)
{
}
template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
void UNUSED_PARAM7(const T1 &, const T2 &, const T3 &, const T4 &, const T5 &, const T6 &, const T7 &)
{
}
template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
void UNUSED_PARAM8(const T1 &, const T2 &, const T3 &, const T4 &, const T5 &, const T6 &, const T7 &, const T8 &)
{
}
template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
void UNUSED_PARAM9(const T1 &, const T2 &, const T3 &, const T4 &, const T5 &, const T6 &, const T7 &, const T8 &, const T9 &)
{
}
} // namespace

#define unrolled_loop4(_type_, _it_, _start_, _end_, _code_)         \
  if (_start_ + 0 < _end_) {                                         \
    enum { _it_ = (_start_ + 0) < _end_ ? (_start_ + 0) : _start_ }; \
    _code_                                                           \
  }                                                                  \
  if (_start_ + 1 < _end_) {                                         \
    enum { _it_ = (_start_ + 1) < _end_ ? (_start_ + 1) : _start_ }; \
    _code_                                                           \
  }                                                                  \
  if (_start_ + 2 < _end_) {                                         \
    enum { _it_ = (_start_ + 2) < _end_ ? (_start_ + 2) : _start_ }; \
    _code_                                                           \
  }                                                                  \
  if (_start_ + 3 < _end_) {                                         \
    enum { _it_ = (_start_ + 3) < _end_ ? (_start_ + 3) : _start_ }; \
    _code_                                                           \
  }                                                                  \
  do {                                                               \
  } while (false)

#ifdef __GNUC__
#define MAY_ALIAS __attribute__((__may_alias__))
#else
#define MAY_ALIAS
#endif

#if defined(__GNUC__) && __GNUC__ - 0 >= 3
#define ISLIKELY(x) __builtin_expect(!!(x), 1)
#define ISUNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define ISLIKELY(x) (x)
#define ISUNLIKELY(x) (x)
#endif

#endif
