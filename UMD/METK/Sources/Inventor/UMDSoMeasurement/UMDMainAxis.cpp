// #########################################
// #####
// #####  UMDMainAxis.cpp
// #####
// #########################################

#include "UMDMainAxis.h"
#include <stdlib.h>
#ifndef __APPLE__
#include <malloc.h>
#endif

#define JACOBI_ROTATE(a,i,j,k,l) g=a[i][j];\
  h=a[k][l];\
  a[i][j]=g-s*(h+g*tau);\
a[k][l]=h+s*(g-h*tau);

#define NR_END 1
#define FREE_ARG char*


//--------------------------------------------------------------------------
// Numerical Recipes standard error handler
//--------------------------------------------------------------------------
void nrerror(char error_text[])
{
  fprintf(stderr,"Numerical Recipes run-time error...\n");
  fprintf(stderr,"%s\n",error_text);
  fprintf(stderr,"...now exiting to system...\n");
  exit(1);
}


//--------------------------------------------------------------------------
// Allocate a variable lenght float vector with subscript range v[nl..nh]
//--------------------------------------------------------------------------
float *vL_vector(long nl, long nh)

{
  // malloc -> Anfordern von Speicherplatz;
  // Rückgabewert ist Anfangsadresse des neu angelegten Speicherblocks
  float *v = (float *) malloc((size_t)((nh-nl+1+NR_END)*sizeof(float)));

  if (!v) nrerror("allocation failure in vector()");

  return v+NR_END-nl;
}


//--------------------------------------------------------------------------
// Free a float vector allocated with vector()
//--------------------------------------------------------------------------
void free_vector(float *v, long nl, long /*nh*/)
{
  free((FREE_ARG) (v+nl-NR_END));
}


//--------------------------------------------------------------------------
// Allocate a float matrix with subscript range m[nrl..nrh][ncl..nch]
//--------------------------------------------------------------------------
float **matrix(long nrl, long nrh, long ncl, long nch)

{
  // r -> row: Zeile
  // c -> column: Spalte

  long i;
  long nrow, ncol;
  float **m;

  nrow = nrh-nrl+1;
  ncol = nch-ncl+1;

  // Allocate pointers to rows
  m=(float **) malloc((size_t)((nrow+NR_END)*sizeof(float*)));
  if (!m) nrerror("allocation failure 1 in matrix()");
  m += NR_END;
  m -= nrl;

  // Allocate rows and set pointers to them
  m[nrl]=(float *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(float)));
  if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

  // Return pointer to array of pointers to rows.
  return m;
}



//--------------------------------------------------------------------------
// Free a float matrix allocated by matrix()
//--------------------------------------------------------------------------
void free_matrix(float **m, long nrl, long /*nrh*/, long ncl, long /*nch*/)
{
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}



//--------------------------------------------------------------------------
//  Compute the jacobi matrix.
//--------------------------------------------------------------------------
void jacobi(float **a, int n, float d[], float **v, int *nrot)

{
  int j,iq,ip,i;
  float tresh,theta,tau,t,sm,s,h,g,c,*b,*z;

  b = vL_vector(1,n);
  z = vL_vector(1,n);

  // Make v be a unit Matrix.
  for (ip=1;ip<=n;ip++)
  {
    for (iq=1;iq<=n;iq++)
      v[ip][iq]=0.0;

    v[ip][ip]=1.0;
  }

  for (ip=1;ip<=n;ip++)
  {
    b[ip]=d[ip]=a[ip][ip];
    z[ip]=0.0;
  }

  *nrot=0;

  for (i=1;i<=50;i++)
  {
    sm=0.0;
    for (ip=1;ip<=n-1;ip++)
    {
      for (iq=ip+1;iq<=n;iq++)
        sm += fabs(a[ip][iq]);
    }

    if (sm == 0.0)
    {
      free_vector(z,1,n);
      free_vector(b,1,n);
      return;
    }

    if (i < 4)
      tresh=0.2*sm/(n*n);
    else
      tresh=0.0;

    for (ip=1;ip<=n-1;ip++)
    {
      for (iq=ip+1;iq<=n;iq++)
      {
        g=100.0*fabs(a[ip][iq]);

        if (i > 4 && (float)(fabs(d[ip])+g) == (float)fabs(d[ip])
          && (float)(fabs(d[iq])+g) == (float)fabs(d[iq]))
          a[ip][iq]=0.0;

        else if (fabs(a[ip][iq]) > tresh)
        {
          h=d[iq]-d[ip];
          if ((float)(fabs(h)+g) == (float)fabs(h))
            t=(a[ip][iq])/h;

          else
          {
            theta=0.5*h/(a[ip][iq]);
            t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
            if (theta < 0.0) t = -t;
          }

          c=1.0/sqrt(1+t*t);
          s=t*c;
          tau=s/(1.0+c);
          h=t*a[ip][iq];
          z[ip] -= h;
          z[iq] += h;
          d[ip] -= h;
          d[iq] += h;
          a[ip][iq]=0.0;

          for (j=1;j<=ip-1;j++)
          {
            JACOBI_ROTATE(a,j,ip,j,iq)
          }

          for (j=ip+1;j<=iq-1;j++)
          {
            JACOBI_ROTATE(a,ip,j,j,iq)
          }

          for (j=iq+1;j<=n;j++)
          {
            JACOBI_ROTATE(a,ip,j,iq,j)
          }

          for (j=1;j<=n;j++)
          {
            JACOBI_ROTATE(v,j,ip,j,iq)
          }

          ++(*nrot);
        }
      }
    }

    for (ip=1;ip<=n;ip++)
    {
      b[ip] += z[ip];
      d[ip]=b[ip];
      z[ip]=0.0;
    }
  }
  nrerror("Too many iterations in routine jacobi");
} //jacobi

#undef JACOBI_ROTATE



UMDMainAxis::UMDMainAxis() {
  _baryCenter = NULL;
  _xDiameter = _yDiameter = _zDiameter = 0;

  _midPoint = new float[3];
  _xAxis = new float[3];
  _yAxis = new float[3];
  _zAxis = new float[3];
}


UMDMainAxis::~UMDMainAxis() {
  if (_xAxis) { delete[] _xAxis; _xAxis = NULL; }
  if (_yAxis) { delete[] _yAxis; _yAxis = NULL; }
  if (_zAxis) { delete[] _zAxis; _zAxis = NULL; }
  if (_baryCenter) { delete[] _baryCenter; _baryCenter = NULL; }
  if (_midPoint) { delete[] _midPoint; _midPoint = NULL; }
}


void UMDMainAxis::getInverseMatrix(float **aMatrix, float **invMatrix) {
  // Die inverse Matrix berechnen (nur fuer 3x3 Matrizen)
  // zunaechst die Determinante D bestimmen und anschliessend 1/D

  float det = aMatrix[1][1] * aMatrix[2][2] * aMatrix[3][3]
    + aMatrix[1][2] * aMatrix[2][3] * aMatrix[3][1]
    + aMatrix[1][3] * aMatrix[2][1] * aMatrix[3][2]
    - aMatrix[1][3] * aMatrix[2][2] * aMatrix[3][1]
    - aMatrix[1][1] * aMatrix[2][3] * aMatrix[3][2]
    - aMatrix[1][2] * aMatrix[2][1] * aMatrix[3][3];

  if (det != 0) det = 1.0/det;

  // nun die Adjunkte Matrix bilden, diese transponieren und
  // anschliessend mal D

  invMatrix[1][1] =  det * (aMatrix[2][2] * aMatrix[3][3] - aMatrix[2][3] * aMatrix[3][2]);
  invMatrix[2][1] = -det * (aMatrix[2][1] * aMatrix[3][3] - aMatrix[2][3] * aMatrix[3][1]);
  invMatrix[3][1] =  det * (aMatrix[2][1] * aMatrix[3][2] - aMatrix[2][2] * aMatrix[3][1]);
  invMatrix[1][2] = -det * (aMatrix[1][2] * aMatrix[3][3] - aMatrix[1][3] * aMatrix[3][2]);
  invMatrix[2][2] =  det * (aMatrix[1][1] * aMatrix[3][3] - aMatrix[1][3] * aMatrix[3][1]);
  invMatrix[3][2] = -det * (aMatrix[1][1] * aMatrix[3][2] - aMatrix[1][2] * aMatrix[3][1]);
  invMatrix[1][3] =  det * (aMatrix[1][2] * aMatrix[2][3] - aMatrix[1][3] * aMatrix[2][2]);
  invMatrix[2][3] = -det * (aMatrix[1][1] * aMatrix[2][3] - aMatrix[1][3] * aMatrix[2][1]);
  invMatrix[3][3] =  det * (aMatrix[1][1] * aMatrix[2][2] - aMatrix[1][2] * aMatrix[2][1]);
} //getInverseMatrix


float* UMDMainAxis::calcBaryCenter(const float* vertices, int size) {
  // Berechnet aus einer Menge von Punkten den Schwerpunkt
  // und gibt diesen zurueck
  if (size != 0) {
    float* meanVec = new float[3];
    meanVec[0] = 0;
    meanVec[1] = 0;
    meanVec[2] = 0;

    for (int counter = 0; counter < size; counter++) {
      meanVec[0] += *vertices++;
      meanVec[1] += *vertices++;
      meanVec[2] += *vertices++;
    }
    // Division durch Anzahl der Punkte
    meanVec[0] /= size;
    meanVec[1] /= size;
    meanVec[2] /= size;

    return meanVec;
  }
  else return 0;
} // calcMeanVec


float UMDMainAxis::dotProduct(const float* vec1, const float* vec2) {
  return
    (* vec1     ) * (* vec2     ) +
    (*(vec1 + 1)) * (*(vec2 + 1)) +
    (*(vec1 + 2)) * (*(vec2 + 2));
}


void UMDMainAxis::getBoundingBox(const float* pointSet, const long size, float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ) {
  // return the bounding box of a pointset
  if (pointSet != NULL && size > 0) {
    const float* pointer = pointSet;
    minX = maxX = *(pointer++);
    minY = maxY = *(pointer++);
    minZ = maxZ = *(pointer++);
    for (long counter = 1; counter < size; counter++) {
      if (minX > *pointer) minX = *pointer;
      if (maxX < *pointer) maxX = *pointer;
      pointer++;
      if (minY > *pointer) minY = *pointer;
      if (maxY < *pointer) maxY = *pointer;
      pointer++;
      if (minZ > *pointer) minZ = *pointer;
      if (maxZ < *pointer) maxZ = *pointer;
      pointer++;
    } // for
  } // if
}


void UMDMainAxis::getCovarianceMatrix(const float* vertices, const long size, float **covaMatrix, const float* meanVec) {
  // Berechnung der Kovarianz-Matrix aus einer Punktmenge;
  // uebergeben wird die Punktmenge sowie eine Matrix, in der
  // am Ende die Kovarianz-Matrix steht.

  // den Schwerpunkt von den Punkten abziehen
  float* difPoints = new float[size * 3];
  int i,j;
  int counter, counter2 = 0;
  for (counter = 0; counter < size; counter++) {
    difPoints[counter2] = vertices[counter2] - meanVec[0];
    counter2++;
    difPoints[counter2] = vertices[counter2] - meanVec[1];
    counter2++;
    difPoints[counter2] = vertices[counter2] - meanVec[2];
    counter2++;
  }

  // Erstellen der Kovarianz-Matrix (ist symmetrisch -> doppelte
  // Werte nur einmal berechnen)

  // Loeschen der Kovarianz-Matrix.
  covaMatrix[1][1] = 0; covaMatrix[1][2] = 0; covaMatrix[1][3] = 0;
  covaMatrix[2][1] = 0; covaMatrix[2][2] = 0; covaMatrix[2][3] = 0;
  covaMatrix[3][1] = 0; covaMatrix[3][2] = 0; covaMatrix[3][3] = 0;

  for (counter = 0 ; counter < size; counter++) {
    covaMatrix[1][1] += difPoints[counter * 3    ] * difPoints[counter * 3    ];
    covaMatrix[1][2] += difPoints[counter * 3    ] * difPoints[counter * 3 + 1];
    covaMatrix[1][3] += difPoints[counter * 3    ] * difPoints[counter * 3 + 2];

    covaMatrix[2][2] += difPoints[counter * 3 + 1] * difPoints[counter * 3 + 1];
    covaMatrix[2][3] += difPoints[counter * 3 + 1] * difPoints[counter * 3 + 2];

    covaMatrix[3][3] += difPoints[counter * 3 + 2] * difPoints[counter * 3 + 2];
  }

  delete[] difPoints;

  // doppelte Werte kopieren
  covaMatrix[2][1] = covaMatrix[1][2];
  covaMatrix[3][1] = covaMatrix[1][3];
  covaMatrix[3][2] = covaMatrix[2][3];

  // alle Matrix-Elemente durch Anzahl der Punkte teilen
  for (j = 1; j < 4; j++)
    for (i = 1; i < 4; i++)
      covaMatrix[j][i] /= size;

} //getCovarianceMatrix


void UMDMainAxis::computeMainAxis(const float* vertices, long size) {

  float** covaMatrix = matrix(1,3,1,3);
  float** jacobiMat  = matrix(1,3,1,3);
  float** invMatrix  = matrix(1,3,1,3);
  float*  eigenValues = vL_vector(1,3);

  // Schwerpunkt des Objektes bestimmen
  if (_baryCenter) { delete[] _baryCenter; _baryCenter = NULL; }
  _baryCenter = calcBaryCenter(vertices, size);

  // Berechnen der Kovarianz-Matrix, die dann der Jacobi-Matrix-
  // Berechnung uebergeben wird
  getCovarianceMatrix(vertices, size, covaMatrix, _baryCenter);

  // Jacobi-Matrix berechnen
  int nrot; // dummy variable
  jacobi(covaMatrix, 3, eigenValues, jacobiMat, &nrot);

  // Hauptachsen bestimmen
  int counter;
  for (counter = 0; counter < 3; counter++) {
    _xAxis[counter] = jacobiMat[counter + 1][1];
    _yAxis[counter] = jacobiMat[counter + 1][2];
    _zAxis[counter] = jacobiMat[counter + 1][3];
  }

  // Punkte des Objektes mit den Eigenvektoren der
  // Jacobi-Matrix multiplizieren (virtuelle Rotation des
  // neuen Hauptachsen-Koordinatensystems in Weltkoordinaten)
  const float* points = vertices;
  float* newVertices = new float[size * 3];
  int counter2 = 0;
  for (counter = 0; counter < size; counter++) {

    newVertices[counter2++] = dotProduct(points, _xAxis); //tempPoint.dot(_xAxis);
    newVertices[counter2++] = dotProduct(points, _yAxis); //tempPoint.dot(_yAxis);
    newVertices[counter2++] = dotProduct(points, _zAxis); //tempPoint.dot(_zAxis);
    points += 3;
  }

  // Ausmaße der objekt-orientierten Bounding Box besorgen
  float minX, maxX, minY, maxY, minZ, maxZ;
  getBoundingBox(newVertices, size, minX, maxX, minY, maxY, minZ, maxZ);

  // Durchmesser der objekt-orientierten Bounding Box
  _xDiameter = maxX - minX;
  _yDiameter = maxY - minY;
  _zDiameter = maxZ - minZ;

  // Die Haelfte der Ausdehnung der Bounding-Box in x-,y- und z-Richtung
  float half_x = _xDiameter / 2.;
  float half_y = _yDiameter / 2.;
  float half_z = _zDiameter / 2.;

  // Um das vorruebergehend "rotierte" Objekt wieder in seine
  // Ausgangslage zu bringen, ist die inverse Jacobi-Matrix
  // von noeten.
  getInverseMatrix(jacobiMat, invMatrix);

  // Anschliessend brauchen aber nur die mich interessierenden
  // Punkte wieder zuruecktransformiert werden
  float* tmp = stretchVector(_xAxis, half_x);
  for (counter = 0; counter < 3; counter++) {
    _midPoint[counter] =
       minX           * invMatrix[1][counter + 1] +
      (minY + half_y) * invMatrix[2][counter + 1] +
      (minZ + half_z) * invMatrix[3][counter + 1] + tmp[counter];
  }

  // Ordnung muss sein
  delete[] tmp;
  delete[] newVertices;
  free_matrix(covaMatrix, 1, 3, 1, 3);
  free_matrix(jacobiMat, 1, 3, 1, 3);
  free_matrix(invMatrix, 1, 3, 1, 3);
  free(eigenValues);
} //computeMainAxis


void UMDMainAxis::getMainAxis(float*& xAxis, float*& yAxis, float*& zAxis) {
  xAxis = new float[3];
  yAxis = new float[3];
  zAxis = new float[3];
  for (int counter = 0; counter < 3; counter++) {
    xAxis[counter] = _xAxis[counter];
    yAxis[counter] = _yAxis[counter];
    zAxis[counter] = _zAxis[counter];
  }
}


void UMDMainAxis::getBaryCenter(float* x, float* y, float* z) {
/*  float* baryCenter = new float[3];
  baryCenter[0] = _baryCenter[0];
  baryCenter[1] = _baryCenter[1];
  baryCenter[2] = _baryCenter[2];
  return baryCenter;
*/
  *x = _baryCenter[0];
  *y = _baryCenter[1];
  *z = _baryCenter[2];
}


void UMDMainAxis::getMidPoint(float* x, float* y, float* z) {
/*  float* midPoint = new float[3];
  midPoint[0] = _midPoint[0];
  midPoint[1] = _midPoint[1];
  midPoint[2] = _midPoint[2];
  return midPoint;
*/
  *x = _midPoint[0];
  *y = _midPoint[1];
  *z = _midPoint[2];
}


void UMDMainAxis::getExtension(float& xExt, float& yExt, float& zExt) {
  xExt = _xDiameter;
  yExt = _yDiameter;
  zExt = _zDiameter;
}


float* UMDMainAxis::getLargestMainAxis() {
  float* lA = new float[3];
  if (_xDiameter > _yDiameter && _xDiameter > _zDiameter) {
    lA[0] = _xAxis[0];
    lA[1] = _xAxis[1];
    lA[2] = _xAxis[2];
    return lA;
  }
  if (_yDiameter > _xDiameter && _yDiameter > _zDiameter) {
    lA[0] = _yAxis[0];
    lA[1] = _yAxis[1];
    lA[2] = _yAxis[2];
    return lA;
  }
  lA[0] = _zAxis[0];
  lA[1] = _zAxis[1];
  lA[2] = _zAxis[2];
  return lA;
}


float UMDMainAxis::getLargestExtension() {
  if (_xDiameter > _yDiameter && _xDiameter > _zDiameter)
    return _xDiameter;
  if (_yDiameter > _xDiameter && _yDiameter > _zDiameter)
    return _yDiameter;
  return _zDiameter;
}


float* UMDMainAxis::stretchVector(const float* vector, const float length) {
  float* strVec = new float[3];

  // Summe der Quadrate des Richtungsvektors
  float sumVec = vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];

  // Falls diese Summe Null ist -> Abbruch
  if (sumVec == 0) {
    strVec[0] = strVec[1] = strVec[2] = 0;
    return strVec;
  }

  // Berechnung des Endpunktes durch Umstellung der Formel
  // |aVector| = sqrt(aVector[0]^2 + aVector[1]^2 + aVector[2]^2):
  float factor = sqrtf((length * length) / sumVec);

  strVec[0] = vector[0] * factor;
  strVec[1] = vector[1] * factor;
  strVec[2] = vector[2] * factor;
  return strVec;
}
