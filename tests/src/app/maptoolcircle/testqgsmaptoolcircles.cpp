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


class TestQgsMapToolCircles : public QObject
{
    Q_OBJECT

  public:
    TestQgsMapToolCircles();

  private slots:
    void initTestCase();
    void cleanupTestCase();

    void testCircle_data();
    void testCircle();

  private:
    QgisApp *mQgisApp = nullptr;
    QgsMapToolCapture *mParentTool = nullptr;
    QgsMapCanvas *mCanvas = nullptr;


    const QList<QString> coordinateList =
    {
      "XY", "XYZ", "XYM", "XYZM"
    };
    const QList<QString> drawingCircleMethods =
    {
      "2Points", "2PointsWithDeletedVertex",
      "3Points", "3PointsWithDeletedVertex",
      "centerPoint", "centerPointWithDeletedVertex",
    };
    QMap<QString, QString> drawFunctionUserNames;
    QMap<QString, QString> expectedWkts;

    QMap<QString, QgsVectorLayer *> vectorLayerMap;

    void initAttributs();

    QgsFeatureId drawCircleFrom2Points();
    QgsFeatureId drawCircleFrom2PointsWithDeletedVertex();
    QgsFeatureId drawCircleFrom3Points();
    QgsFeatureId drawCircleFrom3PointsWithDeletedVertex();
    QgsFeatureId drawCircleFromCenterPoint();
    QgsFeatureId drawCircleFromCenterPointWithDeletedVertex();

    QgsFeatureId drawCircle( QString );
};

TestQgsMapToolCircles::TestQgsMapToolCircles() = default;

//runs before all tests
void TestQgsMapToolCircles::initTestCase()
{
  QgsApplication::init();
  QgsApplication::initQgis();

  mQgisApp = new QgisApp();

  mCanvas = new QgsMapCanvas();
  mCanvas->setDestinationCrs( QgsCoordinateReferenceSystem( QStringLiteral( "EPSG:27700" ) ) );

  QList<QgsMapLayer *> layerList;
  // make testing layers
  vectorLayerMap["XY"] = new QgsVectorLayer( QStringLiteral( "LineString?crs=EPSG:27700" ), QStringLiteral( "layer line " ), QStringLiteral( "memory" ) );
  QVERIFY( vectorLayerMap["XY"]->isValid() );
  layerList << vectorLayerMap["XY"];

  vectorLayerMap["XYZ"] = new QgsVectorLayer( QStringLiteral( "LineStringZ?crs=EPSG:27700" ), QStringLiteral( "layer line Z" ), QStringLiteral( "memory" ) );
  QVERIFY( vectorLayerMap["XYZ"]->isValid() );
  layerList << vectorLayerMap["XYZ"];

  vectorLayerMap["XYM"] = new QgsVectorLayer( QStringLiteral( "LineStringM?crs=EPSG:27700" ), QStringLiteral( "layer line M" ), QStringLiteral( "memory" ) );
  QVERIFY( vectorLayerMap["XYM"]->isValid() );
  layerList << vectorLayerMap["XYM"];

  vectorLayerMap["XYZM"] = new QgsVectorLayer( QStringLiteral( "LineStringZM?crs=EPSG:27700" ), QStringLiteral( "layer line ZM" ), QStringLiteral( "memory" ) );
  QVERIFY( vectorLayerMap["XYZM"]->isValid() );
  layerList << vectorLayerMap["XYZM"];

  // add and set layers in canvas
  QgsProject::instance()->addMapLayers( layerList );
  mCanvas->setLayers( layerList );

  mParentTool = new QgsMapToolAddFeature( mCanvas, QgsMapToolCapture::CaptureLine );

  initAttributs();
}

void TestQgsMapToolCircles::initAttributs()
{
  drawFunctionUserNames["2Points"] = "from 2 points";
  drawFunctionUserNames["2PointsWithDeletedVertex"] = "from 2 points with deleted vertex";
  drawFunctionUserNames["3Points"] = "from 3 points";
  drawFunctionUserNames["3PointsWithDeletedVertex"] = "from 3 points with deleted vertex";
  drawFunctionUserNames["centerPoint"] = "from center point";
  drawFunctionUserNames["centerPointWithDeletedVertex"] = "from center point with deleted vertex";

  expectedWkts[QString( "XY" ) + QString( "2Points" )] = "CompoundCurve (CircularString (0 2, 1 1, 0 0, -1 1, 0 2))";
  expectedWkts[QString( "XY" ) + QString( "2PointsWithDeletedVertex" )] = "CompoundCurve (CircularString (0 2, 1 1, 0 0, -1 1, 0 2))";
  expectedWkts[QString( "XY" ) + QString( "3Points" )] = "CompoundCurve (CircularString (0 2, 1 1, 0 0, -1 1, 0 2))";
  expectedWkts[QString( "XY" ) + QString( "3PointsWithDeletedVertex" )] = "CompoundCurve (CircularString (0 2, 1 1, 0 0, -1 1, 0 2))";
  expectedWkts[QString( "XY" ) + QString( "centerPoint" )] = "CompoundCurve (CircularString (0 2, 2 0, 0 -2, -2 0, 0 2))";
  expectedWkts[QString( "XY" ) + QString( "centerPointWithDeletedVertex" )] = "CompoundCurve (CircularString (0 2, 2 0, 0 -2, -2 0, 0 2))";

  expectedWkts[QString( "XYZ" ) + QString( "2Points" )] = "CompoundCurveZ (CircularStringZ (0 2 444, 1 1 444, 0 0 444, -1 1 444, 0 2 444))";
  expectedWkts[QString( "XYZ" ) + QString( "2PointsWithDeletedVertex" )] = "CompoundCurveZ (CircularStringZ (0 2 444, 1 1 444, 0 0 444, -1 1 444, 0 2 444))";
  expectedWkts[QString( "XYZ" ) + QString( "3Points" )] = "CompoundCurveZ (CircularStringZ (0 2 444, 1 1 444, 0 0 444, -1 1 444, 0 2 444))";
  expectedWkts[QString( "XYZ" ) + QString( "3PointsWithDeletedVertex" )] = "CompoundCurveZ (CircularStringZ (0 2 444, 1 1 444, 0 0 444, -1 1 444, 0 2 444))";
  expectedWkts[QString( "XYZ" ) + QString( "centerPoint" )] = "CompoundCurveZ (CircularStringZ (0 2 444, 2 0 444, 0 -2 444, -2 0 444, 0 2 444))";
  expectedWkts[QString( "XYZ" ) + QString( "centerPointWithDeletedVertex" )] = "CompoundCurveZ (CircularStringZ (0 2 444, 2 0 444, 0 -2 444, -2 0 444, 0 2 444))";

  expectedWkts[QString( "XYM" ) + QString( "2Points" )] = "CompoundCurveM (CircularStringM (0 2 222, 1 1 222, 0 0 222, -1 1 222, 0 2 222))";
  expectedWkts[QString( "XYM" ) + QString( "2PointsWithDeletedVertex" )] = "CompoundCurveM (CircularStringM (0 2 222, 1 1 222, 0 0 222, -1 1 222, 0 2 222))";
  expectedWkts[QString( "XYM" ) + QString( "3Points" )] = "CompoundCurveM (CircularStringM (0 2 222, 1 1 222, 0 0 222, -1 1 222, 0 2 222))";
  expectedWkts[QString( "XYM" ) + QString( "3PointsWithDeletedVertex" )] = "CompoundCurveM (CircularStringM (0 2 222, 1 1 222, 0 0 222, -1 1 222, 0 2 222))";
  expectedWkts[QString( "XYM" ) + QString( "centerPoint" )] = "CompoundCurveM (CircularStringM (0 2 222, 2 0 222, 0 -2 222, -2 0 222, 0 2 222))";
  expectedWkts[QString( "XYM" ) + QString( "centerPointWithDeletedVertex" )] = "CompoundCurveM (CircularStringM (0 2 222, 2 0 222, 0 -2 222, -2 0 222, 0 2 222))";

  expectedWkts[QString( "XYZM" ) + QString( "2Points" )] = "CompoundCurveZM (CircularStringZM (0 2 444 222, 1 1 444 222, 0 0 444 222, -1 1 444 222, 0 2 444 222))";
  expectedWkts[QString( "XYZM" ) + QString( "2PointsWithDeletedVertex" )] = "CompoundCurveZM (CircularStringZM (0 2 444 222, 1 1 444 222, 0 0 444 222, -1 1 444 222, 0 2 444 222))";
  expectedWkts[QString( "XYZM" ) + QString( "3Points" )] = "CompoundCurveZM (CircularStringZM (0 2 444 222, 1 1 444 222, 0 0 444 222, -1 1 444 222, 0 2 444 222))";
  expectedWkts[QString( "XYZM" ) + QString( "3PointsWithDeletedVertex" )] = "CompoundCurveZM (CircularStringZM (0 2 444 222, 1 1 444 222, 0 0 444 222, -1 1 444 222, 0 2 444 222))";
  expectedWkts[QString( "XYZM" ) + QString( "centerPoint" )] = "CompoundCurveZM (CircularStringZM (0 2 444 222, 2 0 444 222, 0 -2 444 222, -2 0 444 222, 0 2 444 222))";
  expectedWkts[QString( "XYZM" ) + QString( "centerPointWithDeletedVertex" )] = "CompoundCurveZM (CircularStringZM (0 2 444 222, 2 0 444 222, 0 -2 444 222, -2 0 444 222, 0 2 444 222))";
}

void TestQgsMapToolCircles::cleanupTestCase()
{
  QgsApplication::exitQgis();
}


QgsFeatureId TestQgsMapToolCircles::drawCircleFrom2Points()
{
  QgsMapToolCircle2Points mapTool( mParentTool, mCanvas );
  mCanvas->setMapTool( &mapTool );

  TestQgsMapToolAdvancedDigitizingUtils utils( &mapTool );
  utils.mouseClick( 0, 0, Qt::LeftButton );
  utils.mouseMove( 0, 2 );
  utils.mouseClick( 0, 2, Qt::RightButton );

  return utils.newFeatureId();
}

QgsFeatureId TestQgsMapToolCircles::drawCircleFrom2PointsWithDeletedVertex()
{
  QgsMapToolCircle2Points mapTool( mParentTool, mCanvas );
  mCanvas->setMapTool( &mapTool );

  TestQgsMapToolAdvancedDigitizingUtils utils( &mapTool );
  utils.mouseClick( 4, 1, Qt::LeftButton );
  utils.keyClick( Qt::Key_Backspace );
  utils.mouseClick( 0, 0, Qt::LeftButton );
  utils.mouseMove( 0, 2 );
  utils.mouseClick( 0, 2, Qt::RightButton );

  return utils.newFeatureId();
}

QgsFeatureId TestQgsMapToolCircles::drawCircleFrom3Points()
{
  QgsMapToolCircle3Points mapTool( mParentTool, mCanvas );
  mCanvas->setMapTool( &mapTool );

  TestQgsMapToolAdvancedDigitizingUtils utils( &mapTool );
  utils.mouseClick( 0, 0, Qt::LeftButton );
  utils.mouseClick( 0, 2, Qt::LeftButton );
  utils.mouseMove( 1, 1 );
  utils.mouseClick( 1, 1, Qt::RightButton );

  return utils.newFeatureId();
}

QgsFeatureId TestQgsMapToolCircles::drawCircleFrom3PointsWithDeletedVertex()
{
  QgsMapToolCircle3Points mapTool( mParentTool, mCanvas );
  mCanvas->setMapTool( &mapTool );

  TestQgsMapToolAdvancedDigitizingUtils utils( &mapTool );
  utils.mouseClick( 0, 0, Qt::LeftButton );
  utils.mouseClick( 4, 1, Qt::LeftButton );
  utils.keyClick( Qt::Key_Backspace );
  utils.mouseClick( 0, 2, Qt::LeftButton );
  utils.mouseMove( 1, 1 );
  utils.mouseClick( 1, 1, Qt::RightButton );

  return utils.newFeatureId();
}

QgsFeatureId TestQgsMapToolCircles::drawCircleFromCenterPoint()
{
  QgsMapToolCircleCenterPoint mapTool( mParentTool, mCanvas );
  mCanvas->setMapTool( &mapTool );

  TestQgsMapToolAdvancedDigitizingUtils utils( &mapTool );
  utils.mouseClick( 0, 0, Qt::LeftButton );
  utils.mouseMove( 0, 2 );
  utils.mouseClick( 0, 2, Qt::RightButton );

  return utils.newFeatureId();
}

QgsFeatureId TestQgsMapToolCircles::drawCircleFromCenterPointWithDeletedVertex()
{
  QgsMapToolCircleCenterPoint mapTool( mParentTool, mCanvas );
  mCanvas->setMapTool( &mapTool );

  TestQgsMapToolAdvancedDigitizingUtils utils( &mapTool );
  utils.mouseClick( 4, 1, Qt::LeftButton );
  utils.keyClick( Qt::Key_Backspace );
  utils.mouseClick( 0, 0, Qt::LeftButton );
  utils.mouseMove( 0, 2 );
  utils.mouseClick( 0, 2, Qt::RightButton );

  return utils.newFeatureId();
}

QgsFeatureId TestQgsMapToolCircles::drawCircle( QString drawMethod )
{
  if ( drawMethod == "2Points" )
  {
    return drawCircleFrom2Points();
  }
  else if ( drawMethod == "2PointsWithDeletedVertex" )
  {
    return drawCircleFrom2PointsWithDeletedVertex();
  }
  else if ( drawMethod == "3Points" )
  {
    return drawCircleFrom3Points();
  }
  else if ( drawMethod == "3PointsWithDeletedVertex" )
  {
    return drawCircleFrom3PointsWithDeletedVertex();
  }
  else if ( drawMethod == "centerPoint" )
  {
    return drawCircleFromCenterPoint();
  }
  else if ( drawMethod == "centerPointWithDeletedVertex" )
  {
    return drawCircleFromCenterPointWithDeletedVertex();
  }
  else
  {
    return FID_NULL;
  }
}


void TestQgsMapToolCircles::testCircle_data()
{
  QTest::addColumn<QString>( "wktGeometry" );
  QTest::addColumn<QString>( "wktExpected" );
  QTest::addColumn<qlonglong>( "featureCount" );
  QTest::addColumn<long>( "featureCountExpected" );

  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 444 );
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 222 );

  QgsFeatureId newFid;
  QgsFeature f;
  QString wkt;
  QgsVectorLayer *mLayer;

  QString coordinate;
  QString drawMethod;
  QString rowStringName;

  QListIterator<QString> coordinateIter( coordinateList );
  QListIterator<QString> drawCircleIter( drawingCircleMethods );

  while ( coordinateIter.hasNext() )
  {
    coordinate = coordinateIter.next();
    mLayer = vectorLayerMap[coordinate];

    mCanvas->setCurrentLayer( mLayer );

    while ( drawCircleIter.hasNext() )
    {
      drawMethod = drawCircleIter.next();

      mLayer->startEditing();
      newFid = drawCircle( drawMethod );
      f = mLayer->getFeature( newFid );

      wkt = expectedWkts[coordinate + drawMethod];
      rowStringName = coordinate + " " + drawFunctionUserNames[drawMethod];
      QTest::newRow( rowStringName.toStdString().c_str() ) << f.geometry().asWkt() << wkt << mLayer->featureCount() << ( long )1;

      mLayer->rollBack();
    }
    drawCircleIter.toFront();
  }
}

void TestQgsMapToolCircles::testCircle()
{
  QFETCH( qlonglong, featureCount );
  QFETCH( long, featureCountExpected );
  QCOMPARE( featureCount, featureCountExpected );

  QFETCH( QString, wktGeometry );
  QFETCH( QString, wktExpected );
  QCOMPARE( wktGeometry, wktExpected );
}


QGSTEST_MAIN( TestQgsMapToolCircles )
#include "testqgsmaptoolcircles.moc"
