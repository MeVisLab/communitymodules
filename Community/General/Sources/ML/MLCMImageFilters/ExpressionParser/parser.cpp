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


#include "parser.h"

#include <math.h>

using namespace std;

const double Parser::pi = 4.0*atan(1.0);
const double Parser::e = exp(1.0);


// below erf codes are from http://www.digitalmars.com/archives/cplusplus/3634.html

static const double rel_error= 1E-12;        //calculate 12 significant figures
//you can adjust rel_error to trade off between accuracy and speed
//but don't ask for > 15 figures (assuming usual 52 bit mantissa in a double)

double erfc(double);

double erf(double x)
//erf(x) = 2/sqrt(pi)*integral(exp(-t^2),t,0,x)
//       = 2/sqrt(pi)*[x - x^3/3 + x^5/5*2! - x^7/7*3! + ...]
//       = 1-erfc(x)
{
  static const double two_sqrtpi=  1.128379167095512574;        // 2/sqrt(pi)
  if (fabs(x) > 2.2) {
    return 1.0 - erfc(x);        //use continued fraction when fabs(x) > 2.2
  }
  double sum= x, term= x, xsqr= x*x;
  int j= 1;
  do {
    term*= xsqr/j;
    sum-= term/(2*j+1);
    ++j;
    term*= xsqr/j;
    sum+= term/(2*j+1);
    ++j;
  } while (fabs(term/sum) > rel_error);   // CORRECTED LINE
  return two_sqrtpi*sum;
}


double erfc(double x)
//erfc(x) = 2/sqrt(pi)*integral(exp(-t^2),t,x,inf)
//        = exp(-x^2)/sqrt(pi) * [1/x+ (1/2)/x+ (2/2)/x+ (3/2)/x+ (4/2)/x+ ...]
//        = 1-erf(x)
//expression inside [] is a continued fraction so '+' means add to denominator only

{
  static const double one_sqrtpi=  0.564189583547756287;        // 1/sqrt(pi)
  if (fabs(x) < 2.2) {
    return 1.0 - erf(x);        //use series when fabs(x) < 2.2
  }
  if ( /* signbit(x) */ x < 0) {               //continued fraction only valid for x>0
    return 2.0 - erfc(-x);
  }
  double a=1, b=x;                //last two convergent numerators
  double c=x, d=x*x+0.5;          //last two convergent denominators
  double q1, q2= b/d;             //last two convergents (a/c and b/d)
  double n= 1.0, t;
  do {
    t= a*n+b*x;
    a= b;
    b= t;
    t= c*n+d*x;
    c= d;
    d= t;
    n+= 0.5;
    q1= q2;
    q2= b/d;
  } while (fabs(q1-q2)/q2 > rel_error);
  return one_sqrtpi*exp(-x*x)*q2;
}








//===========================================================================
Parser::Parser (const char* infix, ostream& ostr) :
  tokenList(0), isValid(1)
{
  if ( infix == NULL )
    {
      ostr << "Parser: null string not allowed" << endl;
      isValid = 0;
      return;
    }

  int infixLength = strlen(infix);
  if ( infixLength == 0 )
    {
      ostr << "Parser: empty string not allowed" << endl;
      isValid = 0;
      return;
    }

  tokenListLength = 0;
  tokenList = new Token[infixLength];

  Token* tokenStack = new Token[infixLength];
  int tokenStackTop = -1;

  Token token, topOp, op;

  const char* s = infix;
  for (const char* f = s; *f; f = s)
    {
      int length;

      // get token from input string
      if ( IsWhiteSpace(*s) )
        {
          s++;
          continue;
        }

      if ( IsIdentifier(*s) )
        {
          for (length = 0; IsIdentifier(*s); length++)
            s++;

          if ( IsVariable(f,length,&token.variable) )
            {
              token.type = ttVariable;
            }
          else if ( IsConstant(f,length,&token.number) )
            {
              token.type = ttNumber;
            }
          else if ( IsFunction(f,length,&token.uf) )
            {
              token.type = ttUnaryFunction;
            }
          else
            {
              char* save = new char[length+1];
              strncpy(save,f,length);
              save[length+1] = 0;
              ostr << "Parser: invalid identifier " << save << endl;
              delete[] save;
              isValid = 0;
              delete[] tokenStack;
              return;
            }
        }
      else if ( IsNumber(*s) )
        {
          for (length = 0; IsNumber(*s); length++)
            s++;

          int decPoints = 0;
          for (int i = 0; i < length; i++)
            decPoints += ( f[i] == '.' );
          if ( decPoints > 1 )
            {
              ostr << "Parser: too many decimal points in number" << endl;
              isValid = 0;
              delete[] tokenStack;
              return;
            }

          char* number = new char[length+1];
          strncpy(number,f,length);
          number[length] = 0;
          token.type = ttNumber;
          token.number = atof(number);
          delete[] number;
        }
      else if ( IsOperator(*s,&token.bf) )
        {
          token.type = ttBinaryFunction;
          s++;
        }
      else if ( *s == '(' )
        {
          token.type = ttLeftParenthesis;
          s++;
        }
      else if ( *s == ')' )
        {
          token.type = ttRightParenthesis;
          s++;
        }
      else
        {
          if ( *s )
            {
              ostr << "Parser: invalid symbol " << *s << endl;
              isValid = 0;
              delete[] tokenStack;
              return;
            }
        }

      // parse current token
      if ( token.type == ttNumber || token.type == ttVariable )
        {
          tokenList[tokenListLength++] = token;
        }
      else
        {
          if ( tokenStackTop >= 0 )
            topOp = tokenStack[tokenStackTop];
          while ( tokenStackTop >= 0 && HigherPrecedence(topOp,token) )
            {
              op = tokenStack[tokenStackTop--];
              tokenList[tokenListLength++] = op;
              if ( tokenStackTop >= 0 )
                topOp = tokenStack[tokenStackTop];
            }
          if ( token.type != ttRightParenthesis )
            {
              tokenStack[++tokenStackTop] = token;
            }
          else if ( tokenStackTop >= 0 )
            {
              op = tokenStack[tokenStackTop--];
            }
          else
            {
              ostr << "Parser:  unmatched right parenthesis" << endl;
              isValid = 0;
              delete[] tokenStack;
              return;
            }
        }
    }

  while ( tokenStackTop >= 0 )
    {
      op = tokenStack[tokenStackTop--];
      if ( op.type == ttLeftParenthesis )
        {
          ostr << "Parser: unmatched left parenthesis" << endl;
          isValid = 0;
          delete[] tokenStack;
          return;
        }
      tokenList[tokenListLength++] = op;
    }
  if ( tokenListLength == 0 )
    {
      ostr << "Parser: null expression" << endl;
      isValid = 0;
      delete[] tokenStack;
      return;
    }

  isValid = IsValidExpression();
  if ( isValid == 0 )
    ostr << "Parser: invalid expression" << endl;
  delete[] tokenStack;
}
//---------------------------------------------------------------------------
Parser::~Parser ()
{
  delete[] tokenList;
}
//---------------------------------------------------------------------------
int Parser::IsVariable (const char* ss, int length, double** var)
{
  if ( length == 1 )
    {
      switch ( *ss )
        {
        case 'x':  *var = &x;  return 1;
        case 'y':  *var = &y;  return 1;
        case 'z':  *var = &z;  return 1;
        case 'c':  *var = &c;  return 1;
        case 't':  *var = &t;  return 1;
        case 'u':  *var = &u;  return 1;
        default:   *var = 0;   return 0;
        }
    }
  return 0;
}
//---------------------------------------------------------------------------
int Parser::IsConstant (const char* string, int length, double* value)
{
  if ( length == 2 && strcmp(string,"pi") )
    {
      *value = pi;
      return 1;
    }

  if ( length == 1 && strcmp(string,"e") )
    {
      *value = e;
      return 1;
    }

  return 0;
}
//---------------------------------------------------------------------------
int Parser::IsOperator (char s, BinaryFunction* f)
{
  switch ( s )
    {
    case '^':  *f = Power;     return 1;
    case '*':  *f = Multiply;  return 1;
    case '/':  *f = Divide;    return 1;
    case '+':  *f = Add;       return 1;
    case '-':  *f = Subtract;  return 1;
    default :  *f = 0;         return 0;
    }
}
//---------------------------------------------------------------------------
double sign(double f)
{
  return (f<0)?-1:(f>0)?1:0;
}

const double randfac = 1.0/RAND_MAX;

double randomnumber(double /* f */)
{
  return rand()*randfac;
}

//double erf(double f)
//{
//  erf(f);
//}

int Parser::IsFunction (const char* string, int length, UnaryFunction* f)
{
  switch ( length )
    {
    case 3:
      if ( strncmp(string,"sin",3) == 0 )
        {
          *f = sin;
          return 1;
        }
      if ( strncmp(string,"cos",3) == 0 )
        {
          *f = cos;
          return 1;
        }
      if ( strncmp(string,"tan",3) == 0 )
        {
          *f = tan;
          return 1;
        }
      if ( strncmp(string,"exp",3) == 0 )
        {
          *f = exp;
          return 1;
        }
      if ( strncmp(string,"log",3) == 0 )
        {
          *f = log;
          return 1;
        }
      
      if ( strncmp(string,"erf",3) == 0 )
        {
          *f = erf;
          return 1;
        }
        
    case 4:
      if ( strncmp(string,"sqrt",4) == 0 )
        {
          *f = sqrt;
          return 1;
        }
      if ( strncmp(string,"atan",4) == 0 )
        {
          *f = atan;
          return 1;
        }
      if ( strncmp(string,"sign",4) == 0 )
        {
          *f = sign;
          return 1;
        }
      if ( strncmp(string,"asin",4) == 0 )
      {
        *f = asin;
        return 1;
      }
      if ( strncmp(string,"acos",4) == 0 )
      {
        *f = acos;
        return 1;
      }
    case 5:
      if ( strncmp(string,"floor",5) == 0 )
        {
          *f = floor;
          return 1;
        }
    case 12:
      if ( strncmp(string,"randomnumber",12) == 0 )
        {
          *f = randomnumber;
          return 1;
        }
    default:
      *f = 0;
      return 0;
    }
}
//---------------------------------------------------------------------------
int Parser::
HigherPrecedence (Token& op1, Token& op2)
{
  int result;

  switch ( op1.type )
    {
    case ttLeftParenthesis:
      result = 0;
      break;
    case ttBinaryFunction:
      if ( op1.bf == Power || op1.bf == Multiply || op1.bf == Divide )
        {
          result = op2.type == ttRightParenthesis
            ||   op2.bf == Multiply
            ||   op2.bf == Divide
            ||   op2.bf == Add
            ||   op2.bf == Subtract;
          break;
        }
      if ( op1.bf == Add || op1.bf == Subtract )
        {
          result = op2.type == ttRightParenthesis
            ||   op2.bf == Add
            ||   op2.bf == Subtract;
          break;
        }
      result = 0;
      break;
    case ttUnaryFunction:
      result = op2.type == ttRightParenthesis
        ||   op2.type == ttBinaryFunction;
      break;
    default:
      result = 0;
    }

  return result;
}
//---------------------------------------------------------------------------
int Parser::IsValidExpression ()
{
  int stackSize = 0;
  maxStackSize = 0;

  for (int i = 0; i < tokenListLength; i++)
    {
      Token token = tokenList[i];

      switch ( token.type )
        {
        case ttNumber:
        case ttVariable:
          // push
          stackSize++;
          if ( stackSize > maxStackSize )
            maxStackSize = stackSize;
          break;
        case ttUnaryFunction:
          // pop
          // push
          if ( stackSize < 1 )
            return 0;
          break;
        case ttBinaryFunction:
          // pop
          // pop
          // push
          if ( stackSize < 2 )
            return 0;
          stackSize--;
          break;
        default:
          break;
        }
    }

  return ( stackSize == 1 );  // pop final result
}
//---------------------------------------------------------------------------
double Parser::
Value (double _x, double _y, double _z, double _c, double _t, double _u)
{
  x = _x;
  y = _y;
  z = _z;
  c = _c;
  t = _t;
  u = _u;

  // cheap stack
  int top = -1;
  double* stack = new double[maxStackSize];

  double op1, op2;

  for (int i = 0; i < tokenListLength; i++)
    {
      Token token = tokenList[i];

      switch ( token.type )
        {
        case ttNumber:
          stack[++top] = token.number;       // push
          break;
        case ttVariable:
          stack[++top] = *token.variable;    // push
          break;
        case ttUnaryFunction:
          op1 = stack[top--];                // pop
          stack[++top] = token.uf(op1);      // push
          break;
        case ttBinaryFunction:
          op2 = stack[top--];                // pop
          op1 = stack[top--];                // pop
          stack[++top] = token.bf(op1,op2);  // push
          break;
        default:
          break;
        }
    }

  double result = stack[top];
  delete[] stack;
  return result;
}
//===========================================================================

#ifdef PARSER_TEST

int main (int argc, char** argv)
{
  Parser p(argv[1]);
  cout << "expression " << argv[1] << " is ";
  if ( p.isValid )
    cout << "valid";
  else
    cout << "not valid";
  cout << endl;

  if ( p.isValid )
    cout << p.Value(1.0,2.0,3.0) << endl;

  return 0;
}

#endif


