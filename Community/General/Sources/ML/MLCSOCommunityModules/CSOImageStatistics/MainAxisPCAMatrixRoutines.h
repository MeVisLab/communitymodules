



/////////////////////////////////////
/// TAKEN FROM NUMERICAL RECIPIES ///
/////////////////////////////////////

#ifndef __MainAxisPCAMatrixRoutines_H
#define __MainAxisPCAMatrixRoutines_H

//#include "MathUtilsSystemDependencies.h"

ML_START_NAMESPACE

#define JACOBI_ROTATE(a,i,j,k,l) \
    g=a[i][j];                   \
    h=a[k][l];                   \
    a[i][j]=g-s*(h+g*tau);       \
    a[k][l]=h+s*(g-h*tau);

#define NR_END 1
#define FREE_ARG char*


//--------------------------------------------------------------------------
// Numerical Recipes standard error handler
//--------------------------------------------------------------------------
void nrerror(char /*error_text*/[])
{        
    // do nothing
}


//--------------------------------------------------------------------------
// Allocate a variable length float vector with subscript range v[nl..nh]
//--------------------------------------------------------------------------
float *vL_vector(long nl, long nh)
{    
    float *v = (float *) malloc((size_t)((nh-nl+1+NR_END)*sizeof(float)));
    
    if (!v){ nrerror("allocation failure in vector()"); return NULL; }
    
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
    
    long i=0;
    long nrow=0, ncol=0;
    float **m=NULL;
    
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
    int j=0,iq=0,ip=0,i=0;
    float tresh=0,theta=0,tau=0,t=0,sm=0,s=0,h=0,g=0,c=0,*b=NULL,*z=NULL;
    
    b = vL_vector(1,n);
    z = vL_vector(1,n);
    
    // Make v be a unit Matrix.
    for (ip=1;ip<=n;ip++) 
    {
        for (iq=1;iq<=n;iq++) {
            v[ip][iq]=0.0;
        }
        
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
            for (iq=ip+1;iq<=n;iq++) {
                sm += fabs(a[ip][iq]);
            }
        }
        
        if (sm == 0.0) 
        {
            free_vector(z,1,n);
            free_vector(b,1,n);
            return;
        }
        
        if (i < 4) {
            tresh=0.2f*sm/(n*n);
        } else {
            tresh=0.0f;
        }
        
        for (ip=1;ip<=n-1;ip++) {
            
            for (iq=ip+1;iq<=n;iq++) {
                
                g=100.0f*fabs(a[ip][iq]);
                
                if ((i > 4) && ((float)(fabs(d[ip])+g) == (float)fabs(d[ip]))
                            && ((float)(fabs(d[iq])+g) == (float)fabs(d[iq]))){
                    
                    a[ip][iq]=0.0;
                }                
                else if (fabs(a[ip][iq]) > tresh) 
                {
                    h=d[iq]-d[ip];
                    if ((float)(fabs(h)+g) == (float)fabs(h)){
                        t=(a[ip][iq])/h;
                    } else {
                        theta=0.5f*h/(a[ip][iq]);
                        t=1.0f/(fabs(theta)+static_cast<float>(sqrt(1.0f+theta*theta)));
                        if (theta < 0.0) { t = -t; }
                    }
                    
                    c=1.0f/sqrt(1+t*t);
                    s=t*c;
                    tau=s/(1.0f+c);
                    h=t*a[ip][iq];
                    z[ip] -= h;
                    z[iq] += h;
                    d[ip] -= h;
                    d[iq] += h;
                    a[ip][iq]=0.0;
                    
                    for (j=1;j<=ip-1;j++){                   
                        JACOBI_ROTATE(a,j,ip,j,iq)
                    }
                    
                    for (j=ip+1;j<=iq-1;j++) {
                        JACOBI_ROTATE(a,ip,j,j,iq)
                    }
                    
                    for (j=iq+1;j<=n;j++) {
                        JACOBI_ROTATE(a,ip,j,iq,j)
                    }
                    
                    for (j=1;j<=n;j++) {
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

ML_END_NAMESPACE

#endif // __MainAxisPCAMatrixRoutines_H
