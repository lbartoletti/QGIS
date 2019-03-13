/***************************************************************************
                         qgsspline.cpp
                         -------------------
    begin                : March 2019
    copyright            : (C) 2019 by Lo<C3><AF>c Bartoletti
    email                : lbartoletti at tuxfamily dot org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsspline.h"


void QgsSpline::computeUniformKnotVector()
{
  int i, n, numPoints, numKnots;

  // If non-uniform, let user define its knots.
  if ( !mUniform ) return;

  numPoints = mControlPoints.size();
  numKnots  = numPoints + mDegree + 1;
  n = numPoints - mDegree;

  mKnots.clear();

  if ( mClamped )
  {
    for ( i = 0; i <= mDegree;  i++ ) mKnots.push_back( 0. );
    for ( /* */; i < numPoints; i++ ) mKnots.push_back( ( double )( i - mDegree ) / ( double )( n ) );
    for ( /* */; i < numKnots;  i++ ) mKnots.push_back( 1. );
  }
  else
  {
    for ( i = 0; i < numKnots; i++ ) mKnots.push_back( ( double )( i ) / ( double )( numKnots - 1 ) );
  }
}

int QgsSpline::findSpan( int n, int p, double u, const std::vector<double> & U ) const
{
  int low, high, mid;

  // Special case
  if ( u <= U.at( p ) ) return p;
  if ( u >= U.at( n+1 ) ) return n;

  // Do binary search
  low = p;
  high = n + 1;
  mid = ( low + high ) / 2;
  while ( u < U.at( mid ) || u >= U.at( mid + 1 ) )
  {
    if ( u < U.at( mid ) )
      high = mid;
    else
      low = mid;
    mid = ( low + high ) / 2;
  }
  return mid;
}

void QgsSpline::basisFuns( int i, double u, int p, const std::vector<double> & U, double * N_ ) const
{
  double left[ p+1 ], right[ p+1 ];
  double saved, temp;
  int j, r;

  N_[ 0 ] = 1.;
  for ( j = 1; j <= p; j++ )
  {
    left [ j ] = u - U.at( i+1-j );
    right[ j ] = U.at( i+j ) - u;
    saved = 0.;
    for ( r = 0; r < j; r++ )
    {
      temp = N_[ r ] / ( right[ r+1 ] + left[ j-r ] );
      N_[ r ] = saved + right[ r+1 ] * temp;
      saved = left[ j-r ] * temp;
    }
    N_[ j ] = saved;
  }
}



QgsSpline::QgsSpline( const QVector<QgsPoint> &controlPoints, int degree )
  : mControlPoints( controlPoints )
  , mKnots()
  , mDegree( degree )
  , mUniform( true )
  , mClamped( true )
{
  computeUniformKnotVector();
}

QgsSpline::QgsSpline( const QVector<QgsPoint> &controlPoints, const QVector<double> &knots, int degree )
  : mControlPoints( controlPoints )
  , mKnots( knots )
  , mDegree( degree )
  , mUniform( false )
  , mClamped( true )
{
  computeUniformKnotVector();
}

void QgsSpline::insertControlPoint( int position, QgsPoint point )
{
  mControlPoints.insert( position, point );
  computeUniformKnotVector();
}

void QgsSpline::pushControlPoint( QgsPoint point )
{
  mControlPoints.push_back( point );
  computeUniformKnotVector();
}

void QgsSpline::removeControlPoint( int position )
{
  mControlPoints.removeAt( position );
  computeUniformKnotVector();
}

void QgsSpline::replaceControlPoint( int position, QgsPoint point )
{
  mControlPoints.replace( position, point);
}


QgsPoint QgsSpline::evaluates( const double param )
{
  QgsPoint point;

  if ( ( param < 0 )  || ( param > 1 ) )
    return point;

  // Find span and corresponding non-zero basis functions
    int span = findSpan(degree, knots, u);
    QVector<double> N = basisFuns(degree, span, knots, u);

    // Compute point
    for (int j = 0; j <= degree; j++) {
        point += (double)(N[j]) * control_points[span - degree + j];
    }
}

QgsPointSequence QgsSpline::evaluates( const double start, const double stop )
{
  double from = start, to = stop;
  if ( start > stop )
  {
    to = start;
    from = stop;
  }

  QgsPointSequence pts;
  if ( ( 0 < from ) || ( to > 1 ) )
    return pts;

  for ( double i = from; i < to; i += mStep )
  {
    pts.push_back( evaluates( i ) );
  }

  return pts;
}

//! Returns spline as LineString
QgsLineString* toLineString( void )
{

  QgsLineString* line(new QgsLineString() );
  line->points(evaluates(0,1));

  return line;
}

