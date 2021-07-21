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

    void testCircleZ_data();
    void testCircleZ();

    void testCircleM_data();
    void testCircleM();

    void testCircleZM_data();
    void testCircleZM();

  private:
    QgisApp *mQgisApp = nullptr;
    QgsMapToolCapture *mParentTool = nullptr;
    QgsMapCanvas *mCanvas = nullptr;

    QgsVectorLayer *mLayer = nullptr;
    QgsVectorLayer *mLayerZ = nullptr;
    QgsVectorLayer *mLayerM = nullptr;
    QgsVectorLayer *mLayerZM = nullptr;

    QgsFeatureId drawCircleFrom2Points();
    QgsFeatureId drawCircleFrom2PointsWithDeletedVertex();
    QgsFeatureId drawCircleFrom3Points();
    QgsFeatureId drawCircleFrom3PointsWithDeletedVertex();
    QgsFeatureId drawCircleFromCenterPoint();
    QgsFeatureId drawCircleFromCenterPointWithDeletedVertex();
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

  // make testing layers
  QList<QgsMapLayer *> layerList;
  mLayer = new QgsVectorLayer( QStringLiteral( "LineString?crs=EPSG:27700" ), QStringLiteral( "layer line " ), QStringLiteral( "memory" ) );
  QVERIFY( mLayer->isValid() );
  layerList << mLayer;

  mLayerZ = new QgsVectorLayer( QStringLiteral( "LineStringZ?crs=EPSG:27700" ), QStringLiteral( "layer line Z" ), QStringLiteral( "memory" ) );
  QVERIFY( mLayerZ->isValid() );
  layerList << mLayerZ;

  mLayerM = new QgsVectorLayer( QStringLiteral( "LineStringM?crs=EPSG:27700" ), QStringLiteral( "layer line M" ), QStringLiteral( "memory" ) );
  QVERIFY( mLayerM->isValid() );
  layerList << mLayerM;

  mLayerZM = new QgsVectorLayer( QStringLiteral( "LineStringZM?crs=EPSG:27700" ), QStringLiteral( "layer line ZM" ), QStringLiteral( "memory" ) );
  QVERIFY( mLayerZM->isValid() );
  layerList << mLayerZM;

  // add and set layers in canvas
  QgsProject::instance()->addMapLayers( layerList );
  mCanvas->setLayers( layerList );

  mParentTool = new QgsMapToolAddFeature( mCanvas, QgsMapToolCapture::CaptureLine );
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


void TestQgsMapToolCircles::testCircle_data()
{
  QTest::addColumn<QString>( "wktGeometry" );
  QTest::addColumn<QString>( "wktExpected" );
  QTest::addColumn<qlonglong>( "featureCount" );
  QTest::addColumn<long>( "featureCountExpected" );

  QgsFeatureId newFid;
  QgsFeature f;
  QString wkt;

  mCanvas->setCurrentLayer( mLayer );

  // testCircleFrom2Points
  mLayer->startEditing();
  newFid = drawCircleFrom2Points();
  f = mLayer->getFeature( newFid );

  wkt = "CompoundCurve (CircularString (0 2, 1 1, 0 0, -1 1, 0 2))";
  QTest::newRow( "from 2 points" ) << f.geometry().asWkt() << wkt << mLayer->featureCount() << ( long )1;

  mLayer->rollBack();

  // testCircleFrom2PointsWithDeletedVertex
  mLayer->startEditing();
  newFid = drawCircleFrom2PointsWithDeletedVertex();
  f = mLayer->getFeature( newFid );

  wkt = "CompoundCurve (CircularString (0 2, 1 1, 0 0, -1 1, 0 2))";
  QTest::newRow( "from 2 points with deleted vertex" ) << f.geometry().asWkt() << wkt << mLayer->featureCount() << ( long )1;

  mLayer->rollBack();

  // testCircleFrom3Points
  mLayer->startEditing();
  newFid = drawCircleFrom3Points();
  f = mLayer->getFeature( newFid );

  wkt = "CompoundCurve (CircularString (0 2, 1 1, 0 0, -1 1, 0 2))";
  QTest::newRow( "from 3 points" ) << f.geometry().asWkt() << wkt << mLayer->featureCount() << ( long )1;

  mLayer->rollBack();

  // testCircleFrom3PointsWithDeletedVertex
  mLayer->startEditing();
  newFid = drawCircleFrom3PointsWithDeletedVertex();
  f = mLayer->getFeature( newFid );

  wkt = "CompoundCurve (CircularString (0 2, 1 1, 0 0, -1 1, 0 2))";
  QTest::newRow( "from 3 points with deleted vertex" ) << f.geometry().asWkt() << wkt << mLayer->featureCount() << ( long )1;

  mLayer->rollBack();

  // testCircleFromCenterPoint
  mLayer->startEditing();
  newFid = drawCircleFromCenterPoint();
  f = mLayer->getFeature( newFid );

  wkt = "CompoundCurve (CircularString (0 2, 2 0, 0 -2, -2 0, 0 2))";
  QTest::newRow( "from center point" ) << f.geometry().asWkt() << wkt << mLayer->featureCount() << ( long )1;

  mLayer->rollBack();

  // testCircleFromCenterPointWithDeletedVertex
  mLayer->startEditing();
  newFid = drawCircleFromCenterPointWithDeletedVertex();
  f = mLayer->getFeature( newFid );

  wkt = "CompoundCurve (CircularString (0 2, 2 0, 0 -2, -2 0, 0 2))";
  QTest::newRow( "from center point with deleted vertex" ) << f.geometry().asWkt() << wkt << mLayer->featureCount() << ( long )1;

  mLayer->rollBack();
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


void TestQgsMapToolCircles::testCircleZ_data()
{
  QTest::addColumn<QString>( "wktGeometry" );
  QTest::addColumn<QString>( "wktExpected" );
  QTest::addColumn<qlonglong>( "featureCount" );
  QTest::addColumn<long>( "featureCountExpected" );

  QgsFeatureId newFid;
  QgsFeature f;
  QString wkt;

  mCanvas->setCurrentLayer( mLayerZ );

  // testCircleFrom2Points
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 333 );
  mLayerZ->startEditing();
  newFid = drawCircleFrom2Points();
  f = mLayerZ->getFeature( newFid );

  wkt = "CompoundCurveZ (CircularStringZ (0 2 333, 1 1 333, 0 0 333, -1 1 333, 0 2 333))";
  QTest::newRow( "from 2 points" ) << f.geometry().asWkt() << wkt << mLayerZ->featureCount() << ( long )1;

  mLayerZ->rollBack();

  // testCircleFrom2PointsWithDeletedVertex
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 333 );
  mLayerZ->startEditing();
  newFid = drawCircleFrom2PointsWithDeletedVertex();
  f = mLayerZ->getFeature( newFid );

  wkt = "CompoundCurveZ (CircularStringZ (0 2 333, 1 1 333, 0 0 333, -1 1 333, 0 2 333))";
  QTest::newRow( "from 2 points with deleted vertex" ) << f.geometry().asWkt() << wkt << mLayerZ->featureCount() << ( long )1;

  mLayerZ->rollBack();

  // testCircleFrom3Points
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 111 );
  mLayerZ->startEditing();
  newFid = drawCircleFrom3Points();
  f = mLayerZ->getFeature( newFid );

  wkt = "CompoundCurveZ (CircularStringZ (0 2 111, 1 1 111, 0 0 111, -1 1 111, 0 2 111))";
  QTest::newRow( "from 3 points" ) << f.geometry().asWkt() << wkt << mLayerZ->featureCount() << ( long )1;

  mLayerZ->rollBack();

  // testCircleFrom3PointsWithDeletedVertex
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 111 );
  mLayerZ->startEditing();
  newFid = drawCircleFrom3PointsWithDeletedVertex();
  f = mLayerZ->getFeature( newFid );

  wkt = "CompoundCurveZ (CircularStringZ (0 2 111, 1 1 111, 0 0 111, -1 1 111, 0 2 111))";
  QTest::newRow( "from 3 points with deleted vertex" ) << f.geometry().asWkt() << wkt << mLayerZ->featureCount() << ( long )1;

  mLayerZ->rollBack();

  // testCircleFromCenterPoint
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 222 );
  mLayerZ->startEditing();
  newFid = drawCircleFromCenterPoint();
  f = mLayerZ->getFeature( newFid );

  wkt = "CompoundCurveZ (CircularStringZ (0 2 222, 2 0 222, 0 -2 222, -2 0 222, 0 2 222))";
  QTest::newRow( "from center point" ) << f.geometry().asWkt() << wkt << mLayerZ->featureCount() << ( long )1;

  mLayerZ->rollBack();

  // testCircleFromCenterPointWithDeletedVertex
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 222 );
  mLayerZ->startEditing();
  newFid = drawCircleFromCenterPointWithDeletedVertex();
  f = mLayerZ->getFeature( newFid );

  wkt = "CompoundCurveZ (CircularStringZ (0 2 222, 2 0 222, 0 -2 222, -2 0 222, 0 2 222))";
  QTest::newRow( "from center point with deleted vertex" ) << f.geometry().asWkt() << wkt << mLayerZ->featureCount() << ( long )1;

  mLayerZ->rollBack();
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 0 );
}

void TestQgsMapToolCircles::testCircleZ()
{
  QFETCH( qlonglong, featureCount );
  QFETCH( long, featureCountExpected );
  QCOMPARE( featureCount, featureCountExpected );

  QFETCH( QString, wktGeometry );
  QFETCH( QString, wktExpected );
  QCOMPARE( wktGeometry, wktExpected );
}


void TestQgsMapToolCircles::testCircleM_data()
{
  QTest::addColumn<QString>( "wktGeometry" );
  QTest::addColumn<QString>( "wktExpected" );
  QTest::addColumn<qlonglong>( "featureCount" );
  QTest::addColumn<long>( "featureCountExpected" );

  QgsFeatureId newFid;
  QgsFeature f;
  QString wkt;

  mCanvas->setCurrentLayer( mLayerM );

  // testCircleFrom2Points
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 333 );
  mLayerM->startEditing();
  newFid = drawCircleFrom2Points();
  f = mLayerM->getFeature( newFid );

  wkt = "CompoundCurveM (CircularStringM (0 2 333, 1 1 333, 0 0 333, -1 1 333, 0 2 333))";
  QTest::newRow( "from 2 points" ) << f.geometry().asWkt() << wkt << mLayerM->featureCount() << ( long )1;

  mLayerM->rollBack();

  // testCircleFrom2PointsWithDeletedVertex
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 333 );
  mLayerM->startEditing();
  newFid = drawCircleFrom2PointsWithDeletedVertex();
  f = mLayerM->getFeature( newFid );

  wkt = "CompoundCurveM (CircularStringM (0 2 333, 1 1 333, 0 0 333, -1 1 333, 0 2 333))";
  QTest::newRow( "from 2 points with deleted vertex" ) << f.geometry().asWkt() << wkt << mLayerM->featureCount() << ( long )1;

  mLayerM->rollBack();

  // testCircleFrom3Points
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 111 );
  mLayerM->startEditing();
  newFid = drawCircleFrom3Points();
  f = mLayerM->getFeature( newFid );

  wkt = "CompoundCurveM (CircularStringM (0 2 111, 1 1 111, 0 0 111, -1 1 111, 0 2 111))";
  QTest::newRow( "from 3 points" ) << f.geometry().asWkt() << wkt << mLayerM->featureCount() << ( long )1;

  mLayerM->rollBack();

  // testCircleFrom3PointsWithDeletedVertex
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 111 );
  mLayerM->startEditing();
  newFid = drawCircleFrom3PointsWithDeletedVertex();
  f = mLayerM->getFeature( newFid );

  wkt = "CompoundCurveM (CircularStringM (0 2 111, 1 1 111, 0 0 111, -1 1 111, 0 2 111))";
  QTest::newRow( "from 3 points with deleted vertex" ) << f.geometry().asWkt() << wkt << mLayerM->featureCount() << ( long )1;

  mLayerM->rollBack();

  // testCircleFromCenterPoint
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 222 );
  mLayerM->startEditing();
  newFid = drawCircleFromCenterPoint();
  f = mLayerM->getFeature( newFid );

  wkt = "CompoundCurveM (CircularStringM (0 2 222, 2 0 222, 0 -2 222, -2 0 222, 0 2 222))";
  QTest::newRow( "from center point" ) << f.geometry().asWkt() << wkt << mLayerM->featureCount() << ( long )1;

  mLayerM->rollBack();

  // testCircleFromCenterPointWithDeletedVertex
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 222 );
  mLayerM->startEditing();
  newFid = drawCircleFromCenterPointWithDeletedVertex();
  f = mLayerM->getFeature( newFid );

  wkt = "CompoundCurveM (CircularStringM (0 2 222, 2 0 222, 0 -2 222, -2 0 222, 0 2 222))";
  QTest::newRow( "from center point with deleted vertex" ) << f.geometry().asWkt() << wkt << mLayerM->featureCount() << ( long )1;

  mLayerM->rollBack();
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 0 );
}

void TestQgsMapToolCircles::testCircleM()
{
  QFETCH( qlonglong, featureCount );
  QFETCH( long, featureCountExpected );
  QCOMPARE( featureCount, featureCountExpected );

  QFETCH( QString, wktGeometry );
  QFETCH( QString, wktExpected );
  QCOMPARE( wktGeometry, wktExpected );
}


void TestQgsMapToolCircles::testCircleZM_data()
{
  QTest::addColumn<QString>( "wktGeometry" );
  QTest::addColumn<QString>( "wktExpected" );
  QTest::addColumn<qlonglong>( "featureCount" );
  QTest::addColumn<long>( "featureCountExpected" );

  QgsFeatureId newFid;
  QgsFeature f;
  QString wkt;

  mCanvas->setCurrentLayer( mLayerZM );

  // testCircleFrom2Points
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 444 );
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 333 );
  mLayerZM->startEditing();
  newFid = drawCircleFrom2Points();
  f = mLayerZM->getFeature( newFid );

  wkt = "CompoundCurveZM (CircularStringZM (0 2 444 333, 1 1 444 333, 0 0 444 333, -1 1 444 333, 0 2 444 333))";
  QTest::newRow( "from 2 points" ) << f.geometry().asWkt() << wkt << mLayerZM->featureCount() << ( long )1;

  mLayerZM->rollBack();

  // testCircleFrom2PointsWithDeletedVertex
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 444 );
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 333 );
  mLayerZM->startEditing();
  newFid = drawCircleFrom2PointsWithDeletedVertex();
  f = mLayerZM->getFeature( newFid );

  wkt = "CompoundCurveZM (CircularStringZM (0 2 444 333, 1 1 444 333, 0 0 444 333, -1 1 444 333, 0 2 444 333))";
  QTest::newRow( "from 2 points with deleted vertex" ) << f.geometry().asWkt() << wkt << mLayerZM->featureCount() << ( long )1;

  mLayerZM->rollBack();

  // testCircleFrom3Points
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 444 );
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 111 );
  mLayerZM->startEditing();
  newFid = drawCircleFrom3Points();
  f = mLayerZM->getFeature( newFid );

  wkt = "CompoundCurveZM (CircularStringZM (0 2 444 111, 1 1 444 111, 0 0 444 111, -1 1 444 111, 0 2 444 111))";
  QTest::newRow( "from 3 points" ) << f.geometry().asWkt() << wkt << mLayerZM->featureCount() << ( long )1;

  mLayerZM->rollBack();

  // testCircleFrom3PointsWithDeletedVertex
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 444 );
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 111 );
  mLayerZM->startEditing();
  newFid = drawCircleFrom3PointsWithDeletedVertex();
  f = mLayerZM->getFeature( newFid );

  wkt = "CompoundCurveZM (CircularStringZM (0 2 444 111, 1 1 444 111, 0 0 444 111, -1 1 444 111, 0 2 444 111))";
  QTest::newRow( "from 3 points with deleted vertex" ) << f.geometry().asWkt() << wkt << mLayerZM->featureCount() << ( long )1;

  mLayerZM->rollBack();

  // testCircleFromCenterPoint
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 444 );
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 222 );
  mLayerZM->startEditing();
  newFid = drawCircleFromCenterPoint();
  f = mLayerZM->getFeature( newFid );

  wkt = "CompoundCurveZM (CircularStringZM (0 2 444 222, 2 0 444 222, 0 -2 444 222, -2 0 444 222, 0 2 444 222))";
  QTest::newRow( "from center point" ) << f.geometry().asWkt() << wkt << mLayerZM->featureCount() << ( long )1;

  mLayerZM->rollBack();

  // testCircleFromCenterPointWithDeletedVertex
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 444 );
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 222 );
  mLayerZM->startEditing();
  newFid = drawCircleFromCenterPointWithDeletedVertex();
  f = mLayerZM->getFeature( newFid );

  wkt = "CompoundCurveZM (CircularStringZM (0 2 444 222, 2 0 444 222, 0 -2 444 222, -2 0 444 222, 0 2 444 222))";
  QTest::newRow( "from center point with deleted vertex" ) << f.geometry().asWkt() << wkt << mLayerZM->featureCount() << ( long )1;

  mLayerZM->rollBack();
  QgsSettingsRegistryCore::settingsDigitizingDefaultZValue.setValue( 0 );
  QgsSettingsRegistryCore::settingsDigitizingDefaultMValue.setValue( 0 );
}

void TestQgsMapToolCircles::testCircleZM()
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
