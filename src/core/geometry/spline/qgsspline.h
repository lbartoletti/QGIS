/***************************************************************************
                         qgsspline.h
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

#ifndef QGSSPLINE_H
#define QGSSPLINE_H

#include "qgis_core.h"
#include "qgis_sip.h"
#include "qgspoint.h"
#include "qgslinestring.h"


/**
 * \ingroup core
 * \class QgsSpline
 * \brief Spline geometry type.
 * \since QGIS 3.8
 */
class CORE_EXPORT QgsSpline
{

  public:

    QgsSpline() = default;
    /**
     * Unform spline with control points
     * \param controlPoints Control points
     * \param degree Degree of the spline
     */
    QgsSpline( const QVector<QgsPoint> &controlPoints, int degree = 3 );

    /**
     * Spline with custom knot vector
     * \param controlPoints Control points
     * \param knots Knots vector
     * \param degree Degree of the spline
     */
    QgsSpline( const QVector<QgsPoint> &controlPoints, const QVector<double> &knots, int degree = 3 );

    /**
     * @brief Computes C(t).
     * @param t The parameter t.
     * @return The computed point.
     */
    QgsPoint operator()( const double t ) const;

    /**
     * @brief Computes C(k)(t).
     * @param t The parameter t.
     * @param k The order k.
     * @return The computed point.
     */
    QgsPoint derivatives( const double t, int k = 1 ) const;

    //! Returns the degree
    int getDegree() const { return mDegree; }

    //! Sets the degree
    void setDegree( const int degree ) { mDegree = degree; }

    //! Is the spline uniform?
    bool isUniform() const { return mUniform; }

    //! Sets the spline uniform or not
    void setUniform( const bool uniform ) { mUniform = uniform; }

    //! Is the spline clamped?
    bool isClamped() const { return mClamped; }

    //! Sets the spline clamped or not
    void setClamped( const bool clamped ) { mClamped = clamped; }

    //! Step between points on the curve
    double getStep() const { return mStep; }

    //! Set steps between points on the curve
    void setStep( const double step ) { mStep = step; }

    //! Inserts a control point before specified \a position.
    void insertControlPoint( int position, QgsPoint point );

    //! Appends a control point.
    void pushControlPoint( QgsPoint point );

    //! Remove control point at \a position.
    void removeControlPoint( int position );

    //! Replace the control point at \a position.
    void replaceControlPoint( int position, QgsPoint point );

    //! Returns the control points.
    const QVector<QgsPoint> controlPoints() const { return mControlPoints; }

    //! Replaces the array of control points.
    void setControlPoints( const QVector<QgsPoint> &controlPoints ) { mControlPoints = controlPoints; }

    //! Returns the knot vector.
    const QVector<double> knotVector() const { return mKnots; }

    //! Returns points on the curve between [0..1]
    QgsPointSequence evaluates( const double start, const double stop );

    //! Returns spline as LineString
    QgsLineString* toLineString( void );

  protected:
    QVector<QgsPoint> mControlPoints;
    QVector<double> mKnots;
    int mDegree = 3;
    bool mUniform = true;
    bool mClamped = true;
    double mStep = 0.1;

    void computeUniformKnotVector();
};

#endif // QGSSPLINE_H
