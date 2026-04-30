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

#ifndef PNDDOCUSAMPLE_H
#define PNDDOCUSAMPLE_H

#include <string>

#include "TObject.h"

/**
 * @class PndDocuSample
 * @brief Documentation Sample Class
 * @details # Sample Documentation Class
 * This file highlights documentation with `Doxygen`. Please see the individual parts for more information.
 *
 * The current paragraph is designated for a long description of the class, possibly also with run / invocation instructions. Since this can be quite extensive, Doxygen provides
 * functionality to properly format text. Two ways of styling text are supported, `HTML` and `Markdown` syntax. I will shortly highlight the latter, as it is a neat, quite natural
 * way of styling text.
 *
 * ## Markdown
 * Markdown is the a markup language which lets you write styled text.
 *
 * Headings are created with a number of consecutive hashes (`#`), text can be **bold** or in *italics*. The following paragraph concisely highlights some features and is followed
 * by the code used to generate it.
 *
 * For more, see the [original page](http://daringfireball.net/projects/markdown/syntax) or [this slightly more extensive
 * example](http://www.unexpected-vortices.com/sw/rippledoc/quick-markdown-example.html).
 *
 * ### Example
 * This text can be *italic*, can be *bold*, or with `fixed-width`. Also, a [link](http://panda.gsi.de) can be set and an image included:
 * ![PANDA Logo](http://www-panda.gsi.de/html/org/logo/PandaLogo2_web.gif "The PANDA logo")
 *
 * Code (uninterpreted) can be included in a block with three tildes (both for start and end of block). Optionally, a language can be specified which is used to generate syntax
 * highlighted code.
 *
 * ~~~{.cxx}
 * int i = 0;
 * i++;
 * ~~~
 *
 * #### Lists
 *   * An unnumbered
 *   * list is done
 *   * Like this
 *
 * And
 *
 *   1. A numbered
 *   2. List is done
 *   3. Like this
 *
 * ### Code
 * The code used to generated the last section is the following:
 *
 * ~~~{.md}
 * ### Example
 * This text can be *italic*, can be *bold*, or with `fixed-width`. Also, a [link](http://panda.gsi.de) can be set and an image included:
 * ![PANDA Logo](http://www-panda.gsi.de/html/org/logo/PandaLogo2_web.gif "The PANDA logo")
 *
 * #### Lists
 *   * An unnumbered
 *   * list is done
 *   * Like this
 *
 * And
 *
 *   1. A numbered
 *   2. List is done
 *   3. Like this
 * ~~~
 *
 * ## Formulas
 * If your class / method needs some mathematical description, you can use LaTeX to create formulas
 * @f[
 *  55 = \sum_{i = 1}^{10} i
 * @f]
 * The formulas are rendered either in-browser with MathJax or during generation of the documentation as an image, depending on the configuration in the doxygen config file.
 *
 * @author Andreas Herten <a.herten@fz-juelich.de>, FZJ
 * @version 1.0
 * @date May 11, 2015
 * @copyright GNU Public License
 */
class PndDocuSample {
 public:
  /**
   * @brief Default constructor does not need much of a description
   */
  PndDocuSample();
  /**
   * @brief Proper constructor
   * @details Use this constructor to directly initialize the class
   * @remark You should use this constructor
   *
   * @param _s A string with some information
   * @param _i Some integer. Does not need to be 42.
   */
  PndDocuSample(std::string _s, int _i);
  ~PndDocuSample(); ///< Default destructor

  /**
   * @brief Set the string
   *
   * @param[in] _s @parblock
   * The input string.
   *
   * You can give additional *directions* of the variable. It can be an input, which should be the case usually, or on output, e.g. if you provide a pointer or reference to a
   * variable to be filled in the course of this method.
   *
   * Notice that in the description of this method the usually one-line `param` description was extended by using `parblock`.
   * @endparblock
   */
  void SetString(std::string _s) { fString = _s; };
  /**
   * @brief Set the number
   * @note Notes are different ways of giving attention to something specific. Like this [panda](http://giphy.com/gifs/r5bFxWGHSSlVe/html5).
   *
   * @param _i Number
   */
  void SetNumber(int _i) { fNumber = _i; };

  /**
   * @brief Returns the current string
   * @return The current content of @p fString
   */
  std::string GetString() { return fString; };
  /**
   * @brief Return the current number
   * @return The current content of @p fNumber
   */
  int GetNumber() { return fNumber; };

  /**
   * @brief You can either document in the header file...
   * @details This is true for all descriptions. They are combined.
   */
  void PrintValues();
  void AddString();
  void SquareNumber();

 private:
  bool fInitialized; ///< Easiest way to document data member variables

  /** @brief This more verbose method can be used as well
   * @details And you can use a detailed comment if you feel the need.
   *
   * Surely, also with *Markdown* and @f$\LaTeX@f$.
   * */
  std::string fString;
  int fNumber;      ///< This is the **fastest**, though.
  Int_t fTransient; //!<! A transient member

  /** @cond CLASSIMP */       // As long as CLASSIMP is not set, the ClassDef() and ClassImp() macros are excluded from the documentation
  ClassDef(PndDocuSample, 1); ///< ClassDef/ClassImp should always be terminated with a semicolon
                              /** @endcond */
};

#endif // PNDDOCUSAMPLE_H
