/**
 \file spline.h
 Interface of SPLINE library - Interpolation and Approximation of Data
 
 \author Ruben Stein
 \date   2010-06-28
 
 This library is a modified version of the original SPLINE library of John Burkardt found by the following link.
 
 Source:     http://people.sc.fsu.edu/~jburkardt/cpp_src/spline/spline.html
 License:    GNU LESSER GENERAL PUBLIC LICENSE Version 3, 29 June 2007
             http://people.sc.fsu.edu/~jburkardt/txt/gnu_lgpl.txt
 
 -------------------------------------------------------------------------------
 Description by John Burkardt:
 
 SPLINE is a C++ library which constructs and evaluates spline functions.
 
 These spline functions are typically used to
 
 - interpolate data exactly at a set of points;
 - approximate data at many points, or over an interval.
 
 -------------------------------------------------------------------------------
 
 The above mentioned library was modified to be linkable as shared library. See
 below for a detailed list of changes. The resulting library is again licensed
 using LGPLv3.

 Maintainer: Ruben Stein (ruben.stein@mevis.fraunhofer.de)

 Modifications:
 2010-06-28: - Added comments in header-file
             - Added namespace spline around functions
             - Added include guards and export macros to allow this to work as
               a shared library. Use the following regular expression to add the
               export macros:
               Find:    ^([A-Za-z].*)
               Replace: SPLINE_EXPORT \1
             - Moved std::string include to header file.
             - Removed "temp" variable from basis_matrix_tmp to get rid of a
               unused variable warning.
             - Moved spline.H import to top of spline.C.
             - Added some variable initializations to remove compiler warnings.
 -------------------------------------------------------------------------------
*/

#ifndef _SPLINE_H
#define _SPLINE_H

//stl
#include <string>

//! \def SPLINE_EXPORT Defines import/export macros necessary to use a shared
//!                    library interface on windows platform.
#ifdef _WIN32
  #ifdef SPLINE_EXPORTS
    #define SPLINE_EXPORT __declspec(dllexport)
  #else
    #define SPLINE_EXPORT __declspec(dllimport)
  #endif
#else
  #define SPLINE_EXPORT
#endif

//------------------------------------------------------------------------------
namespace spline
{
SPLINE_EXPORT double basis_function_b_val ( double tdata[], double tval );
SPLINE_EXPORT double basis_function_beta_val ( double beta1, double beta2, double tdata[], 
  double tval );
SPLINE_EXPORT double *basis_matrix_b_uni ( );
SPLINE_EXPORT double *basis_matrix_beta_uni ( double beta1, double beta2 );
SPLINE_EXPORT double *basis_matrix_bezier ( );
SPLINE_EXPORT double *basis_matrix_hermite ( );
SPLINE_EXPORT double *basis_matrix_overhauser_nonuni ( double alpha, double beta );
SPLINE_EXPORT double *basis_matrix_overhauser_nul ( double alpha );
SPLINE_EXPORT double *basis_matrix_overhauser_nur ( double beta );
SPLINE_EXPORT double *basis_matrix_overhauser_uni ( void);
SPLINE_EXPORT double *basis_matrix_overhauser_uni_l ( );
SPLINE_EXPORT double *basis_matrix_overhauser_uni_r ( );
SPLINE_EXPORT double basis_matrix_tmp ( int left, int n, double mbasis[], int ndata, 
  double tdata[], double ydata[], double tval );
SPLINE_EXPORT void bc_val ( int n, double t, double xcon[], double ycon[], double *xval,
  double *yval );
SPLINE_EXPORT double bez_val ( int n, double x, double a, double b, double y[] );
SPLINE_EXPORT double bp_approx ( int n, double a, double b, double ydata[], double xval );
SPLINE_EXPORT double *bp01 ( int n, double x );
SPLINE_EXPORT double *bpab ( int n, double a, double b, double x );
SPLINE_EXPORT int chfev ( double x1, double x2, double f1, double f2, double d1, double d2,
  int ne, double xe[], double fe[], int next[] );
SPLINE_EXPORT int d3_fs ( double a1[], double a2[], double a3[], int n, double b[], double x[] );
SPLINE_EXPORT double *d3_mxv ( int n, double a[], double x[] );
SPLINE_EXPORT double *d3_np_fs ( int n, double a[], double b[] );
SPLINE_EXPORT void d3_print ( int n, double a[], std::string title );
SPLINE_EXPORT void d3_print_some ( int n, double a[], int ilo, int jlo, int ihi, int jhi );
SPLINE_EXPORT double *d3_uniform ( int n, int *seed );
SPLINE_EXPORT void data_to_dif ( int ntab, double xtab[], double ytab[], double diftab[] );
SPLINE_EXPORT double dif_val ( int ntab, double xtab[], double diftab[], double xval );
SPLINE_EXPORT int i4_max ( int i1, int i2 );
SPLINE_EXPORT int i4_min ( int i1, int i2 );
SPLINE_EXPORT void least_set ( int point_num, double x[], double f[], double w[], 
  int nterms, double b[], double c[], double d[] );
SPLINE_EXPORT double least_val ( int nterms, double b[], double c[], double d[], 
  double x );
SPLINE_EXPORT void least_val2 ( int nterms, double b[], double c[], double d[], double x, 
  double *px, double *pxp );
SPLINE_EXPORT void least_set_old ( int ntab, double xtab[], double ytab[], int ndeg, 
  double ptab[], double b[], double c[], double d[], double *eps, int *ierror );
SPLINE_EXPORT double least_val_old ( double x, int ndeg, double b[], double c[], double d[] );
SPLINE_EXPORT void parabola_val2 ( int ndim, int ndata, double tdata[], double ydata[], 
  int left, double tval, double yval[] );
SPLINE_EXPORT double pchst ( double arg1, double arg2 );
SPLINE_EXPORT double r8_max ( double x, double y );
SPLINE_EXPORT double r8_min ( double x, double y );
SPLINE_EXPORT double r8_uniform_01 ( int *seed );
SPLINE_EXPORT void r8vec_bracket ( int n, double x[], double xval, int *left, int *right );
SPLINE_EXPORT void r8vec_bracket3 ( int n, double t[], double tval, int *left );
SPLINE_EXPORT double *r8vec_even ( int n, double alo, double ahi );
SPLINE_EXPORT double *r8vec_indicator ( int n );
SPLINE_EXPORT void r8vec_order_type ( int n, double x[], int *order );
SPLINE_EXPORT void r8vec_print ( int n, double a[], std::string title );
SPLINE_EXPORT void r8vec_sort_bubble_a ( int n, double a[] );
SPLINE_EXPORT double *r8vec_uniform ( int n, double b, double c, int *seed );
SPLINE_EXPORT int r8vec_unique_count ( int n, double a[], double tol );
SPLINE_EXPORT void r8vec_zero ( int n, double a[] );
SPLINE_EXPORT int s_len_trim ( std::string s );
SPLINE_EXPORT double spline_b_val ( int ndata, double tdata[], double ydata[], double tval );
SPLINE_EXPORT double spline_beta_val ( double beta1, double beta2, int ndata, double tdata[],
  double ydata[], double tval );
SPLINE_EXPORT double spline_constant_val ( int ndata, double tdata[], double ydata[], double tval );
SPLINE_EXPORT double *spline_cubic_set ( int n, double t[], double y[], int ibcbeg, double ybcbeg, 
  int ibcend, double ybcend );
SPLINE_EXPORT double spline_cubic_val ( int n, double t[], double tval, double y[], double ypp[],
  double *ypval, double *yppval );
SPLINE_EXPORT void spline_cubic_val2 ( int n, double t[], double tval, int *left, double y[], 
  double ypp[], double *yval, double *ypval, double *yppval );
SPLINE_EXPORT double *spline_hermite_set ( int ndata, double tdata[], double ydata[], 
  double ypdata[] );
SPLINE_EXPORT void spline_hermite_val ( int ndata, double tdata[], double c[], double tval, 
  double *sval, double *spval );
SPLINE_EXPORT double spline_linear_int ( int ndata, double tdata[], double ydata[], double a, 
  double b );
SPLINE_EXPORT void spline_linear_intset ( int int_n, double int_x[], double int_v[], 
  double data_x[], double data_y[] );
SPLINE_EXPORT void spline_linear_val ( int ndata, double tdata[], double ydata[], 
  double tval, double *yval, double *ypval );
SPLINE_EXPORT double spline_overhauser_nonuni_val ( int ndata, double tdata[], 
  double ydata[], double tval );
SPLINE_EXPORT double spline_overhauser_uni_val ( int ndata, double tdata[], double ydata[],
  double tval );
SPLINE_EXPORT void spline_overhauser_val ( int ndim, int ndata, double tdata[], double ydata[], 
  double tval, double yval[] );
SPLINE_EXPORT void spline_pchip_set ( int n, double x[], double f[], double d[] );
SPLINE_EXPORT void spline_pchip_val ( int n, double x[], double f[], double d[], int ne, 
  double xe[], double fe[] );
SPLINE_EXPORT void spline_quadratic_val ( int ndata, double tdata[], double ydata[], 
  double tval, double *yval, double *ypval );
SPLINE_EXPORT void timestamp ( );
} //namespace spline

#endif //_SPLINE_H
