/***************************************************************************
                        qgsalgorithmcheckgeometrysegmentlength.h
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

#ifndef QGSALGORITHMCHECKGEOMETRYSEGMENTLENGTH_H
#define QGSALGORITHMCHECKGEOMETRYSEGMENTLENGTH_H

#define SIP_NO_FILE

#include "qgis_sip.h"
#include "qgsprocessingalgorithm.h"
#include "qgsfeaturepool.h"

///@cond PRIVATE

class QgsGeometryCheckSegmentLengthAlgorithm : public QgsProcessingAlgorithm
{
  public:

    QgsGeometryCheckSegmentLengthAlgorithm() = default;
    void initAlgorithm( const QVariantMap &configuration = QVariantMap() ) override;
    QString name() const override;
    QString displayName() const override;
    QStringList tags() const override;
    QString group() const override;
    QString groupId() const override;
    QString shortHelpString() const override;
    Flags flags() const override;
    QgsGeometryCheckSegmentLengthAlgorithm *createInstance() const override SIP_FACTORY;

  protected:

    bool prepareAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback ) override;
    QVariantMap processAlgorithm( const QVariantMap &parameters,
                                  QgsProcessingContext &context, QgsProcessingFeedback *feedback ) override;
    bool supportInPlaceEdit( const QgsMapLayer *layer ) const override;

  private:
    QgsFeaturePool *createFeaturePool( QgsVectorLayer *layer, bool selectedOnly = false ) const;

    std::unique_ptr< QgsVectorLayer > mInputLayer;
    int mTolerance{8};
    bool mIsInPlace{false};
};

///@endcond PRIVATE

#endif // QGSALGORITHMCHECKGEOMETRYSEGMENTLENGTH_H
