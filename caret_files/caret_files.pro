######################################################################
# Automatically generated by qmake (1.04a) Wed Jan 15 08:39:18 2003
######################################################################

TARGET   = CaretFiles
TEMPLATE = lib
CONFIG  += staticlib
INCLUDEPATH += .

include(../caret_qmake_include.pro)

# Input
HEADERS += AbstractFile.h \
	   AfniHeader.h \
      AreaColorFile.h \
      ArealEstimationFile.h \
	   AtlasSpaceFile.h \
	   AtlasSurfaceDirectoryFile.h \
      BorderColorFile.h \
      BorderFile.h \
      BorderProjectionFile.h \
	   BrainVoyagerFile.h \
	   ByteSwapping.h \
      Caret6ProjectedItem.h \
      CaretScriptFile.h \
      CellClass.h \
      CellColorFile.h \
      CellFile.h \
      CellProjectionFile.h \
	   CellStudyInfo.h \
      CocomacConnectivityFile.h \
      CommaSeparatedValueFile.h \
      ContourCellColorFile.h \
      ContourCellFile.h \
	   ContourFile.h \
      ColorFile.h \
      CoordinateFile.h \
      CutsFile.h \
	   DeformationFieldFile.h \
	   DeformationMapFile.h \
	   FileException.h \
      FileFilters.h \
      FociColorFile.h \  
      FociFile.h \
      FociProjectionFile.h \
      FociSearchFile.h \
	   FreeSurferCurvatureFile.h \
	   FreeSurferFunctionalFile.h \
	   FreeSurferLabelFile.h \
	   FreeSurferSurfaceFile.h \
	   GenericXmlFile.h \
      GeodesicDistanceFile.h \
      GeodesicHelper.h \
      GiftiCommon.h \
      GiftiDataArray.h \
      GiftiDataArrayFile.h \
      GiftiDataArrayFileSaxReader.h \
      GiftiDataArrayFileStreamReader.h \
      GiftiLabelTable.h \
      GiftiMatrix.h \
      GiftiMetaData.h \
      GiftiNodeDataFile.h \
      ImageFile.h \
      LatLonFile.h \
      MDPlotFile.h \
      MetricFile.h \
      MniObjSurfaceFile.h \
      MultiResMorphFile.h \
      NeurolucidaFile.h \
      NiftiCaretExtension.h \
      NiftiFileHeader.h \
      NodeAttributeFile.h \
      NodeRegionOfInterestFile.h \
      PaintFile.h \
      PaletteFile.h \
	   ParamsFile.h \
      PreferencesFile.h \
      ProbabilisticAtlasFile.h \
      PubMedArticleFile.h \
      RgbPaintFile.h \
      SceneFile.h \
      SectionFile.h \
      SegmentationMaskListFile.h \
      SpecFile.h \
      SpecFileUtilities.h \
      StudyCollectionFile.h \
      StudyMetaDataFile.h \
      StudyMetaDataLink.h \
      StudyMetaDataLinkSet.h \
      StudyNamePubMedID.h \
	   SumsFileListFile.h \
      SureFitVectorFile.h \
      SurfaceFile.h \
      SurfaceShapeFile.h \
      TextFile.h \
      TopographyFile.h \
      TopologyFile.h \
	   TopologyHelper.h \
      TopologyNode.h \
	   TransformationMatrixFile.h \
      VectorFile.h \
      VocabularyFile.h \
	   VolumeFile.h \
      VolumeITKImage.h \
      VolumeModification.h \
	   VtkModelFile.h \
	   WuNilHeader.h \
      WustlRegionFile.h \
      XhtmlTableExtractorFile.h \
      XmlGenericWriter.h \
      XmlGenericWriterAttributes.h \
      minc_cpp.h

SOURCES += AbstractFile.cxx \
	   AfniHeader.cxx \
      AreaColorFile.cxx \
      ArealEstimationFile.cxx \
	   AtlasSpaceFile.cxx \
	   AtlasSurfaceDirectoryFile.cxx \
      BorderColorFile.cxx \
      BorderFile.cxx \
      BorderProjectionFile.cxx \
	   BrainVoyagerFile.cxx \
	   ByteSwapping.cxx \
      Caret6ProjectedItem.cxx \
      CaretScriptFile.cxx \
      CellBase.cxx \
      CellColorFile.cxx \
      CellFile.cxx \
      CellProjectionFile.cxx \
	   CellStudyInfo.cxx \
      CocomacConnectivityFile.cxx \
      ColorFile.cxx \
      CommaSeparatedValueFile.cxx \
      ContourCellColorFile.cxx \
      ContourCellFile.cxx \
	   ContourFile.cxx \
      CoordinateFile.cxx \
      CutsFile.cxx \
	   DeformationFieldFile.cxx \
	   DeformationMapFile.cxx \
	   FileException.cxx \
      FileFilters.cxx \
      FociColorFile.cxx \
      FociFile.cxx \
      FociProjectionFile.cxx \
      FociSearchFile.cxx \
	   FreeSurferCurvatureFile.cxx \
	   FreeSurferFunctionalFile.cxx \
	   FreeSurferLabelFile.cxx \
	   FreeSurferSurfaceFile.cxx \
	   GenericXmlFile.cxx \
      GeodesicDistanceFile.cxx \
      GeodesicHelper.cxx \
      GiftiCommon.cxx \
      GiftiDataArray.cxx \
      GiftiDataArrayFile.cxx \
      GiftiDataArrayFileSaxReader.cxx \
      GiftiDataArrayFileStreamReader.cxx \
      GiftiLabelTable.cxx \
      GiftiMatrix.cxx \
      GiftiMetaData.cxx \
      GiftiNodeDataFile.cxx \
      ImageFile.cxx \
      LatLonFile.cxx \
      MDPlotFile.cxx \
      MetricFile.cxx \
      MniObjSurfaceFile.cxx \
      MultiResMorphFile.cxx \
      NeurolucidaFile.cxx \
      NiftiCaretExtension.cxx \
      NiftiFileHeader.cxx \
      NodeAttributeFile.cxx \
      NodeRegionOfInterestFile.cxx \
      PaintFile.cxx \
      PaletteFile.cxx \
	   ParamsFile.cxx \
      PreferencesFile.cxx \
      ProbabilisticAtlasFile.cxx \
      PubMedArticleFile.cxx \
      RgbPaintFile.cxx \
      SceneFile.cxx \
      SectionFile.cxx \
      SegmentationMaskListFile.cxx \
      SpecFile.cxx \
      SpecFileUtilities.cxx \
      StudyCollectionFile.cxx \
      StudyMetaDataFile.cxx \
      StudyMetaDataLink.cxx \
      StudyMetaDataLinkSet.cxx \
      StudyNamePubMedID.cxx \
	   SumsFileListFile.cxx \
      SureFitVectorFile.cxx \
      SurfaceFile.cxx \
      SurfaceShapeFile.cxx \
      TextFile.cxx \
      TopographyFile.cxx \
      TopologyFile.cxx \
	   TopologyHelper.cxx \
	   TransformationMatrixFile.cxx \
      VectorFile.cxx \
      VocabularyFile.cxx \
	   VolumeFile.cxx \
      VolumeITKImage.cxx \
      VolumeModification.cxx \
	   VtkModelFile.cxx \
	   WuNilHeader.cxx \
      WustlRegionFile.cxx \
      XhtmlTableExtractorFile.cxx \
      XmlGenericWriter.cxx
