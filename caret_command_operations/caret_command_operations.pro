######################################################################
# Automatically generated by qmake (1.04a) Tue Jan 14 11:58:13 2003
######################################################################

TARGET   = CaretCommandOperations
CONFIG   += staticlib
INCLUDEPATH += .

include(../caret_qmake_include.pro)  

TEMPLATE = lib


# Input
HEADERS += CommandBase.h \
           CommandCaretFileNamingUnitTesting.h \
           CommandCaretHelpCreateHtmlIndexFile.h \
           CommandColorFileAddColor.h \
           CommandDataFileCompare.h \
           CommandConvertDataFileToCaret6.h \
           CommandConvertSpecFileToCaret6.h \
           CommandColorFileCreateMissingColors.h \
           CommandDeformationMapApply.h \
           CommandDeformationMapApplyGenericNames.h \
           CommandDeformationMapCreate.h \
           CommandDeformationMapPathUpdate.h \
           CommandException.h \
           CommandExtend.h \
           CommandFileConvert.h \
           CommandFileReadTime.h \
           CommandFileSubstitution.h \
           CommandGiftiInfo.h \
           CommandImageCombine.h \
           CommandImageCompare.h \
           CommandImageFormatConvert.h \
           CommandImageInsertText.h \
           CommandImageResize.h \
           CommandImageToWebPage.h \
           CommandImageView.h \
           CommandHelp.h \
           CommandHelpFull.h \
           CommandHelpGlobalOptions.h \
           CommandHelpHTML.h \
           CommandHelpPDF.h \
           CommandHelpSearch.h \
           CommandMetricClustering.h \
           CommandMetricComposite.h \
           CommandMetricCompositeIdentifiedColumns.h \
           CommandMetricCorrelationCoefficientMap.h \
           CommandMetricExtrema.h \
           CommandMetricFileCreate.h \
           CommandMetricGradient.h \
           CommandMetricInGroupDifference.h \
           CommandMetricInformation.h \
           CommandMetricMath.h \
           CommandMetricMathPostfix.h \
           CommandMetricMultipleCorrelationCoefficientMap.h \
           CommandMetricSetColumnName.h \
           CommandMetricSetColumnToScalar.h \
           CommandMetricSmoothing.h \
           CommandMetricStatisticsAnovaOneWay.h \
           CommandMetricStatisticsAnovaTwoWay.h \
           CommandMetricStatisticsCoordinateDifference.h \
           CommandMetricStatisticsInterhemisphericClusters.h \
           CommandMetricStatisticsKruskalWallis.h \
           CommandMetricStatisticsLeveneMap.h \
           CommandMetricStatisticsNormalization.h \
           CommandMetricStatisticsOneSampleTTest.h \
           CommandMetricStatisticsPairedTTest.h \
           CommandMetricStatisticsShuffledCrossCorrelationMaps.h \
           CommandMetricStatisticsShuffledTMap.h \
           CommandMetricStatisticsSubtraceGroupAverage.h \
           CommandMetricStatisticsTMap.h \
           CommandMetricStatisticsTwoSampleTTest.h \
           CommandMetricStatisticsZMap.h \
           CommandMetricTwinComparison.h \
           CommandMetricTwinPairedDataDiffs.h \
           CommandPaintAddColumns.h \
           CommandPaintAssignNodes.h \
           CommandPaintAssignNodesRelativeToLine.h \
           CommandPaintComposite.h \
           CommandPaintDilation.h \
           CommandPaintFileCreate.h \
           CommandPaintLabelNameUpdate.h \
           CommandPaintSetColumnName.h \
           CommandPreferencesFileSettings.h \
           CommandSceneCreate.h \
           CommandScriptComment.h \
           CommandScriptConvert.h \
           CommandScriptRun.h \
           CommandScriptVariableRead.h \
           CommandScriptVariableSet.h \
           CommandShowScene.h \
           CommandShowSurface.h \
           CommandShowVolume.h \
           CommandSpecFileAdd.h \
           CommandSpecFileChangeResolution.h \
           CommandSpecFileClean.h \
           CommandSpecFileCopy.h \
           CommandSpecFileCreate.h \
           CommandSpecFileDirectoryClean.h \
           CommandSpecFileZip.h \
           CommandStatisticSetRandomSeed.h \
           CommandStatisticalUnitTesting.h \
           CommandStereotaxicSpaces.h \
           CommandStudyMetaDataFileDuplicates.h \
           CommandStudyMetaDataFileValidate.h \
           CommandSurfaceAffineRegression.h \
           CommandSurfaceAlignToStandardOrientation.h \
           CommandSurfaceApplyTransformationMatrix.h \
           CommandSurfaceAverage.h \
           CommandSurfaceBankStraddling.h \
           CommandSurfaceBorderCreateAverage.h \
           CommandSurfaceBorderCreateParallelBorder.h \
           CommandSurfaceBorderCutter.h \
           CommandSurfaceBorderDelete.h \
           CommandSurfaceBorderDrawAroundROI.h \
           CommandSurfaceBorderDrawGeodesic.h \
           CommandSurfaceBorderDrawMetric.h \
           CommandSurfaceBorderFileMerge.h \
           CommandSurfaceBorderIntersection.h \
           CommandSurfaceBorderLandmarkIdentification.h \
           CommandSurfaceBorderLengths.h \
           CommandSurfaceBorderLinkToFocus.h \
           CommandSurfaceBorderMerge.h \
           CommandSurfaceBorderNibbler.h \
           CommandSurfaceBorderProjection.h \
           CommandSurfaceBorderResample.h \
           CommandSurfaceBorderReverse.h \
           CommandSurfaceBorderSetVariability.h \
           CommandSurfaceBorderToMetric.h \
           CommandSurfaceBorderToPaint.h \
           CommandSurfaceBorderUnprojection.h \
           CommandSurfaceBorderVariability.h \
           CommandSurfaceCellCreate.h \
           CommandSurfaceCellProjection.h \
           CommandSurfaceCellUnprojection.h \
           CommandSurfaceCrossoverCheck.h \
           CommandSurfaceCurvature.h \
           CommandSurfaceDistortion.h \
           CommandSurfaceFlatMultiResMorphing.h \
           CommandSurfaceFlatten.h \
           CommandSurfaceFociAttributeAssignment.h \
           CommandSurfaceFociCreate.h \
           CommandSurfaceFociDelete.h \
           CommandSurfaceFociProjection.h \
           CommandSurfaceFociProjectionPals.h \
           CommandSurfaceFociReassignStudyNames.h \
           CommandSurfaceFociStudyValidate.h \
           CommandSurfaceFociUnprojection.h \
           CommandSurfaceInflate.h \
           CommandSurfaceInformation.h \
           CommandSurfaceGenerateInflated.h \
           CommandSurfaceGeodesic.h \
           CommandSurfaceNormals.h \
           CommandSurfacePlaceFociAtExtremum.h \
           CommandSurfacePlaceFociAtLimits.h \
           CommandSurfaceRegionOfInterestSelection.h \
           CommandSurfaceRegistrationPrepareSlits.h \
           CommandSurfaceRegistrationSpherical.h \
           CommandSurfaceRegistrationSphericalSpecOnly.h \
           CommandSurfaceRoiCoordReport.h \
           CommandSurfaceRoiFoldingMeasures.h \
           CommandSurfaceRoiNodeAreas.h \
           CommandSurfaceRoiShapeMeasures.h \
           CommandSurfaceRoiStatisticalReport.h \
           CommandSurfaceSmoothing.h \
           CommandSurfaceSphericalMultiResMorphing.h \
           CommandSurfaceSulcalIdentificationProbabilistic.h \
           CommandSurfaceSulcalDepth.h \
           CommandSurfaceIdentifySulci.h \
           CommandSurfaceToCArrays.h \
           CommandSurfaceToCerebralHull.h \
           CommandSurfaceToSegmentationVolume.h \
           CommandSurfaceToVolume.h \
           CommandSurfaceTopologyDisconnectNodes.h \
           CommandSurfaceTopologyNeighbors.h \
           CommandSurfaceTopologyReport.h \
           CommandSurfaceTransformToStandardView.h \
           CommandSurfacesToSegmentationVolumeMask.h \
           CommandSystemCommandExecute.h \
           CommandSystemFileCopy.h \
           CommandSystemFileDelete.h \
           CommandTransformationMatrixCreate.h \
           CommandVerify.h \
           CommandVersion.h \
           CommandVolumeAnatomyPeaks.h \
           CommandVolumeBiasCorrection.h \
           CommandVolumeBlur.h \
           CommandVolumeClassifyIntensities.h \
           CommandVolumeConvertVectorToVolume.h \
           CommandVolumeCreate.h \
           CommandVolumeCreateCorpusCallosumSlice.h \
           CommandVolumeCreateInStereotaxicSpace.h \
           CommandVolumeDilate.h \
           CommandVolumeDilateErode.h \
           CommandVolumeDilateErodeWithinMask.h \
           CommandVolumeErode.h \
           CommandVolumeEulerCount.h \
           CommandVolumeFileCombine.h \
           CommandVolumeFileMerge.h \
           CommandVolumeFillBiggestObject.h \
           CommandVolumeFillHoles.h \
           CommandVolumeFillSlice.h \
           CommandVolumeFindLimits.h \
           CommandVolumeFloodFill.h \
           CommandVolumeGradient.h \
           CommandVolumeHistogram.h \
           CommandVolumeImportRawFile.h \
           CommandVolumeInformation.h \
           CommandVolumeInformationNifti.h \
           CommandVolumeMakePlane.h \
           CommandVolumeMakeRectangle.h \
           CommandVolumeMakeShell.h \
           CommandVolumeMakeSphere.h \
           CommandVolumeMapToSurface.h \
           CommandVolumeMapToSurfacePALS.h \
           CommandVolumeMapToSurfaceROIFile.h \
           CommandVolumeMapToVtkModel.h \
           CommandVolumeMaskVolume.h \
           CommandVolumeMaskWithVolume.h \
           CommandVolumeNearToPlane.h \
           CommandVolumePadVolume.h \
           CommandVolumeProbAtlasToFunctional.h \
           CommandVolumeReplaceVectorMagnitudeWithVolume.h \
           CommandVolumeReplaceVoxelsWithVectorMagnitude.h \
           CommandVolumeRemoveIslands.h \
           CommandVolumeResample.h \
           CommandVolumeRescaleVoxels.h \
           CommandVolumeResize.h \
           CommandVolumeScale0to255.h \
           CommandVolumeScalePercent0to255.h \
           CommandVolumeSculpt.h \
           CommandVolumeSegmentation.h \
           CommandVolumeSegmentationLigase.h \
           CommandVolumeSegmentationStereotaxicSpace.h \
           CommandVolumeSegmentationToCerebralHull.h \
           CommandVolumeSetOrientation.h \
           CommandVolumeSetOrigin.h \
           CommandVolumeSetSpacing.h \
           CommandVolumeShiftAxis.h \
           CommandVolumeSmearAxis.h \
           CommandVolumeTFCE.h \
           CommandVolumeThreshold.h \
           CommandVolumeThresholdDual.h \
           CommandVolumeThresholdInverse.h \
           CommandVolumeFslToVector.h \
           CommandVolumeTopologyCorrector.h \
           CommandVolumeTopologyGraph.h \
           CommandVolumeTopologyReport.h \
           CommandVolumeVectorCombine.h \
           OffScreenOpenGLWidget.h \
           ScriptBuilderParameters.h

SOURCES += CommandBase.cxx \
           CommandCaretFileNamingUnitTesting.cxx \
           CommandCaretHelpCreateHtmlIndexFile.cxx \
           CommandColorFileAddColor.cxx \
           CommandColorFileCreateMissingColors.cxx \
           CommandConvertDataFileToCaret6.cxx \
           CommandConvertSpecFileToCaret6.cxx \
           CommandDataFileCompare.cxx \
           CommandDeformationMapApply.cxx \
           CommandDeformationMapApplyGenericNames.cxx \
           CommandDeformationMapCreate.cxx \
           CommandDeformationMapPathUpdate.cxx \
           CommandException.cxx \
           CommandExtend.cxx \
           CommandFileConvert.cxx \
           CommandFileReadTime.cxx \
           CommandFileSubstitution.cxx \
           CommandGiftiInfo.cxx \
           CommandHelp.cxx \
           CommandHelpFull.cxx \
           CommandHelpGlobalOptions.cxx \
           CommandHelpHTML.cxx \
           CommandHelpPDF.cxx \
           CommandHelpSearch.cxx \
           CommandImageCombine.cxx \
           CommandImageCompare.cxx \
           CommandImageFormatConvert.cxx \
           CommandImageInsertText.cxx \
           CommandImageResize.cxx \
           CommandImageToWebPage.cxx \
           CommandImageView.cxx \
           CommandMetricClustering.cxx \
           CommandMetricComposite.cxx \
           CommandMetricCompositeIdentifiedColumns.cxx \
           CommandMetricCorrelationCoefficientMap.cxx \
           CommandMetricExtrema.cxx \
           CommandMetricFileCreate.cxx \
           CommandMetricGradient.cxx \
           CommandMetricInGroupDifference.cxx \
           CommandMetricInformation.cxx \
           CommandMetricMath.cxx \
           CommandMetricMathPostfix.cxx \
           CommandMetricMultipleCorrelationCoefficientMap.cxx \
           CommandMetricSetColumnName.cxx \
           CommandMetricSetColumnToScalar.cxx \
           CommandMetricSmoothing.cxx \
           CommandMetricStatisticsAnovaOneWay.cxx \
           CommandMetricStatisticsAnovaTwoWay.cxx \
           CommandMetricStatisticsCoordinateDifference.cxx \
           CommandMetricStatisticsInterhemisphericClusters.cxx \
           CommandMetricStatisticsKruskalWallis.cxx \
           CommandMetricStatisticsLeveneMap.cxx \
           CommandMetricStatisticsNormalization.cxx \
           CommandMetricStatisticsOneSampleTTest.cxx \
           CommandMetricStatisticsPairedTTest.cxx \
           CommandMetricStatisticsShuffledCrossCorrelationMaps.cxx \
           CommandMetricStatisticsShuffledTMap.cxx \
           CommandMetricStatisticsSubtraceGroupAverage.cxx \
           CommandMetricStatisticsTMap.cxx \
           CommandMetricStatisticsTwoSampleTTest.cxx \
           CommandMetricStatisticsZMap.cxx \
           CommandMetricTwinComparison.cxx \
           CommandMetricTwinPairedDataDiffs.cxx \
           CommandPaintAddColumns.cxx \
           CommandPaintAssignNodes.cxx \
           CommandPaintAssignNodesRelativeToLine.cxx \
           CommandPaintComposite.cxx \
           CommandPaintDilation.cxx \
           CommandPaintFileCreate.cxx \
           CommandPaintLabelNameUpdate.cxx \
           CommandPaintSetColumnName.cxx \
           CommandPreferencesFileSettings.cxx \
           CommandSceneCreate.cxx \
           CommandScriptComment.cxx \
           CommandScriptConvert.cxx \
           CommandScriptRun.cxx \
           CommandScriptVariableRead.cxx \
           CommandScriptVariableSet.cxx \
           CommandShowScene.cxx \
           CommandShowSurface.cxx \
           CommandShowVolume.cxx \
           CommandSpecFileAdd.cxx \
           CommandSpecFileChangeResolution.cxx \
           CommandSpecFileClean.cxx \
           CommandSpecFileCopy.cxx \
           CommandSpecFileCreate.cxx \
           CommandSpecFileDirectoryClean.cxx \
           CommandSpecFileZip.cxx \
           CommandStatisticSetRandomSeed.cxx \
           CommandStatisticalUnitTesting.cxx \
           CommandStereotaxicSpaces.cxx \
           CommandStudyMetaDataFileDuplicates.cxx \
           CommandStudyMetaDataFileValidate.cxx \
           CommandSurfaceAffineRegression.cxx \
           CommandSurfaceAlignToStandardOrientation.cxx \
           CommandSurfaceApplyTransformationMatrix.cxx \
           CommandSurfaceAverage.cxx \
           CommandSurfaceBankStraddling.cxx \
           CommandSurfaceBorderCreateAverage.cxx \
           CommandSurfaceBorderCreateParallelBorder.cxx \
           CommandSurfaceBorderCutter.cxx \
           CommandSurfaceBorderDelete.cxx \
           CommandSurfaceBorderDrawAroundROI.cxx \
           CommandSurfaceBorderDrawGeodesic.cxx \
           CommandSurfaceBorderDrawMetric.cxx \
           CommandSurfaceBorderFileMerge.cxx \
           CommandSurfaceBorderIntersection.cxx \
           CommandSurfaceBorderLandmarkIdentification.cxx \
           CommandSurfaceBorderLengths.cxx \
           CommandSurfaceBorderLinkToFocus.cxx \
           CommandSurfaceBorderMerge.cxx \
           CommandSurfaceBorderNibbler.cxx \
           CommandSurfaceBorderProjection.cxx \
           CommandSurfaceBorderResample.cxx \
           CommandSurfaceBorderReverse.cxx \
           CommandSurfaceBorderSetVariability.cxx \
           CommandSurfaceBorderToMetric.cxx \
           CommandSurfaceBorderToPaint.cxx \
           CommandSurfaceBorderUnprojection.cxx \
           CommandSurfaceBorderVariability.cxx \
           CommandSurfaceCellCreate.cxx \
           CommandSurfaceCellProjection.cxx \
           CommandSurfaceCellUnprojection.cxx \
           CommandSurfaceCrossoverCheck.cxx \
           CommandSurfaceCurvature.cxx \
           CommandSurfaceDistortion.cxx \
           CommandSurfaceFlatMultiResMorphing.cxx \
           CommandSurfaceFlatten.cxx \
           CommandSurfaceFociAttributeAssignment.cxx \
           CommandSurfaceFociCreate.cxx \
           CommandSurfaceFociDelete.cxx \
           CommandSurfaceFociProjection.cxx \
           CommandSurfaceFociProjectionPals.cxx \
           CommandSurfaceFociReassignStudyNames.cxx \
           CommandSurfaceFociStudyValidate.cxx \
           CommandSurfaceFociUnprojection.cxx \
           CommandSurfaceGenerateInflated.cxx \
           CommandSurfaceGeodesic.cxx \
           CommandSurfaceIdentifySulci.cxx \
           CommandSurfaceInflate.cxx \
           CommandSurfaceInformation.cxx \
           CommandSurfaceNormals.cxx \
           CommandSurfacePlaceFociAtExtremum.cxx \
           CommandSurfacePlaceFociAtLimits.cxx \
           CommandSurfaceRegionOfInterestSelection.cxx \
           CommandSurfaceRegistrationPrepareSlits.cxx \
           CommandSurfaceRegistrationSpherical.cxx \
           CommandSurfaceRegistrationSphericalSpecOnly.cxx \
           CommandSurfaceRoiCoordReport.cxx \
           CommandSurfaceRoiFoldingMeasures.cxx \
           CommandSurfaceRoiNodeAreas.cxx \
           CommandSurfaceRoiShapeMeasures.cxx \
           CommandSurfaceRoiStatisticalReport.cxx \
           CommandSurfaceSmoothing.cxx \
           CommandSurfaceSphericalMultiResMorphing.cxx \
           CommandSurfaceSulcalDepth.cxx \
           CommandSurfaceSulcalIdentificationProbabilistic.cxx \
           CommandSurfaceToCArrays.cxx \
           CommandSurfaceToCerebralHull.cxx \
           CommandSurfaceToSegmentationVolume.cxx \
           CommandSurfaceToVolume.cxx \
           CommandSurfaceTopologyDisconnectNodes.cxx \
           CommandSurfaceTopologyNeighbors.cxx \
           CommandSurfaceTopologyReport.cxx \
           CommandSurfaceTransformToStandardView.cxx \
           CommandSurfacesToSegmentationVolumeMask.cxx \
           CommandSystemCommandExecute.cxx \
           CommandSystemFileCopy.cxx \
           CommandSystemFileDelete.cxx \
           CommandTransformationMatrixCreate.cxx \
           CommandVerify.cxx \
           CommandVersion.cxx \
           CommandVolumeAnatomyPeaks.cxx \
           CommandVolumeBiasCorrection.cxx \
           CommandVolumeBlur.cxx \
           CommandVolumeClassifyIntensities.cxx \
           CommandVolumeConvertVectorToVolume.cxx \
           CommandVolumeCreate.cxx \
           CommandVolumeCreateCorpusCallosumSlice.cxx \
           CommandVolumeCreateInStereotaxicSpace.cxx \
           CommandVolumeDilate.cxx \
           CommandVolumeDilateErode.cxx \
           CommandVolumeDilateErodeWithinMask.cxx \
           CommandVolumeErode.cxx \
           CommandVolumeEulerCount.cxx \
           CommandVolumeFileCombine.cxx \
           CommandVolumeFileMerge.cxx \
           CommandVolumeFillBiggestObject.cxx \
           CommandVolumeFillHoles.cxx \
           CommandVolumeFillSlice.cxx \
           CommandVolumeFindLimits.cxx \
           CommandVolumeFloodFill.cxx \
           CommandVolumeGradient.cxx \
           CommandVolumeHistogram.cxx \
           CommandVolumeImportRawFile.cxx \
           CommandVolumeInformation.cxx \
           CommandVolumeInformationNifti.cxx \
           CommandVolumeMakePlane.cxx \
           CommandVolumeMakeRectangle.cxx \
           CommandVolumeMakeShell.cxx \
           CommandVolumeMakeSphere.cxx \
           CommandVolumeMapToSurface.cxx \
           CommandVolumeMapToSurfacePALS.cxx \
           CommandVolumeMapToSurfaceROIFile.cxx \
           CommandVolumeMapToVtkModel.cxx \
           CommandVolumeMaskVolume.cxx \
           CommandVolumeMaskWithVolume.cxx \
           CommandVolumeNearToPlane.cxx \
           CommandVolumePadVolume.cxx \
           CommandVolumeProbAtlasToFunctional.cxx \
           CommandVolumeReplaceVectorMagnitudeWithVolume.cxx \
           CommandVolumeReplaceVoxelsWithVectorMagnitude.cxx \
           CommandVolumeRemoveIslands.cxx \
           CommandVolumeResample.cxx \
           CommandVolumeRescaleVoxels.cxx \
           CommandVolumeResize.cxx \
           CommandVolumeScale0to255.cxx \
           CommandVolumeScalePercent0to255.cxx \
           CommandVolumeSculpt.cxx \
           CommandVolumeSegmentation.cxx \
           CommandVolumeSegmentationLigase.cxx \
           CommandVolumeSegmentationStereotaxicSpace.cxx \
           CommandVolumeSegmentationToCerebralHull.cxx \
           CommandVolumeSetOrientation.cxx \
           CommandVolumeSetOrigin.cxx \
           CommandVolumeSetSpacing.cxx \
           CommandVolumeShiftAxis.cxx \
           CommandVolumeSmearAxis.cxx \
           CommandVolumeTFCE.cxx \
           CommandVolumeThreshold.cxx \
           CommandVolumeThresholdDual.cxx \
           CommandVolumeThresholdInverse.cxx \
           CommandVolumeFslToVector.cxx \
           CommandVolumeTopologyCorrector.cxx \
           CommandVolumeTopologyGraph.cxx \
           CommandVolumeTopologyReport.cxx \
           CommandVolumeVectorCombine.cxx \
           OffScreenOpenGLWidget.cxx \
           ScriptBuilderParameters.cxx
