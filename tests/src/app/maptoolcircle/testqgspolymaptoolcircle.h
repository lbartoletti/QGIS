#ifndef TESTQGSPOLYMAPTOOLCIRCLE_H
#define TESTQGSPOLYMAPTOOLCIRCLE_H

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

class TestQgsPolyMapToolCircle: public QObject
{
  Q_OBJECT

  private slots:
    void initTestCase();
    void cleanupTestCase();

  protected slots:
    virtual void testCircleFrom2Points() = 0;
    virtual void testCircleFrom2PointsWithDeletedVertex() = 0;
    virtual void testCircleFrom3Points() = 0;
    virtual void testCircleFrom3PointsWithDeletedVertex() = 0;
    virtual void testCircleFromCenterPoint() = 0;
    virtual void testCircleFromCenterPointWithDeletedVertex() = 0;

  protected:
    QgisApp *mQgisApp = nullptr;
    QgsMapToolCapture *mParentTool = nullptr;
    QgsMapCanvas *mCanvas = nullptr;
    QgsVectorLayer *mLayer = nullptr;

    virtual QgsVectorLayer* getLayer() = 0;

    QgsFeatureId drawCircleFrom2Points(QgsMapToolCapture*, QgsMapCanvas*, QgsVectorLayer*);
    QgsFeatureId drawCircleFrom2PointsWithDeletedVertex(QgsMapToolCapture*, QgsMapCanvas*, QgsVectorLayer*);
    QgsFeatureId drawCircleFrom3Points(QgsMapToolCapture*, QgsMapCanvas*, QgsVectorLayer*);
    QgsFeatureId drawCircleFrom3PointsWithDeletedVertex(QgsMapToolCapture*, QgsMapCanvas*, QgsVectorLayer*);
    QgsFeatureId drawCircleFromCenterPoint(QgsMapToolCapture*, QgsMapCanvas*, QgsVectorLayer*);
    QgsFeatureId drawCircleFromCenterPointWithDeletedVertex(QgsMapToolCapture*, QgsMapCanvas*, QgsVectorLayer*);
};


void TestQgsPolyMapToolCircle::initTestCase()
{
  QgsApplication::init();
  QgsApplication::initQgis();

  mQgisApp = new QgisApp();

  mCanvas = new QgsMapCanvas();
  mCanvas->setDestinationCrs( QgsCoordinateReferenceSystem( QStringLiteral( "EPSG:27700" ) ) );

  // make testing layers
  mLayer = getLayer();
  QVERIFY( mLayer->isValid() );
  QgsProject::instance()->addMapLayers( QList<QgsMapLayer *>() << mLayer );

  // set layers in canvas
  mCanvas->setLayers( QList<QgsMapLayer *>() << mLayer );
  mCanvas->setCurrentLayer( mLayer );

  mParentTool = new QgsMapToolAddFeature( mCanvas, QgsMapToolCapture::CaptureLine );
}


void TestQgsPolyMapToolCircle::cleanupTestCase()
{
  QgsApplication::exitQgis();
}


QgsFeatureId TestQgsPolyMapToolCircle::drawCircleFrom2Points(
      QgsMapToolCapture* mParentTool, QgsMapCanvas* mCanvas, QgsVectorLayer* mLayer
    )
{
  mLayer->startEditing();
  QgsMapToolCircle2Points mapTool( mParentTool, mCanvas );
  mCanvas->setMapTool( &mapTool );

  TestQgsMapToolAdvancedDigitizingUtils utils( &mapTool );
  utils.mouseClick( 0, 0, Qt::LeftButton );
  utils.mouseMove( 0, 2 );
  utils.mouseClick( 0, 2, Qt::RightButton );
  return utils.newFeatureId();
}


QgsFeatureId TestQgsPolyMapToolCircle::drawCircleFrom2PointsWithDeletedVertex(
      QgsMapToolCapture* mParentTool, QgsMapCanvas* mCanvas, QgsVectorLayer*
    )
{
  mLayer->startEditing();
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


QgsFeatureId TestQgsPolyMapToolCircle::drawCircleFrom3Points(
      QgsMapToolCapture* mParentTool, QgsMapCanvas* mCanvas, QgsVectorLayer*
    )
{
  mLayer->startEditing();
  QgsMapToolCircle3Points mapTool( mParentTool, mCanvas );
  mCanvas->setMapTool( &mapTool );

  TestQgsMapToolAdvancedDigitizingUtils utils( &mapTool );
  utils.mouseClick( 0, 0, Qt::LeftButton );
  utils.mouseClick( 0, 2, Qt::LeftButton );
  utils.mouseMove( 1, 1 );
  utils.mouseClick( 1, 1, Qt::RightButton );
  return utils.newFeatureId();
}


QgsFeatureId TestQgsPolyMapToolCircle::drawCircleFrom3PointsWithDeletedVertex(
      QgsMapToolCapture* mParentTool, QgsMapCanvas* mCanvas, QgsVectorLayer*
    )
{
  mLayer->startEditing();
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


QgsFeatureId TestQgsPolyMapToolCircle::drawCircleFromCenterPoint(
      QgsMapToolCapture* mParentTool, QgsMapCanvas* mCanvas, QgsVectorLayer*
    )
{
  mLayer->startEditing();
  QgsMapToolCircleCenterPoint mapTool( mParentTool, mCanvas );
  mCanvas->setMapTool( &mapTool );

  TestQgsMapToolAdvancedDigitizingUtils utils( &mapTool );
  utils.mouseClick( 0, 0, Qt::LeftButton );
  utils.mouseMove( 0, 2 );
  utils.mouseClick( 0, 2, Qt::RightButton );
  return utils.newFeatureId();
}


QgsFeatureId TestQgsPolyMapToolCircle::drawCircleFromCenterPointWithDeletedVertex(
      QgsMapToolCapture* mParentTool, QgsMapCanvas* mCanvas, QgsVectorLayer*
    )
{
  mLayer->startEditing();
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


#endif // TESTQGSPOLYMAPTOOLCIRCLE_H
