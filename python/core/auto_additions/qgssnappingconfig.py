# The following has been generated automatically from src/core/qgssnappingconfig.h
QgsSnappingConfig.SnappingMode.baseClass = QgsSnappingConfig
# monkey patching scoped based enum
QgsSnappingConfig.SnappingTypes.NoSnapFlag.__doc__ = "No snapping"
QgsSnappingConfig.SnappingTypes.VertexFlag.__doc__ = "On vertices"
QgsSnappingConfig.SnappingTypes.SegmentFlag.__doc__ = "On segments"
QgsSnappingConfig.SnappingTypes.AreaFlag.__doc__ = "On Area"
QgsSnappingConfig.SnappingTypes.CentroidFlag.__doc__ = "On centroid"
QgsSnappingConfig.SnappingTypes.MiddleOfSegmentFlag.__doc__ = "On Middle segment"
QgsSnappingConfig.SnappingTypes.__doc__ = 'SnappingTypeFlag defines on what object the snapping is performed\n\n.. versionadded:: 3.12\n\n' + '* ``NoSnapFlag``: ' + QgsSnappingConfig.SnappingTypes.NoSnapFlag.__doc__ + '\n' + '* ``VertexFlag``: ' + QgsSnappingConfig.SnappingTypes.VertexFlag.__doc__ + '\n' + '* ``SegmentFlag``: ' + QgsSnappingConfig.SnappingTypes.SegmentFlag.__doc__ + '\n' + '* ``AreaFlag``: ' + QgsSnappingConfig.SnappingTypes.AreaFlag.__doc__ + '\n' + '* ``CentroidFlag``: ' + QgsSnappingConfig.SnappingTypes.CentroidFlag.__doc__ + '\n' + '* ``MiddleOfSegmentFlag``: ' + QgsSnappingConfig.SnappingTypes.MiddleOfSegmentFlag.__doc__
# --
QgsSnappingConfig.SnappingTypes.baseClass = QgsSnappingConfig
QgsSnappingConfig.SnappingTypeFlag.baseClass = QgsSnappingConfig
SnappingTypeFlag = QgsSnappingConfig  # dirty hack since SIP seems to introduce the flags in module
