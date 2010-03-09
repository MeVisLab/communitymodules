// Copyright (c) 2010, Biomedical Imaging Group Rotterdam (BIGR),
// Departments of Radiology and Medical Informatics, Erasmus MC. All
// rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of BIGR nor the names of its contributors
//       may be used to endorse or promote products derived from this software
//       without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL BIGR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef PARSER_H
#define PARSER_H


#include <cmath>
#include <iostream>

typedef double (*UnaryFunction)(double);
typedef double (*BinaryFunction)(double,double);

typedef enum
  {
    ttVariable,
    ttNumber,
    ttUnaryFunction,
    ttBinaryFunction,
    ttLeftParenthesis,
    ttRightParenthesis
  }
  parserTokenType;

typedef struct
{
  parserTokenType type;
  union
  {
    double* variable;
    double number;
    UnaryFunction uf;
    BinaryFunction bf;
  };
}
  Token;


class Parser
{
public:
  Parser (const char* infix, std::ostream& ostr = std::cout);
  ~Parser ();

  int IsValidExpression();

  int IsWhiteSpace (char c)
  { return c == ' ' || c == '\t' || c == '\n'; }
  int IsIdentifier (char c)
  { return isalpha(c); }
  int IsNumber (char c)
  { return isdigit(c) || c == '.'; }

  int IsVariable (const char* string, int length, double** var);
  int IsConstant (const char* string, int length, double* value);
  int IsFunction (const char* string, int length, UnaryFunction* f);
  int IsOperator (char c, BinaryFunction* f);

  int HigherPrecedence (Token& op1, Token& op2);
  double Value ( double _x, double _y = 0, double _z = 0,
                 double _c = 0, double _t = 0, double _u = 0);

  int isValid;

private:
  // constants
  static const double pi, e;

  // variables
  double x, y, z, c, t, u;

  // functions
  static double Power    (double op1, double op2) { return pow(op1,op2); }
  static double Multiply (double op1, double op2) { return op1*op2; }
  static double Divide   (double op1, double op2) { return op1/op2; }
  static double Add      (double op1, double op2) { return op1+op2; }
  static double Subtract (double op1, double op2) { return op1-op2; }

  int tokenListLength;
  Token* tokenList;

  int maxStackSize;
};

#endif
