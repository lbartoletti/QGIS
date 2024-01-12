/***************************************************************************
                        qgsalgorithmcheckgeometrycontained.cpp
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

#include <algorithm>

#include "qgsalgorithmcheckgeometrycontained.h"
#include "qgsgeometrycheckcontext.h"
#include "qgsgeometrycheckerror.h"
#include "qgsgeometrycontainedcheck.h"
#include "qgspoint.h"
#include "qgsvectorlayer.h"
#include "qgsvectordataproviderfeaturepool.h"

///@cond PRIVATE

auto QgsGeometryCheckContainedAlgorithm::name() const -> QString
{
  return QStringLiteral( "checkgeometrycontained" );
}

auto QgsGeometryCheckContainedAlgorithm::displayName() const -> QString
{
  return QObject::tr( "Check Geometry (Contained)" );
}

auto QgsGeometryCheckContainedAlgorithm::tags() const -> QStringList
{
  return QObject::tr( "check,geometry,contained" ).split( ',' );
}

auto QgsGeometryCheckContainedAlgorithm::group() const -> QString
{
  return QObject::tr( "Check geometry" );
}

auto QgsGeometryCheckContainedAlgorithm::groupId() const -> QString
{
  return QStringLiteral( "checkgeometry" );
}

auto QgsGeometryCheckContainedAlgorithm::shortHelpString() const -> QString
{
  return QObject::tr( "This algorithm checks the geometries contained in other geometries." );
}

auto QgsGeometryCheckContainedAlgorithm::flags() const -> QgsProcessingAlgorithm::Flags
{
  return QgsProcessingAlgorithm::flags() | QgsProcessingAlgorithm::FlagNoThreading;
}

auto QgsGeometryCheckContainedAlgorithm::createInstance() const -> QgsGeometryCheckContainedAlgorithm *
{
  return new QgsGeometryCheckContainedAlgorithm();
}

void QgsGeometryCheckContainedAlgorithm::initAlgorithm( const QVariantMap &configuration )
{
  Q_UNUSED( configuration )

  addParameter( new QgsProcessingParameterMultipleLayers( QStringLiteral( "INPUTS" ), QObject::tr( "Input layers (must contain a polygon type)" ) ) );
  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "ERRORS" ), QObject::tr( "Errors layer" ), QgsProcessing::TypeVectorPoint ) );
  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "OUTPUT" ), QObject::tr( "Output layer" ), QgsProcessing::TypeVectorAnyGeometry ) );

  std::unique_ptr< QgsProcessingParameterNumber > tolerance = std::make_unique< QgsProcessingParameterNumber >( QStringLiteral( "TOLERANCE" ),
      QObject::tr( "Tolerance" ), QgsProcessingParameterNumber::Integer, 8, false, 1, 13 );
  tolerance->setFlags( tolerance->flags() | QgsProcessingParameterDefinition::FlagAdvanced );
  addParameter( tolerance.release() );
}

auto QgsGeometryCheckContainedAlgorithm::prepareAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback * ) -> bool
{
  mTolerance = parameterAsInt( parameters, QStringLiteral( "TOLERANCE" ), context );

  return true;
}

auto QgsGeometryCheckContainedAlgorithm::createFeaturePool( QgsVectorLayer *layer, bool selectedOnly ) const -> QgsFeaturePool *
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


auto QgsGeometryCheckContainedAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback ) -> QVariantMap
{
  QString dest_output;
  QString dest_errors;
  QList<QgsMapLayer *> input_layers = parameterAsLayerList( parameters, QStringLiteral( "INPUTS" ), context );
  if ( input_layers.isEmpty() )
    throw QgsProcessingException( invalidSourceError( parameters, QStringLiteral( "INPUTS" ) ) );

  // There should be at least one polygon layer in the input layers
  auto polygon_geometry_type = []( QgsMapLayer * input_layer )
  {
    if ( auto vlayer = dynamic_cast<QgsVectorLayer *>( input_layer ) )
      return vlayer->geometryType() == Qgis::GeometryType::Polygon;
    return false;
  };
  if ( std::none_of( input_layers.cbegin(), input_layers.cend(), polygon_geometry_type ) )
    throw QgsProcessingException( QObject::tr( "No polygon layer in input layers selection" ) );

  QgsFields fields = outputFields();

  std::unique_ptr< QgsFeatureSink > sink_errors( parameterAsSink( parameters, QStringLiteral( "ERRORS" ), context, dest_errors, fields, Qgis::WkbType::Point, QgsProject::instance()->crs() ) );
  if ( !sink_errors )
    throw QgsProcessingException( invalidSinkError( parameters, QStringLiteral( "ERRORS" ) ) );

  QgsProcessingMultiStepFeedback multiStepFeedback( 3, feedback );

  std::unique_ptr<QgsGeometryCheckContext> checkContext = std::make_unique<QgsGeometryCheckContext>( mTolerance, QgsProject::instance()->crs(), QgsProject::instance()->transformContext(), QgsProject::instance() );

  // Test detection
  QList<QgsGeometryCheckError *> checkErrors;
  QStringList messages;

  const QgsGeometryContainedCheck check( checkContext.get(), QVariantMap() );

  multiStepFeedback.setCurrentStep( 1 );
  feedback->setProgressText( QObject::tr( "Preparing features…" ) );
  QMap<QString, QgsFeaturePool *> featurePools;
  for ( auto input_layer : input_layers )
  {
    auto vlayer = dynamic_cast<QgsVectorLayer *>( input_layer );
    if ( vlayer )
      featurePools.insert( vlayer->id(), createFeaturePool( vlayer ) );
  }
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
          << QgsProject::instance()->mapLayer( error->layerId() )->name()
          << error->featureId()
          << error->vidx().part
          << error->vidx().ring
          << error->vidx().vertex
          << error->location().x()
          << error->location().y()
          << error->value().toString();
    f.setAttributes( attrs );

    f.setGeometry( QgsGeometry::fromPoint( QgsPoint( error->location().x(), error->location().y() ) ) );
    if ( !sink_errors->addFeature( f, QgsFeatureSink::FastInsert ) )
      throw QgsProcessingException( writeFeatureError( sink_errors.get(), parameters, QStringLiteral( "ERRORS" ) ) );

    i++;
    feedback->setProgress( 100.0 * step * static_cast<double>( i ) );
  }

  QVariantMap outputs;
  outputs.insert( QStringLiteral( "ERRORS" ), dest_errors );

  return outputs;
}

///@endcond
