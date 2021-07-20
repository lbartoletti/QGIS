/***************************************************************************
     testqgsmaptoolcircle.cpp
     ------------------------
    Date                 : April 2021
    Copyright            : (C) 2021 by Loïc Bartoletti
    Email                : loic dot bartoletti @oslandia dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgstest.h"

#include "qgisapp.h"
#include "qgsgeometry.h"
#include "qgsmapcanvas.h"
#include "qgssettingsregistrycore.h"
#include "qgsvectorlayer.h"
#include "qgsmaptooladdfeature.h"

#include "testqgsmaptoolutils.h"
#include "qgsmaptoolcircle2points.h"
#include "qgsmaptoolcircle3points.h"
#include "qgsmaptoolcirclecenterpoint.h"

#include "testqgspolymaptoolcircle.h"

class TestQgsMapToolCircle : public TestQgsPolyMapToolCircle
{
    Q_OBJECT

  public:
    TestQgsMapToolCircle();

  private slots:
    void testCircleFrom2Points();
    void testCircleFrom2PointsWithDeletedVertex();
    void testCircleFrom3Points();
    void testCircleFrom3PointsWithDeletedVertex();
    void testCircleFromCenterPoint();
    void testCircleFromCenterPointWithDeletedVertex();

  private:
    QgsVectorLayer* getLayer();
};

TestQgsMapToolCircle::TestQgsMapToolCircle() = default;


//runs before all tests
QgsVectorLayer* TestQgsMapToolCircle::getLayer() {
  return new QgsVectorLayer( QStringLiteral( "LineString?crs=EPSG:27700" ), QStringLiteral( "layer line " ), QStringLiteral( "memory" ) );
}


void TestQgsMapToolCircle::testCircleFrom2Points()
{
  QgsFeatureId newFid = drawCircleFrom2Points( mParentTool, mCanvas, mLayer );

  QCOMPARE( mLayer->featureCount(), ( long )1 );
  QgsFeature f = mLayer->getFeature( newFid );

  QString wkt = "CompoundCurve (CircularString (0 2, 1 1, 0 0, -1 1, 0 2))";
  QCOMPARE( f.geometry().asWkt(), wkt );

  mLayer->rollBack();
}

void TestQgsMapToolCircle::testCircleFrom2PointsWithDeletedVertex()
{
  QgsFeatureId newFid = drawCircleFrom2PointsWithDeletedVertex( mParentTool, mCanvas, mLayer );

  QCOMPARE( mLayer->featureCount(), ( long )1 );
  QgsFeature f = mLayer->getFeature( newFid );

  QString wkt = "CompoundCurve (CircularString (0 2, 1 1, 0 0, -1 1, 0 2))";
  QCOMPARE( f.geometry().asWkt(), wkt );

  mLayer->rollBack();
}

void TestQgsMapToolCircle::testCircleFrom3Points()
{
  QgsFeatureId newFid = drawCircleFrom3Points( mParentTool, mCanvas, mLayer );

  QCOMPARE( mLayer->featureCount(), ( long )1 );
  QgsFeature f = mLayer->getFeature( newFid );

  QString wkt = "CompoundCurve (CircularString (0 2, 1 1, 0 0, -1 1, 0 2))";
  QCOMPARE( f.geometry().asWkt(), wkt );

  mLayer->rollBack();
}

void TestQgsMapToolCircle::testCircleFrom3PointsWithDeletedVertex()
{
  QgsFeatureId newFid = drawCircleFrom3PointsWithDeletedVertex( mParentTool, mCanvas, mLayer );

  QCOMPARE( mLayer->featureCount(), ( long )1 );
  QgsFeature f = mLayer->getFeature( newFid );

  QString wkt = "CompoundCurve (CircularString (0 2, 1 1, 0 0, -1 1, 0 2))";
  QCOMPARE( f.geometry().asWkt(), wkt );

  mLayer->rollBack();
}

void TestQgsMapToolCircle::testCircleFromCenterPoint()
{
  QgsFeatureId newFid = drawCircleFromCenterPoint( mParentTool, mCanvas, mLayer );

  QCOMPARE( mLayer->featureCount(), ( long )1 );
  QgsFeature f = mLayer->getFeature( newFid );

  QString wkt = "CompoundCurve (CircularString (0 2, 2 0, 0 -2, -2 0, 0 2))";
  QCOMPARE( f.geometry().asWkt(), wkt );

  mLayer->rollBack();
}

void TestQgsMapToolCircle::testCircleFromCenterPointWithDeletedVertex()
{
  QgsFeatureId newFid = drawCircleFromCenterPointWithDeletedVertex( mParentTool, mCanvas, mLayer );

  QCOMPARE( mLayer->featureCount(), ( long )1 );
  QgsFeature f = mLayer->getFeature( newFid );

  QString wkt = "CompoundCurve (CircularString (0 2, 2 0, 0 -2, -2 0, 0 2))";
  QCOMPARE( f.geometry().asWkt(), wkt );

  mLayer->rollBack();
}

QGSTEST_MAIN( TestQgsMapToolCircle )
#include "testqgsmaptoolcircle.moc"
