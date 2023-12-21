/***************************************************************************
                         testqgsprocessingcheckgeometry.cpp
                         ---------------------
    begin                : December 2023
    copyright            : (C) 2023 by Jacky Volpes
    email                : jacky dot volpes at oslandia dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qgsnativealgorithms.h"
#include "qgsprocessingregistry.h"
#include "qgstest.h"
#include "qgsvectorlayer.h"

class TestQgsProcessingCheckGeometry: public QgsTest
{
    Q_OBJECT

  public:
    TestQgsProcessingCheckGeometry() : QgsTest( QStringLiteral( "Processing Algorithms Check Geometry" ) ) {}

  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase(); // will be called after the last testfunction was executed.
    void init() {} // will be called before each testfunction is executed.
    void cleanup() {} // will be called after every testfunction.

    void areaAlg();
    void dangleAlg();

    void angleAlg_data();
    void angleAlg();

    void segmentLengthAlg_data();
    void segmentLengthAlg();

    void multipartAlg_data();
    void multipartAlg();

  private:

    QgsVectorLayer *mLineLayer = nullptr;
    QgsVectorLayer *mPolygonLayer = nullptr;

    void exportToSpreadsheet( const QString &outputPath );
};

void TestQgsProcessingCheckGeometry::initTestCase()
{
  QgsApplication::init();
  QgsApplication::initQgis();

  // Set up the QgsSettings environment
  QCoreApplication::setOrganizationName( QStringLiteral( "QGIS" ) );
  QCoreApplication::setOrganizationDomain( QStringLiteral( "qgis.org" ) );
  QCoreApplication::setApplicationName( QStringLiteral( "QGIS-TEST" ) );

  QgsApplication::processingRegistry()->addProvider( new QgsNativeAlgorithms( QgsApplication::processingRegistry() ) );

  const QDir testDataDir( QDir( TEST_DATA_DIR ).absoluteFilePath( "geometry_checker" ) );

  //create a line layer that will be used in tests
  mLineLayer = new QgsVectorLayer( testDataDir.absoluteFilePath( "line_layer.shp" ), QStringLiteral( "lines" ), QStringLiteral( "ogr" ) );
  // Register the layer with the registry
  QgsProject::instance()->addMapLayers( QList<QgsMapLayer *>() << mLineLayer );
  QVERIFY( mLineLayer->isValid() );

  //create a poly layer that will be used in tests
  mPolygonLayer = new QgsVectorLayer( testDataDir.absoluteFilePath( "polygon_layer.shp" ), QStringLiteral( "polygons" ), QStringLiteral( "ogr" ) );
  // Register the layer with the registry
  QgsProject::instance()->addMapLayers( QList<QgsMapLayer *>() << mPolygonLayer );
  QVERIFY( mPolygonLayer->isValid() );
}

void TestQgsProcessingCheckGeometry::cleanupTestCase()
{
  QgsApplication::exitQgis();
}

void TestQgsProcessingCheckGeometry::angleAlg_data()
{
  QTest::addColumn<QgsVectorLayer *>( "layerToTest" );
  QTest::addColumn<int>( "expectedErrorCount" );
  QTest::newRow( "Line layer" ) << mLineLayer << 4;
  QTest::newRow( "Polygon layer" ) << mPolygonLayer << 4;
}

void TestQgsProcessingCheckGeometry::angleAlg()
{
  QFETCH( QgsVectorLayer *, layerToTest );
  QFETCH( int, expectedErrorCount );

  std::unique_ptr< QgsProcessingAlgorithm > alg(
    QgsApplication::processingRegistry()->createAlgorithmById( QStringLiteral( "native:checkgeometryangle" ) )
  );
  QVERIFY( alg != nullptr );

  QVariantMap parameters;
  parameters.insert( QStringLiteral( "INPUT" ), QVariant::fromValue( layerToTest ) );
  parameters.insert( QStringLiteral( "MIN_ANGLE" ), 15 );
  parameters.insert( QStringLiteral( "OUTPUT" ), QgsProcessing::TEMPORARY_OUTPUT );
  parameters.insert( QStringLiteral( "ERRORS" ), QgsProcessing::TEMPORARY_OUTPUT );

  bool ok = false;
  QgsProcessingFeedback feedback;
  std::unique_ptr< QgsProcessingContext > context = std::make_unique< QgsProcessingContext >();

  QVariantMap results;
  results = alg->run( parameters, *context, &feedback, &ok );
  QVERIFY( ok );

  std::unique_ptr<QgsVectorLayer> outputLayer( qobject_cast< QgsVectorLayer * >( context->getMapLayer( results.value( QStringLiteral( "OUTPUT" ) ).toString() ) ) );
  std::unique_ptr<QgsVectorLayer> errorsLayer( qobject_cast< QgsVectorLayer * >( context->getMapLayer( results.value( QStringLiteral( "ERRORS" ) ).toString() ) ) );
  QVERIFY( outputLayer->isValid() );
  QVERIFY( errorsLayer->isValid() );
  QCOMPARE( outputLayer->featureCount(), expectedErrorCount );
  QCOMPARE( errorsLayer->featureCount(), expectedErrorCount );
}

void TestQgsProcessingCheckGeometry::segmentLengthAlg_data()
{
  QTest::addColumn<QgsVectorLayer *>( "layerToTest" );
  QTest::addColumn<int>( "expectedErrorCount" );
  QTest::newRow( "Line layer" ) << mLineLayer << 1;
  QTest::newRow( "Polygon layer" ) << mPolygonLayer << 3;
}

void TestQgsProcessingCheckGeometry::segmentLengthAlg()
{
  QFETCH( QgsVectorLayer *, layerToTest );
  QFETCH( int, expectedErrorCount );

  std::unique_ptr< QgsProcessingAlgorithm > alg(
    QgsApplication::processingRegistry()->createAlgorithmById( QStringLiteral( "native:checkgeometrysegmentlength" ) )
  );
  QVERIFY( alg != nullptr );

  QVariantMap parameters;
  parameters.insert( QStringLiteral( "INPUT" ), QVariant::fromValue( layerToTest ) );
  parameters.insert( QStringLiteral( "MIN_SEGMENT_LENGTH" ), 0.03 );
  parameters.insert( QStringLiteral( "OUTPUT" ), QgsProcessing::TEMPORARY_OUTPUT );
  parameters.insert( QStringLiteral( "ERRORS" ), QgsProcessing::TEMPORARY_OUTPUT );

  bool ok = false;
  QgsProcessingFeedback feedback;
  std::unique_ptr< QgsProcessingContext > context = std::make_unique< QgsProcessingContext >();

  QVariantMap results;
  results = alg->run( parameters, *context, &feedback, &ok );
  QVERIFY( ok );

  std::unique_ptr<QgsVectorLayer> outputLayer( qobject_cast< QgsVectorLayer * >( context->getMapLayer( results.value( QStringLiteral( "OUTPUT" ) ).toString() ) ) );
  std::unique_ptr<QgsVectorLayer> errorsLayer( qobject_cast< QgsVectorLayer * >( context->getMapLayer( results.value( QStringLiteral( "ERRORS" ) ).toString() ) ) );
  QVERIFY( outputLayer->isValid() );
  QVERIFY( errorsLayer->isValid() );
  QCOMPARE( outputLayer->featureCount(), expectedErrorCount );
  QCOMPARE( errorsLayer->featureCount(), expectedErrorCount );
}

void TestQgsProcessingCheckGeometry::areaAlg()
{
  std::unique_ptr< QgsProcessingAlgorithm > alg(
    QgsApplication::processingRegistry()->createAlgorithmById( QStringLiteral( "native:checkgeometryarea" ) )
  );
  QVERIFY( alg != nullptr );

  QVariantMap parameters;
  parameters.insert( QStringLiteral( "INPUT" ), QVariant::fromValue( mPolygonLayer ) );
  parameters.insert( QStringLiteral( "AREATHRESHOLD" ), 0.04 );
  parameters.insert( QStringLiteral( "OUTPUT" ), QgsProcessing::TEMPORARY_OUTPUT );
  parameters.insert( QStringLiteral( "ERRORS" ), QgsProcessing::TEMPORARY_OUTPUT );

  bool ok = false;
  QgsProcessingFeedback feedback;
  std::unique_ptr< QgsProcessingContext > context = std::make_unique< QgsProcessingContext >();

  QVariantMap results;
  results = alg->run( parameters, *context, &feedback, &ok );
  QVERIFY( ok );

  std::unique_ptr<QgsVectorLayer> outputLayer( qobject_cast< QgsVectorLayer * >( context->getMapLayer( results.value( QStringLiteral( "OUTPUT" ) ).toString() ) ) );
  std::unique_ptr<QgsVectorLayer> errorsLayer( qobject_cast< QgsVectorLayer * >( context->getMapLayer( results.value( QStringLiteral( "ERRORS" ) ).toString() ) ) );
  QVERIFY( outputLayer->isValid() );
  QVERIFY( errorsLayer->isValid() );
  QCOMPARE( outputLayer->featureCount(), 8 );
  QCOMPARE( errorsLayer->featureCount(), 8 );
}

void TestQgsProcessingCheckGeometry::dangleAlg()
{
  std::unique_ptr< QgsProcessingAlgorithm > alg(
    QgsApplication::processingRegistry()->createAlgorithmById( QStringLiteral( "native:checkgeometrydangle" ) )
  );
  QVERIFY( alg != nullptr );

  QVariantMap parameters;
  parameters.insert( QStringLiteral( "INPUT" ), QVariant::fromValue( mLineLayer ) );
  parameters.insert( QStringLiteral( "OUTPUT" ), QgsProcessing::TEMPORARY_OUTPUT );
  parameters.insert( QStringLiteral( "ERRORS" ), QgsProcessing::TEMPORARY_OUTPUT );

  bool ok = false;
  QgsProcessingFeedback feedback;
  std::unique_ptr< QgsProcessingContext > context = std::make_unique< QgsProcessingContext >();

  QVariantMap results;
  results = alg->run( parameters, *context, &feedback, &ok );
  QVERIFY( ok );

  std::unique_ptr<QgsVectorLayer> outputLayer( qobject_cast< QgsVectorLayer * >( context->getMapLayer( results.value( QStringLiteral( "OUTPUT" ) ).toString() ) ) );
  std::unique_ptr<QgsVectorLayer> errorsLayer( qobject_cast< QgsVectorLayer * >( context->getMapLayer( results.value( QStringLiteral( "ERRORS" ) ).toString() ) ) );
  QVERIFY( outputLayer->isValid() );
  QVERIFY( errorsLayer->isValid() );
  QCOMPARE( outputLayer->featureCount(), 6 );
  QCOMPARE( errorsLayer->featureCount(), 6 );
}

void TestQgsProcessingCheckGeometry::multipartAlg_data()
{
  QTest::addColumn<QgsVectorLayer *>( "layerToTest" );
  QTest::addColumn<int>( "expectedErrorCount" );
  QTest::newRow( "Line layer" ) << mLineLayer << 8;
  QTest::newRow( "Polygon layer" ) << mPolygonLayer << 24;
}

void TestQgsProcessingCheckGeometry::multipartAlg()
{
  QFETCH( QgsVectorLayer *, layerToTest );
  QFETCH( int, expectedErrorCount );
  std::unique_ptr< QgsProcessingAlgorithm > alg(
    QgsApplication::processingRegistry()->createAlgorithmById( QStringLiteral( "native:checkgeometrymultipart" ) )
  );
  QVERIFY( alg != nullptr );

  QVariantMap parameters;
  parameters.insert( QStringLiteral( "INPUT" ), QVariant::fromValue( layerToTest ) );
  parameters.insert( QStringLiteral( "OUTPUT" ), QgsProcessing::TEMPORARY_OUTPUT );
  parameters.insert( QStringLiteral( "ERRORS" ), QgsProcessing::TEMPORARY_OUTPUT );

  bool ok = false;
  QgsProcessingFeedback feedback;
  std::unique_ptr< QgsProcessingContext > context = std::make_unique< QgsProcessingContext >();

  QVariantMap results;
  results = alg->run( parameters, *context, &feedback, &ok );
  QVERIFY( ok );

  std::unique_ptr<QgsVectorLayer> outputLayer( qobject_cast< QgsVectorLayer * >( context->getMapLayer( results.value( QStringLiteral( "OUTPUT" ) ).toString() ) ) );
  std::unique_ptr<QgsVectorLayer> errorsLayer( qobject_cast< QgsVectorLayer * >( context->getMapLayer( results.value( QStringLiteral( "ERRORS" ) ).toString() ) ) );
  QVERIFY( outputLayer->isValid() );
  QVERIFY( errorsLayer->isValid() );
  QCOMPARE( outputLayer->featureCount(), expectedErrorCount );
  QCOMPARE( errorsLayer->featureCount(), expectedErrorCount );
}

QGSTEST_MAIN( TestQgsProcessingCheckGeometry )
#include "testqgsprocessingcheckgeometry.moc"
