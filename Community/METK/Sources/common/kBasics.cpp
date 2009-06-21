//----------------------------------------------------------------------------------
/*!
// \file    kBasics.cpp
// \author  Konrad Mühler
// \date    2005-07-05
//
//
*/
//----------------------------------------------------------------------------------

#ifndef WIN32
  // Include some ML tools to resolve platform dependencies.
  #include "mlUtils.h"
#endif

//#include "stdafx.h"//muss mit rein und hat was mit vorkompilierten headern zu tun
#include "kBasics.h"


namespace kBasics
{
  //! Splits a string at a delimiter and returns a vector of parts.
  //! \param toSplit[in] source string
  //! \param delimiter[in] delimiter string
  //! \param max_size[in] maximum number of parts (-1 = unlimited)
  //! \param strVector[out] pointer to target vector
  void split(const string toSplit,const char delimiter, const int max_size, vector<string>* strVector)
  {
    string::size_type oldID = 0, newID = 0;
    int split_size=1;
    strVector->clear();
    while((newID = toSplit.find(delimiter, oldID)) != string::npos && (split_size<max_size || max_size==-1))
    {
      strVector->push_back(toSplit.substr(oldID, newID - oldID));
      oldID = newID + 1;
      split_size++;
    }
    strVector->push_back(toSplit.substr(oldID));
  }



  //! Splits a string at a delimiter under respect of Quationmarks (') and returns a vector of parts
  /*! \param toSplit
  *  \param delimiter
  *  \param strVector[out]
  */
  void splitQuotated(const string toSplit, const char delimiter, const char quotationmark, vector<string>* strVector)
  {
    vector<string> splitted;
    split(toSplit,delimiter,1000,&splitted);  //MAX_INT toDo
    for (vector<string>::size_type i=0; i<splitted.size(); i++)
    {
      //if (((string)splitted[i])[0]=='\'')
      if (((string)splitted[i])[0]==quotationmark)
      {
        string tempStr = "";
        tempStr = tempStr + splitted[i];

        if (splitted[i][splitted[i].size()-1]!=quotationmark)
        {
          do
          {
            i++;
            tempStr = tempStr + " " + splitted[i];
          } while ( i<splitted.size() && splitted[i].find(quotationmark, 0)==string::npos );
        }
        strVector->push_back(tempStr);
      }
      else
      {
        strVector->push_back(splitted[i]);
      }
    }
  }



  //! Converts a char-pointer to string, to handle possible null pointers
  //! \param  value
  //! \return const string
  const string CharToString(const char* value)
  {
    if (value!=NULL)
    {
      return string(value);
    }
    return "";
  }

  //! Converts a string into an int
  /*! \param value
  *  \return int
  */
  const int StringToInt(const string value)
  {
    int intVal = atoi(value.c_str());
    return intVal;
  }



  //! Converts an int into a string
  /*! \param value
  *  \return string
  */
  const string IntToString(const int value)
  {
    stringstream sstr;
    sstr << value;
    string strVal = sstr.str();
    return strVal;
  }



  //! Converts a string into a float
  /*! Führte bei mehrfach-Anwendung speziell bei V3-Parametern Crahs-On-Close
  *  \param value
  *  \return float
  */
  float StringToFloat(const string value)
  {
    float floatVal;
    floatVal = atof(value.c_str());
    return floatVal;
  }



  //! Converts a float into a string
  //! \param value
  //! \return string
  const string FloatToString(const float value)
  {
    stringstream sstr;
    sstr << value;
    string strVal = sstr.str();
    return strVal;
  }



  //! Converts a string into a double
  /*! \param value
  *  \return double
  */
  double StringToDouble(const string value)
  {
    double doubleVal;
    doubleVal = atof(value.c_str());
    return doubleVal;
  }



  //! Converts a double into a string
  /*! \param value
  *  \return string
  */
  const string DoubleToString(const double value)
  {
    stringstream sstr;
    sstr << value;
    string strVal = sstr.str();
    return strVal;
  }



  //! Converts a bool into a string
  /*! \param value
  *  \return string
  */
  const string BoolToString(const bool value)
  {
    string strVal;
    if (value) strVal="true"; else strVal="false";
    return strVal;
  }

  const bool StringToBool(const string value)
  {
    return toUp(value)=="TRUE";
  }


  //! Deletes all substrings (toDelete) in a string and returns the result
  /*! \param toDelete
  *  \param orgString
  *  \return string
  */
  const string eraseStrPart(const string toDelete, string orgString)
  {
    if (orgString.find(toDelete,0)!=string::npos)
    {
      orgString.erase(orgString.find(toDelete, 0),toDelete.length());
    }
    return orgString;
  }



  //! Replaces all substrings with a replacement
  /*! \param[in,out] s original string
  *  \param[in] value string to replace
  *  \param[in] replacement
  */
  const string replaceString(string s, const string value, const string replacement)
  {
    unsigned u = 0;
    while ((u=s.find(value, u)) != string::npos)
    {
      s.replace (u, value.size(), replacement);
      u+=replacement.size();
      u--;
    }
    return s;
  }



  //! Replaces all chars with a replacement
  /*! \param[in,out] s original string
  *  \param[in] value char to replace
  *  \param[in] replacement
  */
  const string replaceString(string s, const char value, const string replacement)
  {
    unsigned u = 0;
    while ((u=s.find(value, u)) != string::npos)
    {
      s.replace (u, 1, replacement);
      u+=replacement.size();
      u--;
    }
    return s;
  }



  //! Trim a string by deleting all pre- and postfix spaces
  /*! \param s[in,out] string to trim
  */
  const string trimString(string s, char c)
  {
    while (s[0]==c)
    {
      s=s.substr(1,s.size());
    }

    while (s[s.size()-1]==c)
    {
      s=s.substr(0,s.size()-1);
    }
    return s;
  }



  /*  const string trimStringLFCR(string s)
  {
  while (s[0]=='\n')
  {
  s=s.substr(1,s.size());
  }

  while (s[s.size()-1]==chr(13) || s[s.size()-1]==chr(10))
  {
  s=s.substr(0,s.size()-1);
  }
  return s;
  }*/



  //! Deletes the quotation marks
  /*! \param value[in,out] string to trim */
  const string trimQuotatedStr(string value, const char quotationmark)
  {
    value = replaceString(value, quotationmark, " ");
    value = trimString(value);
    return value;
  }



  //! Extract the path out of a string
  /*! char(92) == '\'
  *  \param value string with path information
  *  \param slash char for dividing
  */
  const string getPath(const string value, const char slash)
  {
    string::size_type index = value.find_last_of(slash); // char(92) == '\'

    return value.substr(0,index);
  }


  //! Extract the filename out of a string
  /*! char(92) == '\'
  *  \param value string with path and filename
  *  \param slash char for dividing
  */
  const string getFilename(const string value, const char slash)
  {
    string::size_type index = value.find_last_of(slash); // char(92) == '\'

    return value.substr(index+1,value.size());
  }



  //! Converts a Vec3 to a string
  /*! \param value
  */
  const string Vec3fToString(SbVec3f value, const char delimiter)
  {
    return DoubleToString(value[0])+delimiter+DoubleToString(value[1])+delimiter+DoubleToString(value[2]);
  }


  const string Vec3fToString(SbVec3f value)
  {
    return Vec3fToString(value, ' ');
  }


  //! Converts a string with space to a Vec3
  /*! \param value
  */
  const SbVec3f StringToVec3f(const string value, const char delimiter)
  {
    vector<string> tempVec;
    if (value!="")
    {
      split(value, delimiter, 3, &tempVec);
      if (tempVec.size()==3)
      {
        SbVec3f myVec3f;
        myVec3f.setValue(StringToFloat(tempVec[0]),StringToFloat(tempVec[1]),StringToFloat(tempVec[2]));
        return myVec3f;
      }
    }
    return 0;
  }


  //! Converts a SbRotation to a string (q0,q1,q2,q3)
  /*! \param value
  */
  const string SbRotationToString(SbRotation value)
  {
    //float q0,q1,q2,q3;
    //value.getValue(q0,q1,q2,q3);

    //changed on 2006-12-15

    SbVec3f axis;
    float x,y,z,radians;

    value.getValue(axis,radians);
    axis.getValue(x,y,z);
    return DoubleToString(x)+" "+DoubleToString(y)+" "+DoubleToString(z)+" "+DoubleToString(radians);
  }


  //! Converts an string to its UPPERCASE
  /*! \param value
  *  \return string
  */
  const string toUp(string value)
  {
    //change each element of the string to upper case
    for(unsigned int i=0;i<value.length();i++)
    {
      value[i] = toupper(value[i]);
    }
    return value;//return the converted string
  }


  const double maxDiff(const SbVec3f value1, const SbVec3f value2)
  {
    double diff1 = fabs(double(value1[0])-double(value2[0]));
    double diff2 = fabs(double(value1[1])-double(value2[1]));
    double diff3 = fabs(double(value1[2])-double(value2[2]));

    return max(diff1,max(diff2,diff3));
  }

  const double maxDiff(const SbVec4f value1, const SbVec4f value2)
  {
    double diff1 = fabs(double(value1[0])-double(value2[0]));
    double diff2 = fabs(double(value1[1])-double(value2[1]));
    double diff3 = fabs(double(value1[2])-double(value2[2]));
    double diff4 = fabs(double(value1[3])-double(value2[3]));

    return max(max(diff1,diff2),max(diff3,diff4));
  }

  const bool fileExists(const char* fileName)
  {
    ifstream theFile(fileName);
    if (theFile) return true;
    return false;
  }


  const string leadingZero(int number, int totalLength)
  {
    string s = IntToString(number);
    for (int i=s.length(); i<totalLength; i++)
    {
      s = "0" + s;
    }

    return s;
  }







  // Theoretically, hue 0 (pure red) is identical to hue 6 in these transforms. Pure
  // red always maps to 6 in this implementation. Therefore UNDEFINED can be
  // defined as 0 in situations where only unsigned numbers are desired.
  // http://jgt.akpeters.com/papers/SmithLyons96/hsv_rgb.html
#define RETURN_HSV(h, s, v) {HSV[0] = h; HSV[1] = s; HSV[2] = v; return HSV;}
#define RETURN_RGB(r, g, b) {RGB[0] = r; RGB[1] = g; RGB[2] = b; return RGB;}
#define UNDEFINED 0

  /*const SbVec3f RGB_to_HSV( SbVec3f RGB )
  {

  // RGB are each on [0, 1]. S and V are returned on [0, 1] and H is
  // returned on [0, 6]. Exception: H is returned UNDEFINED if S==0.
  float R = RGB[0], G = RGB[1], B = RGB[2], v, x, f;
  int i;
  SbVec3f HSV;

  x = min(R, max(G, B));
  v = max(R, max(G, B));
  if(v == x)
  {
  //RETURN_HSV(UNDEFINED, 0, v);
  HSV[0] = 0;
  HSV[1] = 0;
  HSV[2] = v;
  return HSV;
  }
  f = (R == x) ? G - B : ((G == x) ? B - R : R - G);
  i = (R == x) ? 3 : ((G == x) ? 5 : 1);
  //RETURN_HSV(i - f /(v - x), (v - x)/v, v);
  HSV[0] = i - f /(v - x);
  HSV[1] = (v - x)/v;
  HSV[2] = v;
  return HSV;
  }


  SbVec3f HSV_to_RGB( SbVec3f HSV )
  {

  // H is given on [0, 6] or UNDEFINED. S and V are given on [0, 1].
  // RGB are each returned on [0, 1].
  float h = HSV[0], s = HSV[1], v = HSV[2], m, n, f;
  int i;
  SbVec3f RGB;

  if(h == UNDEFINED)
  {
  RETURN_RGB(v, v, v);
  }
  i = floor(h);
  f = h - i;
  if(!(i & 1)) f = 1 - f; // if i is even
  m = v * (1 - s);
  n = v * (1 - s * f);
  switch (i)
  {
  case 6:
  case 0: RETURN_RGB(v, n, m);
  case 1: RETURN_RGB(n, v, m);
  case 2: RETURN_RGB(m, v, n)
  case 3: RETURN_RGB(m, n, v);
  case 4: RETURN_RGB(n, m, v);
  case 5: RETURN_RGB(v, m, n);
  }
  RETURN_RGB(v, v, v);
  } */







  // http://www.cs.rit.edu/~ncs/color/t_convert.html

  // r,g,b values are from 0 to 1
  // h = [0,360], s = [0,1], v = [0,1]
  //    if s == 0, then h = -1 (undefined)
  const SbVec3f RGB_to_HSV( SbVec3f RGB )
  {
    float min_f, max_f, delta, h , s , v;
    SbVec3f HSV;
    min_f = min( RGB[0], min(RGB[1], RGB[2] ) );
    max_f = max( RGB[0], max(RGB[1], RGB[2] ) );
    v = max_f;        // v
    delta = max_f - min_f;
    //if( max != 0 )
    if( delta != 0 )
      s = delta / max_f;    // s
    else {
      // r = g = b = 0    // s = 0, v is undefined
      s = 0;
      //*h = -1;
      h = 0;
      //return;
      RETURN_HSV(h,s,v);
    }
    if( RGB[0] == max_f )
      h = ( RGB[1] - RGB[2] ) / delta;    // between yellow & magenta
    else if( RGB[1] == max_f )
      h = 2 + ( RGB[2] - RGB[0] ) / delta;  // between cyan & yellow
    else
      h = 4 + ( RGB[0] - RGB[1] ) / delta;  // between magenta & cyan
    h *= 60;        // degrees
    if( h < 0 )
      h += 360;

    RETURN_HSV(h,s,v);
  }


  SbVec3f HSV_to_RGB( SbVec3f HSV )
  {
    SbVec3f RGB;
    float r,g,b;
    int i;
    float f, p, q, t;
    if( HSV[1] == 0 ) {
      // achromatic (grey)
      r = g = b = HSV[2];
      RETURN_RGB(r,g,b);
    }
    HSV[0] /= 60;     // sector 0 to 5
    i = floor( HSV[0] );
    f = HSV[0] - i;     // factorial part of h
    p = HSV[2] * ( 1 - HSV[1] );
    q = HSV[2] * ( 1 - HSV[1] * f );
    t = HSV[2] * ( 1 - HSV[1] * ( 1 - f ) );
    switch( i ) {
      case 0:
        r = HSV[2];
        g = t;
        b = p;
        break;
      case 1:
        r = q;
        g = HSV[2];
        b = p;
        break;
      case 2:
        r = p;
        g = HSV[2];
        b = t;
        break;
      case 3:
        r = p;
        g = q;
        b = HSV[2];
        break;
      case 4:
        r = t;
        g = p;
        b = HSV[2];
        break;
      default:    // case 5:
        r = HSV[2];
        g = p;
        b = q;
        break;
    }

    RETURN_RGB(r,g,b);
  }


  template <typename T>
  const int Signum(T value)
  {
    return (value > 0) - (value < 0);
  }



  const int round(float value)
  {
    return floor(value +0.5);
  }


  void DebugTest(string debugStr, short /*level*/)
  {
    std::cout << debugStr << std::endl;
  }


  void initRand()
  {
    time_t sek;
    time(&sek);
    srand((unsigned)sek);
  }

  /* get integer random number in range a <= x <= e */
  int random( int a, int e)
  {
    double r = e - a + 1;
    return a + (int)(r * rand()/(RAND_MAX+1.0));
  }


  //QUICK-SORT Implementierung nack Wiki

  void qs_swap(double* values, int a, int b)
  {
    double temp = values[a];
    values[a] = values[b];
    values[b] = temp;
  }

  int qs_divide(double* values, int start, int end)
  {
    int index = start;
    for (int i=start; i<end; i++)
    {
      if (values[i]<=values[end])
      {
        qs_swap(values, index, i);
        index++;
      }
    }
    qs_swap(values, index,end);
    return index;
  }

  void quickSort(double* values, int start, int end)
  {
    int pivot;
    if (end>start)
    {
      pivot = qs_divide(values, start, end);
      quickSort(values, start, pivot-1);
      quickSort(values, pivot+1, end);
    }
  }


  /** Ermittelt die aktuelle Zeit in ms(?)
  \return double
  */
#ifdef WIN32
  double getCurrentTime()
  {
    long long Frequency=0;
    QueryPerformanceFrequency( (LARGE_INTEGER*) &Frequency);
    const double TimeScale = 1.0/Frequency;
    long long LastTime=0;
    QueryPerformanceCounter( (LARGE_INTEGER*) &LastTime);
    return LastTime * TimeScale;
  }
#else
  double getCurrentTime()
  {
    ML_NAMESPACE::TimeCounter tc;
    return tc.getCurrentValueInSeconds();
  }
#endif

  char getOptions(vector<string>& in,const list<optionstruct>& options,string& value){
    if(in.size()>1){
      if(in[0].size()==2 && in[0][0]=='-'){
        list<optionstruct>::const_iterator it=options.begin();
        while(it!=options.end()){
          if(it->shortname==in[0][1]){
            in.erase(in.begin());
            value="";
            while(in.size()>0 && in[0].size()>0 && in[0][0]!='-'){
              value+=" "+in[0];
              in.erase(in.begin());
            }
            value=trimString(value);
            return it->shortname;
          }
          it++;
        }
        in.erase(in.begin());
        while(in.size()>0 && in[0].size()>0 && in[0][0]!='-'){
          in.erase(in.begin());
        }
        return 0;
      }else if(in[0].size()>2 && in[0][0]=='-' && in[0][1]=='-'){
        list<optionstruct>::const_iterator it=options.begin();
        while(it!=options.end()){
          if(it->name==in[0].substr(2)){
            in.erase(in.begin());
            value="";
            while(in.size()>0 && in[0].size()>0 && in[0][0]!='-'){
              value+=" "+in[0];
              in.erase(in.begin());
            }
            value=trimString(value);
            return it->shortname;
          }
          it++;
        }
        in.erase(in.begin());
        while(in.size()>0 && in[0].size()>0 && in[0][0]!='-'){
          in.erase(in.begin());
        }
        return 0;
      }
    }
    return -1;
  }


  bool lineIntersection(POINT p1, POINT p2, POINT p3, POINT p4, POINT &intersectionPoint)
  {
    float denominator;
    float numerator1, numerator2;

    //If the intersection of line segments is required then it is only necessary to test if ua and ub lie between 0 and 1.
    //Whichever one lies within that range then the corresponding line segment contains the intersection point.
    //-->If both lie within the range of 0 to 1 then the intersection point is within both line segments.

    denominator = (p4.y-p3.y)*(p2.x-p1.x)-(p4.x-p3.x)*(p2.y-p1.y);
    numerator1 = (p4.x-p3.x)*(p1.y-p3.y)-(p4.y-p3.y)*(p1.x-p3.x);
    numerator2 = (p2.x-p1.x)*(p1.y-p3.y)-(p2.y-p1.y)*(p1.x-p3.x);

    if (denominator!=0)
    {
      float ua,ub;
      ua = numerator1/denominator;
      ub = numerator2/denominator;
      if (ua>0 && ua<1 && ub>0 && ub<1)
      {
        //Intersection Point
        intersectionPoint.x = p1.x + ua*(p2.x-p1.x);
        intersectionPoint.y = p1.y + ua*(p2.y-p1.y);
        return true;
      }
      else return false;
    }
    else return false;

    //return false;
  }



  bool boxIntersection(POINT r1_corner1, POINT r1_corner2, POINT r2_corner1, POINT r2_corner2, BOX &intersectionBox)
  {
    bool intersect;
    //Check for real intersection
    if ( r2_corner1.x>=r1_corner2.x
      || r2_corner2.x<=r1_corner1.x
      || r2_corner1.y>=r1_corner2.y
      || r2_corner2.y<=r1_corner1.y)
    {
      intersect = false;
    }
    else
    {
      intersect = true;
      //Calc intersection box
      POINT intersectionPoint;
      bool lineintersect;

      //Corner 1 and 4 for the left side of the box
      if (r1_corner1.x>=r2_corner1.x)
      {
        lineintersect=lineIntersection(r1_corner1,POINT(r1_corner1.x,r1_corner2.y),
          r2_corner1,POINT(r2_corner2.x,r2_corner1.y),intersectionPoint);
        if (lineintersect)
        {
          intersectionBox.corner1=intersectionPoint;
        }
        else
        {
          intersectionBox.corner1.x=r1_corner1.x;
          intersectionBox.corner1.y=r1_corner1.y;
        }

        lineintersect=lineIntersection(r1_corner1,POINT(r1_corner1.x,r1_corner2.y),
          POINT(r2_corner1.x,r2_corner2.y),r2_corner2,intersectionPoint);
        if (lineintersect)
        {
          intersectionBox.corner4=intersectionPoint;
        }
        else
        {
          intersectionBox.corner4.x=r1_corner1.x;
          intersectionBox.corner4.y=r1_corner2.y;
        }
      }
      else
      {
        lineintersect=lineIntersection(r2_corner1,POINT(r2_corner1.x,r2_corner2.y),
          r1_corner1,POINT(r1_corner2.x,r1_corner1.y),intersectionPoint);
        if (lineintersect)
        {
          intersectionBox.corner1=intersectionPoint;
        }
        else
        {
          intersectionBox.corner1.x=r2_corner1.x;
          intersectionBox.corner1.y=r2_corner1.y;
        }

        lineintersect=lineIntersection(r2_corner1,POINT(r2_corner1.x,r2_corner2.y),
          POINT(r1_corner1.x,r1_corner2.y),r1_corner2,intersectionPoint);
        if (lineintersect)
        {
          intersectionBox.corner4=intersectionPoint;
        }
        else
        {
          intersectionBox.corner4.x=r2_corner1.x;
          intersectionBox.corner4.y=r2_corner2.y;
        }
      }

      //Corner 2 and 3 for the right side of the box
      if (r1_corner2.x<=r2_corner2.x)
      {
        lineintersect=lineIntersection(POINT(r1_corner2.x,r1_corner1.y),r1_corner2,
          r2_corner1,POINT(r2_corner2.x,r2_corner1.y),intersectionPoint);
        if (lineintersect)
        {
          intersectionBox.corner2=intersectionPoint;
        }
        else
        {
          intersectionBox.corner2.x=r1_corner2.x;
          intersectionBox.corner2.y=r1_corner1.y;
        }

        lineintersect=lineIntersection(POINT(r1_corner2.x,r1_corner1.y),r1_corner2,
          POINT(r2_corner1.x,r2_corner2.y),r2_corner2,intersectionPoint);
        if (lineintersect)
        {
          intersectionBox.corner3=intersectionPoint;
        }
        else
        {
          intersectionBox.corner3.x=r1_corner2.x;
          intersectionBox.corner3.y=r1_corner2.y;
        }
      }
      else
      {
        lineintersect=lineIntersection(POINT(r2_corner2.x,r2_corner1.y),r2_corner2,
          r1_corner1,POINT(r1_corner2.x,r1_corner1.y),intersectionPoint);
        if (lineintersect)
        {
          intersectionBox.corner2=intersectionPoint;
        }
        else
        {
          intersectionBox.corner2.x=r2_corner2.x;
          intersectionBox.corner2.y=r2_corner1.y;
        }

        lineintersect=lineIntersection(POINT(r2_corner2.x,r2_corner1.y),r2_corner2,
          POINT(r1_corner1.x,r1_corner2.y),r1_corner2,intersectionPoint);
        if (lineintersect)
        {
          intersectionBox.corner3=intersectionPoint;
        }
        else
        {
          intersectionBox.corner3.x=r2_corner2.x;
          intersectionBox.corner3.y=r2_corner2.y;
        }
      }
    }

    //Check for inclusion
    if (!intersect)
    {
      if (r1_corner1.x>=r2_corner1.x
        && r1_corner1.y>=r2_corner1.y
        && r1_corner2.x<=r2_corner2.x
        && r1_corner2.y<=r2_corner2.y)
      {
        intersect=true;
        intersectionBox.corner1.x = r1_corner1.x;
        intersectionBox.corner1.y = r1_corner1.y;
        intersectionBox.corner2.x = r1_corner2.x;
        intersectionBox.corner2.y = r1_corner1.y;
        intersectionBox.corner3.x = r1_corner2.x;
        intersectionBox.corner3.y = r1_corner2.y;
        intersectionBox.corner4.x = r1_corner1.x;
        intersectionBox.corner4.y = r1_corner2.y;
      }
      else if (r2_corner1.x>=r1_corner1.x
        && r2_corner1.y>=r1_corner1.y
        && r2_corner2.x<=r1_corner2.x
        && r2_corner2.y<=r1_corner2.y)
      {
        intersect=true;
        intersectionBox.corner1.x = r2_corner1.x;
        intersectionBox.corner1.y = r2_corner1.y;
        intersectionBox.corner2.x = r2_corner2.x;
        intersectionBox.corner2.y = r2_corner1.y;
        intersectionBox.corner3.x = r2_corner2.x;
        intersectionBox.corner3.y = r2_corner2.y;
        intersectionBox.corner4.x = r2_corner1.x;
        intersectionBox.corner4.y = r2_corner2.y;
      }
    }

    return intersect;
  }


  bool pointInBox(POINTF p1, POINT corner1, POINT corner2)
  {
    bool isInBox;
    isInBox=false;

    if (p1.x>=corner1.x && p1.x<=corner2.x &&
      p1.y>=corner1.y && p1.y<=corner2.y)
    {
      isInBox=true;
    }

    return isInBox;
  }


  long hex2int(const string& hexStr)
  {
    char *offset;
    if (hexStr.length( ) > 2)
    {
      if (hexStr[0] == '0' && hexStr[1] == 'x')
      {
        return strtol(hexStr.c_str( ), &offset, 0);
      }
      else
      {
        std::cout << "hex must start with 0x";
        return 0;
      }
    }
    return 0;
  }


} //namespace kBasics
