/***************************************************************-*-c++-*-

  $Id$

*************************************************************************/

//---------------------------------------------------------------------------
#ifndef ParseImagesH
#define ParseImagesH
//---------------------------------------------------------------------------

#include <string>

#include "parser.h"
  
// FUNCTION GROUP: Parsing of expressions on images

inline Parser *makeParser(const std::string &expression)
{
  Parser *pParser = new Parser(expression.c_str());
  if (!pParser->isValid) {
    delete pParser;
    pParser = NULL;
  }
  return pParser;
}

inline void destroyParser(Parser *pParser)
{
  delete pParser;
}

//////////
// Parses expression, and evaluates it for each pixel in im. Use an
// 'x' in the expression for the input pixel value.
template <class T> 
void parseSubImage(TSubImg<T> *inIm,
                  TSubImg<T> *outIm, 
                  Parser *pParser,
                  const SubImgBox &box)
{
  //we have a valid parser now

  Vector p;

  for (p.u = box.v1.u; p.u <= box.v2.u; ++p.u) 
    
    for (p.t = box.v1.t; p.t <= box.v2.t; ++p.t)
      
      for (p.c = box.v1.c; p.c <= box.v2.c; ++p.c)
        
        for (p.z = box.v1.z; p.z <= box.v2.z; ++p.z)
          
          for (p.y = box.v1.y; p.y <= box.v2.y; ++p.y) 
            
            {
              p.x = box.v1.x;
              T *in = inIm->getImgPos(p);
              T *out = outIm->getImgPos(p);
              T *end = in + box.v2.x - box.v1.x + 1;

              while ( in != end )
                *out++ = pParser->Value(*in++);
              
            }
}

//////////
// Parses expression, and evaluates it for each pixel in im. Use an
// 'x' in the expression for the input pixel value of the first input
// image, and y for the input pixel value of th second input.
template <class T> 
void parse2SubImages(TSubImg<T> *inIm1, TSubImg<T> *inIm2,
                    TSubImg<T> *outIm, 
                    Parser *pParser,
                    const SubImgBox &box)
{
  //we have a valid parser now
  
  Vector p;
  
  for (p.u = box.v1.u; p.u <= box.v2.u; ++p.u) 
    
    for (p.t = box.v1.t; p.t <= box.v2.t; ++p.t)
      
      for (p.c = box.v1.c; p.c <= box.v2.c; ++p.c)
        
        for (p.z = box.v1.z; p.z <= box.v2.z; ++p.z)
          
          for (p.y = box.v1.y; p.y <= box.v2.y; ++p.y) 
            
            {
              p.x = box.v1.x;
              T *in1 = inIm1->getImgPos(p);
              T *in2 = inIm2->getImgPos(p);
              T *out = outIm->getImgPos(p);
              T *end = in1 + box.v2.x - box.v1.x + 1;

              while ( in1 != end )
                *out++ = pParser->Value(*in1++,*in2++);
              
            }
}


//////////
// Parses expression, and evaluates it for each pixel in im. Use an
// 'x' in the expression for the input pixel value.
template <class T> 
void parseCoordinatesImage( TSubImg<T> *outIm, 
                           Parser *pParser,
                           const SubImgBox &box )
{
  //we have a valid parser now

  int x, y, z, c, t, u;

  for (u = box.v1.u; u <= box.v2.u; ++u) 
    
    for (t = box.v1.t; t <= box.v2.t; ++t)
      
      for (c = box.v1.c; c <= box.v2.c; ++c)
        
        for (z = box.v1.z; z <= box.v2.z; ++z)
          
          for (y = box.v1.y; y <= box.v2.y; ++y) 
            
            {
              x = box.v1.x;
              T *out = outIm->getImgPos(Vector(x,y,z,c,t,u));

              while ( x <= box.v2.x )
                *out++ = pParser->Value(x++,y,z,c,t,u);
            }
}

#endif
