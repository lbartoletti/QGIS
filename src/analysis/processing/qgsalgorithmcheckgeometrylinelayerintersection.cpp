/***************************************************************************
                        qgsalgorithmcheckgeometrylinelayerintersection.cpp
                        ---------------------
   begin                : January 2024
   copyright            : (C) 2024 by Jacky Volpes
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

#include "qgsalgorithmcheckgeometrylinelayerintersection.h"
#include "qgsgeometrycheckcontext.h"
#include "qgsgeometrycheckerror.h"
#include "qgsgeometrylinelayerintersectioncheck.h"
#include "qgspoint.h"
#include "qgsvectorlayer.h"
#include "qgsvectordataproviderfeaturepool.h"

///@cond PRIVATE

auto QgsGeometryCheckLineLayerIntersectionAlgorithm::name() const -> QString
{
  return QStringLiteral( "checkgeometrylinelayerintersection" );
}

auto QgsGeometryCheckLineLayerIntersectionAlgorithm::displayName() const -> QString
{
  return QObject::tr( "Check Geometry (Line layer intersection)" );
}

auto QgsGeometryCheckLineLayerIntersectionAlgorithm::tags() const -> QStringList
{
  return QObject::tr( "check,geometry,line,layer,intersection" ).split( ',' );
}

auto QgsGeometryCheckLineLayerIntersectionAlgorithm::group() const -> QString
{
  return QObject::tr( "Check geometry" );
}

auto QgsGeometryCheckLineLayerIntersectionAlgorithm::groupId() const -> QString
{
  return QStringLiteral( "checkgeometry" );
}

auto QgsGeometryCheckLineLayerIntersectionAlgorithm::shortHelpString() const -> QString
{
  return QObject::tr( "This algorithm check the line intersection of geometry (linestring) with another layer." );
}

auto QgsGeometryCheckLineLayerIntersectionAlgorithm::flags() const -> QgsProcessingAlgorithm::Flags
{
  return QgsProcessingAlgorithm::flags() | QgsProcessingAlgorithm::FlagNoThreading;
}

auto QgsGeometryCheckLineLayerIntersectionAlgorithm::createInstance() const -> QgsGeometryCheckLineLayerIntersectionAlgorithm *
{
  return new QgsGeometryCheckLineLayerIntersectionAlgorithm();
}

void QgsGeometryCheckLineLayerIntersectionAlgorithm::initAlgorithm( const QVariantMap &configuration )
{
  Q_UNUSED( configuration )

  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INPUT" ), QObject::tr( "Input layer" ), QList< int >() << QgsProcessing::TypeVectorLine ) );
  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INTERSECTION_LAYER" ), QObject::tr( "Intersection layer" ), QList< int >() << QgsProcessing::TypeVectorLine << QgsProcessing:: TypeVectorPolygon ) );
  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "ERRORS" ), QObject::tr( "Errors layer" ), QgsProcessing::TypeVectorPoint ) );
  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "OUTPUT" ), QObject::tr( "Output layer" ), QgsProcessing::TypeVectorLine ) );

  std::unique_ptr< QgsProcessingParameterNumber > tolerance = std::make_unique< QgsProcessingParameterNumber >( QStringLiteral( "TOLERANCE" ),
      QObject::tr( "Tolerance" ), QgsProcessingParameterNumber::Integer, 8, false, 1, 13 );
  tolerance->setFlags( tolerance->flags() | QgsProcessingParameterDefinition::FlagAdvanced );
  addParameter( tolerance.release() );

}

auto QgsGeometryCheckLineLayerIntersectionAlgorithm::prepareAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback * ) -> bool
{
  mTolerance = parameterAsInt( parameters, QStringLiteral( "TOLERANCE" ), context );

  return true;
}

auto QgsGeometryCheckLineLayerIntersectionAlgorithm::createFeaturePool( QgsVectorLayer *layer, bool selectedOnly ) const -> QgsFeaturePool *
{

  return new QgsVectorDataProviderFeaturePool( layer, selectedOnly );
}

static auto outputFields( ) -> QgsFields
{
  QgsFields fields;
  fields.append( QgsField( QStringLiteral( "gc_layerid" ), QVariant::String ) );
  fields.append( QgsField( QStringLiteral( "gc_layername" ), QVariant::String ) );
  fields.append( QgsField( QStringLiteral( "gc_featid" ), QVariant::Int ) );
  fields.append( QgsField( QStringLiteral( "gc_partidx" ), QVariant::Int ) );
  fields.append( QgsField( QStringLiteral( "gc_ringidx" ), QVariant::Int ) );
  fields.append( QgsField( QStringLiteral( "gc_vertidx" ), QVariant::Int ) );
  fields.append( QgsField( QStringLiteral( "gc_errorx" ), QVariant::Double ) );
  fields.append( QgsField( QStringLiteral( "gc_errory" ), QVariant::Double ) );
  fields.append( QgsField( QStringLiteral( "gc_error" ), QVariant::String ) );
  return fields;
}


auto QgsGeometryCheckLineLayerIntersectionAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback ) -> QVariantMap
{
  QString dest_output;
  QString dest_errors;

  std::unique_ptr< QgsProcessingFeatureSource > source( parameterAsSource( parameters, QStringLiteral( "INPUT" ), context ) );
  if ( !source )
    throw QgsProcessingException( invalidSourceError( parameters, QStringLiteral( "INPUT" ) ) );

  std::unique_ptr< QgsProcessingFeatureSource > intersection_layer( parameterAsSource( parameters, QStringLiteral( "INTERSECTION_LAYER" ), context ) );
  if ( !intersection_layer )
    throw QgsProcessingException( invalidSourceError( parameters, QStringLiteral( "INTERSECTION_LAYER" ) ) );

  mInputLayer.reset( source->materialize( QgsFeatureRequest() ) );
  mIntersectionLayer.reset( intersection_layer->materialize( QgsFeatureRequest() ) );

  if ( !mInputLayer.get() )
    throw QgsProcessingException( QObject::tr( "Could not load source layer for INPUT" ) );

  if ( !mIntersectionLayer.get() )
    throw QgsProcessingException( QObject::tr( "Could not load source layer for INTERSECTION_LAYER" ) );

  QgsFields fields = outputFields();

  std::unique_ptr< QgsFeatureSink > sink_output( parameterAsSink( parameters, QStringLiteral( "OUTPUT" ), context, dest_output, fields, mInputLayer->wkbType(), mInputLayer->sourceCrs() ) );
  if ( !sink_output )
    throw QgsProcessingException( invalidSinkError( parameters, QStringLiteral( "OUTPUT" ) ) );

  std::unique_ptr< QgsFeatureSink > sink_errors( parameterAsSink( parameters, QStringLiteral( "ERRORS" ), context, dest_errors, fields, Qgis::WkbType::Point, mInputLayer->sourceCrs() ) );
  if ( !sink_errors )
    throw QgsProcessingException( invalidSinkError( parameters, QStringLiteral( "ERRORS" ) ) );

  QgsProcessingMultiStepFeedback multiStepFeedback( 3, feedback );

  QgsProject *project = mInputLayer->project() ? mInputLayer->project() : QgsProject::instance();
  std::unique_ptr<QgsGeometryCheckContext> checkContext = std::make_unique<QgsGeometryCheckContext>( mTolerance, mInputLayer->sourceCrs(), project->transformContext(), project );

  // Test detection
  QList<QgsGeometryCheckError *> checkErrors;
  QStringList messages;

  QVariantMap configurationCheck;
  configurationCheck.insert( "checkLayer", mIntersectionLayer->id() );
  const QgsGeometryLineLayerIntersectionCheck check( checkContext.get(), configurationCheck );

  multiStepFeedback.setCurrentStep( 1 );
  feedback->setProgressText( QObject::tr( "Preparing features…" ) );
  QMap<QString, QgsFeaturePool *> featurePools;
  featurePools.insert( mInputLayer->id(), createFeaturePool( mInputLayer.get() ) );
  featurePools.insert( mIntersectionLayer->id(), createFeaturePool( mIntersectionLayer.get() ) );

  multiStepFeedback.setCurrentStep( 2 );
  feedback->setProgressText( QObject::tr( "Collecting errors…" ) );
  check.collectErrors( featurePools, checkErrors, messages, feedback );

  multiStepFeedback.setCurrentStep( 3 );
  feedback->setProgressText( QObject::tr( "Exporting errors…" ) );
  double step{checkErrors.size() > 0 ? 100.0 / checkErrors.size() : 1};
  long i = 0;
  feedback->setProgress( 0.0 );


  for ( QgsGeometryCheckError *error : checkErrors )
  {

    if ( feedback->isCanceled() )
      break;

    QgsFeature f;
    QgsAttributes attrs = f.attributes();

    attrs << error->layerId()
          << mInputLayer->name()
          << error->featureId()
          << error->vidx().part
          << error->vidx().ring
          << error->vidx().vertex
          << error->location().x()
          << error->location().y()
          << error->value().toString();
    f.setAttributes( attrs );

    f.setGeometry( error->geometry() );
    if ( !sink_output->addFeature( f, QgsFeatureSink::FastInsert ) )
      throw QgsProcessingException( writeFeatureError( sink_output.get(), parameters, QStringLiteral( "OUTPUT" ) ) );

    f.setGeometry( QgsGeometry::fromPoint( QgsPoint( error->location().x(), error->location().y() ) ) );
    if ( !sink_errors->addFeature( f, QgsFeatureSink::FastInsert ) )
      throw QgsProcessingException( writeFeatureError( sink_errors.get(), parameters, QStringLiteral( "ERRORS" ) ) );

    i++;
    feedback->setProgress( 100.0 * step * static_cast<double>( i ) );
  }


  QVariantMap outputs;
  outputs.insert( QStringLiteral( "OUTPUT" ), dest_output );
  outputs.insert( QStringLiteral( "ERRORS" ), dest_errors );

  return outputs;
}

///@endcond
