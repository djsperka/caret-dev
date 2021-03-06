
/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <sstream>

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDir>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTextEdit>
#include <QToolTip>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceBorderLandmarkIdentification.h"
#include "BrainModelVolume.h"
#include "BrainModelVolumeSureFitSegmentation.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "DisplaySettingsVolume.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiFileSelectionButton.h"
#include "GuiGraphWidget.h"
#include "GuiStereotaxicSpaceComboBox.h"
#include "GuiStructureComboBox.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowFileActions.h"
#include "GuiSpeciesComboBox.h"
#include "GuiVolumeSureFitSegmentationDialog.h"
#include "GuiVolumeSelectionControl.h"
#include "StatisticHistogram.h"
#include "ParamsFile.h"
#include "QtListBoxSelectionDialog.h"
#include "QtUtilities.h"
#include "SpecFile.h"
#include "Species.h"
#include "StringUtilities.h"
#include "VolumeFile.h"
#include "WuQDataEntryDialog.h"

#include "global_variables.h"

/**
 * Constructor.
 */
GuiVolumeSureFitSegmentationDialog::GuiVolumeSureFitSegmentationDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setAttribute(Qt::WA_DeleteOnClose);
   initializeGraphScaleFlag = true;
   typeOfVolumesToWrite = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI_GZIP;
   peakHistogram = NULL;
   anatomicalVolumeOptionsGroupBox = NULL;
   segmentationVolumeOptionsGroupBox = NULL;
   volumeSelectionGroupBox = NULL;
   mode = MODE_NONE;

   setWindowTitle("SureFit Segmentation Operations");
   
   //
   // label for current page legend
   //
   currentPageLegend = new QLabel("");
   
   //
   // stacked widget for pages
   //
   pagesStackedWidget = new QStackedWidget;
   
   //
   // Create the pages
   //
   changeDirectoryPage = createChangeDirectoryPage();
   addPage(changeDirectoryPage, "Change/Verify Current Directory");
   
   subjectInfoPage = createSubjectInfoPage();
   addPage(subjectInfoPage, "Subject Information");
    
   specFilePage = createSpecFilePage();
   addPage(specFilePage, "Spec File Selection");
   
   volumeSelectionPage = createVolumeSelectionPage();
   addPage(volumeSelectionPage, "Volume Selection");
   
   volumeFileTypePage = createVolumeFileTypePage();
   addPage(volumeFileTypePage, "Volume File Type");
   
   volumeAttributesPage = createVolumeAttributesPage();
   addPage(volumeAttributesPage, "Volume Attributes");
    
   grayWhitePeaksPage = createGrayWhitePeaksPage();
   addPage(grayWhitePeaksPage, "Set Gray and White Matter Peaks");
    
   segmentationSelectionsPage = createSegmentationSelectionsPage();
   addPage(segmentationSelectionsPage, "Segmentation Operations");
   
   segmentationCompletePage = createSegmentationCompletePage();
   addPage(segmentationCompletePage, "Segmentation Complete");
      
   //
   // back button
   //
   backPushButton = new QPushButton("Back");
   backPushButton->setAutoDefault(false);
   QObject::connect(backPushButton, SIGNAL(clicked()),
                    this, SLOT(slotBackPushButton()));
   
   //
   // next button
   //
   nextPushButton = new QPushButton("Next");
   nextPushButton->setAutoDefault(false);
   QObject::connect(nextPushButton, SIGNAL(clicked()),
                    this, SLOT(slotNextPushButton()));
   
   //
   // close button
   //
   QPushButton* closePushButton = new QPushButton("Close");
   closePushButton->setAutoDefault(false);
   QObject::connect(closePushButton, SIGNAL(clicked()),
                    this, SLOT(slotCloseButton()));
                    
   //
   // Make all of the buttons the same size
   //
   QtUtilities::makeButtonsSameSize(backPushButton,
                                    nextPushButton,
                                    closePushButton);
                                    
   //
   // Layout the buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout();
   buttonsLayout->addWidget(backPushButton);
   buttonsLayout->addWidget(nextPushButton);
   buttonsLayout->addWidget(closePushButton);
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(currentPageLegend);
   dialogLayout->addWidget(pagesStackedWidget);
   dialogLayout->addLayout(buttonsLayout);
   
}

/**
 * Destructor.
 */
GuiVolumeSureFitSegmentationDialog::~GuiVolumeSureFitSegmentationDialog()
{
}

/**
 * add a page to the dialog.
 */
void 
GuiVolumeSureFitSegmentationDialog::addPage(QWidget* w, const QString& legend)
{
   pagesStackedWidget->addWidget(w);
   pageLegends.push_back(legend);
}      

/**
 * called when close button is pressed.
 */
void 
GuiVolumeSureFitSegmentationDialog::slotCloseButton()
{
   //
   // Turn off any thresholding
   //
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   dsv->setAnatomyThresholdValid(false);
   GuiBrainModelOpenGL::updateAllGL();
   WuQDialog::close();
}
      
/**
 * called to show the dialog (overrides parent's method).
 */
void 
GuiVolumeSureFitSegmentationDialog::show()
{
   showPage(pagesStackedWidget->widget(0));
   WuQDialog::show();
   slotEnableDisablePushButtons();
}

/**
 * create change directory page.
 */
QWidget* 
GuiVolumeSureFitSegmentationDialog::createChangeDirectoryPage()
{
   //
   // Current directory line edit
   //
   currentDirectoryLineEdit = new QLineEdit;
   currentDirectoryLineEdit->setReadOnly(true);
   
   //
   // Pushbutton to change the current directory
   //
   QPushButton* directoryPushButton = new QPushButton("Change Current Directory...");
   directoryPushButton->setAutoDefault(false);
   directoryPushButton->setFixedSize(directoryPushButton->sizeHint());
   QObject::connect(directoryPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDirectoryPushButton()));
   
   //
   // Current directory text
   //
   currentDirectoryLabel = new QLabel("");
   
   //
   // Dummy widget stretched at bottom
   //
   QWidget* dummyWidget = new QWidget;
   
   //
   // Widget for page and layout
   //
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(currentDirectoryLineEdit);
   layout->addWidget(directoryPushButton);
   layout->addWidget(currentDirectoryLabel);
   layout->setStretchFactor(currentDirectoryLineEdit, 0);
   layout->setStretchFactor(currentDirectoryLabel, 0);
   layout->setStretchFactor(directoryPushButton, 0);
   layout->setStretchFactor(dummyWidget, 100);
   
   return w;
}

/**
 * called when directory push button is pressed.
 */
void 
GuiVolumeSureFitSegmentationDialog::slotDirectoryPushButton()
{
   GuiMainWindowFileActions* fileActions = theMainWindow->getFileActions();
   fileActions->slotSetCurrentDirectory();
   updateCurrentDirectoryPage();
}

/**
 * see if the current directory is set to the caret installation directory.
 */
bool 
GuiVolumeSureFitSegmentationDialog::currentDirectoryIsCaretInstallationDirectory()
{
   const QString currentPath = QDir::currentPath();
   const bool inInstallDir =  (currentPath.contains("caret/bin") ||
                               currentPath.contains("caret\\bin") ||
                               currentPath.contains("caret/apps"));
   return inInstallDir;
}
      
/**
 * update the current directory page.
 */
void 
GuiVolumeSureFitSegmentationDialog::updateCurrentDirectoryPage()
{   
   QString msg;
   
   if (currentDirectoryIsCaretInstallationDirectory()) {
      msg = 
         "<font color=red>"
         "The current path needs to be changed as it is currently set to<br>"
         "the directory containing the Caret program.  <br>"
         "</font>"
         "<br>"
         "Press the <B>Change Current Directory</B> push button to change the <br>"
         "current directory to the directory containing the volume you <br>"
         "plan to segment.";
   }
   else {
      msg = 
         "If the current directory is not set to the directory containing the<br>"
         "volume you are segmenting use the <B>Change Current Directory</B> to<br>"
         "set the current directory.<br>"
         "<br>"
         "If you have loaded a spec file containing a volume, there is no need<br>"
         "to change the current directory.";
   }
   
   currentDirectoryLineEdit->setText(QDir::currentPath());
   currentDirectoryLabel->setTextFormat(Qt::RichText);
   currentDirectoryLabel->setText(msg);
   
   slotEnableDisablePushButtons();
}
      
/**
 * Create the subject info page.
 */
QWidget* 
GuiVolumeSureFitSegmentationDialog::createSubjectInfoPage()
{
   //
   // Species information
   //
   QLabel* speciesLabel = new QLabel("Species");
   speciesComboBox = new GuiSpeciesComboBox;
   speciesComboBox->setSelectedSpecies(theMainWindow->getBrainSet()->getSpecies());
   QObject::connect(speciesComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotEnableDisablePushButtons()));
   
   //
   // Stereotaxic space
   //
   QLabel* stereotaxicSpaceLabel = new QLabel("Stereotaxic Space");
   stereotaxicSpaceComboBox = new GuiStereotaxicSpaceComboBox;
   stereotaxicSpaceComboBox->setSelectedStereotaxicSpace(theMainWindow->getBrainSet()->getStereotaxicSpace());
   QObject::connect(stereotaxicSpaceComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotEnableDisablePushButtons()));
                    
   //
   // Subject information
   //
   QLabel* subjectLabel = new QLabel("Subject");
   subjectLineEdit = new QLineEdit;
   QObject::connect(subjectLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));
   
   //
   // Hemisphere
   //
   //
   // hemisphere combo box and line edit
   //
   QLabel* structureLabel = new QLabel("Structure");
   structureComboBox = new GuiStructureComboBox(0, 0, true);
   structureComboBox->setStructure(theMainWindow->getBrainSet()->getStructure().getType());
   QObject::connect(structureComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotEnableDisablePushButtons()));
   
   //
   // Arrange the widgets in a grid layout
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(speciesLabel, 0, 0);
   gridLayout->addWidget(speciesComboBox, 0, 1);
   gridLayout->addWidget(stereotaxicSpaceLabel, 1, 0);
   gridLayout->addWidget(stereotaxicSpaceComboBox, 1, 1);
   gridLayout->addWidget(subjectLabel, 2, 0);
   gridLayout->addWidget(subjectLineEdit, 2, 1);
   gridLayout->addWidget(structureLabel, 3, 0);
   gridLayout->addWidget(structureComboBox, 3, 1);
   gridLayout->setColumnStretch(0, 0);
   
   //
   // Widget for page and layout
   //
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addLayout(gridLayout);
   layout->addStretch();
   
   return w;
}

/**
 * Create the spec file page.
 */
QWidget* 
GuiVolumeSureFitSegmentationDialog::createSpecFilePage()
{
   //
   // Line edit for name of spec file
   //
   specFileNameLineEdit = new QLineEdit;
   QObject::connect(specFileNameLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));
                    
   //
   // Button to select spec file
   //
   GuiFileSelectionButton* specFilePushButton = 
                     new GuiFileSelectionButton(NULL,
                                                "Choose Spec File...",
                                                FileFilters::getSpecFileFilter(),
                                                false);
   QObject::connect(specFilePushButton, SIGNAL(fileSelected(const QString&)),
                    specFileNameLineEdit, SLOT(setText(const QString&)));
   QObject::connect(specFilePushButton, SIGNAL(fileSelected(const QString&)),
                    this, SLOT(slotSpecFileSelected(const QString&)));
   specFilePushButton->setFixedSize(specFilePushButton->sizeHint());
   QGroupBox* specFileGroupBox = new QGroupBox("Spec File Selection");
   QVBoxLayout* specFileLayout = new QVBoxLayout(specFileGroupBox);
   specFileLayout->addWidget(specFilePushButton);
   specFileLayout->addWidget(specFileNameLineEdit);
   
   specFileCommentTextEdit = new QTextEdit;
   specFileCommentTextEdit->setMaximumHeight(100);
   QGroupBox* commentGroupBox = new QGroupBox("Comment");
   QVBoxLayout* commentGroupLayout = new QVBoxLayout(commentGroupBox);
   commentGroupLayout->addWidget(specFileCommentTextEdit);
   
   const QString msg = 
      "The spec file is used to organize a subject's data files.\n"
      "\n"
      "Use the \"Choose Spec File\" pushbutton to choose a different spec file or to \n"
      "use a different directory.  In the Choose Spec File selection dialog, press the \n"
      "icon containing a folder with a \"*\" in its top right corner to create new\n"
      "directories.\n"
      "\n"
      "All new files created during the SureFit process will be placed into the\n"
      "directory containing the spec file.\n"
      "\n"
      "If you choose to generate raw and fiducial coord files, all surface related\n"
      "files will be removed from the spec file.\n"
      ;
   QLabel* label = new QLabel(msg);

   //
   // Widget and layout for dialog
   //
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(specFileGroupBox);
   layout->addWidget(commentGroupBox);
   layout->addWidget(label);
   layout->addStretch();
   
   return w;
}

/**
 * called when a spec file is selected on spec file page.
 */
void 
GuiVolumeSureFitSegmentationDialog::slotSpecFileSelected(const QString& name)
{
   try {
      SpecFile sf;
      sf.readFile(name);
      specFileCommentTextEdit->setPlainText(sf.getFileComment());
      specFileCommentTextEdit->setWindowModified(false);
   }
   catch (FileException&) {
   }
}

/**
 * Create the volume file type page.
 */
QWidget* 
GuiVolumeSureFitSegmentationDialog::createVolumeFileTypePage()
{
   fileTypeAfniRadioButton  = new QRadioButton("AFNI");
   fileTypeNiftiRadioButton = new QRadioButton("NIFTI");
   fileTypeNiftiGzipRadioButton = new QRadioButton("NIFTI-GZIP");
   
   QButtonGroup* buttGroup = new QButtonGroup(this);
   buttGroup->addButton(fileTypeAfniRadioButton);  
   buttGroup->addButton(fileTypeNiftiRadioButton); 
   buttGroup->addButton(fileTypeNiftiGzipRadioButton);
   QObject::connect(buttGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEnableDisablePushButtons()));
          
   QLabel* infoLabel = new QLabel("\n"
          "Choose the type of volume files that Caret will create\n"
          "when volume files are automatically saved.  Temporary\n"
          "volume files will also be saved in this file format.\n"
          "It is RECOMMENDED that you use the NIFTI file format. \n"
          "NIFTI is supported by most brain mapping software packages\n"
          "and stores all data in one file.");
      
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(fileTypeAfniRadioButton);
   layout->addWidget(fileTypeNiftiRadioButton);
   layout->addWidget(fileTypeNiftiGzipRadioButton);
   layout->addWidget(infoLabel);
   layout->addStretch();
   
   return w;
}
      
/**
 * Create the volume selection page.
 */
QWidget* 
GuiVolumeSureFitSegmentationDialog::createVolumeSelectionPage()
{
   //
   // Mode radio buttons
   //
   modeAnatomicalVolumeProcessingRadioButton = 
      new QRadioButton("Anatomical Volume Segmentation");
   modeSegmentationVolumeProcessingRadioButton =
      new QRadioButton("Segmentation Volume Post Processing");
   
   //
   // Button group to keep mode buttons mutually exclusive
   //
   QButtonGroup* modeButtonGroup = new QButtonGroup(this);
   modeButtonGroup->addButton(modeAnatomicalVolumeProcessingRadioButton);
   modeButtonGroup->addButton(modeSegmentationVolumeProcessingRadioButton);
   QObject::connect(modeButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEnableDisablePushButtons()));
                    
   //
   // Group box for mode buttons
   //
   QGroupBox* modeGroupBox = new QGroupBox("Segmentation Mode");
   QVBoxLayout* modeGroupLayout = new QVBoxLayout(modeGroupBox);
   modeGroupLayout->addWidget(modeAnatomicalVolumeProcessingRadioButton);
   modeGroupLayout->addWidget(modeSegmentationVolumeProcessingRadioButton);
   modeGroupBox->setFixedSize(modeGroupBox->sizeHint());
   
   //
   // Control for selecting the anatomical volume
   //
   volumeAnatomySelectionControl = new GuiVolumeSelectionControl(0,
                                                          true,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                               GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                          "volumeAnatomySelectionControl",
                                                          false,
                                                          false,
                                                          false);
   QObject::connect(volumeAnatomySelectionControl, SIGNAL(signalSelectionChanged()),
                    this, SLOT(slotEnableDisablePushButtons()));
   
   //
   // Control for selecting the segmentation volume
   //
   volumeSegmentationSelectionControl = new GuiVolumeSelectionControl(0,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                                                          true,
                                                          false,
                               GuiVolumeSelectionControl::LABEL_MODE_FILE_LABEL_AND_NAME,
                                                          "volumeSegmentationSelectionControl",
                                                          false,
                                                          false,
                                                          false);
   QObject::connect(volumeSegmentationSelectionControl, SIGNAL(signalSelectionChanged()),
                    this, SLOT(slotEnableDisablePushButtons()));
                    
   //
   // Group box and layout for volume selection
   //
   volumeSelectionGroupBox = new QGroupBox("Volume Selection");
   QVBoxLayout* volumeSelectionGroupLayout = new QVBoxLayout(volumeSelectionGroupBox);
   volumeSelectionGroupLayout->addWidget(volumeAnatomySelectionControl);
   volumeSelectionGroupLayout->addWidget(volumeSegmentationSelectionControl);
   volumeSelectionGroupBox->setFixedSize(volumeSelectionGroupBox->sizeHint());
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(modeGroupBox);
   layout->addWidget(volumeSelectionGroupBox);
   layout->addStretch();
   
   return w;
}

/**
 * Create the anatomy attributes page.
 */
QWidget* 
GuiVolumeSureFitSegmentationDialog::createVolumeAttributesPage()
{
   volumeAttributesLabel = new QLabel("");

   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(volumeAttributesLabel);
   
   return w;
}

/**
 * update the volume attributes page.
 */
void 
GuiVolumeSureFitSegmentationDialog::updateVolumeAttributesPage()
{
   QString voxelRangeMessage;
   VolumeFile* vfa = volumeAnatomySelectionControl->getSelectedVolumeFile();
   if (vfa != NULL) {
      float minVoxel, maxVoxel;
      vfa->getMinMaxVoxelValues(minVoxel, maxVoxel);
      std::ostringstream s1;
      QString fontStart1, fontEnd1;
      if (minVoxel != 0.0) {
         fontStart1 = "<font color=red>";
         fontEnd1 = "</font>";
      }
      QString fontStart2, fontEnd2;
      if (maxVoxel != 255.0) {
         fontStart2 = "<font color=red>";
         fontEnd2 = "</font>";
      }
      s1 << "The selected anatomy volume's voxels range from " 
         << fontStart1.toAscii().constData()
         << minVoxel 
         << fontEnd1.toAscii().constData()
         << " to " 
         << fontStart2.toAscii().constData()
         << maxVoxel 
         << fontEnd2.toAscii().constData()
         << ".";
      voxelRangeMessage += s1.str().c_str();
   }
/*
   VolumeFile* vfb = volumeAnatomySelectionControl->getSelectedVolumeFile();
   if (vfb != NULL) {
      float minVoxel, maxVoxel;
      vfa->getMinMaxVoxelValues(minVoxel, maxVoxel);
      std::ostringstream s1;
      QString fontStart1, fontEnd1;
      if (minVoxel != 0.0) {
         fontStart1 = "<font color=red>";
         fontEnd1 = "</font>";
      }
      QString fontStart2, fontEnd2;
      if (maxVoxel != 255.0) {
         fontStart2 = "<font color=red>";
         fontEnd2 = "</font>";
      }
      s1 << "The selected anatomy volume's voxels range from " 
         << fontStart1.toAscii().constData()
         << minVoxel 
         << fontEnd1.toAscii().constData()
         << " to " 
         << fontStart2.toAscii().constData()
         << maxVoxel 
         << fontEnd2.toAscii().constData()
         << ".";
      voxelRangeMessage += s1.str().c_str();
   }
*/
   std::ostringstream str;
   str << "The volume must be in an LPI (-x is left, -y is posterior, -z is inferior)<br>"
       << "orientation.<br>"
       << "<br>"
       << "The origin (stereotaxic coordinates of [0, 0, 0]) must be located at the <br>"
       << "anterior commissure.  If you have a full hemisphere, pressing the \"R\" <br>"
       << "button in the toolbar should result in the the crosshairs intersecting at <br>"
       << "the anterior commissure.<br>"
       << "<br>"
       << "The voxel size must be 1mm in each axis.<br>"
       << "<br>"
       << "The ANATOMY voxels must be in the range 0 to 255.<br>";
   if (vfa != NULL) {
      str << voxelRangeMessage.toAscii().constData() << "<br>";
   }
   str << "<br>"
       << "All of these items may be adjusted by selecting \"Edit Volume Attributes\" <br>"
       << "from the \"Volume Menu\".<br>"
       << "<br>"
       << "If any of the above conditions are not met, an error message will be displayed<br>"
       << "when the \"Next\" button is pressed.<br>";
   
   volumeAttributesLabel->setTextFormat(Qt::RichText);
   volumeAttributesLabel->setText(str.str().c_str());
}
      
/**
 * Create the gray white peaks page.
 */
QWidget* 
GuiVolumeSureFitSegmentationDialog::createGrayWhitePeaksPage()
{
   //
   // Create the histogram widget
   //
   peakHistogramWidget = new GuiGraphWidget(0, "");
   
   //
   // GroupBox and layout for histogram
   //
   QGroupBox* histoGroupBox = new QGroupBox("Histogram");
   QVBoxLayout* histoLayout = new QVBoxLayout(histoGroupBox);
   histoLayout->addWidget(peakHistogramWidget);
                    
   //
   // Gray peak radio button and spin box
   //
   grayPeakRadioButton = new QRadioButton("Gray");
   grayPeakRadioButton->setToolTip(
                 "When this item is selected, anatomy volume voxels\n"
                 "greater than the gray peak threshold will be \n"
                 "colored in green.");
   grayPeakSpinBox = new QSpinBox;
   grayPeakSpinBox->setMinimum(0);
   grayPeakSpinBox->setMaximum(255);
   grayPeakSpinBox->setSingleStep(1);
   QObject::connect(grayPeakSpinBox, SIGNAL(valueChanged(int)),
                    peakHistogramWidget, SLOT(slotSetMinimumPeak(int)));
   QObject::connect(grayPeakSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotPeakRadioButton()));
   
   //
   // White peak radio button and spin box
   //
   whitePeakRadioButton = new QRadioButton("White");
   whitePeakRadioButton->setToolTip(
                 "When this item is selected, anatomy volume voxels\n"
                 "greater than the white peak threshold will be \n"
                 "colored in green.");
   whitePeakSpinBox = new QSpinBox;
   whitePeakSpinBox->setMinimum(0);
   whitePeakSpinBox->setMaximum(255);
   whitePeakSpinBox->setSingleStep(1);
   QObject::connect(whitePeakSpinBox, SIGNAL(valueChanged(int)),
                    peakHistogramWidget, SLOT(slotSetMaximumPeak(int)));
   QObject::connect(whitePeakSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotPeakRadioButton()));
   
   //
   // Override check box and spin box
   //
   overrideThreshSetCheckBox = new QCheckBox("Override");
   overrideThreshSetCheckBox->setToolTip(
                 "Enable this item to override the average\n"
                 "of the gray and white matter peaks.");
   overridePeakSpinBox = new QSpinBox;
   overridePeakSpinBox->setMinimum(0);
   overridePeakSpinBox->setMaximum(255);
   overridePeakSpinBox->setSingleStep(1);
   QObject::connect(overrideThreshSetCheckBox, SIGNAL(toggled(bool)),
                    overridePeakSpinBox, SLOT(setEnabled(bool)));
   overridePeakSpinBox->setToolTip(
                 "Enter the value for overriding the average\n"
                 "of the gray and white matter peaks here.");
                  
   //
   // Smooth button
   //
   QPushButton* smoothHistogramPushButton = new QPushButton("Smooth");
   smoothHistogramPushButton->setFixedSize(smoothHistogramPushButton->sizeHint());
   smoothHistogramPushButton->setAutoDefault(false);
   smoothHistogramPushButton->setToolTip("Smoothing the histogram may make the peaks\n"
                                         "easier to identify.  Each time the smooth\n"
                                         "button is pressed, one iteration of smoothing\n"
                                         "is performed so multiple pressings of the \n"
                                         "smooth button may be needed.  To reset the\n"
                                         "graph, press the Back button to return to the\n"
                                         "previous page and then press the Next button.");
   QObject::connect(smoothHistogramPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSmoothHistogramPushButton()));
                    
   //
   // Group box for gray/white/override controls
   //
   QGroupBox* peaksGroupBox = new QGroupBox("Peaks");
   QGridLayout* peaksGridLayout = new QGridLayout(peaksGroupBox);
   peaksGridLayout->addWidget(grayPeakRadioButton, 0, 0);
   peaksGridLayout->addWidget(grayPeakSpinBox, 0, 1);
   peaksGridLayout->addWidget(whitePeakRadioButton, 1, 0);
   peaksGridLayout->addWidget(whitePeakSpinBox, 1, 1);
   peaksGridLayout->addWidget(overrideThreshSetCheckBox, 2, 0);
   peaksGridLayout->addWidget(overridePeakSpinBox, 2, 1);
   peaksGridLayout->addWidget(smoothHistogramPushButton, 3, 0, 2, 1);
   peaksGroupBox->setFixedSize(peaksGroupBox->sizeHint());
   
   //
   // Button group for white/gray radio buttons
   //
   QButtonGroup* grayWhiteButtonGroup = new QButtonGroup(this);
   QObject::connect(grayWhiteButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotPeakRadioButton()));
   grayWhiteButtonGroup->addButton(grayPeakRadioButton, 0);
   grayWhiteButtonGroup->addButton(whitePeakRadioButton, 1);
   
   //
   // Estimated peaks labels
   //
   QLabel* csfPeakTextLabel = new QLabel("CSF ");
   csfPeakLabel = new QLabel("");
   QLabel* grayPeakTextLabel = new QLabel("Gray ");
   grayPeakLabel = new QLabel("");
   QLabel* whitePeakTextLabel = new QLabel("White ");
   whitePeakLabel = new QLabel("");
   
   //
   // Layout for estimated peaks
   //
   QGridLayout* peakEstGridLayout = new QGridLayout;
   peakEstGridLayout->addWidget(csfPeakTextLabel, 0, 0);
   peakEstGridLayout->addWidget(csfPeakLabel, 0, 1);
   peakEstGridLayout->addWidget(grayPeakTextLabel, 1, 0);
   peakEstGridLayout->addWidget(grayPeakLabel, 1, 1);
   peakEstGridLayout->addWidget(whitePeakTextLabel, 2, 0);
   peakEstGridLayout->addWidget(whitePeakLabel, 2, 1);
   QGroupBox* peakEstGroupBox = new QGroupBox("Peak Estimates");
   QHBoxLayout* peakEstBoxLayout = new QHBoxLayout(peakEstGroupBox);
   peakEstBoxLayout->addLayout(peakEstGridLayout);
   peakEstBoxLayout->addStretch();
   
   //
   // Layout for page
   //
   QVBoxLayout* rightColumnLayout = new QVBoxLayout;
   rightColumnLayout->addWidget(peaksGroupBox);
   rightColumnLayout->addWidget(peakEstGroupBox);
   rightColumnLayout->addStretch();
   
   QWidget* w = new QWidget;
   QHBoxLayout* layout = new QHBoxLayout(w);
   layout->addWidget(histoGroupBox);
   layout->addLayout(rightColumnLayout);

   return w;
}

/**
 * called when gray or white peak radio button selected.
 */
void 
GuiVolumeSureFitSegmentationDialog::slotPeakRadioButton()
{
   DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
   if (grayPeakRadioButton->isChecked()) {
      dsv->setAnatomyThreshold(grayPeakSpinBox->value());
      dsv->setAnatomyThresholdValid(true);
   }
   else if (whitePeakRadioButton->isChecked()) {
      dsv->setAnatomyThreshold(whitePeakSpinBox->value());
      dsv->setAnatomyThresholdValid(true);
   }
   else {
      dsv->setAnatomyThresholdValid(false);
   }
   
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Create the segmentation selections page.
 */
QWidget* 
GuiVolumeSureFitSegmentationDialog::createSegmentationSelectionsPage()
{
   disconnectEyeCheckBox = new QCheckBox("Disconnect Eye and Skull");
   disconnectEyeCheckBox->setChecked(true);
   
   disconnectHindbrainCheckBox = new QCheckBox("Disconnect Hindbrain");
   hindLoHiThreshComboBox = new QComboBox;
   hindLoHiThreshComboBox->insertItem(HIND_BRAIN_THRESHOLD_HIGH,
                                      "High Threshold");
   hindLoHiThreshComboBox->insertItem(HIND_BRAIN_THRESHOLD_LOW,
                                      "Low Threshold");
   QObject::connect(disconnectHindbrainCheckBox, SIGNAL(toggled(bool)),
                    hindLoHiThreshComboBox, SLOT(setEnabled(bool)));
   disconnectHindbrainCheckBox->setChecked(true);
   QHBoxLayout* hindbrainLayout = new QHBoxLayout;
   hindbrainLayout->addWidget(disconnectHindbrainCheckBox);
   hindbrainLayout->addWidget(hindLoHiThreshComboBox);
   hindbrainLayout->addStretch();
   
   cutCorpusCallosumCheckBox = new QCheckBox("Cut Corpus Callosum");
   cutCorpusCallosumCheckBox->setChecked(true);
   
   generateSegmentationCheckBox = new QCheckBox("Generate Segmentation");
   generateSegmentationCheckBox->setChecked(true);
   QObject::connect(generateSegmentationCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotEnableDisableSelectionCheckBoxes()));
   
   fillVentriclesCheckBox = new QCheckBox("Fill Ventricles");
   fillVentriclesCheckBox->setChecked(true);
   
   volumeErrorCorrectionCheckBox = new QCheckBox("Volume Error Correction Method");
   volumeErrorCorrectionCheckBox->setChecked(true);
   QObject::connect(volumeErrorCorrectionCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotEnableDisableSelectionCheckBoxes()));
   
   volumeErrorCorrectionMethodComboBox = new QComboBox;
   std::vector<BrainModelVolumeSureFitSegmentation::ERROR_CORRECTION_METHOD> errorCorrectionValues;
   std::vector<QString> errorCorrectionNames;
   BrainModelVolumeSureFitSegmentation::getErrorCorrectionMethodsAndNames(
      errorCorrectionNames, errorCorrectionValues);
   int defaultErrorIndex = 0;
   for (unsigned int i = 0; i < errorCorrectionNames.size(); i++) {
      if (errorCorrectionValues[i] !=
          BrainModelVolumeSureFitSegmentation::ERROR_CORRECTION_METHOD_NONE) {
         volumeErrorCorrectionMethodComboBox->addItem(
            errorCorrectionNames[i],
            static_cast<int>(errorCorrectionValues[i]));
         if (errorCorrectionValues[i] ==
             BrainModelVolumeSureFitSegmentation::ERROR_CORRECTION_METHOD_SUREFIT_AND_GRAPH) {
            defaultErrorIndex = volumeErrorCorrectionMethodComboBox->count() - 1;
         }
      }
   }
   volumeErrorCorrectionMethodComboBox->setCurrentIndex(defaultErrorIndex);
   
   QHBoxLayout* errorCorrectionLayout = new QHBoxLayout;
   errorCorrectionLayout->addWidget(volumeErrorCorrectionCheckBox);
   errorCorrectionLayout->addWidget(volumeErrorCorrectionMethodComboBox);
   errorCorrectionLayout->addStretch();
   
   generateRawAndFiducialSurfaceCheckBox = new QCheckBox("Generate Raw and Fiducial Surfaces");
   generateRawAndFiducialSurfaceCheckBox->setChecked(true);
   QObject::connect(generateRawAndFiducialSurfaceCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotEnableDisableSelectionCheckBoxes()));
   
   generateTopologicallyCorrectFiducialSurfaceCheckBox = new QCheckBox("Correct All Topological Errors in Fiducial Surface (USE WITH CAUTION)");
   generateTopologicallyCorrectFiducialSurfaceCheckBox->setChecked(false);
   QObject::connect(generateTopologicallyCorrectFiducialSurfaceCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotGenerateTopologicallyCorrectFiducialSurfaceCheckBox(bool)));
                    
   generateSurfaceLimitPolygonsCheckBox = new QCheckBox("Limit Polygons in Generated Surface(s)");
   generateSurfaceLimitPolygonsCheckBox->setChecked(true);
   
   generateInflatedSurfaceCheckBox = new QCheckBox("Generate Inflated Surface");
   generateInflatedSurfaceCheckBox->setChecked(true);
   
   generateVeryInflatedSurfaceCheckBox = new QCheckBox("Generate Very Inflated Surface");
   generateVeryInflatedSurfaceCheckBox->setChecked(true);
   
   generateEllipsoidSurfaceCheckBox = new QCheckBox("Generate Ellipsoid Surface (for Flattening)");
   generateEllipsoidSurfaceCheckBox->setChecked(true);
   
   generateSphericalSurfaceCheckBox = new QCheckBox("Generate Spherical Surface");
   generateSphericalSurfaceCheckBox->setChecked(false);
   
   generateCompMedWallSurfaceCheckBox = new QCheckBox("Generate Compressed Medial Wall Surface");
   generateCompMedWallSurfaceCheckBox->setChecked(false);
   
   generateHullCheckBox = new QCheckBox("Generate Cerebral Hull");
   generateHullCheckBox->setChecked(true);
   
   generateDepthCurvatureGeographyCheckBox = new QCheckBox("Create Curvature, Depth, and Geography Attributes");
   generateDepthCurvatureGeographyCheckBox->setChecked(true);
   
   generateLandmarkBordersCheckBox = new QCheckBox("Create Flatten and Registration Landmark Borders");
   generateLandmarkBordersCheckBox->setChecked(false);
   
   autoSaveFilesCheckBox = new QCheckBox("Auto Save Generated Data Files");
   autoSaveFilesCheckBox->setChecked(true);
   
   //
   // Group box and layout for anatomical volume controls
   //
   anatomicalVolumeOptionsGroupBox = 
      new QGroupBox("Anatomical Volume Options");
   QVBoxLayout* anatomicalVolumeGroupLayout =
      new QVBoxLayout(anatomicalVolumeOptionsGroupBox);
   anatomicalVolumeGroupLayout->addWidget(disconnectEyeCheckBox);
   anatomicalVolumeGroupLayout->addLayout(hindbrainLayout);
   anatomicalVolumeGroupLayout->addWidget(cutCorpusCallosumCheckBox);
   anatomicalVolumeGroupLayout->addWidget(generateSegmentationCheckBox);
   anatomicalVolumeGroupLayout->addWidget(fillVentriclesCheckBox);

   //
   // Group box and layout for segmentation volume options
   //
   segmentationVolumeOptionsGroupBox = 
      new QGroupBox("Segmentation Volume Options");
   QVBoxLayout* segmentationVolumeGroupLayout =
      new QVBoxLayout(segmentationVolumeOptionsGroupBox);
   segmentationVolumeGroupLayout->addLayout(errorCorrectionLayout);
   segmentationVolumeGroupLayout->addWidget(generateRawAndFiducialSurfaceCheckBox);
   segmentationVolumeGroupLayout->addWidget(generateSurfaceLimitPolygonsCheckBox);
   segmentationVolumeGroupLayout->addWidget(generateTopologicallyCorrectFiducialSurfaceCheckBox);
   segmentationVolumeGroupLayout->addWidget(generateInflatedSurfaceCheckBox);
   segmentationVolumeGroupLayout->addWidget(generateVeryInflatedSurfaceCheckBox);
   segmentationVolumeGroupLayout->addWidget(generateEllipsoidSurfaceCheckBox);
   segmentationVolumeGroupLayout->addWidget(generateSphericalSurfaceCheckBox);
   segmentationVolumeGroupLayout->addWidget(generateCompMedWallSurfaceCheckBox);
   segmentationVolumeGroupLayout->addWidget(generateHullCheckBox);
   segmentationVolumeGroupLayout->addWidget(generateDepthCurvatureGeographyCheckBox);
   segmentationVolumeGroupLayout->addWidget(generateLandmarkBordersCheckBox);
   segmentationVolumeGroupLayout->addWidget(autoSaveFilesCheckBox);

   //
   // widget and layout for page
   //
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(anatomicalVolumeOptionsGroupBox);
   layout->addWidget(segmentationVolumeOptionsGroupBox);
   layout->addStretch();
   
   return w;
}

/**
 * called when correct topological errors in fiducial surface checkbox toggled.
 */
void 
GuiVolumeSureFitSegmentationDialog::slotGenerateTopologicallyCorrectFiducialSurfaceCheckBox(bool b)
{
   static bool warningAlreadyGivenFlag = false;
   
   if (b) {
      if (warningAlreadyGivenFlag == false) {
         //warningAlreadyGivenFlag = true;
         
         const QString msg = 
            "This option, which corrects all topological errors in the fiducial surface,"
            "should be used with caution.  While the resulting surface will be topologically"
            "correct, it will not necessarily be anatomically correct.  In areas of the "
            "surface where correction occurs, there may be adjacent triangles with sharply"
            "angled edges, such as those you might find on a diamond or cubic zirconia.  It"
            "is best to correct the topological errors in the segmentation, particularly if"
            "the topological errors are large or involve non-cortical material such as a"
            "blood vessel or the optic chiasm.\n"
            "\n"
            "This option is probably best used if there are small topological errors in"
            "the medial wall or if the user is unable to find remaining topological "
            "errors.  The user may check for topological errors by selecting Surface"
            "Menu->Topology->Topology Error Report.  An \"Euler Count\" of 2 indicates"
            "that there are no topological errors and the surface is topologically "
            "equivalent to a sphere.\n"
            "\n"
            "If you are using surface shape depth maps for comparing coritical shape "
            "your topological corrections MUST be done in the volume since the "
            "segmentation volume is used to generate the cerebral hull which in turn"
            "is used to compute depth measurements.\n"
            "\n"
            "When this option is used, two fiducial coordinate files and two closed"
            "topology files will be produced.  One fiducial coordinate and closed"
            "topology file pair will contain the uncorrected surface and the other"
            "will contain the corrected surface.\n"
            "\n"
            "If you have questions or wish to provided feedback on this option, please"
            "send an email to john@brainmap.wustl.edu.";
         
         QMessageBox::warning(this, "WARNING", msg);
      }
   }
}
      
/**
 * called to enable/disable selection check boxes.
 */
void 
GuiVolumeSureFitSegmentationDialog::slotEnableDisableSelectionCheckBoxes()
{
   anatomicalVolumeOptionsGroupBox->setEnabled(false);
   segmentationVolumeOptionsGroupBox->setEnabled(false);
   disconnectEyeCheckBox->setEnabled(false);
   disconnectHindbrainCheckBox->setEnabled(false);
   cutCorpusCallosumCheckBox->setEnabled(false);
   generateSegmentationCheckBox->setEnabled(false);
   fillVentriclesCheckBox->setEnabled(false);
   generateRawAndFiducialSurfaceCheckBox->setEnabled(false);
   generateSurfaceLimitPolygonsCheckBox->setEnabled(false);
   generateInflatedSurfaceCheckBox->setEnabled(false);
   generateVeryInflatedSurfaceCheckBox->setEnabled(false);
   generateEllipsoidSurfaceCheckBox->setEnabled(false);
   generateSphericalSurfaceCheckBox->setEnabled(false);
   generateCompMedWallSurfaceCheckBox->setEnabled(false);
   generateHullCheckBox->setEnabled(false);
   volumeErrorCorrectionCheckBox->setEnabled(false);
   volumeErrorCorrectionMethodComboBox->setEnabled(false);
   generateDepthCurvatureGeographyCheckBox->setEnabled(false);
   generateLandmarkBordersCheckBox->setEnabled(false);
   
   const VolumeFile* anatomyVolumeFile = volumeAnatomySelectionControl->getSelectedVolumeFile();
   const VolumeFile* segmentationVolumeFile = volumeSegmentationSelectionControl->getSelectedVolumeFile();
   if ((anatomyVolumeFile == NULL) &&
       (segmentationVolumeFile == NULL)) {
      return;
   }
   
   const bool anatValid = ((anatomyVolumeFile != NULL) &&
                           (mode == MODE_ANATOMICAL_VOLUME_SEGMENTATION));
   if (anatValid) {
      anatomicalVolumeOptionsGroupBox->setEnabled(true);
      disconnectEyeCheckBox->setEnabled(true);
      disconnectHindbrainCheckBox->setEnabled(true);
      cutCorpusCallosumCheckBox->setEnabled(true);
      generateSegmentationCheckBox->setEnabled(true);
      fillVentriclesCheckBox->setEnabled(true);
      volumeErrorCorrectionCheckBox->setEnabled(true);
   }
   
   switch (mode) {
      case MODE_NONE:
         break;
      case MODE_ANATOMICAL_VOLUME_SEGMENTATION:
         if (anatValid &&
             generateSegmentationCheckBox->isChecked()) {
            segmentationVolumeOptionsGroupBox->setEnabled(true);
         }
         break;
      case MODE_SEMENTATION_VOLUME_POST_PROCESSING:
         if (segmentationVolumeFile != NULL) {
            segmentationVolumeOptionsGroupBox->setEnabled(true);
         }
         break;
   }

   if (segmentationVolumeOptionsGroupBox->isEnabled()) {
      volumeErrorCorrectionCheckBox->setEnabled(true);
      generateRawAndFiducialSurfaceCheckBox->setEnabled(true);
      const bool createRawFidValid = generateRawAndFiducialSurfaceCheckBox->isChecked() &&
                                     generateRawAndFiducialSurfaceCheckBox->isEnabled();
      generateSurfaceLimitPolygonsCheckBox->setEnabled(createRawFidValid);
      generateTopologicallyCorrectFiducialSurfaceCheckBox->setEnabled(createRawFidValid);
      generateInflatedSurfaceCheckBox->setEnabled(createRawFidValid);
      generateVeryInflatedSurfaceCheckBox->setEnabled(createRawFidValid);
      generateEllipsoidSurfaceCheckBox->setEnabled(createRawFidValid);
      generateSphericalSurfaceCheckBox->setEnabled(createRawFidValid);
      generateCompMedWallSurfaceCheckBox->setEnabled(createRawFidValid);
      generateDepthCurvatureGeographyCheckBox->setEnabled(createRawFidValid);
      const bool validLandmarkSpaceFlag =
         BrainModelSurfaceBorderLandmarkIdentification::isStereotaxicSpaceSupported(
                                           theMainWindow->getBrainSet()->getStereotaxicSpace());
      generateLandmarkBordersCheckBox->setEnabled((anatomyVolumeFile != NULL) &&
                                                  createRawFidValid &&
                                                  (generateInflatedSurfaceCheckBox->isEnabled() &&
                                                   generateInflatedSurfaceCheckBox->isChecked()) &&
                                                  (generateVeryInflatedSurfaceCheckBox->isEnabled() &&
                                                   generateVeryInflatedSurfaceCheckBox->isChecked()) &&
                                                  (generateEllipsoidSurfaceCheckBox->isEnabled() &&
                                                   generateEllipsoidSurfaceCheckBox->isChecked()) &&
                                                  (generateDepthCurvatureGeographyCheckBox->isEnabled() &&
                                                   generateDepthCurvatureGeographyCheckBox->isChecked()) &&
                                                  validLandmarkSpaceFlag);
      generateHullCheckBox->setEnabled(generateDepthCurvatureGeographyCheckBox->isEnabled() &&
                                       generateDepthCurvatureGeographyCheckBox->isChecked());
   }
   
   volumeErrorCorrectionMethodComboBox->setEnabled(
      volumeErrorCorrectionCheckBox->isEnabled() &&
      volumeErrorCorrectionCheckBox->isChecked());
}      

/**
 * Create the segmentation complete page.
 */
QWidget* 
GuiVolumeSureFitSegmentationDialog::createSegmentationCompletePage()
{
   QLabel* completeLabel = new QLabel("The segmentation has successfully completed.");
              
   segmentationCompleteTimeLabel = new QLabel("");
   segmentationCompleteHandleLabel = new QLabel("");
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(completeLabel);
   layout->addWidget(segmentationCompleteTimeLabel);
   layout->addWidget(segmentationCompleteHandleLabel);
   layout->addStretch();
   
   return w;
}

/**
 * Called when back pushbutton pressed.
 */
void 
GuiVolumeSureFitSegmentationDialog::slotBackPushButton()
{
   BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
   if (bmv == NULL) {
      QMessageBox::critical(this, "ERROR", "There are no volumes loaded.");
      showPage(pagesStackedWidget->widget(0), true);
      return;
   }

   const int indx = pagesStackedWidget->currentIndex() - 1;
   if (indx >= 0) {
      QWidget* nextPage = pagesStackedWidget->widget(indx);
      
      if (nextPage == grayWhitePeaksPage) {
         //
         // Doing anatomy volume ?
         //
         const bool anatValid = (volumeAnatomySelectionControl->getSelectedVolumeFile()
                                 != NULL);
         if (anatValid == false) {
            //
            // Since no anatomical volume, go to volume attributes page
            //
            nextPage = volumeAttributesPage;
         }
      }
      
      showPage(nextPage, true);
   }
} 

/**
 * Called to select the next page.
 */
void
GuiVolumeSureFitSegmentationDialog::slotNextPushButton()
{
   BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
   if (bmv == NULL) {
      QMessageBox::critical(this, "ERROR", "There are no volumes loaded.");
      showPage(pagesStackedWidget->widget(0));
      return;
   }
   
   //
   // Get the current page
   //
   QWidget* currentPage = pagesStackedWidget->currentWidget();
   
   //
   // If the current page is subject info
   //
   if (currentPage == subjectInfoPage) {
      QString msg;
      const StereotaxicSpace space = stereotaxicSpaceComboBox->getSelectedStereotaxicSpace();
      const Structure::STRUCTURE_TYPE hemisphere = structureComboBox->getSelectedStructure();
      const Species species = speciesComboBox->getSelectedSpecies();
      const QString subject = subjectLineEdit->text();
      
      if (space.isValid() == false) {
         msg.append("Steretaxic Space is invalid.");
      }
      if (hemisphere == Structure::STRUCTURE_TYPE_INVALID) {
         msg.append("Hemisphere must be not be Unknown.\n");
      }
      if (species.isValid() == false) {
         msg.append("The species is invalid.\n");
      }
      if (subject.isEmpty()) {
         msg.append("You must enter a subject.\n");
      }
      
      if (msg.isEmpty() == false) {
         QMessageBox::critical(this, "ERROR", msg);
         return;
      }
      
      //
      // Update brain set
      //
      const bool dataChanged = ((theMainWindow->getBrainSet()->getStructure()  != hemisphere) ||
                                (theMainWindow->getBrainSet()->getSpecies()    != species) ||
                                (theMainWindow->getBrainSet()->getSubject()    != subject) ||
                                (theMainWindow->getBrainSet()->getStereotaxicSpace() != space));
      theMainWindow->getBrainSet()->setStructure(hemisphere);
      theMainWindow->getBrainSet()->setSpecies(Species(species));
      theMainWindow->getBrainSet()->setSubject(subject);
      theMainWindow->getBrainSet()->setStereotaxicSpace(space);
      
      //
      // Update spec file if needed
      //
      if (dataChanged) {
         const QString specFileName(theMainWindow->getBrainSet()->getSpecFileName());
         SpecFile specFile;
         try {
            specFile.readFile(specFileName);
            specFile.setStructure(theMainWindow->getBrainSet()->getStructure().getTypeAsString());
            specFile.setSpecies(theMainWindow->getBrainSet()->getSpecies().getName());
            specFile.setSubject(theMainWindow->getBrainSet()->getSubject());
            specFile.setSpace(theMainWindow->getBrainSet()->getStereotaxicSpace());
            specFile.writeFile(specFileName);
         }
         catch (FileException&) {
         }
      }
   }

   //
   // If the current page is the spec file page
   //
   if (currentPage == specFilePage) {
      const QString specFileName(specFileNameLineEdit->text());
      const QString filename(FileUtilities::basename(specFileName));
      if (QFile::exists(specFileName)) {
         if (StringUtilities::endsWith(specFileName, "/") ||
             StringUtilities::endsWith(specFileName, "\\")) {
            QMessageBox::critical(this, "ERROR",
               "Name of spec file is missing.");
            return;
         }         
      }
      else {
         const QString directory(FileUtilities::dirname(specFileName));
         if (QFile::exists(directory) == false) {
            QMessageBox::critical(this, "ERROR",
               "Directory of spec file does not exist."
               "Create the directory before continuing.");
            return;
         }
         
         if (filename == SpecFile::getSpecFileExtension()) {
            QMessageBox::critical(this, "ERROR",
               "Name of spec file is missing, just have extension.\n");
            return;
         }
      }
      
      //
      // Set the name of the spec file and the current directory
      //
      theMainWindow->getBrainSet()->setSpecFileName(specFileName);
      QDir::setCurrent(FileUtilities::dirname(specFileName));
      
      //
      // Update spec file if necessary
      //
      {
         SpecFile specFile;
         try {
            specFile.readFile(FileUtilities::basename(specFileName));
         }
         catch (FileException&) {
            //
            // Spec file may not exist but that is OK 
            //
         }
         const bool updateSpecFile = ((specFile.getStructure() != theMainWindow->getBrainSet()->getStructure().getTypeAsString()) ||
                                      (specFile.getSpecies().getName()    != theMainWindow->getBrainSet()->getSpecies().getName()) ||
                                      (specFile.getSubject()    != theMainWindow->getBrainSet()->getSubject()) ||
                                      (specFileCommentTextEdit->document()->isModified()));
         if (updateSpecFile) {
            specFile.setStructure(theMainWindow->getBrainSet()->getStructure().getTypeAsString());
            specFile.setSpecies(theMainWindow->getBrainSet()->getSpecies());
            specFile.setSubject(theMainWindow->getBrainSet()->getSubject());
            specFile.setFileComment(specFileCommentTextEdit->toPlainText());
            try {
               specFile.writeFile(specFileName);
            }
            catch (FileException&) {
            }
         }
      }
   }
   
   VolumeFile* vfa = volumeAnatomySelectionControl->getSelectedVolumeFile();
   const VolumeFile* vfs = volumeSegmentationSelectionControl->getSelectedVolumeFile();

   //
   // If the current page is the volume selection page
   //
   if (currentPage == volumeSelectionPage) {
      if ((vfa == NULL) && (vfs == NULL)) {
         QMessageBox::critical(this, "ERROR", "You must select a volume.");
         return;
      }
      
      if (vfa != NULL) {
         if (vfa != bmv->getSelectedVolumeAnatomyFile()) {
            if (QMessageBox::warning(this, "Warning",
                  "The anatomy volume selected for segmenting is "
                     "different than the anatomy volume selected on "
                     "the Display Control Dialog.", 
                  (QMessageBox::Ok | QMessageBox::Cancel),
                  QMessageBox::Cancel)
                     == QMessageBox::Cancel) {
               return;
            }
         }
      }

      if (vfs != NULL) {
         if (vfs != bmv->getSelectedVolumeSegmentationFile()) {
            if (QMessageBox::warning(this, "Warning",
                  "The segmentation volume selected for processing is "
                     "different than the segmentation volume selected "
                     "on the Display Control Dialog.", 
                  (QMessageBox::Ok | QMessageBox::Cancel),
                  QMessageBox::Cancel)
                     == QMessageBox::Cancel) {
               return;
            }
         }
      }
      
      switch (mode) {
         case MODE_NONE:
            break;
         case MODE_ANATOMICAL_VOLUME_SEGMENTATION:
            break;
         case MODE_SEMENTATION_VOLUME_POST_PROCESSING:
            if (vfa == NULL) {
               if (QMessageBox::warning(this, "Warning",
                     "No anatomical volume is selected.  The anatomical "
                        "volume is required if you want to generate "
                        "landmark borders.", 
                     (QMessageBox::Ok | QMessageBox::Cancel),
                     QMessageBox::Cancel)
                        == QMessageBox::Cancel) {
                  return;
               }
            }
            break;
      }
   }
   
   QString msg;
         
   //
   // If the current page is the attributes page
   //
   if (currentPage == volumeAttributesPage) {
      if ((vfa == NULL) && (vfs == NULL)) {
         QMessageBox::critical(this, "ERROR", "You must select a volume.");
         return;
      }
      
      if (vfa != NULL) {
         VolumeFile::ORIENTATION orient[3];
         vfa->getOrientation(orient);
         
         bool orientationError = false;
         if (orient[0] != VolumeFile::ORIENTATION_LEFT_TO_RIGHT) {
            msg.append("The X axis must be in left-to-right orientation.\n");
            orientationError = true;
         }
         if (orient[1] != VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR) {
            msg.append("The Y axis must be in posterior-to-anterior orientation.\n");
            orientationError = true;
         }
         if (orient[2] != VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR) {
            msg.append("The Z axis must be in inferior-to-superior orientation.\n");
            orientationError = true;
         }
         if (orientationError) {
            msg.append("Press the OK button to close this message dialog.  After doing so,\n"
                       "select \"Edit Volume Attributes\" from the Volume Menu.  Press the tab\n"
                       "labeled \"Orientation\".\n");
         }
         
         float minVoxel, maxVoxel;
         vfa->getMinMaxVoxelValues(minVoxel, maxVoxel);
         if ((minVoxel < 0.0) || (maxVoxel > 255.0) || (maxVoxel <= 1.0)) {
            std::ostringstream str;
            str << "Voxels must be in the range (0.0, 255.0).\n"
                << "The selected volume's range is (" << minVoxel << ", " << maxVoxel << ").\n"
                << "Press the OK button to close this message dialog.  After doing so,"
                << "select \"Edit Volume Attributes\" from the Volume Menu.  Press the tab"
                << "labeled \"Data\" and then press the button labeled \"Rescale Voxels\".";
            if (msg.isEmpty() == false) {
               msg.append("\n");
            }
            msg.append(str.str().c_str());
         }
      }
      
      if (vfs != NULL) {
         VolumeFile::ORIENTATION orient[3];
         vfs->getOrientation(orient);
         float origin[3];
         vfs->getOrigin(origin);
         float spacing[3];
         vfs->getSpacing(spacing);
         
         bool orientationError = false;
         if (orient[0] != VolumeFile::ORIENTATION_LEFT_TO_RIGHT) {
            msg.append("The X axis must be in left-to-right orientation.\n");
            orientationError = true;
         }
         if (orient[1] != VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR) {
            msg.append("The Y axis must be in posterior-to-anterior orientation.\n");
            orientationError = true;
         }
         if (orient[2] != VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR) {
            msg.append("The Z axis must be in inferior-to-superior orientation.\n");
            orientationError = true;
         }
         if (orientationError) {
            msg.append("Press the OK button to close this message dialog.  After doing so,\n"
                       "select \"Edit Volume Attributes\" from the Volume Menu.  Press the tab\n"
                       "labeled \"Orientation\".\n");
         }
      }
      if (msg.isEmpty() == false) {
         QMessageBox::critical(this, "ERROR", msg);
         return;
      }

      bool voxelSizeOK = true;
      if (vfa != NULL) {
         float anatomySpacing[3];
         vfa->getSpacing(anatomySpacing);
         for (int i = 0; i < 3; i++) {
            if ((anatomySpacing[i] < 0.99) || (anatomySpacing[i] > 1.01)) {
               voxelSizeOK = false;
            }
         }
      }
      
      if (vfa != NULL) {
         float anatomyOrigin[3];
         vfa->getOrigin(anatomyOrigin);
         if ((anatomyOrigin[0] >= 0.0) || (anatomyOrigin[1] >= 0.0) || (anatomyOrigin[2] >= 0.0)) {
            QString msg("At least one anatomy origin value is greater than or equal to zero."
                            "This indicates that the origin may not be at the Anterior"
                            "commissure.  Do you want to continue ?");
            if (QMessageBox::warning(this, 
                                     "Warning", 
                                     msg,
                                    (QMessageBox::Yes | QMessageBox::No),
                                    QMessageBox::No)
                                       == QMessageBox::No) {
               return;
            }
         }
      }
   }
   
   //
   // If the current page is the volume file type page
   // 
   if (currentPage == volumeFileTypePage) {
      if (fileTypeAfniRadioButton->isChecked()) {
         typeOfVolumesToWrite = VolumeFile::FILE_READ_WRITE_TYPE_AFNI;
      }
      else if (fileTypeNiftiRadioButton->isChecked()) {
         typeOfVolumesToWrite = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI;
      }
      else if (fileTypeNiftiGzipRadioButton->isChecked()) {
         typeOfVolumesToWrite = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI_GZIP;
      }
      else {
         QMessageBox::critical(this, "ERROR", "Choose a volume file write type.");
         return;
      }
   }
   
   //
   // If the current page is the gray/white peaks page
   //
   if (currentPage == grayWhitePeaksPage) {
      QString msg;
      if (grayPeakSpinBox->value() <= 0) {
         msg.append("The gray peak must be greater than zero.\n");
      }
      if (whitePeakSpinBox->value() <= 0) {
         msg.append("The white peak must be greater than zero.\n");
      }
      if (whitePeakSpinBox->value() < grayPeakSpinBox->value()) {
         msg.append("The white peak must be greater than the gray peak.\n");
      }
      
      if (msg.isEmpty() == false) {
         QMessageBox::critical(this, "ERROR", msg);
         return;
      }
      
      //
      // Update the parameters file
      //
      ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
      pf->setParameter(ParamsFile::keyCGMpeak, grayPeakSpinBox->value());
      pf->setParameter(ParamsFile::keyWMpeak, whitePeakSpinBox->value());
      if (overrideThreshSetCheckBox->isChecked()) {
         pf->setParameter(ParamsFile::keyWMThreshSet, overridePeakSpinBox->value());
      }
      else {
         pf->setParameter(ParamsFile::keyWMThreshSet, 0);
      }
      
      //
      // Write the parameters file
      //
      try {
         theMainWindow->getBrainSet()->writeParamsFile(pf->getFileName());
         //pf->writeFile(pf->getFileName());
      }
      catch (FileException& e) {
         std::cout << "INFO: Unable to write parameters file: " 
                   << e.whatQString().toAscii().constData() << std::endl;
      }
      
      //
      // Turn off any thresholding
      //
      DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
      dsv->setAnatomyThresholdValid(false);
      GuiBrainModelOpenGL::updateAllGL();
   }
   
   //
   // If segmentation selections page
   //
   if (currentPage == segmentationSelectionsPage) {
      performSegmentation();
   }
   
   //=================================================================================
   //
   // Move to next page
   //
   const int indx = pagesStackedWidget->currentIndex() + 1;   
   if (indx < pagesStackedWidget->count()) {
      QWidget* nextPage = pagesStackedWidget->widget(indx);
      
      //
      // If next page is gray/white peaks page
      //
      if (nextPage == grayWhitePeaksPage) {
         //
         // Doing anatomy volume ?
         //
         const bool anatValid = 
            ((volumeAnatomySelectionControl->getSelectedVolumeFile() != NULL) &&
             (mode == MODE_ANATOMICAL_VOLUME_SEGMENTATION));
         if (anatValid == false) {
            //
            // Skip to segmentation page
            //
            nextPage = segmentationSelectionsPage;
         }
      }

      showPage(nextPage);
   }
}

/**
 * Prepare some pages when they are about to be shown
 */
void
GuiVolumeSureFitSegmentationDialog::showPage(QWidget* page,
                                             const bool backPushButtonPressed)
{
   //
   // If NOT moving forward thru pages
   //
   if (backPushButtonPressed == false) {
      //
      // If about to show the current directory page
      //
      if (page == changeDirectoryPage) {
         updateCurrentDirectoryPage();
      }
      
      //
      // If the page about to show is the subject info page
      //
      if (page == subjectInfoPage) {
         //
         // Set species, subject, and hemisphere
         //
         stereotaxicSpaceComboBox->setSelectedStereotaxicSpace(theMainWindow->getBrainSet()->getStereotaxicSpace());
         speciesComboBox->setSelectedSpecies(theMainWindow->getBrainSet()->getSpecies()); 
         subjectLineEdit->setText(theMainWindow->getBrainSet()->getSubject());
         structureComboBox->setStructure(theMainWindow->getBrainSet()->getStructure().getType());
      }
      
      //
      // If the volume attributes page is about to show
      //
      if (page == volumeAttributesPage) {
         updateVolumeAttributesPage();
      }
      
      //
      // If the current page is the volume file type page
      // 
      if (page == volumeFileTypePage) {
         fileTypeAfniRadioButton->setChecked(false);
         fileTypeNiftiRadioButton->setChecked(false);
         fileTypeNiftiGzipRadioButton->setChecked(false);
         switch (typeOfVolumesToWrite) {
            case VolumeFile::FILE_READ_WRITE_TYPE_RAW:
               break;
            case VolumeFile::FILE_READ_WRITE_TYPE_AFNI:
               fileTypeAfniRadioButton->setChecked(true);
               break;
            case VolumeFile::FILE_READ_WRITE_TYPE_ANALYZE:
               break;
            case VolumeFile::FILE_READ_WRITE_TYPE_NIFTI:
               fileTypeNiftiRadioButton->setChecked(true);
               break;
            case VolumeFile::FILE_READ_WRITE_TYPE_NIFTI_GZIP:
               fileTypeNiftiGzipRadioButton->setChecked(true);
               break;
            case VolumeFile::FILE_READ_WRITE_TYPE_SPM_OR_MEDX: 
               break;
            case VolumeFile::FILE_READ_WRITE_TYPE_WUNIL:
               break;
            case VolumeFile::FILE_READ_WRITE_TYPE_UNKNOWN:
               break;
         }
      }
   

      //
      // If the spec file page is about to be displayed
      //
      if (page == specFilePage) {
         QString name = theMainWindow->getBrainSet()->getSpecFileName();
         if (name.isEmpty()) {
            std::ostringstream str;
            str << QDir::currentPath().toAscii().constData()
                << QString(QDir::separator()).toAscii().constData()
                << theMainWindow->getBrainSet()->getSpecies().getName().toAscii().constData()
                << "."
                << theMainWindow->getBrainSet()->getSubject().toAscii().constData()
                << "."
                << theMainWindow->getBrainSet()->getStructure().getTypeAsString().toAscii().constData()
                << SpecFile::getSpecFileExtension().toAscii().constData();
            name = str.str().c_str();
         }
         specFileNameLineEdit->setText(name);
         slotSpecFileSelected(name);
      }
      
      //
      // If about to show volume selection page
      //
      if (page == volumeSelectionPage) {
         //
         // anatomy selected and no anatomy files but have segmentation files
         //
         if ((volumeAnatomySelectionControl->getSelectedVolumeType() 
                == VolumeFile::VOLUME_TYPE_ANATOMY) &&
             (theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles() <= 0) &&
             (theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles() > 0)) {
            volumeAnatomySelectionControl->setSelectedVolumeType(VolumeFile::VOLUME_TYPE_SEGMENTATION);
         }
         
         //
         // segmentation selected and no segmentation files but have anatomy files
         //
         if ((volumeSegmentationSelectionControl->getSelectedVolumeType() 
                == VolumeFile::VOLUME_TYPE_SEGMENTATION) &&
             (theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles() <= 0) &&
             (theMainWindow->getBrainSet()->getNumberOfVolumeAnatomyFiles() > 0)) {
                  volumeSegmentationSelectionControl->setSelectedVolumeType(VolumeFile::VOLUME_TYPE_ANATOMY);
         }
      }
      
      //
      // If the page about to be shown is the gray white peaks page
      //
      if (page == grayWhitePeaksPage) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         
         ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
         int gray = 0;
         int white = 0;
         pf->getParameter(ParamsFile::keyCGMpeak, gray);
         pf->getParameter(ParamsFile::keyWMpeak, white);
         
         int peakOverride = 0;
         pf->getParameter(ParamsFile::keyWMThreshSet, peakOverride);
         overridePeakSpinBox->setValue(peakOverride);
         overrideThreshSetCheckBox->setChecked(peakOverride != 0);
         overridePeakSpinBox->setEnabled(overrideThreshSetCheckBox->isChecked());
         
         //
         // Get the selected volume
         //
         VolumeFile* vfa = volumeAnatomySelectionControl->getSelectedVolumeFile();
         if (vfa != NULL) {
            if (peakHistogram != NULL) {
               delete peakHistogram;
               peakHistogram = NULL;
            }
            peakHistogram = vfa->getHistogram();
            loadHistogramIntoGraph();

            //
            // Estimate the peaks
            //
            int grayBucket, whiteBucket, grayMinBucket, whiteMaxBucket, grayWhiteBucket, csfBucket;
            peakHistogram->getGrayWhitePeakEstimates(grayBucket, 
                                                     whiteBucket, 
                                                     grayMinBucket, 
                                                     whiteMaxBucket, 
                                                     grayWhiteBucket,
                                                     csfBucket);
            int csfPeakEstimate = 0;
            int grayPeakEstimate = 0;
            int whitePeakEstimate = 0;
            if (grayBucket >= 0) {
               csfPeakEstimate = static_cast<int>(peakHistogram->getDataValueForBucket(csfBucket));
               grayPeakEstimate = static_cast<int>(peakHistogram->getDataValueForBucket(grayBucket));
               whitePeakEstimate = static_cast<int>(peakHistogram->getDataValueForBucket(whiteBucket));
            }
            csfPeakLabel->setNum(csfPeakEstimate);
            grayPeakLabel->setNum(grayPeakEstimate);
            whitePeakLabel->setNum(whitePeakEstimate);
            
            //
            // If no peaks yet, use estimates
            //
            if ((gray == 0) && (white == 0)) {
               gray = grayPeakEstimate;
               white = whitePeakEstimate;
            }

            //
            // Initialize graph scaling
            //
            if (initializeGraphScaleFlag) {
               const float minX = gray - 5.0;
               const float maxX = white + 10.0;
               initializeGraphScaleFlag = false;
               float theMax = -1.0;
               
               //
               // Use max of gray and white if available
               //
               if ((grayBucket > 0) &&
                   (whiteBucket > 0)) {
                  if (grayBucket > whiteBucket) {
                     float x, y;
                     peakHistogram->getDataForBucket(grayBucket, x, y);
                     theMax = y;
                  }
                  else {
                     float x, y;
                     peakHistogram->getDataForBucket(whiteBucket, x, y);
                     theMax = y;
                  }
                  theMax *= 2.0;
               }
               else {
                  const int numBuckets = peakHistogram->getNumberOfBuckets();
                  for (int j = 0; j < numBuckets; j++) {
                     float x, y;
                     peakHistogram->getDataForBucket(j, x, y);
                     if ((x >= minX) && (x <= maxX)) {
                        theMax = std::max(theMax, y) * 1.75;
                     }
                  }
               }
               if (theMax > 0.0) {
                  peakHistogramWidget->setScaleYMaximum(theMax);
               }
            }
            
            //
            // Set the controls
            //
            grayPeakSpinBox->setValue(gray);
            whitePeakSpinBox->setValue(white);
            
            //
            // Update histogram widget
            //
            peakHistogramWidget->slotSetMinimumPeak(grayPeakSpinBox->value());
            peakHistogramWidget->slotSetMaximumPeak(whitePeakSpinBox->value());
            
            //
            // For volume coloring
            //
            if ((whitePeakRadioButton->isChecked() == false) && 
                (grayPeakRadioButton->isChecked() == false)) {
               grayPeakRadioButton->setChecked(true);
            }
            slotPeakRadioButton();
         }
         
         QApplication::restoreOverrideCursor();
      }
      
      //
      // If the segmentation selections
      //
      if (page == segmentationSelectionsPage) {
         slotEnableDisableSelectionCheckBoxes();
      }
      
      //
      // If the segmentation complete page is about to show
      //
      if (page == segmentationCompletePage) {
         //
         // Total execution time
         //
         std::ostringstream str;
         str << "Total Time: "
             << segmentationTime
             << " seconds.";
         segmentationCompleteTimeLabel->setText(str.str().c_str());
         
         //
         // Handles in fiducial surface
         //
         str.str("");
         if (generateRawAndFiducialSurfaceCheckBox->isEnabled() &&
             generateRawAndFiducialSurfaceCheckBox->isChecked()) {
            BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
            if (bms != NULL) {
               const TopologyFile* tf = bms->getTopologyFile();
               if (tf != NULL) {
                  int numFaces,
                      numVertices,
                      numEdges,
                      eulerCount,
                      numberOfHoles,
                      numberOfObjects;
                  tf->getEulerCount(false,
                                    numFaces,
                                    numVertices,
                                    numEdges,
                                    eulerCount,
                                    numberOfHoles,
                                    numberOfObjects);
                                    
                  if ((numberOfHoles == 0) && (numberOfObjects == 1)) {
                     str << "The fiducial surface has no topological defects.";
                  }
                  else {
                     if (numberOfHoles > 0) {
                        str << "The fiducial surface has "
                            << numberOfHoles
                            << " topological defects\n";
                     }
                     if (numberOfObjects > 1) {
                        str << "The fiducial surface has "
                            << (numberOfObjects - 1)
                            << " islands.\n";
                     }
                  }
               }
            }
         }
         segmentationCompleteHandleLabel->setText(str.str().c_str());
      }
   } // moving forward thru pages
   
   //
   // Show the pages
   //
   pagesStackedWidget->setCurrentWidget(page);  
   currentPageLegend->setText(pageLegends[pagesStackedWidget->currentIndex()]); 
   
   slotEnableDisablePushButtons();
}

/**
 * load histogram into graph.
 */
void 
GuiVolumeSureFitSegmentationDialog::loadHistogramIntoGraph()
{
   //
   // Clear all data from the graph
   //
   peakHistogramWidget->removeAllData();
         
   //
   // Put data in histogram widget
   //
   const int numBuckets = peakHistogram->getNumberOfBuckets();
   std::vector<double> dataX, dataY;
   for (int j = 0; j < numBuckets; j++) {
      float x, y;
      peakHistogram->getDataForBucket(j, x, y);
      if (j == 0) {
         if (x > 0.0) {
            dataX.push_back(0.0);
            dataY.push_back(0.0);
         }
      }
      dataX.push_back(x);
      dataY.push_back(y);
      if (j == (numBuckets - 1)) {
         if (x < 255.0) {
            dataX.push_back(255.0);
            dataY.push_back(0.0);
         }
      }
   }
   peakHistogramWidget->addData(dataX, dataY, 
                                QColor(255, 0, 0), 
                                GuiGraphWidget::DRAW_DATA_TYPE_BARS);
}

/**
 * called when smooth button pressed.
 */
void 
GuiVolumeSureFitSegmentationDialog::slotSmoothHistogramPushButton()
{
   double xMin, xMax, yMin, yMax;
   peakHistogramWidget->getGraphMinMax(xMin, xMax, yMin, yMax);
   
   const float strength = 0.50;
   const int iterations = 1;
   const int neighborDepth = 3;
   peakHistogram->smoothHistogram(strength,
                                  iterations,
                                  neighborDepth);
   loadHistogramIntoGraph();
   peakHistogramWidget->setGraphMinMax(xMin, xMax, yMin, yMax);
}

/**
 * Enable/Disable push buttons.
 */
void
GuiVolumeSureFitSegmentationDialog::slotEnableDisablePushButtons()
{
   bool backValid = false;
   bool nextValid = false;
   
   //
   // Get the current page
   //
   QWidget* currentPage = pagesStackedWidget->currentWidget();
   
   if (currentPage == changeDirectoryPage) {
      if (currentDirectoryIsCaretInstallationDirectory() == false) {
         nextValid = true;
      }
   }
   else if (currentPage == subjectInfoPage) {
      backValid = true;
      if (speciesComboBox->getSelectedSpecies().isValid() &&
          stereotaxicSpaceComboBox->getSelectedStereotaxicSpace().isValid() &&
          (subjectLineEdit->text().isEmpty() == false) &&
          (structureComboBox->getSelectedStructure() != Structure::STRUCTURE_TYPE_INVALID)) {
         nextValid = true;
      }
   }
   else if (currentPage == specFilePage) {
      backValid = true;
      if (specFileNameLineEdit->text().isEmpty() == false) {
         nextValid = true;
      }
   }
   else if (currentPage == volumeSelectionPage) {
      backValid = true;
      volumeSelectionGroupBox->setEnabled(false);
      volumeAnatomySelectionControl->setEnabled(false);
      volumeSegmentationSelectionControl->setEnabled(false);
      
      mode = MODE_NONE;
      if (modeAnatomicalVolumeProcessingRadioButton->isChecked()) {
         mode = MODE_ANATOMICAL_VOLUME_SEGMENTATION;
      }
      else if (modeSegmentationVolumeProcessingRadioButton->isChecked()) {
         mode = MODE_SEMENTATION_VOLUME_POST_PROCESSING;
      }
      
      switch (mode) {
         case MODE_NONE:
            break;
         case MODE_ANATOMICAL_VOLUME_SEGMENTATION:
            volumeSelectionGroupBox->setEnabled(true);
            volumeAnatomySelectionControl->setEnabled(true);
            if (volumeAnatomySelectionControl->getSelectedVolumeFile() != NULL) {
               nextValid = true;
            }
            break;
         case MODE_SEMENTATION_VOLUME_POST_PROCESSING:
            volumeSelectionGroupBox->setEnabled(true);
            volumeAnatomySelectionControl->setEnabled(true);
            volumeSegmentationSelectionControl->setEnabled(true);
            if (volumeSegmentationSelectionControl->getSelectedVolumeFile() != NULL) {
               nextValid = true;
            }
            break;
      }
   }
   else if (currentPage == volumeFileTypePage) {
      if (fileTypeAfniRadioButton->isChecked()) {
         typeOfVolumesToWrite = VolumeFile::FILE_READ_WRITE_TYPE_AFNI;
      }
      else if (fileTypeNiftiGzipRadioButton->isChecked()) {
         typeOfVolumesToWrite = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI_GZIP;
      }
      else if (fileTypeNiftiRadioButton->isChecked()) {
         typeOfVolumesToWrite = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI;
      }
      backValid = true;
      nextValid = (fileTypeAfniRadioButton->isChecked() ||
                   fileTypeNiftiRadioButton->isChecked() ||
                   (fileTypeNiftiGzipRadioButton->isChecked()));
   }
   else if (currentPage == volumeAttributesPage) {
      backValid = true;
      nextValid = true;
   }
   else if (currentPage == grayWhitePeaksPage) {
      backValid = true;
      nextValid = true;
   }
   else if (currentPage == segmentationSelectionsPage) {
      backValid = true;
      nextValid = true;
   }
   else if (currentPage == segmentationCompletePage) {
      backValid = true;
      nextValid = false;
   }
   
   backPushButton->setEnabled(backValid);
   nextPushButton->setEnabled(nextValid);
}

/**
 * Perform the segmentation.
 */
bool 
GuiVolumeSureFitSegmentationDialog::performSegmentation()
{
   BrainModelVolume* bmv = theMainWindow->getBrainSet()->getBrainModelVolume();
   if (bmv == NULL) {
      QMessageBox::critical(this, "ERROR", "There are no volumes loaded.");
      return true;
   }
   
   VolumeFile* anatomyVolume = volumeAnatomySelectionControl->getSelectedVolumeFile();
   VolumeFile* segmentationVolume = volumeSegmentationSelectionControl->getSelectedVolumeFile();

   //
   // Get the selected volume
   //
   switch (mode) {
      case MODE_NONE:
         break;
      case MODE_ANATOMICAL_VOLUME_SEGMENTATION:
         if (anatomyVolume == NULL) {
            QMessageBox::critical(this, "ERROR", "No anatomical volume selected");
         }
         break;
      case MODE_SEMENTATION_VOLUME_POST_PROCESSING:
         if (segmentationVolume == NULL) {
            QMessageBox::critical(this, "ERROR", "No segmentation volume selected");
         }
         break;
   }
   
   if ((anatomyVolume == NULL) && (segmentationVolume == NULL)) {
      QMessageBox::critical(this, "ERROR", "No Anatomical Volume is Selected.");
      return true;
   }
   
   const float zeros[3] = { 0.0, 0.0, 0.0 };
   int acIJK[3] = { 0, 0, 0 };
   if (anatomyVolume != NULL) {
      if (anatomyVolume->convertCoordinatesToVoxelIJK(zeros, acIJK) == false) {
         if (QMessageBox::warning(this, "WARNING", 
                                    "The Anterior Commissure is not located inside the volume."
                                    "If you are segmenting a partial hemisphere, this is "
                                    "probably okay.  If you are segmenting a full hemisphere,"
                                    "the origin of the volume is probably not set correctly.",
                                    (QMessageBox::Ok | QMessageBox::Cancel),
                                    QMessageBox::Cancel)
                                       == QMessageBox::Cancel) {
            return true;
         }
      }
   }
   else if (segmentationVolume != NULL) {
      if (segmentationVolume->convertCoordinatesToVoxelIJK(zeros, acIJK) == false) {
         if (QMessageBox::warning(this, "WARNING", 
                                    "The Anterior Commissure is not located inside the volume."
                                    "If you are segmenting a partial hemisphere, this is "
                                    "probably okay.  If you are segmenting a full hemisphere,"
                                    "the origin of the volume is probably not set correctly.",
                                    (QMessageBox::Ok | QMessageBox::Cancel),
                                    QMessageBox::Cancel)
                                       == QMessageBox::Cancel) {
            return true;
         }
      }
   }
   
   //
   // Notify user about surface files in the spec file being removed
   //
   if (generateRawAndFiducialSurfaceCheckBox->isChecked() &&
       generateRawAndFiducialSurfaceCheckBox->isEnabled()) {
      if (QMessageBox::information(this, "INFORMATION",
            "You have chosen to generate raw and fiducial surfaces.  "
            "If the selected spec file contains any surface type files"
            "(coordinate, metric, topology, etc.) they will be removed"
            "from the spec file and deleted from the disk.  These files"
            "will be deleted since the number of nodes in the newly "
            "generated files will be different and thus incompatible "
            " with the previously generated files.\n",
            (QMessageBox::Ok | QMessageBox::Cancel),
            QMessageBox::Ok)
               == QMessageBox::Cancel) {
         return true;
      }
   }
   
   const Structure::STRUCTURE_TYPE hemisphere = structureComboBox->getSelectedStructure();
   
   //
   // If generating raw, fiducial, and ellipsoid surfaces
   //
   int paddingAmount[6] = { 0, 0, 0, 0, 0, 0 };
   if ((generateRawAndFiducialSurfaceCheckBox->isChecked() &&
        generateRawAndFiducialSurfaceCheckBox->isEnabled()) &&
       (generateEllipsoidSurfaceCheckBox->isChecked() &&
        generateEllipsoidSurfaceCheckBox->isEnabled())) {
        
      //
      // Check parameters file for padding from previous runs
      //
      int negX, posX, negY, posY, negZ, posZ;
      ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
      pf->getParameter(ParamsFile::keyOldPadNegX, negX);
      pf->getParameter(ParamsFile::keyOldPadPosX, posX);
      pf->getParameter(ParamsFile::keyOldPadNegY, negY);
      pf->getParameter(ParamsFile::keyOldPadPosY, posY);
      pf->getParameter(ParamsFile::keyOldPadNegZ, negZ);
      pf->getParameter(ParamsFile::keyOldPadPosZ, posZ);
      
      QApplication::beep();
      
      //
      // Dialog for optional padding
      //
      WuQDataEntryDialog pad(this);
      pad.setWindowTitle("Partial Hemisphere Padding");
      pad.setTextAtTop("If the volume being reconstructed is a full hemisphere,\n"
                  "no padding is needed.  In this case, leave all checkboxes\n"
                  "unchecked and press the \"OK\" button.\n"
                  "\n"
                  "If the volume being reconstructed is a partial hemisphere,\n"
                  "please identify the location(s) of the cuts.  Prior to \n"
                  "reconstructing the surface, the volume will be padded where\n"
                  "cuts have been made.  This padding is necessary so that \n"
                  "the surface can be flattened in Caret.  After identifying\n"
                  "the cuts, press the \"OK\" button to continue.",
                  false);
      QCheckBox* negxCheckBox = pad.addCheckBox("Negative X", negX != 0);
      QCheckBox* posxCheckBox = pad.addCheckBox("Positive X", posX != 0);
      QCheckBox* negyCheckBox = pad.addCheckBox("Negative Y (Posterior)", negY != 0);
      QCheckBox* posyCheckBox = pad.addCheckBox("Positive Y (Anterior)", posY != 0);
      QCheckBox* negzCheckBox = pad.addCheckBox("Negative Z (Inferior)", negZ != 0);
      QCheckBox* poszCheckBox = pad.addCheckBox("Positive Z (Superior)", posZ != 0);
      switch (hemisphere) {
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
            negxCheckBox->setText("Negative X (Lateral)");
            posxCheckBox->setText("Positive X (Medial)");
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
            negxCheckBox->setText("Negative X (Medial)");
            posxCheckBox->setText("Positive X (Lateral)");
            break;
         case Structure::STRUCTURE_TYPE_CEREBELLUM:
            break;
         case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
            break;
         case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
            break;
         case Structure::STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
         case Structure::STRUCTURE_TYPE_SUBCORTICAL:
         case Structure::STRUCTURE_TYPE_ALL:
         case Structure::STRUCTURE_TYPE_INVALID:
            break;
      }
      QSpinBox* paddingAmountSpinBox = pad.addSpinBox("Padding Slices",
                                                       30, 1, 500, 1);     
      if (pad.exec() == QDialog::Accepted) {
         if (negxCheckBox->isChecked()) {
            paddingAmount[0] = paddingAmountSpinBox->value();
         }
         if (posxCheckBox->isChecked()) {
            paddingAmount[1] = paddingAmountSpinBox->value();
         }
         if (negyCheckBox->isChecked()) {
            paddingAmount[2] = paddingAmountSpinBox->value();
         }
         if (posyCheckBox->isChecked()) {
            paddingAmount[3] = paddingAmountSpinBox->value();
         }
         if (negzCheckBox->isChecked()) {
            paddingAmount[4] = paddingAmountSpinBox->value();
         }
         if (poszCheckBox->isChecked()) {
            paddingAmount[5] = paddingAmountSpinBox->value();
         }
         
         //
         // Update params file with padding
         //
         pf->setParameter(ParamsFile::keyOldPadNegX, paddingAmount[0]);
         pf->setParameter(ParamsFile::keyOldPadPosX, paddingAmount[1]);
         pf->setParameter(ParamsFile::keyOldPadNegY, paddingAmount[2]);
         pf->setParameter(ParamsFile::keyOldPadPosY, paddingAmount[3]);
         pf->setParameter(ParamsFile::keyOldPadNegZ, paddingAmount[4]);
         pf->setParameter(ParamsFile::keyOldPadPosZ, paddingAmount[5]);
         try {
            theMainWindow->getBrainSet()->writeParamsFile(pf->getFileName());
         }
         catch (FileException&) {
         }
      }
      else {
         return true;
      }
   }
         
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    
   BrainModelVolumeSureFitSegmentation::ERROR_CORRECTION_METHOD
      errorCorrectionMethod = BrainModelVolumeSureFitSegmentation::ERROR_CORRECTION_METHOD_NONE;
   if (volumeErrorCorrectionCheckBox->isEnabled() &&
       volumeErrorCorrectionCheckBox->isChecked()) {
      errorCorrectionMethod = 
         static_cast<BrainModelVolumeSureFitSegmentation::ERROR_CORRECTION_METHOD>(
            volumeErrorCorrectionMethodComboBox->itemData(
               volumeErrorCorrectionMethodComboBox->currentIndex()).toInt());
   }
   
   //
   // Segmentation algorithm
   //
   BrainModelVolumeSureFitSegmentation bmvs(theMainWindow->getBrainSet(), 
                                     anatomyVolume,
                                     segmentationVolume,
                                     typeOfVolumesToWrite,
                                     acIJK,
                                     paddingAmount,
                                     whitePeakSpinBox->value(),
                                     grayPeakSpinBox->value(),
                                     0.0,
                                     hemisphere,
                                     disconnectEyeCheckBox->isChecked() &&
                                        disconnectEyeCheckBox->isEnabled(),
                                     disconnectHindbrainCheckBox->isChecked() &&
                                        disconnectHindbrainCheckBox->isEnabled(),
                                     (hindLoHiThreshComboBox->currentIndex() == HIND_BRAIN_THRESHOLD_HIGH),
                                     cutCorpusCallosumCheckBox->isChecked() &&
                                        cutCorpusCallosumCheckBox->isEnabled(),
                                     generateSegmentationCheckBox->isChecked() &&
                                        generateSegmentationCheckBox->isEnabled(),
                                     fillVentriclesCheckBox->isChecked() &&
                                        fillVentriclesCheckBox->isEnabled(),
                                     errorCorrectionMethod,
                                     generateRawAndFiducialSurfaceCheckBox->isChecked() &&
                                        generateRawAndFiducialSurfaceCheckBox->isEnabled(),
                                     (generateSurfaceLimitPolygonsCheckBox->isChecked() == false),
                                     generateTopologicallyCorrectFiducialSurfaceCheckBox->isChecked() &&
                                        generateTopologicallyCorrectFiducialSurfaceCheckBox->isEnabled(),
                                     generateInflatedSurfaceCheckBox->isChecked() &&
                                        generateInflatedSurfaceCheckBox->isEnabled(),
                                     generateVeryInflatedSurfaceCheckBox->isChecked() &&
                                        generateVeryInflatedSurfaceCheckBox->isEnabled(),
                                     generateEllipsoidSurfaceCheckBox->isChecked() &&
                                        generateEllipsoidSurfaceCheckBox->isEnabled(),
                                     generateSphericalSurfaceCheckBox->isChecked() &&
                                        generateSphericalSurfaceCheckBox->isEnabled(),
                                     generateCompMedWallSurfaceCheckBox->isChecked() &&
                                        generateCompMedWallSurfaceCheckBox->isEnabled(),
                                     generateHullCheckBox->isChecked() &&
                                        generateHullCheckBox->isEnabled(),
                                     generateDepthCurvatureGeographyCheckBox->isChecked() &&
                                        generateDepthCurvatureGeographyCheckBox->isEnabled(),
                                     generateLandmarkBordersCheckBox->isChecked() &&
                                        generateLandmarkBordersCheckBox->isEnabled(),
                                     autoSaveFilesCheckBox->isChecked());

   //
   // Make the dialog disappear while the algorithms are executed
   //
   this->hide();
   
   QTime algorithmTimer;
   algorithmTimer.start();
   segmentationTime = 0.0;
   try {
      bmvs.execute();
      WuQDialog::show();  // use this otherwise initial page gets shown
      segmentationTime = algorithmTimer.elapsed() * 0.001;
      
      //
      // make newest segmentation the selected segmentation
      //
      DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
      if (theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles() > 0) {
         dsv->setSelectedSegmentationVolume(theMainWindow->getBrainSet()->getNumberOfVolumeSegmentationFiles() - 1);
      }
      
      //
      // Make segmentation the primary overlay, no secondary overlay,
      // and set the underlay to anatomy
      //
      BrainModelVolumeVoxelColoring* bmvvc = theMainWindow->getBrainSet()->getVoxelColoring();
      bmvvc->setPrimaryOverlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION);
      bmvvc->setSecondaryOverlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE);
      bmvvc->setUnderlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY);
      
   }
   catch (BrainModelAlgorithmException& e) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, "ERROR", e.whatQString());
      WuQDialog::show();  // use this otherwise initial page gets shown
      segmentationTime = algorithmTimer.elapsed() * 0.001;
      return true;
   }
   
   //
   // If there is a fiducial surface, show it in the main window
   //
   BrainModelSurface* fidbms = theMainWindow->getBrainSet()->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   if (fidbms != NULL) {
      theMainWindow->displayBrainModelInMainWindow(fidbms);
   }
   
   //
   // Notify GUI that volumes have changed
   //
   GuiFilesModified fm;
   fm.setStatusForAll(true);
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
   QApplication::restoreOverrideCursor();
   QApplication::beep();
   
   return false;
}
      
/**
 * Update the dialog.
 */
void 
GuiVolumeSureFitSegmentationDialog::updateDialog()
{
   volumeAnatomySelectionControl->updateControl();
   volumeSegmentationSelectionControl->updateControl();
   const VolumeFile* vfa = volumeAnatomySelectionControl->getSelectedVolumeFile();
   const VolumeFile* vfs = volumeSegmentationSelectionControl->getSelectedVolumeFile();

   if ((vfa == NULL) &&
       (vfs == NULL)) {
      //
      // clear entries on first page and close dialog
      //
      WuQDialog::close();
   }
}

      
