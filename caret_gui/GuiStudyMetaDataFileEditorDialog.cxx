
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

#include <iostream>
#include <limits>
#include <set>

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QRegExpValidator>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTextCursor>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

#include "BrainSet.h"
#include "DisplaySettingsStudyMetaData.h"
#include "DisplaySettingsFoci.h"
#include "FileFilters.h"
#include "FociProjectionFile.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiNameSelectionDialog.h"
#define __GUI_STUDY_META_DATA_FILE_EDITOR_DIALOG_MAIN_
#include "GuiStudyMetaDataFileEditorDialog.h"
#undef  __GUI_STUDY_META_DATA_FILE_EDITOR_DIALOG_MAIN_
#include "HttpFileDownload.h"
#include "NameIndexSort.h"
#include "PubMedArticleFile.h"
#include "QtListBoxSelectionDialog.h"
#include "QtUtilities.h"
#include "WuQFileDialog.h"
#include "WuQWidgetGroup.h"
#include "StudyMetaDataFile.h"
#include "global_variables.h"

const int minimumTextEditHeight = 150;

/**
 * constructor.
 */
GuiStudyMetaDataFileEditorDialog::GuiStudyMetaDataFileEditorDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setMinimumWidth(300);
   currentStudyMetaDataFileIndex = -1;
   setWindowTitle("Study Metadata Editor");
   
   //
   // Create the study selection push buttons
   //
   QWidget* studyPushButtonWidget = createStudySelectionButtons();
   
   //
   // Layout for left side containing buttons
   //
   QVBoxLayout* leftLayout = new QVBoxLayout;
   leftLayout->addWidget(studyPushButtonWidget);
   leftLayout->addStretch();
   
   //
   // widget and its layout for editing widgets
   //
   QWidget* editingWidget = new QWidget;
   editingWidgetsLayout = new QVBoxLayout(editingWidget);
   
   //
   // Scroll area for editing studies
   // 
   QScrollArea* editingScrollArea = new QScrollArea;
   editingScrollArea->setWidget(editingWidget);
   editingScrollArea->setWidgetResizable(true);
   
   //
   // Place the selection and editing widgets in a horizontal box
   //
   QHBoxLayout* horizLayout = new QHBoxLayout;
   horizLayout->addLayout(leftLayout);
   horizLayout->addWidget(editingScrollArea);
   
   //
   // add to the dialog layout
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addLayout(horizLayout);
      
   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Help 
                                                      | QDialogButtonBox::Close);
   dialogLayout->addWidget(buttonBox);
   QObject::connect(buttonBox, SIGNAL(helpRequested()),
                    this, SLOT(slotHelpPushButton()));
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(close()));

   //
   // If there are no studies, create one
   //
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   if (smdf->getNumberOfStudyMetaData() <= 0) {
      StudyMetaData* smd = new StudyMetaData;
      if (smd != NULL) {
         smdf->addStudyMetaData(smd);
         smdf->clearModified();
      }
   }

   updateDialog();
}

/**
 * destructor.
 */
GuiStudyMetaDataFileEditorDialog::~GuiStudyMetaDataFileEditorDialog()
{
}

/**
 * called when help button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotHelpPushButton()
{
   theMainWindow->showHelpViewerDialog("dialogs/study_metadata.html");
}

/**
 * called to close the dialog.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotCloseDialog()
{
   if (getCurrentStudyWidget() != NULL) {
      getCurrentStudyWidget()->slotStudyKeywordsLineEditFinished();
   }

   //
   // Save any data in dialog
   //
   saveStudyMetaDataFromDialog();
   
   //
   // close the dialog
   //
   close();
}
      
/**
 * update the dialog.
 */
void 
GuiStudyMetaDataFileEditorDialog::updateDialog()
{
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   if (currentStudyMetaDataFileIndex >= smdf->getNumberOfStudyMetaData()) {
      currentStudyMetaDataFileIndex = smdf->getNumberOfStudyMetaData() - 1;
   }
   if (currentStudyMetaDataFileIndex < 0) {
      if (smdf->getNumberOfStudyMetaData() > 0) {
         currentStudyMetaDataFileIndex = 0;
      }
      else {
         currentStudyMetaDataFileIndex = -1;
      }
   }
   
   updateStudySelectionSpinBox();
   
   loadStudyMetaDataIntoDialog(true);
}      

/**
 * get the current study widget.
 */
StudyWidget* 
GuiStudyMetaDataFileEditorDialog::getCurrentStudyWidget()
{
   //
   // The study data widget will be the one and only widget in the layout
   //
   if (editingWidgetsLayout->count() > 0) {
      QLayoutItem* li = editingWidgetsLayout->itemAt(0);
      if (li != NULL) {
         QWidget* w = li->widget();
         if (w != NULL) {
           StudyWidget* sw = dynamic_cast<StudyWidget*>(w);
           return sw;
         }
      }
   }
   
   return NULL;
}

/**
 * save study metadata currently in dialog.
 */
void 
GuiStudyMetaDataFileEditorDialog::saveStudyMetaDataFromDialog()
{
  StudyWidget* sw = getCurrentStudyWidget();
  if (sw != NULL) {
     sw->saveDataIncludingFiguresAndTables();
  }
}
      
/**
 * load study metadata into the dialog.
 */
void 
GuiStudyMetaDataFileEditorDialog::loadStudyMetaDataIntoDialog(const bool saveCurrentDataFlag)
{
   if (saveCurrentDataFlag) {
      saveStudyMetaDataFromDialog();
   }
   
   //
   // Remove previous widget and destroy it
   //
   if (editingWidgetsLayout->count() > 0) {
      QLayoutItem* li = editingWidgetsLayout->takeAt(0);
      if (li != NULL) {
         QWidget* w = li->widget();
         if (w != NULL) {
            delete w;
         }
      }
   }
   
   //
   // Load study into dialog
   //
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   if ((currentStudyMetaDataFileIndex >= 0) &&
       (currentStudyMetaDataFileIndex < smdf->getNumberOfStudyMetaData())) {
      StudyMetaData* smd = smdf->getStudyMetaData(currentStudyMetaDataFileIndex);
      
      QWidget* sw = new StudyWidget(this, smd);
      editingWidgetsLayout->addWidget(sw);
      
      //std::cout << "there are " << editingWidgetsLayout->count() << " items in layout." << std::endl;
   }
   
   slotEnableDisablePushButtons();
}
      
/**
 * update the study selection combo box.
 */
void 
GuiStudyMetaDataFileEditorDialog::updateStudySelectionSpinBox()
{
   const StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   studySelectionSpinBox->blockSignals(true);
   studySelectionSpinBox->setMinimum(1);
   studySelectionSpinBox->setMaximum(smdf->getNumberOfStudyMetaData());
   studySelectionSpinBox->setValue(currentStudyMetaDataFileIndex + 1);
   studySelectionSpinBox->blockSignals(false);
}
      
/**
 * called study selection spin box value changed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotStudySelectionSpinBoxValueChanged(int val)
{
   //if (getCurrentStudyWidget() != NULL) {
   //   getCurrentStudyWidget()->slotStudyKeywordsLineEditFinished();
   //}
   
   currentStudyMetaDataFileIndex = val - 1;
   const StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   if (currentStudyMetaDataFileIndex >= smdf->getNumberOfStudyMetaData()) {
      currentStudyMetaDataFileIndex = smdf->getNumberOfStudyMetaData() - 1;
   }
   if (currentStudyMetaDataFileIndex < 0) {
      currentStudyMetaDataFileIndex = 0;
   }
   
   slotEnableDisablePushButtons();
   loadStudyMetaDataIntoDialog(true);
}
      
/**
 * called when first study button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotFirstStudyPushButton()
{
   if (getCurrentStudyWidget() != NULL) {
      getCurrentStudyWidget()->slotStudyKeywordsLineEditFinished();
   }
   currentStudyMetaDataFileIndex = 0;
   updateStudySelectionSpinBox();
   loadStudyMetaDataIntoDialog(true);
}

/**
 * called when study title button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotStudyChooseTitlePushButton()
{
   //
   // Get and sort the titles
   //
   const StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   const int num = smdf->getNumberOfStudyMetaData();
   NameIndexSort nis;
   for (int i = 0; i < num; i++) {
      const StudyMetaData* smd = smdf->getStudyMetaData(i);
      nis.add(i, smd->getTitle());
   }   
   nis.sortByNameCaseSensitive();
   
   //
   // Place sorted titles into a list selection dialog
   //
   std::vector<QString> titlesSorted;
   const int numItems = nis.getNumberOfItems();
   for (int i = 0; i < numItems; i++) {
      int indx;
      QString title;
      nis.getSortedNameAndIndex(i, indx, title);
      titlesSorted.push_back(title);
   }
   QtListBoxSelectionDialog lbsd(this,
                                 "Choose Study by Title",
                                 "",
                                 titlesSorted);
   if (lbsd.exec() == QtListBoxSelectionDialog::Accepted) {
      const int itemNum = lbsd.getSelectedItemIndex();
      int indx;
      QString title;
      nis.getSortedNameAndIndex(itemNum, indx, title);

      //
      // Show the new study
      //
      currentStudyMetaDataFileIndex = indx;
      updateStudySelectionSpinBox();
      loadStudyMetaDataIntoDialog(true);
   }
}
      
/**
 * called when study name button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotStudyChooseNamePushButton()
{
   //
   // Get and sort the names
   //
   const StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   const int num = smdf->getNumberOfStudyMetaData();
   NameIndexSort nis;
   for (int i = 0; i < num; i++) {
      const StudyMetaData* smd = smdf->getStudyMetaData(i);
      nis.add(i, smd->getName());
   }   
   nis.sortByNameCaseSensitive();
   
   //
   // Place sorted authors into a list selection dialog
   //
   std::vector<QString> namesSorted;
   const int numItems = nis.getNumberOfItems();
   for (int i = 0; i < numItems; i++) {
      int indx;
      QString name;
      nis.getSortedNameAndIndex(i, indx, name);
      namesSorted.push_back(name);
   }
   QtListBoxSelectionDialog lbsd(this,
                                 "Choose Study by Name",
                                 "",
                                 namesSorted);
   if (lbsd.exec() == QtListBoxSelectionDialog::Accepted) {
      const int itemNum = lbsd.getSelectedItemIndex();
      int indx;
      QString name;
      nis.getSortedNameAndIndex(itemNum, indx, name);

      //
      // Show the new study
      //
      currentStudyMetaDataFileIndex = indx;
      updateStudySelectionSpinBox();
      loadStudyMetaDataIntoDialog(true);
   }
}

/**
 * called when study author button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotStudyChooseAuthorPushButton()
{
   //
   // Get and sort the authors
   //
   const StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   const int num = smdf->getNumberOfStudyMetaData();
   NameIndexSort nis;
   for (int i = 0; i < num; i++) {
      const StudyMetaData* smd = smdf->getStudyMetaData(i);
      nis.add(i, smd->getAuthors());
   }   
   nis.sortByNameCaseSensitive();
   
   //
   // Place sorted authors into a list selection dialog
   //
   std::vector<QString> authorsSorted;
   const int numItems = nis.getNumberOfItems();
   for (int i = 0; i < numItems; i++) {
      int indx;
      QString author;
      nis.getSortedNameAndIndex(i, indx, author);
      authorsSorted.push_back(author);
   }
   QtListBoxSelectionDialog lbsd(this,
                                 "Choose Study by Author",
                                 "",
                                 authorsSorted);
   if (lbsd.exec() == QtListBoxSelectionDialog::Accepted) {
      const int itemNum = lbsd.getSelectedItemIndex();
      int indx;
      QString author;
      nis.getSortedNameAndIndex(itemNum, indx, author);

      //
      // Show the new study
      //
      currentStudyMetaDataFileIndex = indx;
      updateStudySelectionSpinBox();
      loadStudyMetaDataIntoDialog(true);
   }
}
      
/**
 * called when last study button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotLastStudyPushButton()
{
   if (getCurrentStudyWidget() != NULL) {
      getCurrentStudyWidget()->slotStudyKeywordsLineEditFinished();
   }
   const StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   currentStudyMetaDataFileIndex = smdf->getNumberOfStudyMetaData() - 1;
   if (currentStudyMetaDataFileIndex < 0) {
      currentStudyMetaDataFileIndex = 0;
   }
   updateStudySelectionSpinBox();
   loadStudyMetaDataIntoDialog(true);
}

/**
 * called when new study button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotNewStudyPushButton()
{
   StudyMetaData* currentStudyMetaData = NULL;
   
   StudyWidget* sw = getCurrentStudyWidget();
   if (sw != NULL) {
      currentStudyMetaData = sw->getCurrentStudyMetaData();
   }
   
   GuiStudyMetaDataNewDialog smdnd(currentStudyMetaData,
                                   this);
   if (smdnd.exec() == GuiStudyMetaDataNewDialog::Accepted) {
      StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
      StudyMetaData* smd = smdnd.getNewStudy();
      if (smd != NULL) {
         smdf->addStudyMetaData(smd);
         currentStudyMetaDataFileIndex = smdf->getNumberOfStudyMetaData() - 1;
      }
      else {
         std::cout << "PROGRAM ERROR: new StudyMetaData is NULL." << std::endl;
      }
   }
   else {
      return;
   }
                                   
   updateStudySelectionSpinBox();
   loadStudyMetaDataIntoDialog(true);
}

/**
 * called when delete study button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotDeleteStudyPushButton()
{
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   if ((currentStudyMetaDataFileIndex >= 0) &&
       (currentStudyMetaDataFileIndex < smdf->getNumberOfStudyMetaData())) {
      if (QMessageBox::question(this, "Confirm",
                                  "Delete the current study metadata?",
                                  (QMessageBox::Yes | QMessageBox::No),
                                  QMessageBox::Yes)
                                     == QMessageBox::Yes) {
         smdf->deleteStudyMetaData(currentStudyMetaDataFileIndex);
         if (currentStudyMetaDataFileIndex >= smdf->getNumberOfStudyMetaData()) {
            currentStudyMetaDataFileIndex--;
         }
      }
   }

   updateStudySelectionSpinBox();
   loadStudyMetaDataIntoDialog(false);
}

/**
 * called when add figure button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotFigureAddPushButton()
{
   StudyWidget* sw = getCurrentStudyWidget();
   if (sw != NULL) {
      sw->addFigure(NULL);
   }
}

/**
 * called to add table button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotTableAddPushButton()
{
   StudyWidget* sw = getCurrentStudyWidget();
   if (sw != NULL) {
      sw->addTable(NULL);
   }
}

/**
 * called to add page reference button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotPageReferenceAddPushButton()
{
   StudyWidget* sw = getCurrentStudyWidget();
   if (sw != NULL) {
      sw->addPageReference(NULL);
   }
}

/**
 * called to add provenance button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotProvenanceAddPushButton()
{
   StudyWidget* sw = getCurrentStudyWidget();
   if (sw != NULL) {
      sw->addProvenance(NULL);
   }
}

/**
 * called to enable/disable selection push buttons.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotEnableDisablePushButtons()
{
   studyNewPushButton->setEnabled(true);
   
   studyFirstPushButton->setEnabled(false);
   studyLastPushButton->setEnabled(false);
   studyChooseTitlePushButton->setEnabled(false);
   studyChooseAuthorPushButton->setEnabled(false);
   studyChooseNamePushButton->setEnabled(false);
   studyDeletePushButton->setEnabled(false);
   studySelectionSpinBox->setEnabled(false);
   fetchAllStudiesPushButton->setEnabled(false);
   deleteUnlinkedStudiesPushButton->setEnabled(false);
   deleteStudiesByNamePushButton->setEnabled(false);
   
   figureAddPushButton->setEnabled(false);
   tableAddPushButton->setEnabled(false);
   pageReferenceAddPushButton->setEnabled(false);
   provenanceAddPushButton->setEnabled(false);
   studyPubMedIDFetchPushButton->setEnabled(false);
   
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   const int numStudies = smdf->getNumberOfStudyMetaData();
   if (numStudies > 0) {
      studyFirstPushButton->setEnabled(true);
      studyLastPushButton->setEnabled(true);
      studyChooseTitlePushButton->setEnabled(true);
      studyChooseAuthorPushButton->setEnabled(true);
      studyChooseNamePushButton->setEnabled(true);
      studyDeletePushButton->setEnabled(true);      
      studySelectionSpinBox->setEnabled(true);
      fetchAllStudiesPushButton->setEnabled(true);
      deleteUnlinkedStudiesPushButton->setEnabled(true);
      deleteStudiesByNamePushButton->setEnabled(true);
   }
   
   if (getCurrentStudyWidget() != NULL) {
      figureAddPushButton->setEnabled(true);
      tableAddPushButton->setEnabled(true);
      pageReferenceAddPushButton->setEnabled(true);
      provenanceAddPushButton->setEnabled(true);
      studyPubMedIDFetchPushButton->setEnabled(true);
   }
}
      
/**
 * called when delete unlinked studies button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotDeleteUnlinkedStudiesPushButton()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   BrainSet* brainSet = theMainWindow->getBrainSet();
   const int numRemoved = brainSet->removeUnlinkedStudiesFromStudyMetaDataFile();
   updateStudySelectionSpinBox();
   loadStudyMetaDataIntoDialog(false);

   QApplication::restoreOverrideCursor();   
   QMessageBox::information(this,
                            "INFO",
                            (QString::number(numRemoved)
                             + " studies were deleted."));
}
      
/**
 * called when delete studies by name button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotDeleteStudiesByNamePushButton()
{
   const QString instructions("Select studies that are to be deleted.\n"
                              "Use the CTRL (Apple on Macs) key to \n"
                              "select more than one study.");
     
   std::set<QString> studyNamesSet;
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   for (int i = 0; i < smdf->getNumberOfStudyMetaData(); i++) {
     const QString name(smdf->getStudyMetaData(i)->getName());
     if (name.isEmpty() == false) {
        studyNamesSet.insert(name);
      }
   }
   std::vector<QString> studyNames;
   studyNames.insert(studyNames.end(),
                     studyNamesSet.begin(),
                     studyNamesSet.end());
   
   QtListBoxSelectionDialog lbsd(this,
                                 "Delete Studies by Name",
                                 instructions,
                                 studyNames);
   lbsd.setAllowMultipleItemSelection(true);
   if (lbsd.exec() == QtListBoxSelectionDialog::Accepted) {
      std::vector<QString> selectedItems;
      lbsd.getSelectedItems(selectedItems);
      smdf->deleteStudiesWithNames(selectedItems);
      updateStudySelectionSpinBox();
      loadStudyMetaDataIntoDialog(false);
   }
}
      
/**
 * called when Fetch All PMID's button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotFetchAllStudiesPushButton()
{
   if (QMessageBox::question(this, "Confirm",
                               "Update all studies with valid PubMed IDs?",
                               (QMessageBox::Yes | QMessageBox::No),
                               QMessageBox::Yes)
                                  == QMessageBox::Yes) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
      StudyWidget* sw = getCurrentStudyWidget();
      try {
         smdf->updateAllStudiesWithDataFromPubMedDotCom();
         if (sw != NULL) {
            sw->loadData();
         }
      }
      catch (FileException& e) {
         if (sw != NULL) {
            sw->loadData();
         }
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "ERROR", e.whatQString());
      }
      QApplication::restoreOverrideCursor();   
   }
}
      
/**
 * create the study selection buttons.
 */
QWidget* 
GuiStudyMetaDataFileEditorDialog::createStudySelectionButtons()
{
   studySelectionSpinBox = new QSpinBox;
   QObject::connect(studySelectionSpinBox, SIGNAL(valueChanged(int)),
                    this, SLOT(slotStudySelectionSpinBoxValueChanged(int)));
   studyFirstPushButton = new QPushButton("First");
   studyFirstPushButton->setAutoDefault(false);
   studyFirstPushButton->setToolTip("Press this button to select\n"
                                    "the first Study Metadata.");
   QObject::connect(studyFirstPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFirstStudyPushButton()));
                    
   studyLastPushButton = new QPushButton("Last");
   studyLastPushButton->setAutoDefault(false);
   studyLastPushButton->setToolTip("Press this button to select\n"
                                   "the last Study Metadata.");
   QObject::connect(studyLastPushButton, SIGNAL(clicked()),
                    this, SLOT(slotLastStudyPushButton()));

   studyChooseAuthorPushButton = new QPushButton("Choose by Author...");
   studyChooseAuthorPushButton->setAutoDefault(false);
   studyChooseAuthorPushButton->setToolTip("Press this button to choose\n"
                                   "a study by its author(s).");
   QObject::connect(studyChooseAuthorPushButton, SIGNAL(clicked()),
                    this, SLOT(slotStudyChooseAuthorPushButton()));
                    
   studyChooseNamePushButton = new QPushButton("Choose by Name...");
   studyChooseNamePushButton->setAutoDefault(false);
   studyChooseNamePushButton->setToolTip("Press this button to choose\n"
                                   "a study by its name.");
   QObject::connect(studyChooseNamePushButton, SIGNAL(clicked()),
                    this, SLOT(slotStudyChooseNamePushButton()));
                    
   studyChooseTitlePushButton = new QPushButton("Choose by Title...");
   studyChooseTitlePushButton->setAutoDefault(false);
   studyChooseTitlePushButton->setToolTip("Press this button to choose\n"
                                   "a study by its name.");
   QObject::connect(studyChooseTitlePushButton, SIGNAL(clicked()),
                    this, SLOT(slotStudyChooseTitlePushButton()));
                    
   QGroupBox* ssg = new QGroupBox("Study Selection");
   QVBoxLayout* ssl = new QVBoxLayout(ssg);
   ssl->addWidget(studySelectionSpinBox);
   ssl->addWidget(studyFirstPushButton);
   ssl->addWidget(studyLastPushButton);
   ssl->addWidget(studyChooseAuthorPushButton);
   ssl->addWidget(studyChooseNamePushButton);
   ssl->addWidget(studyChooseTitlePushButton);
   
   fetchAllStudiesPushButton = new QPushButton("Fetch All PMIDs");
   fetchAllStudiesPushButton->setAutoDefault(false);
   fetchAllStudiesPushButton->setToolTip("Update all studies with valid\n"
                                         "PubMed IDs with data from \n"
                                         "www.pubmed.gov.");
   QObject::connect(fetchAllStudiesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFetchAllStudiesPushButton()));
                
   deleteUnlinkedStudiesPushButton = new QPushButton("Delete Unlinked Studies");
   deleteUnlinkedStudiesPushButton->setAutoDefault(false);
   deleteUnlinkedStudiesPushButton->setToolTip("Delete studies not linked\n"
                                               "by any data type.");
   QObject::connect(deleteUnlinkedStudiesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteUnlinkedStudiesPushButton()));
                    
   deleteStudiesByNamePushButton = new QPushButton("Delete By Name...");
   deleteStudiesByNamePushButton->setAutoDefault(false);
   deleteStudiesByNamePushButton->setToolTip("Delete Studies By Name");
   QObject::connect(deleteStudiesByNamePushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteStudiesByNamePushButton()));
                    
   studyNewPushButton = new QPushButton("Add New Study...");
   studyNewPushButton->setAutoDefault(false);
   studyNewPushButton->setToolTip("Press this button to create\n"
                                  "new Study Metadata.");
   QObject::connect(studyNewPushButton, SIGNAL(clicked()),
                    this, SLOT(slotNewStudyPushButton()));
                    
   QGroupBox* studyFileGroupBox = new QGroupBox("File Operations");
   QVBoxLayout* studyFileLayout = new QVBoxLayout(studyFileGroupBox);
   studyFileLayout->addWidget(studyNewPushButton);
   studyFileLayout->addWidget(deleteUnlinkedStudiesPushButton);
   studyFileLayout->addWidget(deleteStudiesByNamePushButton);
   studyFileLayout->addWidget(fetchAllStudiesPushButton);
   
   studyDeletePushButton = new QPushButton("Delete Study...");
   studyDeletePushButton->setAutoDefault(false);
   studyDeletePushButton->setToolTip("Press this button to delete\n"
                                     "the current Study Metadata.");
   QObject::connect(studyDeletePushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteStudyPushButton()));

   figureAddPushButton = new QPushButton("Add Figure");
   figureAddPushButton->setAutoDefault(false);
   figureAddPushButton->setToolTip("Press this button to add a\n"
                                   "figure to the current study.");
   QObject::connect(figureAddPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFigureAddPushButton()));
       
   pageReferenceAddPushButton = new QPushButton("Add Page Ref");
   pageReferenceAddPushButton->setAutoDefault(false);
   pageReferenceAddPushButton->setToolTip("Press this button to add a page\n"
                                          "reference to the current study.");
   QObject::connect(pageReferenceAddPushButton, SIGNAL(clicked()), 
                    this, SLOT(slotPageReferenceAddPushButton()));
   
   provenanceAddPushButton = new QPushButton("Add Provenance");
   provenanceAddPushButton->setAutoDefault(false);
   provenanceAddPushButton->setToolTip("Press this button to add a provenance\n"
                                       "to the current study.");
   QObject::connect(provenanceAddPushButton, SIGNAL(clicked()),
                    this, SLOT(slotProvenanceAddPushButton()));
                    
   tableAddPushButton = new QPushButton("Add Table");
   tableAddPushButton->setAutoDefault(false);
   tableAddPushButton->setToolTip("Press this button to add a\n"
                                  "table to the current study.");
   QObject::connect(tableAddPushButton, SIGNAL(clicked()),
                    this, SLOT(slotTableAddPushButton()));
           
   studyPubMedIDFetchPushButton = new QPushButton("Fetch from PubMed...");
   studyPubMedIDFetchPushButton->setAutoDefault(false);
   studyPubMedIDFetchPushButton->setToolTip("After entering the PubMed ID,\n"
                                       "press this button to retrieve\n"
                                       "the article's information from\n"
                                       "the PubMed Website.");
   QObject::connect(studyPubMedIDFetchPushButton, SIGNAL(clicked()),
                    this, SLOT(slotStudyFetchPubMedIDPushButton()));

   QGroupBox* sog = new QGroupBox("Study Operations");
   QVBoxLayout* sol = new QVBoxLayout(sog);
   sol->addWidget(figureAddPushButton);
   sol->addWidget(pageReferenceAddPushButton);
   sol->addWidget(provenanceAddPushButton);
   sol->addWidget(tableAddPushButton);
   sol->addWidget(studyDeletePushButton);
   sol->addWidget(studyPubMedIDFetchPushButton);
   
   std::vector<QPushButton*> buttons;
   buttons.push_back(studyFirstPushButton);
   buttons.push_back(studyLastPushButton);
   buttons.push_back(studyChooseAuthorPushButton);
   buttons.push_back(studyChooseNamePushButton);
   buttons.push_back(studyChooseTitlePushButton);
   buttons.push_back(studyNewPushButton);
   buttons.push_back(studyDeletePushButton);
   buttons.push_back(figureAddPushButton);
   buttons.push_back(pageReferenceAddPushButton);
   buttons.push_back(provenanceAddPushButton);
   buttons.push_back(tableAddPushButton);
   buttons.push_back(fetchAllStudiesPushButton);
   buttons.push_back(deleteUnlinkedStudiesPushButton);
   buttons.push_back(deleteStudiesByNamePushButton);
   buttons.push_back(studyPubMedIDFetchPushButton);
   QtUtilities::makeButtonsSameSize(buttons);
        
   QWidget* w = new QWidget;
   QVBoxLayout* l = new QVBoxLayout(w);
   l->addWidget(ssg);
   l->addWidget(studyFileGroupBox);
   l->addWidget(sog);
   return w;                    
}      

/**
 * called when pubmed ID button pressed.
 */
void 
GuiStudyMetaDataFileEditorDialog::slotStudyFetchPubMedIDPushButton()
{
   StudyWidget* sw = getCurrentStudyWidget();
   if (sw == NULL) {
      return;
   }
   
   StudyMetaData* studyMetaData = sw->getCurrentStudyMetaData();
   if (studyMetaData == NULL) {
      return;
   }
   
   //
   // Confirm with user
   //
   const QString msg("Retrieve data for article with PubMed ID = " 
                     + studyMetaData->getPubMedID()
                     +"?");
   if (QMessageBox::question(this, 
                             "Confirm", 
                             msg,
                             (QMessageBox::Ok | QMessageBox::Cancel),
                             QMessageBox::Ok)
                                == QMessageBox::Cancel) {
      return;
   }

   //
   // Update the study metadata
   //
   try {
      if (studyMetaData != NULL) {
         studyMetaData->updateDataFromPubMedDotComUsingPubMedID();
         sw->loadData();
      }
   }
   catch (FileException& e) {
      QMessageBox::critical(this, "ERROR", e.whatQString());
   }
/*
   //
   // Make sure a PubMed ID was entered
   //
   const QString pubMedID = studyPubMedIDLineEdit->text().trimmed();
   if (pubMedID.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "The PubMed ID has not been entered.", "OK");
      return;
   }
   
   //
   // Confirm with user
   //
   const QString msg("Retrieve data for article with PubMed ID = " 
                     + pubMedID
                     +"?");
   if (QMessageBox::question(this, "Confirm", msg, "Continue", "Cancel") != 0) {
      return;
   }
   
   //
   // Get the article
   //
   PubMedArticleFile pubMedFile;
   try {
      pubMedFile.retrieveArticleWithPubMedID(pubMedID);

      if (pubMedFile.getArticleTitle().isEmpty() == false) {
         studyTitleLineEdit->setText(pubMedFile.getArticleTitle());
         studyTitleLineEdit->home(false);
         slotStudyTitleLineEditChanged();
      }
      if (pubMedFile.getAuthors().isEmpty() == false) {
         studyAuthorsLineEdit->setText(pubMedFile.getAuthors());
         studyAuthorsLineEdit->home(false);
         slotStudyAuthorsLineEditChanged();
      }
      if (pubMedFile.getJournalTitle().isEmpty() == false) {
         studyCitationLineEdit->setText(pubMedFile.getJournalTitle());
         studyCitationLineEdit->home(false);
         slotStudyCitationLineEditChanged();
      }
      if (pubMedFile.getDocumentObjectIdentifier().isEmpty() == false) {
         studyDocumentObjectIdentifierLineEdit->setText(pubMedFile.getDocumentObjectIdentifier());
         studyDocumentObjectIdentifierLineEdit->home(false);
         slotStudyDocumentObjectIdentifierLineEditChanged();
      }
      if (pubMedFile.getAbstractText().isEmpty() == false) {
         studyCommentTextEdit->setText(pubMedFile.getAbstractText());
         slotStudyCommentTextEditChanged();
      }
      if (pubMedFile.getMedicalSubjectHeadings().isEmpty() == false) {
         studyMeshLineEdit->setText(pubMedFile.getMedicalSubjectHeadings());
         slotStudyMeshLineEditChanged();
      }
   }
   catch (FileException& e) {
      QMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
      return;
   }
*/
}      

//=====================================================================================
//
// Widget for displaying a study
//
//=====================================================================================
/**
 * constructor.
 */
StudyWidget::StudyWidget(GuiStudyMetaDataFileEditorDialog* parentStudyMetaDataFileEditorDialogIn,
                         StudyMetaData* studyMetaDataIn,
                         QWidget* parentIn)
   : QGroupBox("Study Metadata", parentIn)
{
   parentStudyMetaDataFileEditorDialog = parentStudyMetaDataFileEditorDialogIn;
   studyMetaData = studyMetaDataIn;

   //
   // Validator for PubMed ID which is an optional minus sign followed by digits
   // Also allow it to be a project ID which begins with "ProjID"
   //
   QRegExpValidator* pubMedIDValidator = new QRegExpValidator(
                        QRegExp("[P|0-9][r|0-9][o|0-9][j|0-9][I|0-9][D|0-9]\\d*"),
                        //QRegExp("ProjID\\d+"),  //("\\d+")
                                                              this);
   
   QLabel* studyFileIndexLabel = new QLabel("File Index");
   studyFileIndexDetailsLabel = new QLabel("");
   
   QLabel* studyProjectIDLabel = new QLabel("Project ID");
   studyProjectIDLineEdit = new QLineEdit;
   studyProjectIDLineEdit->setReadOnly(true);
   studyProjectIDLineEdit->setToolTip("A unique number for identifying this study.\n"
                                "It is used to identify the study when a\n"
                                "PubMed ID is not available.");
   QObject::connect(studyProjectIDLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyProjectIDLineEditChanged()));
               
   QLabel* studyNameLabel = new QLabel("Name");
   studyNameLineEdit = new QLineEdit;
   studyNameLineEdit->setToolTip("User-define name assigned to study.\n"
                                 "Sometimes this name is used for \n"
                                 "naming foci.");
   QObject::connect(studyNameLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotStudyNameLineEditChanged()));
                    
   QLabel* studyTitleLabel = new QLabel("Title");
   studyTitleLineEdit = new QLineEdit;
   studyTitleLineEdit->setToolTip("Title of the article.");
   QObject::connect(studyTitleLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyTitleLineEditChanged()));
    
   QLabel* studyAuthorsLabel = new QLabel("Authors");
   studyAuthorsLineEdit = new QLineEdit;
   studyAuthorsLineEdit->setToolTip("Author(s) of the article.");
   QObject::connect(studyAuthorsLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyAuthorsLineEditChanged()));
    
   QLabel* studyCitationLabel = new QLabel("Citation");
   studyCitationLineEdit = new QLineEdit;
   studyCitationLineEdit->setToolTip("Name of journal.");
   QObject::connect(studyCitationLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyCitationLineEditChanged()));
    
   QLabel* studyKeywordsLabel = new QLabel("Keywords");
   studyKeywordsLineEdit = new QLineEdit;
   studyKeywordsLineEdit->setToolTip("Article keywords.");
   QObject::connect(studyKeywordsLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyKeywordsLineEditChanged()));
   QObject::connect(studyKeywordsLineEdit, SIGNAL(editingFinished()),
                   this, SLOT(slotStudyKeywordsLineEditFinished()));
   keywordsModifiedFlag = false;
    
   QLabel* studyMeshLabel = new QLabel("Medical Subject Headings");
   studyMeshLineEdit = new QLineEdit;
   studyMeshLineEdit->setToolTip("Medical Subject Headings");
   QObject::connect(studyMeshLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotStudyMeshLineEditChanged()));
   
   QPushButton* studySpeciesPushButton = new QPushButton("Species...");
   studySpeciesPushButton->setAutoDefault(false);
   QObject::connect(studySpeciesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotStudySpeciesPushButton()));
   studySpeciesLineEdit = new QLineEdit;
   studySpeciesLineEdit->setToolTip("Species used in study.");
   QObject::connect(studySpeciesLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotStudySpeciesLineEditChanged()));
                    
   QPushButton* studyStereotaxicSpacePushButton = new QPushButton("Stereotaxic Space...");
   studyStereotaxicSpacePushButton->setAutoDefault(false);
   QObject::connect(studyStereotaxicSpacePushButton, SIGNAL(clicked()),
                    this, SLOT(slotStudyStereotaxicSpacePushButton()));
   studyStereotaxicSpaceLineEdit = new QLineEdit;
   studyStereotaxicSpaceLineEdit->setToolTip("Short name of stereotaxic space.");
   QObject::connect(studyStereotaxicSpaceLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyStereotaxicSpaceLineEditChanged()));
    
   QLabel* studyStereotaxicSpaceDetailsLabel = new QLabel("Stereo Space Details");
   studyStereotaxicSpaceDetailsLineEdit = new QLineEdit;
   studyStereotaxicSpaceDetailsLineEdit->setToolTip("Details about the stereotaxic space.");
   QObject::connect(studyStereotaxicSpaceDetailsLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyStereotaxicSpaceDetailsLineEditChanged()));
    
   QLabel* studyDataFormatLabel = new QLabel("Data Format");
   QPushButton* studyDataFormatAddPushButton = new QPushButton("Add");
   studyDataFormatAddPushButton->setAutoDefault(false);
   QObject::connect(studyDataFormatAddPushButton, SIGNAL(clicked()),
                    this, SLOT(slotStudyDataFormatAddPushButton()));
   QHBoxLayout* studyDataFormatLabelAndButtonLayout = new QHBoxLayout;
   studyDataFormatLabelAndButtonLayout->addWidget(studyDataFormatLabel);
   studyDataFormatLabelAndButtonLayout->addWidget(studyDataFormatAddPushButton);
   studyDataFormatLineEdit = new QLineEdit;
   studyDataFormatLineEdit->setToolTip("Format of study data (Metric, Paint, etc.)");
   QObject::connect(studyDataFormatLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotStudyDataFormatLineEditChanged()));
                    
   QLabel* studyDataTypeLabel = new QLabel("Data Type");
   QPushButton* studyDataTypeAddPushButton = new QPushButton("Add");
   studyDataTypeAddPushButton->setAutoDefault(false);
   QObject::connect(studyDataTypeAddPushButton, SIGNAL(clicked()),
                    this, SLOT(slotStudyDataTypeAddPushButton()));
   QHBoxLayout* studyDataTypeLabelAndButtonLayout = new QHBoxLayout;
   studyDataTypeLabelAndButtonLayout->addWidget(studyDataTypeLabel);
   studyDataTypeLabelAndButtonLayout->addWidget(studyDataTypeAddPushButton);
   studyDataTypeLineEdit = new QLineEdit;
   studyDataTypeLineEdit->setToolTip("Type of study data (fMRI, PET, etc.)");
   QObject::connect(studyDataTypeLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotStudyDataTypeLineEditChanged()));
                    
   QLabel* studyPubMedIDLabel = new QLabel("PubMed ID");
   QPushButton* studyPubMedIDPushButton = new QPushButton("Link");
   studyPubMedIDPushButton->setAutoDefault(false);
   studyPubMedIDPushButton->setToolTip("Press this button to \n"
                                       "view the web-page for\n"
                                       "this PubMed ID.");
   QObject::connect(studyPubMedIDPushButton, SIGNAL(clicked()),
                    this, SLOT(slotStudyPubMedIDPushButton()));
   studyPubMedIDLineEdit = new QLineEdit;
   studyPubMedIDLineEdit->setValidator(pubMedIDValidator);
   studyPubMedIDLineEdit->setToolTip("The PubMed ID of the article (www.pubmed.org)\n"
                                "If there is not a PubMed ID for this study, this\n"
                                "value should be the same as the Project ID.");
   QObject::connect(studyPubMedIDLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyPubMedIDSpinBoxChanged()));
   QHBoxLayout* studyPubMedIdLabelAndFetchButtonLayout = new QHBoxLayout;
   studyPubMedIdLabelAndFetchButtonLayout->addWidget(studyPubMedIDLabel);
   studyPubMedIdLabelAndFetchButtonLayout->addWidget(studyPubMedIDPushButton);
   
   QLabel* studyDocumentObjectIdentifierLabel = new QLabel("DOI-URL");
   QPushButton* studyDocumentObjectIdentifierPushButton = new QPushButton("Link");
   studyDocumentObjectIdentifierPushButton->setAutoDefault(false);
   studyDocumentObjectIdentifierPushButton->setToolTip("Press this button to \n"
                                                       "view the web-page for\n"
                                                       "this DOI or URL.");
   QObject::connect(studyDocumentObjectIdentifierPushButton, SIGNAL(clicked()),
                    this, SLOT(slotStudyDocumentObjectIdentifierPushButton()));
   QHBoxLayout* doiURLLayout = new QHBoxLayout;
   doiURLLayout->addWidget(studyDocumentObjectIdentifierLabel);
   doiURLLayout->addWidget(studyDocumentObjectIdentifierPushButton);
   studyDocumentObjectIdentifierLineEdit = new QLineEdit;
   studyDocumentObjectIdentifierLineEdit->setToolTip("DOI(Document Object Identifier, dx.ori.org) or URL of article.\n"
                                                     "Use the DOI if it is available, otherwise use the URL.");
   QObject::connect(studyDocumentObjectIdentifierLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyDocumentObjectIdentifierLineEditChanged()));
    
   QLabel* studyCommentLabel = new QLabel("Comment");
   studyCommentTextEdit = new QTextEdit;
   studyCommentTextEdit->setReadOnly(false);
   studyCommentTextEdit->setToolTip("Comments about the article.");
   studyCommentTextEdit->setMinimumHeight(minimumTextEditHeight);
   QObject::connect(studyCommentTextEdit, SIGNAL(textChanged()),
                   this, SLOT(slotStudyCommentTextEditChanged()));
   
   QLabel* studyPartitioningSchemeAbbreviationLabel = new QLabel("Part Scheme Abbrev");
   studyPartitioningSchemeAbbreviationLineEdit = new QLineEdit;
   studyPartitioningSchemeAbbreviationLineEdit->setToolTip("Abbreviated name of partitioning scheme.");
   QObject::connect(studyPartitioningSchemeAbbreviationLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyPartitioningSchemeAbbreviationLineEditChanged()));
   
   QLabel* studyPartitioningSchemeFullNameLabel = new QLabel("Part Scheme Name");
   studyPartitioningSchemeFullNameLineEdit = new QLineEdit;
   studyPartitioningSchemeFullNameLineEdit->setToolTip("Full name of partitioning scheme.");
   QObject::connect(studyPartitioningSchemeFullNameLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyPartitioningSchemeFullNameLineEditChanged()));
   
   QLabel* studyQualityLabel = new QLabel("Quality");
   studyQualityLineEdit = new QLineEdit;
   studyQualityLineEdit->setToolTip("Trustworthiness of data.");
   QObject::connect(studyQualityLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotStudyQualityLineEditChanged()));
   
   QLabel* studyLastSaveLabel = new QLabel("Last Saved");
   studyLastSaveLineEdit = new QLineEdit;
   studyLastSaveLineEdit->setToolTip("Contains last date and time this\n"
                                     "study was last saved.");
   studyLastSaveLineEdit->setReadOnly(true);
   
   int rowCounter = 0;
   const int studyIndexRow = rowCounter++;
   const int studyProjectIDRow = rowCounter++;
   const int studyPubMedIDRow = rowCounter++;
   const int studyNameRow = rowCounter++;
   const int studyTitleRow = rowCounter++;
   const int studyAuthorsRow = rowCounter++;
   const int studyCitationRow = rowCounter++;
   const int studyDocumentObjectIdentifierRow = rowCounter++;
   const int studySpeciesRow = rowCounter++;
   const int studyStereotaxicSpaceRow = rowCounter++;
   const int studyStereotaxicSpaceDetailsRow = rowCounter++;
   const int studyDataFormatRow = rowCounter++;
   const int studyDataTypeRow = rowCounter++;
   const int studyKeywordsRow = rowCounter++;
   const int studyMeshRow = rowCounter++;
   const int studyPartitioningSchemeAbbreviationRow = rowCounter++;
   const int studyPartitioningSchemeFullNameRow = rowCounter++;
   const int studyQualityRow = rowCounter++;
   const int studyCommentRow = rowCounter++;
   const int studyLastSaveRow = rowCounter++;
   
   QGridLayout* grid = new QGridLayout;
   grid->addWidget(studyFileIndexLabel, studyIndexRow, 0);
   grid->addWidget(studyFileIndexDetailsLabel, studyIndexRow, 1);
   grid->addWidget(studyProjectIDLabel, studyProjectIDRow, 0);
   grid->addWidget(studyProjectIDLineEdit, studyProjectIDRow, 1);
   grid->addLayout(studyPubMedIdLabelAndFetchButtonLayout, studyPubMedIDRow, 0);
   grid->addWidget(studyPubMedIDLineEdit, studyPubMedIDRow, 1);
   grid->addWidget(studyNameLabel, studyNameRow, 0);
   grid->addWidget(studyNameLineEdit, studyNameRow, 1);
   grid->addWidget(studyTitleLabel, studyTitleRow, 0);
   grid->addWidget(studyTitleLineEdit, studyTitleRow, 1);
   grid->addWidget(studyAuthorsLabel, studyAuthorsRow, 0);
   grid->addWidget(studyAuthorsLineEdit, studyAuthorsRow, 1);
   grid->addWidget(studyCitationLabel, studyCitationRow, 0);
   grid->addWidget(studyCitationLineEdit, studyCitationRow, 1);
   grid->addLayout(doiURLLayout, studyDocumentObjectIdentifierRow, 0);
   grid->addWidget(studyDocumentObjectIdentifierLineEdit, studyDocumentObjectIdentifierRow, 1);
   grid->addWidget(studySpeciesPushButton, studySpeciesRow, 0);
   grid->addWidget(studySpeciesLineEdit, studySpeciesRow, 1);
   grid->addWidget(studyStereotaxicSpacePushButton, studyStereotaxicSpaceRow, 0);
   grid->addWidget(studyStereotaxicSpaceLineEdit, studyStereotaxicSpaceRow, 1);
   grid->addWidget(studyStereotaxicSpaceDetailsLabel, studyStereotaxicSpaceDetailsRow, 0);
   grid->addWidget(studyStereotaxicSpaceDetailsLineEdit, studyStereotaxicSpaceDetailsRow, 1);
   grid->addLayout(studyDataFormatLabelAndButtonLayout, studyDataFormatRow, 0);
   grid->addWidget(studyDataFormatLineEdit, studyDataFormatRow, 1);
   grid->addLayout(studyDataTypeLabelAndButtonLayout, studyDataTypeRow, 0);
   grid->addWidget(studyDataTypeLineEdit, studyDataTypeRow, 1);
   grid->addWidget(studyKeywordsLabel, studyKeywordsRow, 0);
   grid->addWidget(studyKeywordsLineEdit, studyKeywordsRow, 1);
   grid->addWidget(studyMeshLabel, studyMeshRow, 0);
   grid->addWidget(studyMeshLineEdit, studyMeshRow, 1);
   grid->addWidget(studyPartitioningSchemeAbbreviationLabel, studyPartitioningSchemeAbbreviationRow, 0);
   grid->addWidget(studyPartitioningSchemeAbbreviationLineEdit, studyPartitioningSchemeAbbreviationRow, 1);
   grid->addWidget(studyPartitioningSchemeFullNameLabel, studyPartitioningSchemeFullNameRow, 0);
   grid->addWidget(studyPartitioningSchemeFullNameLineEdit, studyPartitioningSchemeFullNameRow, 1);
   grid->addWidget(studyQualityLabel, studyQualityRow, 0);
   grid->addWidget(studyQualityLineEdit, studyQualityRow, 1);
   grid->addWidget(studyCommentLabel, studyCommentRow, 0);
   grid->addWidget(studyCommentTextEdit, studyCommentRow, 1);
   grid->addWidget(studyLastSaveLabel, studyLastSaveRow, 0);
   grid->addWidget(studyLastSaveLineEdit, studyLastSaveRow, 1);


   QLabel* studyMslIDLabel = new QLabel("MSL ID");
   studyMslIDLineEdit = new QLineEdit;
   studyMslIDLineEdit->setReadOnly(true);
   QObject::connect(studyMslIDLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyMslIDLineEditChanged()));
    
   QLabel* studyParentIDLabel = new QLabel("Parent ID");
   studyParentIDLineEdit = new QLineEdit;
   studyParentIDLineEdit->setReadOnly(true);
   QObject::connect(studyParentIDLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyParentIDLineEditChanged()));
    
   QLabel* studyCoreDataCompletedLabel = new QLabel("Core Data Completed");
   studyCoreDataCompletedLineEdit = new QLineEdit;
   studyCoreDataCompletedLineEdit->setReadOnly(true);
   QObject::connect(studyCoreDataCompletedLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyCoreDataCompletedLineEditChanged()));
    
   QLabel* studyCompletedLabel = new QLabel("Completed");
   studyCompletedLineEdit = new QLineEdit;
   studyCompletedLineEdit->setReadOnly(true);
   QObject::connect(studyCompletedLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyCompletedLineEditChanged()));
    
   QLabel* studyPublicAccessLabel = new QLabel("Public Access");
   studyPublicAccessLineEdit = new QLineEdit;
   studyPublicAccessLineEdit->setReadOnly(true);
   QObject::connect(studyPublicAccessLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(slotStudyPublicAccessLineEditChanged()));
    
   int rowCounter2 = 0;
   const int studyMslIDRow = rowCounter2++;
   const int studyParentIDRow = rowCounter2++;
   const int studyCoreDataCompletedRow = rowCounter2++;
   const int studyCompletedRow = rowCounter2++;
   const int studyPublicAccessRow = rowCounter2++;

   QGridLayout* grid2 = new QGridLayout;
   grid2->addWidget(studyMslIDLabel, studyMslIDRow, 0);
   grid2->addWidget(studyMslIDLineEdit, studyMslIDRow, 1);
   grid2->addWidget(studyParentIDLabel, studyParentIDRow, 0);
   grid2->addWidget(studyParentIDLineEdit, studyParentIDRow, 1);
   grid2->addWidget(studyCoreDataCompletedLabel, studyCoreDataCompletedRow, 0);
   grid2->addWidget(studyCoreDataCompletedLineEdit, studyCoreDataCompletedRow, 1);
   grid2->addWidget(studyCompletedLabel, studyCompletedRow, 0);
   grid2->addWidget(studyCompletedLineEdit, studyCompletedRow, 1);
   grid2->addWidget(studyPublicAccessLabel, studyPublicAccessRow, 0);
   grid2->addWidget(studyPublicAccessLineEdit, studyPublicAccessRow, 1);
   
   //
   // layouts for tables and figures
   //
   figuresLayout = new QVBoxLayout;
   pageReferenceLayout = new QVBoxLayout;
   provenanceLayout = new QVBoxLayout;
   tablesLayout  = new QVBoxLayout;
   
   //
   // Layout for study information
   //
   layout = new QVBoxLayout(this);
   layout->addLayout(grid);
   layout->addLayout(figuresLayout);
   layout->addLayout(pageReferenceLayout);
   layout->addLayout(tablesLayout);
   layout->addLayout(grid2);
   layout->addLayout(provenanceLayout);
   
   //
   // Place the study data widgets into a widgets group for blocking signals
   //
   allWidgetsGroup = new WuQWidgetGroup(this);
   allWidgetsGroup->addWidget(studyFileIndexDetailsLabel);
   allWidgetsGroup->addWidget(studyProjectIDLineEdit);
   allWidgetsGroup->addWidget(studyNameLineEdit);
   allWidgetsGroup->addWidget(studyTitleLineEdit);
   allWidgetsGroup->addWidget(studyAuthorsLineEdit);
   allWidgetsGroup->addWidget(studyCitationLineEdit);
   allWidgetsGroup->addWidget(studyKeywordsLineEdit);
   allWidgetsGroup->addWidget(studyMeshLineEdit);
   allWidgetsGroup->addWidget(studySpeciesLineEdit);
   allWidgetsGroup->addWidget(studyStereotaxicSpaceLineEdit);
   allWidgetsGroup->addWidget(studyStereotaxicSpaceDetailsLineEdit);
   allWidgetsGroup->addWidget(studyDataFormatLineEdit);
   allWidgetsGroup->addWidget(studyDataTypeLineEdit);
   allWidgetsGroup->addWidget(studyPubMedIDLineEdit);
   allWidgetsGroup->addWidget(studyDocumentObjectIdentifierLineEdit);
   allWidgetsGroup->addWidget(studyCommentTextEdit);
   allWidgetsGroup->addWidget(studyPartitioningSchemeAbbreviationLineEdit);
   allWidgetsGroup->addWidget(studyPartitioningSchemeFullNameLineEdit);
   allWidgetsGroup->addWidget(studyQualityLineEdit);
   allWidgetsGroup->addWidget(studyLastSaveLineEdit);
   allWidgetsGroup->addWidget(studyMslIDLineEdit);
   allWidgetsGroup->addWidget(studyParentIDLineEdit);
   allWidgetsGroup->addWidget(studyCoreDataCompletedLineEdit);
   allWidgetsGroup->addWidget(studyCompletedLineEdit);
   allWidgetsGroup->addWidget(studyPublicAccessLineEdit);

   loadData();
}

/**
 * destructor.
 */
StudyWidget::~StudyWidget()
{
}

/**
 * called when PubMed ID "LINK" button is pressed.
 */
void 
StudyWidget::slotStudyPubMedIDPushButton()
{
   QString theURL = studyPubMedIDLineEdit->text().trimmed(); 
   if (theURL.startsWith(StudyMetaData::getProjectIDInPubMedIDPrefix())) {
      const QString msg("The contents of the PubMed ID edit box\n"
                        "must be a PubMed ID, not a Project ID");
      QMessageBox::critical(this, "ERROR", msg);
      return;
   }
   if (theURL.startsWith("http:")) {
      theURL = theURL;  
   }
   else {
      theURL = "http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=retrieve&db=pubmed&list_uids=" + theURL;
   }
   
   theMainWindow->displayWebPage(theURL);
}
      
/**
 * called when study data format ADD button is pressed.
 */
void 
StudyWidget::slotStudyDataFormatAddPushButton()
{
   std::vector<QString> dataFormats;
   StudyMetaData::getStudyDataFormatEntries(dataFormats);
   QtListBoxSelectionDialog lb(this,
                               "Data Formats",
                               "",
                               dataFormats);
   lb.setAllowMultipleItemSelection(true);
   if (lb.exec() == QtListBoxSelectionDialog::Accepted) {
      std::vector<QString> items;
      lb.getSelectedItems(items);
      
      QString s = studyDataFormatLineEdit->text().trimmed();
      for (unsigned int i = 0; i < items.size(); i++) {
         if (s.isEmpty() == false) {
            s += "; ";
         }
         s += items[i];
      }
      studyDataFormatLineEdit->setText(s);
      slotStudyDataFormatLineEditChanged();
   }
}

/**
 * called when study data type ADD button is pressed.
 */
void 
StudyWidget::slotStudyDataTypeAddPushButton()
{
   std::vector<QString> dataTypes;
   StudyMetaData::getStudyDataTypeEntries(dataTypes);
   QtListBoxSelectionDialog lb(this,
                               "Data Types",
                               "",
                               dataTypes);
   lb.setAllowMultipleItemSelection(true);
   if (lb.exec() == QtListBoxSelectionDialog::Accepted) {
      std::vector<QString> items;
      lb.getSelectedItems(items);
      
      QString s = studyDataTypeLineEdit->text().trimmed();
      for (unsigned int i = 0; i < items.size(); i++) {
         if (s.isEmpty() == false) {
            s += "; ";
         }
         s += items[i];
      }
      studyDataTypeLineEdit->setText(s);
      slotStudyDataTypeLineEditChanged();
   }
}

/**
 * called when DOI/URL "LINK" button is pressed.
 */
void 
StudyWidget::slotStudyDocumentObjectIdentifierPushButton()
{
   QString theURL = studyDocumentObjectIdentifierLineEdit->text().trimmed(); 
   if (theURL.startsWith("http:")) {
      theURL = theURL;  
   }
   else {
      theURL = "http://dx.doi.org/" + theURL;
   }
   
   theMainWindow->displayWebPage(theURL);
}

/**
 * load data into the widget.
 */
void 
StudyWidget::loadData()
{
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   const QString indexString(QString::number(smdf->getStudyMetaDataIndex(studyMetaData) + 1) 
                             + " of "
                             + QString::number(smdf->getNumberOfStudyMetaData()));

   allWidgetsGroup->blockSignals(true);
   
   studyFileIndexDetailsLabel->setText(indexString);
   studyProjectIDLineEdit->setText(studyMetaData->getProjectID());
   studyNameLineEdit->setText(studyMetaData->getName());
   studyNameLineEdit->home(false);
   studyTitleLineEdit->setText(studyMetaData->getTitle());
   studyTitleLineEdit->home(false);
   studyAuthorsLineEdit->setText(studyMetaData->getAuthors());
   studyAuthorsLineEdit->home(false);
   studyCitationLineEdit->setText(studyMetaData->getCitation());
   studyCitationLineEdit->home(false);
   studyPubMedIDLineEdit->setText(studyMetaData->getPubMedID());
   studyDocumentObjectIdentifierLineEdit->setText(studyMetaData->getDocumentObjectIdentifier());
   studyDocumentObjectIdentifierLineEdit->home(false);
   studySpeciesLineEdit->setText(studyMetaData->getSpecies());
   studySpeciesLineEdit->home(false);
   studyStereotaxicSpaceLineEdit->setText(studyMetaData->getStereotaxicSpace());
   studyStereotaxicSpaceLineEdit->home(false);
   studyStereotaxicSpaceDetailsLineEdit->setText(studyMetaData->getStereotaxicSpaceDetails());
   studyStereotaxicSpaceDetailsLineEdit->home(false);
   studyDataFormatLineEdit->setText(studyMetaData->getStudyDataFormat());
   studyDataFormatLineEdit->home(false);
   studyDataTypeLineEdit->setText(studyMetaData->getStudyDataType());
   studyDataTypeLineEdit->home(false);
   studyKeywordsLineEdit->setText(studyMetaData->getKeywords());
   studyKeywordsLineEdit->home(false);
   keywordsModifiedFlag = false;
   studyMeshLineEdit->setText(studyMetaData->getMedicalSubjectHeadings());
   studyMeshLineEdit->home(false);
   studyPartitioningSchemeAbbreviationLineEdit->setText(studyMetaData->getPartitioningSchemeAbbreviation());
   studyPartitioningSchemeAbbreviationLineEdit->home(false);
   studyPartitioningSchemeFullNameLineEdit->setText(studyMetaData->getPartitioningSchemeFullName());
   studyPartitioningSchemeFullNameLineEdit->home(false);
   studyQualityLineEdit->setText(studyMetaData->getQuality());
   studyQualityLineEdit->home(false);
   studyCommentTextEdit->setPlainText(studyMetaData->getComment());
   studyLastSaveLineEdit->setText(studyMetaData->getMostRecentDateAndTimeStamp());
   
   studyMslIDLineEdit->setText(studyMetaData->getMslID());
   studyParentIDLineEdit->setText(studyMetaData->getParentID());
   studyCoreDataCompletedLineEdit->setText(studyMetaData->getCoreDataCompleted());
   studyCompletedLineEdit->setText(studyMetaData->getCompleted());
   studyPublicAccessLineEdit->setText(studyMetaData->getPublicAccess());

   //QTextCursor tc;
   //tc.movePosition(QTextCursor::Start);
   //studyCommentTextEdit->setTextCursor(tc);

   allWidgetsGroup->blockSignals(false);
   
   const int numFigures = studyMetaData->getNumberOfFigures();
   for (int i = 0; i < numFigures; i++) {
      addFigure(studyMetaData->getFigure(i));
   }
   
   const int numPageRef = studyMetaData->getNumberOfPageReferences();
   for (int i = 0; i < numPageRef; i++) {
      addPageReference(studyMetaData->getPageReference(i));
   }
   
   const int numProvenance = studyMetaData->getNumberOfProvenances();
   for (int i = 0; i < numProvenance; i++) {
      addProvenance(studyMetaData->getProvenance(i));
   }
   const int numTables = studyMetaData->getNumberOfTables();
   for (int i = 0; i < numTables; i++) {
      addTable(studyMetaData->getTable(i));
   }
}

/**
 * add a figure to the study (if NULL create new figure).
 */
void 
StudyWidget::addFigure(StudyMetaData::Figure* figure)
{
   if (figure == NULL) {
      figure = new StudyMetaData::Figure;
      studyMetaData->addFigure(figure);
   }
   StudyFigureWidget* figureWidget = new StudyFigureWidget(figure,
                                                           this);
   figuresLayout->addWidget(figureWidget);
}
      
/**
 * remove a figure widget from its layout (does not delete the widget).
 */
void 
StudyWidget::removeFigureWidget(StudyFigureWidget* figureWidget)
{
   figuresLayout->removeWidget(figureWidget);
   StudyMetaData::Figure* figure = figureWidget->getFigureInThisWidget();
   studyMetaData->deleteFigure(figure);
}

/**
 * add a table to the study (if NULL add new, empty table).
 */
void 
StudyWidget::addTable(StudyMetaData::Table* table)
{
   if (table == NULL) {
      //
      // Determine the number for the new table
      //
      QString tableNumber;
      StudyMetaData* smd = getCurrentStudyMetaData();
      const int numTables = smd->getNumberOfTables();
      if (numTables > 0) {
         const StudyMetaData::Table* t = smd->getTable(numTables - 1);
         bool ok = false;
         int tn = t->getNumber().toInt(&ok);
         if (ok) {
            tn++;
            tableNumber = QString::number(tn);
         }
      }
      
      //
      // Create the new table, add a subheader to the table, and add table to the study widget
      //
      table = new StudyMetaData::Table;
      if (tableNumber.isEmpty() == false) {
         table->setNumber(tableNumber);
      }
      table->addSubHeader(new StudyMetaData::SubHeader);
      studyMetaData->addTable(table);
   }
   StudyTableWidget* tableWidget = new StudyTableWidget(table,
                                                        this);
   tablesLayout->addWidget(tableWidget);
}
      
/**
 * remove a table widget from its layout (does not delete the widget).
 */
void 
StudyWidget::removeTableWidget(StudyTableWidget* tableWidget)
{
   tablesLayout->removeWidget(tableWidget);
   StudyMetaData::Table* table = tableWidget->getTableInThisWidget();
   studyMetaData->deleteTable(table);
}

/**
 * add a page reference to the study (if NULL add new, empty page reference).
 */
void 
StudyWidget::addPageReference(StudyMetaData::PageReference* pageReference)
{
   if (pageReference == NULL) {
      pageReference = new StudyMetaData::PageReference;
      studyMetaData->addPageReference(pageReference);
   }
   StudyPageReferenceWidget* pageReferenceWidget = new StudyPageReferenceWidget(pageReference,
                                                                                this);
   pageReferenceLayout->addWidget(pageReferenceWidget);
}

/** 
 * add a provenance to the study (if NULL add new, empty provenance)
 */
void
StudyWidget::addProvenance(StudyMetaData::Provenance* provenance)
{
   if (provenance == NULL) {
      provenance = new StudyMetaData::Provenance;
      studyMetaData->addProvenance(provenance);
   }
   StudyProvenanceWidget* provenanceWidget = new StudyProvenanceWidget(provenance,
                                                                       this);
   provenanceLayout->addWidget(provenanceWidget);
}

/**
 * remove a page reference widget from its layout (does not delete the widget).
 */
void 
StudyWidget::removePageReferenceWidget(StudyPageReferenceWidget* pageReferenceWidget)
{
   pageReferenceLayout->removeWidget(pageReferenceWidget);
   StudyMetaData::PageReference* pageRef = pageReferenceWidget->getPageReferenceInThisWidget();
   studyMetaData->deletePageReference(pageRef);
}

/**
 * remove a provenance widget from its layout (does not delete the widget).
 */
void
StudyWidget::removeProvenanceWidget(StudyProvenanceWidget* provenanceWidget)
{
   provenanceLayout->removeWidget(provenanceWidget);
   StudyMetaData::Provenance* provenance = provenanceWidget->getProvenanceInThisWidget();
   studyMetaData->deleteProvenance(provenance);
}

/**
 * save all data including figures and tables.
 */
void 
StudyWidget::saveDataIncludingFiguresAndTables()
{
   //
   // Make sure study is still valid
   //
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   if (smdf->getStudyMetaDataIndex(studyMetaData) >= 0) {
      slotSaveData();
      
      //
      // Save figure data
      //
      
      //
      // Save table data
      //
   }
}

/**
 * called when species button pressed.
 */
void 
StudyWidget::slotStudySpeciesPushButton()
{
   GuiNameSelectionDialog nsd(this,
                              GuiNameSelectionDialog::LIST_SPECIES,
                              GuiNameSelectionDialog::LIST_SPECIES);
   if (nsd.exec() == GuiNameSelectionDialog::Accepted) {
      studySpeciesLineEdit->setText(nsd.getNameSelected());
      slotStudySpeciesLineEditChanged();
   }
}
      
/**
 * called when stereotaxic space button pressed.
 */
void 
StudyWidget::slotStudyStereotaxicSpacePushButton()
{
   GuiNameSelectionDialog nsd(this,
                              GuiNameSelectionDialog::LIST_STEREOTAXIC_SPACES,
                              GuiNameSelectionDialog::LIST_STEREOTAXIC_SPACES);
   if (nsd.exec() == GuiNameSelectionDialog::Accepted) {
      studyStereotaxicSpaceLineEdit->setText(nsd.getNameSelected());
      slotStudyStereotaxicSpaceLineEditChanged();
   }
}
      
/**
 * called when study number changed.
 */
void 
StudyWidget::slotStudyProjectIDLineEditChanged()
{
   studyMetaData->setProjectID(studyProjectIDLineEdit->text());
}

/**
 * called when title changed.
 */
void 
StudyWidget::slotStudyTitleLineEditChanged()
{
   studyMetaData->setTitle(studyTitleLineEdit->text());
}

/**
 * called when authors changed.
 */
void 
StudyWidget::slotStudyAuthorsLineEditChanged()
{
   studyMetaData->setAuthors(studyAuthorsLineEdit->text());
}

/**
 * called when citation changed.
 */
void 
StudyWidget::slotStudyCitationLineEditChanged()
{
   studyMetaData->setCitation(studyCitationLineEdit->text());
}

/**
 * called when mesh changed.
 */
void 
StudyWidget::slotStudyMeshLineEditChanged()
{
   studyMetaData->setMedicalSubjectHeadings(studyMeshLineEdit->text());
}

/**
 * called when name changed.
 */
void 
StudyWidget::slotStudyNameLineEditChanged()
{
   studyMetaData->setName(studyNameLineEdit->text());
}
      
/**
 * called when pubmed id changed.
 */
void 
StudyWidget::slotStudyPubMedIDSpinBoxChanged()
{
   studyMetaData->setPubMedID(studyPubMedIDLineEdit->text());
}

/**
 * called when doi changed.
 */
void 
StudyWidget::slotStudyDocumentObjectIdentifierLineEditChanged()
{
   studyMetaData->setDocumentObjectIdentifier(studyDocumentObjectIdentifierLineEdit->text());
}

/**
 * called when species changed.
 */
void 
StudyWidget::slotStudySpeciesLineEditChanged()
{
   studyMetaData->setSpecies(studySpeciesLineEdit->text());
}
      
/**
 * called when space changed.
 */
void 
StudyWidget::slotStudyStereotaxicSpaceLineEditChanged()
{
   studyMetaData->setStereotaxicSpace(studyStereotaxicSpaceLineEdit->text());
}

/**
 * called when space details changed.
 */
void 
StudyWidget::slotStudyStereotaxicSpaceDetailsLineEditChanged()
{
   studyMetaData->setStereotaxicSpaceDetails(studyStereotaxicSpaceDetailsLineEdit->text());
}

/**
 * called when data format details changed.
 */
void 
StudyWidget::slotStudyDataFormatLineEditChanged()
{
   studyMetaData->setStudyDataFormat(studyDataFormatLineEdit->text());
}

/**
 * called when data type details changed.
 */
void 
StudyWidget::slotStudyDataTypeLineEditChanged()
{
   studyMetaData->setStudyDataType(studyDataTypeLineEdit->text());
}

/**
 * called when keywords changed.
 */
void 
StudyWidget::slotStudyKeywordsLineEditChanged()
{
   keywordsModifiedFlag = (studyMetaData->getKeywords() != studyKeywordsLineEdit->text());
   studyMetaData->setKeywords(studyKeywordsLineEdit->text());
}      

/**
 * called when keywords editing finished (return pressed or loses focus).
 */
void 
StudyWidget::slotStudyKeywordsLineEditFinished()
{
   if (keywordsModifiedFlag) {
      QTimer::singleShot(0, this, SLOT(slotUpdateKeywordsAndGUI()));
      keywordsModifiedFlag = false;
   }
}

/**
 * called to update keywords in GUI.
 */
void 
StudyWidget::slotUpdateKeywordsAndGUI()
{
   // GUI UPDATE IS TOO SLOW
   return;
/*
   BrainSet* bs = theMainWindow->getBrainSet();
   bs->getDisplaySettingsStudyMetaData()->update();
   const StudyMetaDataFile* smdf = bs->getStudyMetaDataFile();
   if ((bs->getFociProjectionFile()->empty() == false) &&
       (smdf->getModified())) {
      bs->getDisplaySettingsFoci()->update();
      bs->assignFociColors();
      GuiFilesModified fm;
      fm.setFociModified();
      fm.setStudyMetaDataModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
   }
*/
}

/**
 * called when part scheme changed.
 */
void 
StudyWidget::slotStudyPartitioningSchemeAbbreviationLineEditChanged()
{
   studyMetaData->setPartitioningSchemeAbbreviation(studyPartitioningSchemeAbbreviationLineEdit->text());
}

/**
 * called when part schem name changed.
 */
void 
StudyWidget::slotStudyPartitioningSchemeFullNameLineEditChanged()
{
   studyMetaData->setPartitioningSchemeFullName(studyPartitioningSchemeFullNameLineEdit->text());
}

/**
 * called when quality changed.
 */
void 
StudyWidget::slotStudyQualityLineEditChanged()
{
   studyMetaData->setQuality(studyQualityLineEdit->text());
}

/**
 * called when msl id changed.
 */
void  
StudyWidget::slotStudyMslIDLineEditChanged()
{
   studyMetaData->setMslID(studyMslIDLineEdit->text());
}

/**
 * called when parent id changed.
 */
void  
StudyWidget::slotStudyParentIDLineEditChanged()
{
   studyMetaData->setParentID(studyParentIDLineEdit->text());
}

/**
 * called when core metadata changed.
 */
void  
StudyWidget::slotStudyCoreDataCompletedLineEditChanged()
{
   studyMetaData->setCoreDataCompleted(studyCoreDataCompletedLineEdit->text());
}

/**
 * called when completed changed.
 */
void  
StudyWidget::slotStudyCompletedLineEditChanged()
{
   studyMetaData->setCompleted(studyCompletedLineEdit->text());
}

/**
 * called when public access changed.
 */
void  
StudyWidget::slotStudyPublicAccessLineEditChanged()
{
   studyMetaData->setPublicAccess(studyPublicAccessLineEdit->text());
}

/**
 * called when comment changed.
 */
void 
StudyWidget::slotStudyCommentTextEditChanged()
{
   studyMetaData->setComment(studyCommentTextEdit->toPlainText());
}

/**
 * save the data into the study meta data table subheader.
 */
void 
StudyWidget::slotSaveData()
{
   //
   // Make sure study is still valid
   //
   StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   if (smdf->getStudyMetaDataIndex(studyMetaData) >= 0) {
   }
}

//=====================================================================================
//
// Widget for displaying a table
//
//=====================================================================================

/**
 * constructor.
 */
StudyTableWidget::StudyTableWidget(StudyMetaData::Table* tableIn,
                                   StudyWidget* parentStudyWidgetIn,
                                   QWidget* parentIn)
   : QGroupBox("Table", parentIn)
{
   table = tableIn;
   parentStudyWidget = parentStudyWidgetIn;
   
   QLabel* tableNumberLabel = new QLabel("Number");
   tableNumberLineEdit = new QLineEdit;
   tableNumberLineEdit->setToolTip("Enter the table number here.");
   QObject::connect(tableNumberLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotTableNumberLineEditChanged()));
   
   QLabel* tableHeaderLabel = new QLabel("Header");
   tableHeaderLineEdit = new QLineEdit;
   tableHeaderLineEdit->setToolTip("Enter the table header here.");
   QObject::connect(tableHeaderLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotTableHeaderLineEditChanged()));
   
   QLabel* tableFooterLabel = new QLabel("Footer");
   tableFooterTextEdit = new QTextEdit;
   tableFooterTextEdit->setReadOnly(false);
   tableFooterTextEdit->setMinimumHeight(minimumTextEditHeight);
   tableFooterTextEdit->setToolTip("Enter the table footer here.");
   QObject::connect(tableFooterTextEdit, SIGNAL(textChanged()),
                    this, SLOT(slotTableFooterTextEditChanged()));
    
   QLabel* tableSizeUnitsLabel = new QLabel("Size Units");
   tableSizeUnitsLineEdit = new QLineEdit;
   tableSizeUnitsLineEdit->setToolTip("Enter the size units (mm, voxels, etc) here.");
   QObject::connect(tableSizeUnitsLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotTableSizeUnitsLineEditChanged()));
    
   QLabel* tableVoxelSizeLabel = new QLabel("Voxel Size");
   tableVoxelSizeLineEdit = new QLineEdit;
   tableVoxelSizeLineEdit->setToolTip("Enter voxel size here.");
   QObject::connect(tableVoxelSizeLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotTableVoxelSizeLineEditChanged()));
    
   QPushButton* tableStatisticPushButton = new QPushButton("Statistic...");
   tableStatisticPushButton->setAutoDefault(false);
   QObject::connect(tableStatisticPushButton, SIGNAL(clicked()),
                    this, SLOT(slotTableStatisticPushButton()));
   tableStatisticLineEdit = new QLineEdit;
   tableStatisticLineEdit->setToolTip("Enter the statistic type here.");
   QObject::connect(tableStatisticLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotTableStatisticLineEditChanged()));
    
   QLabel* tableStatisticDescriptionLabel = new QLabel("Stat Description");
   tableStatisticDescriptionLineEdit = new QLineEdit;
   tableStatisticDescriptionLineEdit->setToolTip("Enter a description of the \n"
                                                 "statistic test here.");
   QObject::connect(tableStatisticDescriptionLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotTableStatisticDescriptionLineEditChanged()));
    
   int rowCounter = 0;
   const int tableNumberRow = rowCounter++;
   const int tableHeaderRow = rowCounter++;
   const int tableFooterRow = rowCounter++;
   const int tableSizeUnitsRow = rowCounter++;
   const int tableVoxelSizeRow = rowCounter++;
   const int tableStatisticRow = rowCounter++;
   const int tableStatisticDescriptionRow = rowCounter++;
   
   QGridLayout* grid = new QGridLayout;
   grid->addWidget(tableNumberLabel, tableNumberRow, 0);
   grid->addWidget(tableNumberLineEdit, tableNumberRow, 1);
   grid->addWidget(tableHeaderLabel, tableHeaderRow, 0);
   grid->addWidget(tableHeaderLineEdit, tableHeaderRow, 1);
   grid->addWidget(tableFooterLabel, tableFooterRow, 0);
   grid->addWidget(tableFooterTextEdit, tableFooterRow, 1);
   grid->addWidget(tableSizeUnitsLabel, tableSizeUnitsRow, 0);
   grid->addWidget(tableSizeUnitsLineEdit, tableSizeUnitsRow, 1);
   grid->addWidget(tableVoxelSizeLabel, tableVoxelSizeRow, 0);
   grid->addWidget(tableVoxelSizeLineEdit, tableVoxelSizeRow, 1);
   grid->addWidget(tableStatisticPushButton, tableStatisticRow, 0);
   grid->addWidget(tableStatisticLineEdit, tableStatisticRow, 1);
   grid->addWidget(tableStatisticDescriptionLabel, tableStatisticDescriptionRow, 0);
   grid->addWidget(tableStatisticDescriptionLineEdit, tableStatisticDescriptionRow, 1);
   
   QPushButton* addSubHeaderPushButton = new QPushButton("Add Sub Header to This Table");
   addSubHeaderPushButton->setAutoDefault(false);
   QObject::connect(addSubHeaderPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddSubHeaderPushButton()));
                    
   QPushButton* deleteThisTablePushButton = new QPushButton("Delete This Table");
   deleteThisTablePushButton->setAutoDefault(false);
   QObject::connect(deleteThisTablePushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteThisTablePushButton()));
   
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(addSubHeaderPushButton);
   buttonsLayout->addWidget(deleteThisTablePushButton);
   buttonsLayout->addStretch();
   
   subHeadersLayout = new QVBoxLayout;
   
   layout = new QVBoxLayout(this);
   layout->addLayout(grid);
   layout->addLayout(subHeadersLayout);
   layout->addLayout(buttonsLayout);

   allWidgetsGroup = new WuQWidgetGroup(this);
   allWidgetsGroup->addWidget(tableNumberLineEdit);
   allWidgetsGroup->addWidget(tableHeaderLineEdit);
   allWidgetsGroup->addWidget(tableFooterTextEdit);
   allWidgetsGroup->addWidget(tableSizeUnitsLineEdit);
   allWidgetsGroup->addWidget(tableVoxelSizeLineEdit);
   allWidgetsGroup->addWidget(tableStatisticLineEdit);
   allWidgetsGroup->addWidget(tableStatisticDescriptionLineEdit);
   
   loadData();
}

/**
 * destructor.
 */
StudyTableWidget::~StudyTableWidget()
{
}

/**
 * add a sub header to this table widget.
 */
void 
StudyTableWidget::addSubHeader(StudyMetaData::SubHeader* subHeader)
{
   if (subHeader == NULL) {
      subHeader = new StudyMetaData::SubHeader;
      subHeader->setNumber(QString::number(table->getNumberOfSubHeaders() + 1));
      table->addSubHeader(subHeader);
   }
   StudySubHeaderWidget* subHeaderWidget = new StudySubHeaderWidget(subHeader, this, NULL);
   subHeadersLayout->addWidget(subHeaderWidget);
}

/**
 * called when add sub header button is pressed.
 */
void 
StudyTableWidget::slotAddSubHeaderPushButton()
{
   addSubHeader(NULL);
}
      
/**
 * called when delete this table widget button is pressed.
 */
void 
StudyTableWidget::slotDeleteThisTablePushButton()
{
   if (QMessageBox::question(this, "Confirm",
                               "Delete this table",
                               (QMessageBox::Yes | QMessageBox::No),
                               QMessageBox::Yes) ==
                                  QMessageBox::Yes) {
      //
      // Remove this widget from its parent layout
      //
      parentStudyWidget->removeTableWidget(this);
      
      //
      // Tell QT to delete this widget when it gets pack to its event loop
      //
      this->deleteLater();
   }
}

/**
 * remove a sub header widget.
 */
void 
StudyTableWidget::removeSubHeaderWidget(StudySubHeaderWidget* subHeaderWidget)
{
   subHeadersLayout->removeWidget(subHeaderWidget);
   StudyMetaData::SubHeader* subHeader = subHeaderWidget->getSubHeaderInThisWidget();
   table->deleteSubHeader(subHeader);
}
      
/**
 * load data into the widget.
 */
void 
StudyTableWidget::loadData()
{
   allWidgetsGroup->blockSignals(true);

   tableNumberLineEdit->setText(table->getNumber());
   tableHeaderLineEdit->setText(table->getHeader());
   tableHeaderLineEdit->home(false);
   tableFooterTextEdit->setPlainText(table->getFooter());
   //QTextCursor tc;
   //tc.movePosition(QTextCursor::Start);
   //tableFooterTextEdit->setTextCursor(tc);
   tableSizeUnitsLineEdit->setText(table->getSizeUnits());
   tableSizeUnitsLineEdit->home(false);
   tableVoxelSizeLineEdit->setText(table->getVoxelDimensions());
   tableVoxelSizeLineEdit->home(false);
   tableStatisticLineEdit->setText(table->getStatisticType());
   tableStatisticLineEdit->home(false);
   tableStatisticDescriptionLineEdit->setText(table->getStatisticDescription());
   tableStatisticDescriptionLineEdit->home(false);

   for (int i = 0; i < table->getNumberOfSubHeaders(); i++) {
      addSubHeader(table->getSubHeader(i));
   }
   
   allWidgetsGroup->blockSignals(false);
}

/**
 * called when statistic push button pressed.
 */
void 
StudyTableWidget::slotTableStatisticPushButton()
{
   GuiNameSelectionDialog nsd(this,
                              GuiNameSelectionDialog::LIST_STATISTICS,
                              GuiNameSelectionDialog::LIST_STATISTICS);
   if (nsd.exec() == GuiNameSelectionDialog::Accepted) {
      tableStatisticLineEdit->setText(nsd.getNameSelected());
      slotTableStatisticLineEditChanged();
   }
}
      
/**
 * called when table number changed.
 */
void 
StudyTableWidget::slotTableNumberLineEditChanged()
{
   table->setNumber(tableNumberLineEdit->text());
}

/**
 * called when table header changed.
 */
void 
StudyTableWidget::slotTableHeaderLineEditChanged()
{
   table->setHeader(tableHeaderLineEdit->text());
}

/**
 * called when table footer changed.
 */
void 
StudyTableWidget::slotTableFooterTextEditChanged()
{
   table->setFooter(tableFooterTextEdit->toPlainText());
}

/**
 * called when table size units changed.
 */
void 
StudyTableWidget::slotTableSizeUnitsLineEditChanged()
{
   table->setSizeUnits(tableSizeUnitsLineEdit->text());
}

/**
 * called when table voxel size changed.
 */
void 
StudyTableWidget::slotTableVoxelSizeLineEditChanged()
{
   table->setVoxelDimensions(tableVoxelSizeLineEdit->text());
}

/**
 * called when table statistic changed.
 */
void 
StudyTableWidget::slotTableStatisticLineEditChanged()
{
   table->setStatisticType(tableStatisticLineEdit->text());
}

/**
 * called when table statistic description changed.
 */
void 
StudyTableWidget::slotTableStatisticDescriptionLineEditChanged()
{
   table->setStatisticDescription(tableStatisticDescriptionLineEdit->text());
}
      
/**
 * save the data into the study meta data table.
 */
void 
StudyTableWidget::slotSaveData()
{
}            

//=====================================================================================
//
// Widget for displaying a sub header
//
//=====================================================================================
/**
 * constructor.
 */
StudySubHeaderWidget::StudySubHeaderWidget(StudyMetaData::SubHeader* subHeaderIn,
                                           StudyTableWidget* parentStudyTableWidgetIn,
                                           StudyPageReferenceWidget* parentPageReferenceWidgetIn,
                                           QWidget* parentIn)
   : QGroupBox("Sub Header", parentIn)
{
   subHeader = subHeaderIn;
   parentStudyTableWidget = parentStudyTableWidgetIn;
   parentPageReferenceWidget = parentPageReferenceWidgetIn;
   
   QLabel* subHeaderNumberLabel = new QLabel("Number");
   subHeaderNumberLineEdit = new QLineEdit;
   subHeaderNumberLineEdit->setToolTip("");
   QObject::connect(subHeaderNumberLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotSubHeaderNumberLineEditChanged()));
   
   QLabel* subHeaderNameLabel = new QLabel("Name");
   subHeaderNameLineEdit = new QLineEdit;
   subHeaderNameLineEdit->setToolTip("");
   QObject::connect(subHeaderNameLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotSubHeaderNameLineEditChanged()));
   
   QLabel* subHeaderShortNameLabel = new QLabel("Short Name");
   subHeaderShortNameLineEdit = new QLineEdit;
   subHeaderShortNameLineEdit->setToolTip("");
   QObject::connect(subHeaderShortNameLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotSubHeaderShortNameLineEditChanged()));
   QObject::connect(subHeaderShortNameLineEdit, SIGNAL(editingFinished()),
                   this, SLOT(slotSubHeaderShortNameEditingFinished()));
   shortNameModifiedFlag = false;
   
   QLabel* subHeaderTaskDescriptionLabel = new QLabel("Task Description");
   subHeaderTaskDescriptionLineEdit = new QLineEdit;
   subHeaderTaskDescriptionLineEdit->setToolTip("");
   QObject::connect(subHeaderTaskDescriptionLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotSubHeaderTaskDescriptionLineEditChanged()));
   
   QLabel* subHeaderTaskBaselineLabel = new QLabel("Task Baseline");
   subHeaderTaskBaselineLineEdit = new QLineEdit;
   subHeaderTaskBaselineLineEdit->setToolTip("");
   QObject::connect(subHeaderTaskBaselineLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotSubHeaderTaskBaselineLineEditChanged()));
   
   QLabel* subHeaderTestAttributesLabel = new QLabel("Test Attributes");
   subHeaderTestAttributesLineEdit = new QLineEdit;
   subHeaderTestAttributesLineEdit->setToolTip("");
   QObject::connect(subHeaderTestAttributesLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotSubHeaderTestAttributesLineEditChanged()));
   
   int rowCounter = 0;
   const int numberRow = rowCounter++;
   const int nameRow = rowCounter++;
   const int shortNameRow = rowCounter++;
   const int taskDescriptionRow = rowCounter++;
   const int taskBaselineRow = rowCounter++;
   const int testAttributesRow = rowCounter++;
   
   QGridLayout* grid = new QGridLayout;
   grid->addWidget(subHeaderNumberLabel, numberRow, 0);
   grid->addWidget(subHeaderNumberLineEdit, numberRow, 1);
   grid->addWidget(subHeaderNameLabel, nameRow, 0);
   grid->addWidget(subHeaderNameLineEdit, nameRow, 1);
   grid->addWidget(subHeaderShortNameLabel, shortNameRow, 0);
   grid->addWidget(subHeaderShortNameLineEdit, shortNameRow, 1);
   grid->addWidget(subHeaderTaskDescriptionLabel, taskDescriptionRow, 0);
   grid->addWidget(subHeaderTaskDescriptionLineEdit, taskDescriptionRow, 1);
   grid->addWidget(subHeaderTaskBaselineLabel, taskBaselineRow, 0);
   grid->addWidget(subHeaderTaskBaselineLineEdit, taskBaselineRow, 1);
   grid->addWidget(subHeaderTestAttributesLabel, testAttributesRow, 0);
   grid->addWidget(subHeaderTestAttributesLineEdit, testAttributesRow, 1);

   QPushButton* shortNameToClassPushButton = new QPushButton("Short Name to Foci Class");
   shortNameToClassPushButton->setAutoDefault(false);
   shortNameToClassPushButton->setFixedSize(shortNameToClassPushButton->sizeHint());
   QObject::connect(shortNameToClassPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSubHeaderToFociClass()));
                    
   QPushButton* deleteThisSubHeaderPushButton = new QPushButton("Delete This Sub Header");
   deleteThisSubHeaderPushButton->setAutoDefault(false);
   deleteThisSubHeaderPushButton->setFixedSize(deleteThisSubHeaderPushButton->sizeHint());
   QObject::connect(deleteThisSubHeaderPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteThisSubHeader()));
                    
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(shortNameToClassPushButton);
   buttonsLayout->addWidget(deleteThisSubHeaderPushButton);
   buttonsLayout->addStretch();
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addLayout(grid);
   layout->addLayout(buttonsLayout);

   allWidgetsGroup = new WuQWidgetGroup(this);
   allWidgetsGroup->addWidget(subHeaderNumberLineEdit);
   allWidgetsGroup->addWidget(subHeaderNameLineEdit);
   allWidgetsGroup->addWidget(subHeaderShortNameLineEdit);
   allWidgetsGroup->addWidget(subHeaderTaskDescriptionLineEdit);
   allWidgetsGroup->addWidget(subHeaderTaskBaselineLineEdit);
   allWidgetsGroup->addWidget(subHeaderTestAttributesLineEdit);
   
   
   loadData();
}

/**
 * destructor.
 */
StudySubHeaderWidget::~StudySubHeaderWidget()
{
}

/**
 * called when short name to foci class button pressed.
 */
void 
StudySubHeaderWidget::slotSubHeaderToFociClass()
{
   const QString shortName = subHeaderShortNameLineEdit->text();
   if (shortName.isEmpty() == false) {
      if (subHeader != NULL) {
         StudyMetaDataLink smdl;
         smdl.setTableSubHeaderNumber(subHeader->getNumber());
         if (parentStudyTableWidget != NULL) {
            if (parentStudyTableWidget->table != NULL) {
               smdl.setTableNumber(parentStudyTableWidget->table->getNumber());
               if (parentStudyTableWidget->parentStudyWidget != NULL) {
                  if (parentStudyTableWidget->parentStudyWidget->studyMetaData != NULL) {
                     smdl.setPubMedID(
                        parentStudyTableWidget->parentStudyWidget->studyMetaData->getPubMedID());
                     theMainWindow->getBrainSet()->getFociProjectionFile(
                        )->transferTableSubHeaderShortNameToCellClass(smdl,
                                                                      shortName);
                  }
               }
            }
         }
      }
   }
}

/**
 * called when delete this sub header button is pressed.
 */
void 
StudySubHeaderWidget::slotDeleteThisSubHeader()
{
   if (QMessageBox::question(this, "Confirm",
                               "Delete this sub header", 
                               (QMessageBox::Yes | QMessageBox::No),
                               QMessageBox::Yes)
                                  == QMessageBox::Yes) {
      //
      // Remove this widget from its parent layout
      //
      if (parentStudyTableWidget != NULL) {
         parentStudyTableWidget->removeSubHeaderWidget(this);
      }
      if (parentPageReferenceWidget != NULL) {
         parentPageReferenceWidget->removeSubHeaderWidget(this);
      }
         
      //
      // Tell QT to delete this widget when it gets pack to its event loop
      //
      this->deleteLater();
   }
}
      
/**
 * load data into the widget.
 */
void 
StudySubHeaderWidget::loadData()
{
   allWidgetsGroup->blockSignals(true);

   subHeaderNumberLineEdit->setText(subHeader->getNumber());
   subHeaderNameLineEdit->setText(subHeader->getName());
   subHeaderNameLineEdit->home(false);
   subHeaderShortNameLineEdit->setText(subHeader->getShortName());
   subHeaderShortNameLineEdit->home(false);
   shortNameModifiedFlag = false;
   subHeaderTaskDescriptionLineEdit->setText(subHeader->getTaskDescription());
   subHeaderTaskDescriptionLineEdit->home(false);
   subHeaderTaskBaselineLineEdit->setText(subHeader->getTaskBaseline());
   subHeaderTaskBaselineLineEdit->home(false);
   subHeaderTestAttributesLineEdit->setText(subHeader->getTestAttributes());
   subHeaderTestAttributesLineEdit->home(false);

   allWidgetsGroup->blockSignals(false);
}

/**
 * called when sub header number changed.
 */
void 
StudySubHeaderWidget::slotSubHeaderNumberLineEditChanged()
{
   subHeader->setNumber(subHeaderNumberLineEdit->text());
}

/**
 * called when sub header name changed.
 */
void 
StudySubHeaderWidget::slotSubHeaderNameLineEditChanged()
{
   subHeader->setName(subHeaderNameLineEdit->text());
}

/**
 * called when sub header short name changed.
 */
void 
StudySubHeaderWidget::slotSubHeaderShortNameLineEditChanged()
{
   shortNameModifiedFlag = (subHeader->getShortName() != subHeaderShortNameLineEdit->text());
   subHeader->setShortName(subHeaderShortNameLineEdit->text());
}

/**
 * called when sub header short name editing finished.
 */
void 
StudySubHeaderWidget::slotSubHeaderShortNameEditingFinished()
{
   if (shortNameModifiedFlag) {
      QTimer::singleShot(0, this, SLOT(slotUpdateShortNamesAndGUI()));
      shortNameModifiedFlag = false;
   }
}
      
/**
 * called to update gui.
 */
void 
StudySubHeaderWidget::slotUpdateShortNamesAndGUI()
{
   // GUI UPDATE IS TOO SLOW
   return;
/*
   BrainSet* bs = theMainWindow->getBrainSet();
   bs->getDisplaySettingsStudyMetaData()->update();
   const StudyMetaDataFile* smdf = bs->getStudyMetaDataFile();
   if ((bs->getFociProjectionFile()->empty() == false) &&
       (smdf->getModified())) {
      bs->getDisplaySettingsFoci()->update();
      bs->assignFociColors();
      GuiFilesModified fm;
      fm.setFociModified();
      fm.setStudyMetaDataModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
   }
*/
}
      
/**
 * called when sub header task description changed.
 */
void 
StudySubHeaderWidget::slotSubHeaderTaskDescriptionLineEditChanged()
{
   subHeader->setTaskDescription(subHeaderTaskDescriptionLineEdit->text());
}

/**
 * called when sub header task baseline changed.
 */
void 
StudySubHeaderWidget::slotSubHeaderTaskBaselineLineEditChanged()
{
   subHeader->setTaskBaseline(subHeaderTaskBaselineLineEdit->text());
}

/**
 * called when test attributes changed.
 */
void 
StudySubHeaderWidget::slotSubHeaderTestAttributesLineEditChanged()
{
   subHeader->setTestAttributes(subHeaderTestAttributesLineEdit->text());
}
      
/**
 * save the data into the study meta data table.
 */
void 
StudySubHeaderWidget::slotSaveData()
{
}      

//=====================================================================================
//
// Widget for displaying a figure
//
//=====================================================================================
/**
 * constructor.
 */
StudyFigureWidget::StudyFigureWidget(StudyMetaData::Figure* figureIn,
                                     StudyWidget* parentStudyWidgetIn,
                                     QWidget* parentIn)
   : QGroupBox("Figure", parentIn)
{
   figure = figureIn;
   parentStudyWidget = parentStudyWidgetIn;
   
   QLabel* figureNumberLabel = new QLabel("Number");
   figureNumberLineEdit = new QLineEdit;
   figureNumberLineEdit->setToolTip("Enter the figure's number here.");
   QObject::connect(figureNumberLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotFigureNumberLineEditChanged()));
   
   QLabel* figureLegendLabel = new QLabel("Legend");
   figureLegendLineEdit = new QLineEdit;
   figureLegendLineEdit->setToolTip("Enter the figure's legend here.");
   QObject::connect(figureLegendLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotFigureLegendLineEditChanged()));
   
   int rowCount = 0;
   const int figureNumberRow = rowCount++;
   const int figureLegendRow = rowCount++;
   
   QGridLayout* grid = new QGridLayout;
   grid->addWidget(figureNumberLabel, figureNumberRow, 0);
   grid->addWidget(figureNumberLineEdit, figureNumberRow, 1);
   grid->addWidget(figureLegendLabel, figureLegendRow, 0);
   grid->addWidget(figureLegendLineEdit, figureLegendRow, 1);
   
   QPushButton* addPanelPushButton = new QPushButton("Add Panel to This Figure");
   addPanelPushButton->setAutoDefault(false);
   QObject::connect(addPanelPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddPanelPushButton()));
                    
   QPushButton* deleteThisFigurePushButton = new QPushButton("Delete This Figure");
   deleteThisFigurePushButton->setAutoDefault(false);
   QObject::connect(deleteThisFigurePushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteThisFigurePushButton()));
   
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(addPanelPushButton);
   buttonsLayout->addWidget(deleteThisFigurePushButton);
   buttonsLayout->addStretch();
   
   panelsLayout = new QVBoxLayout;
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addLayout(grid);
   layout->addLayout(panelsLayout);
   layout->addLayout(buttonsLayout);

   allWidgetsGroup = new WuQWidgetGroup(this);
   allWidgetsGroup->addWidget(figureNumberLineEdit);
   allWidgetsGroup->addWidget(figureLegendLineEdit);
   
   loadData();
}
             
/**
 * destructor.
 */
StudyFigureWidget::~StudyFigureWidget()
{
}

/**
 * called when add panel button is pressed.
 */
void 
StudyFigureWidget::slotAddPanelPushButton()
{
   addPanel(NULL);
}
      
/**
 * called when delete this figure widget button is pressed.
 */
void 
StudyFigureWidget::slotDeleteThisFigurePushButton()
{
   if (QMessageBox::question(this, "Confirm",
                               "Delete this figure", 
                               (QMessageBox::Yes | QMessageBox::No),
                               QMessageBox::Yes)
                                  == QMessageBox::Yes) {
      //
      // Remove this widget from its parent layout
      //
      parentStudyWidget->removeFigureWidget(this);
      
      //
      // Tell QT to delete this widget when it gets pack to its event loop
      //
      this->deleteLater();
   }
}

/**
 * load data into the widget.
 */
void 
StudyFigureWidget::loadData()
{
   allWidgetsGroup->blockSignals(true);

   figureNumberLineEdit->setText(figure->getNumber());
   figureLegendLineEdit->setText(figure->getLegend());
   figureLegendLineEdit->home(false);

   for (int i = 0; i < figure->getNumberOfPanels(); i++) {
      addPanel(figure->getPanel(i));
   }
   
   allWidgetsGroup->blockSignals(false);
}

/**
 * called when figure number changed.
 */
void 
StudyFigureWidget::slotFigureNumberLineEditChanged()
{
   figure->setNumber(figureNumberLineEdit->text());
}

/**
 * called when figure legend changed.
 */
void 
StudyFigureWidget::slotFigureLegendLineEditChanged()
{
   figure->setLegend(figureLegendLineEdit->text());
}
      
/**
 * save the data into the study meta data table.
 */
void 
StudyFigureWidget::slotSaveData()
{
}
            
/**
 * add a panel to this figure.
 */
void 
StudyFigureWidget::addPanel(StudyMetaData::Figure::Panel* panel)
{
   if (panel == NULL) {
      panel = new StudyMetaData::Figure::Panel;
      figure->addPanel(panel);
   }
   StudyFigurePanelWidget* panelWidget = new StudyFigurePanelWidget(panel, this);
   panelsLayout->addWidget(panelWidget);
}

/**
 * remove a panel widget from its layout (does not delete the widget).
 */
void 
StudyFigureWidget::removePanelWidget(StudyFigurePanelWidget* panelWidget)
{
   panelsLayout->removeWidget(panelWidget);
   StudyMetaData::Figure::Panel* panel = panelWidget->getPanelInThisWidget();
   figure->deletePanel(panel);
}
      
//=====================================================================================
//
// Widget for displaying a figure panel
//
//=====================================================================================
/**
 * constructor.
 */
StudyFigurePanelWidget::StudyFigurePanelWidget(StudyMetaData::Figure::Panel* figurePanelIn,
                                               StudyFigureWidget* parentFigureWidgetIn,
                                               QWidget* parentIn)
   : QGroupBox("Figure Panel", parentIn) 
{
   figurePanel = figurePanelIn;
   parentFigureWidget = parentFigureWidgetIn;

   QLabel* figurePaneIdentifierlLabel = new QLabel("Identifier");
   figurePanelIdentifierLineEdit = new QLineEdit;
   figurePanelIdentifierLineEdit->setToolTip("Enter the figure panel's identifier here.");
   QObject::connect(figurePanelIdentifierLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotFigurePanelIdentifierLineEditChanged()));
   
   QLabel* figurePanelDescriptionLabel = new QLabel("Description");
   figurePanelDescriptionLineEdit = new QLineEdit;
   figurePanelDescriptionLineEdit->setToolTip("Enter the figure panel's description here.");
   QObject::connect(figurePanelDescriptionLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotFigurePanelDescriptionLineEditChanged()));
   
   QLabel* figurePanelTaskDescriptionLabel = new QLabel("Task Description");
   figurePanelTaskDescriptionLineEdit = new QLineEdit;
   figurePanelTaskDescriptionLineEdit->setToolTip("Enter the task's description here.");
   QObject::connect(figurePanelTaskDescriptionLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotFigurePanelTaskDescriptionLineEditChanged()));
   
   QLabel* figurePanelTaskBaselineLabel = new QLabel("Task Baseline");
   figurePanelTaskBaselineLineEdit = new QLineEdit;
   figurePanelTaskBaselineLineEdit->setToolTip("Enter the task's baseline here.");
   QObject::connect(figurePanelTaskBaselineLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotFigurePanelTaskBaselineLineEditChanged()));
   
   QLabel* figurePanelTestAttributeslLabel = new QLabel("Test Attributes");
   figurePanelTestAttributesLineEdit = new QLineEdit;
   figurePanelTestAttributesLineEdit->setToolTip("Enter the test's attributes here.");
   QObject::connect(figurePanelTestAttributesLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotFigurePanelTestAttributesLineEditChanged()));
   
   int rowCounter = 0;
   const int figurePanelIdentifierRow = rowCounter++;
   const int figurePanelDescriptionRow = rowCounter++;
   const int figurePanelTaskDescriptionRow = rowCounter++;
   const int figurePanelTaskBaselineRow = rowCounter++;
   const int figurePanelTestAttributesRow = rowCounter++;
   
   QGridLayout* grid = new QGridLayout;
   grid->addWidget(figurePaneIdentifierlLabel, figurePanelIdentifierRow, 0);
   grid->addWidget(figurePanelIdentifierLineEdit, figurePanelIdentifierRow, 1);
   grid->addWidget(figurePanelDescriptionLabel, figurePanelDescriptionRow, 0);
   grid->addWidget(figurePanelDescriptionLineEdit, figurePanelDescriptionRow, 1);
   grid->addWidget(figurePanelTaskDescriptionLabel, figurePanelTaskDescriptionRow, 0);
   grid->addWidget(figurePanelTaskDescriptionLineEdit, figurePanelTaskDescriptionRow, 1);
   grid->addWidget(figurePanelTaskBaselineLabel, figurePanelTaskBaselineRow, 0);
   grid->addWidget(figurePanelTaskBaselineLineEdit, figurePanelTaskBaselineRow, 1);
   grid->addWidget(figurePanelTestAttributeslLabel, figurePanelTestAttributesRow, 0);
   grid->addWidget(figurePanelTestAttributesLineEdit, figurePanelTestAttributesRow, 1);
   
   QPushButton* deleteThisPanelPushButton = new QPushButton("Delete This Panel");
   deleteThisPanelPushButton->setAutoDefault(false);
   deleteThisPanelPushButton->setFixedSize(deleteThisPanelPushButton->sizeHint());
   QObject::connect(deleteThisPanelPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteThisPanelPushButton()));
                    
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(deleteThisPanelPushButton);
   buttonsLayout->addStretch();
   
   layout = new QVBoxLayout(this);
   layout->addLayout(grid);
   layout->addLayout(buttonsLayout);

   allWidgetsGroup = new WuQWidgetGroup(this);
   allWidgetsGroup->addWidget(figurePanelIdentifierLineEdit);
   allWidgetsGroup->addWidget(figurePanelDescriptionLineEdit);
   allWidgetsGroup->addWidget(figurePanelTaskDescriptionLineEdit);
   allWidgetsGroup->addWidget(figurePanelTaskBaselineLineEdit);
   allWidgetsGroup->addWidget(figurePanelTestAttributesLineEdit);
   
   loadData();
}

/**
 * destructor.
 */
StudyFigurePanelWidget::~StudyFigurePanelWidget()
{
}

/**
 * called when delete this panel button pressed.
 */
void 
StudyFigurePanelWidget::slotDeleteThisPanelPushButton()
{
   if (QMessageBox::question(this, "Confirm",
                               "Delete this panel", 
                               (QMessageBox::Yes | QMessageBox::No),
                               QMessageBox::Yes)
                                  == QMessageBox::Yes) {
      //
      // Remove this widget from its parent layout
      //
      parentFigureWidget->removePanelWidget(this);
      
      //
      // Tell QT to delete this widget when it gets pack to its event loop
      //
      this->deleteLater();
   }
}
      
/**
 * load data into the widget.
 */
void 
StudyFigurePanelWidget::loadData()
{
   allWidgetsGroup->blockSignals(true);
   
   figurePanelIdentifierLineEdit->setText(figurePanel->getPanelNumberOrLetter());
   figurePanelIdentifierLineEdit->home(false);
   figurePanelDescriptionLineEdit->setText(figurePanel->getDescription());
   figurePanelDescriptionLineEdit->home(false);
   figurePanelTaskDescriptionLineEdit->setText(figurePanel->getTaskDescription());
   figurePanelTaskDescriptionLineEdit->home(false);
   figurePanelTaskBaselineLineEdit->setText(figurePanel->getTaskBaseline());
   figurePanelTaskBaselineLineEdit->home(false);
   figurePanelTestAttributesLineEdit->setText(figurePanel->getTestAttributes());
   figurePanelTestAttributesLineEdit->home(false);
   
   allWidgetsGroup->blockSignals(false);
}

/**
 * called when figure panel identifier changed.
 */
void 
StudyFigurePanelWidget::slotFigurePanelIdentifierLineEditChanged()
{
   figurePanel->setPanelNumberOrLetter(figurePanelIdentifierLineEdit->text());
}

/**
 * called when figure panel description changed.
 */
void 
StudyFigurePanelWidget::slotFigurePanelDescriptionLineEditChanged()
{
   figurePanel->setDescription(figurePanelDescriptionLineEdit->text());
}

/**
 * called when figure panel task description changed.
 */
void 
StudyFigurePanelWidget::slotFigurePanelTaskDescriptionLineEditChanged()
{
   figurePanel->setTaskDescription(figurePanelTaskDescriptionLineEdit->text());
}

/**
 * called when figure panel task baseline changed.
 */
void 
StudyFigurePanelWidget::slotFigurePanelTaskBaselineLineEditChanged()
{
   figurePanel->setTaskBaseline(figurePanelTaskBaselineLineEdit->text());
}

/**
 * called when figure panel test attributes changed.
 */
void 
StudyFigurePanelWidget::slotFigurePanelTestAttributesLineEditChanged()
{
   figurePanel->setTestAttributes(figurePanelTestAttributesLineEdit->text());
}
      
/**
 * save the data into the study meta data figure panel.
 */
void 
StudyFigurePanelWidget::slotSaveData()
{
}

//=====================================================================================
//
// Widget for displaying a page reference
//
//=====================================================================================
/**
 * constructor.
 */
StudyPageReferenceWidget::StudyPageReferenceWidget(StudyMetaData::PageReference* pageReferenceIn,
                                   StudyWidget* parentStudyWidgetIn,
                                   QWidget* parentIn)
   : QGroupBox("Page Reference", parentIn)
{
   pageReference = pageReferenceIn;
   parentStudyWidget = parentStudyWidgetIn;
   
   QLabel* pageReferenceNumberLabel = new QLabel("Number");
   pageReferenceNumberLineEdit = new QLineEdit;
   pageReferenceNumberLineEdit->setToolTip("Enter the page number here.");
   QObject::connect(pageReferenceNumberLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotPageReferencePageNumberLineEditChanged()));
   
   QLabel* pageReferenceHeaderLabel = new QLabel("Header");
   pageReferenceHeaderLineEdit = new QLineEdit;
   pageReferenceHeaderLineEdit->setToolTip("Enter the page header here.");
   QObject::connect(pageReferenceHeaderLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotPageReferenceHeaderLineEditChanged()));
   
   QLabel* pageReferenceCommentLabel = new QLabel("Comment");
   pageReferenceCommentTextEdit = new QTextEdit;
   pageReferenceCommentTextEdit->setReadOnly(false);
   pageReferenceCommentTextEdit->setMinimumHeight(minimumTextEditHeight);
   pageReferenceCommentTextEdit->setToolTip("Enter the pageReference footer here.");
   QObject::connect(pageReferenceCommentTextEdit, SIGNAL(textChanged()),
                    this, SLOT(slotPageReferenceCommentTextEditChanged()));
    
   QLabel* pageReferenceSizeUnitsLabel = new QLabel("Size Units");
   pageReferenceSizeUnitsLineEdit = new QLineEdit;
   pageReferenceSizeUnitsLineEdit->setToolTip("Enter the size units (mm, voxels, etc) here.");
   QObject::connect(pageReferenceSizeUnitsLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotPageReferenceSizeUnitsLineEditChanged()));
    
   QLabel* pageReferenceVoxelSizeLabel = new QLabel("Voxel Size");
   pageReferenceVoxelSizeLineEdit = new QLineEdit;
   pageReferenceVoxelSizeLineEdit->setToolTip("Enter voxel size here.");
   QObject::connect(pageReferenceVoxelSizeLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotPageReferenceVoxelSizeLineEditChanged()));
    
   QPushButton* pageReferenceStatisticPushButton = new QPushButton("Statistic...");
   pageReferenceStatisticPushButton->setAutoDefault(false);
   QObject::connect(pageReferenceStatisticPushButton, SIGNAL(clicked()),
                    this, SLOT(slotPageReferenceStatisticPushButton()));
   pageReferenceStatisticLineEdit = new QLineEdit;
   pageReferenceStatisticLineEdit->setToolTip("Enter the statistic type here.");
   QObject::connect(pageReferenceStatisticLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotPageReferenceStatisticLineEditChanged()));
    
   QLabel* pageReferenceStatisticDescriptionLabel = new QLabel("Stat Description");
   pageReferenceStatisticDescriptionLineEdit = new QLineEdit;
   pageReferenceStatisticDescriptionLineEdit->setToolTip("Enter a description of the \n"
                                                 "statistic test here.");
   QObject::connect(pageReferenceStatisticDescriptionLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotPageReferenceStatisticDescriptionLineEditChanged()));
    
   int rowCounter = 0;
   const int pageReferenceNumberRow = rowCounter++;
   const int pageReferenceHeaderRow = rowCounter++;
   const int pageReferenceCommentRow = rowCounter++;
   const int pageReferenceSizeUnitsRow = rowCounter++;
   const int pageReferenceVoxelSizeRow = rowCounter++;
   const int pageReferenceStatisticRow = rowCounter++;
   const int pageReferenceStatisticDescriptionRow = rowCounter++;
   
   QGridLayout* grid = new QGridLayout;
   grid->addWidget(pageReferenceNumberLabel, pageReferenceNumberRow, 0);
   grid->addWidget(pageReferenceNumberLineEdit, pageReferenceNumberRow, 1);
   grid->addWidget(pageReferenceHeaderLabel, pageReferenceHeaderRow, 0);
   grid->addWidget(pageReferenceHeaderLineEdit, pageReferenceHeaderRow, 1);
   grid->addWidget(pageReferenceCommentLabel, pageReferenceCommentRow, 0);
   grid->addWidget(pageReferenceCommentTextEdit, pageReferenceCommentRow, 1);
   grid->addWidget(pageReferenceSizeUnitsLabel, pageReferenceSizeUnitsRow, 0);
   grid->addWidget(pageReferenceSizeUnitsLineEdit, pageReferenceSizeUnitsRow, 1);
   grid->addWidget(pageReferenceVoxelSizeLabel, pageReferenceVoxelSizeRow, 0);
   grid->addWidget(pageReferenceVoxelSizeLineEdit, pageReferenceVoxelSizeRow, 1);
   grid->addWidget(pageReferenceStatisticPushButton, pageReferenceStatisticRow, 0);
   grid->addWidget(pageReferenceStatisticLineEdit, pageReferenceStatisticRow, 1);
   grid->addWidget(pageReferenceStatisticDescriptionLabel, pageReferenceStatisticDescriptionRow, 0);
   grid->addWidget(pageReferenceStatisticDescriptionLineEdit, pageReferenceStatisticDescriptionRow, 1);
   
   QPushButton* addSubHeaderPushButton = new QPushButton("Add Sub Header to This Page Reference");
   addSubHeaderPushButton->setAutoDefault(false);
   QObject::connect(addSubHeaderPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddSubHeaderPushButton()));
                    
   QPushButton* deleteThisPageReferencePushButton = new QPushButton("Delete This Page Reference");
   deleteThisPageReferencePushButton->setAutoDefault(false);
   QObject::connect(deleteThisPageReferencePushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteThisPageReferencePushButton()));
   
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(addSubHeaderPushButton);
   buttonsLayout->addWidget(deleteThisPageReferencePushButton);
   buttonsLayout->addStretch();
   
   subHeadersLayout = new QVBoxLayout;
   
   layout = new QVBoxLayout(this);
   layout->addLayout(grid);
   layout->addLayout(subHeadersLayout);
   layout->addLayout(buttonsLayout);

   allWidgetsGroup = new WuQWidgetGroup(this);
   allWidgetsGroup->addWidget(pageReferenceNumberLineEdit);
   allWidgetsGroup->addWidget(pageReferenceHeaderLineEdit);
   allWidgetsGroup->addWidget(pageReferenceCommentTextEdit);
   allWidgetsGroup->addWidget(pageReferenceSizeUnitsLineEdit);
   allWidgetsGroup->addWidget(pageReferenceVoxelSizeLineEdit);
   allWidgetsGroup->addWidget(pageReferenceStatisticLineEdit);
   allWidgetsGroup->addWidget(pageReferenceStatisticDescriptionLineEdit);
   
   loadData();
}

/**
 * destructor.
 */
StudyPageReferenceWidget::~StudyPageReferenceWidget()
{
}

/**
 * add a sub header to this page reference widget.
 */
void 
StudyPageReferenceWidget::addSubHeader(StudyMetaData::SubHeader* subHeader)
{
   if (subHeader == NULL) {
      subHeader = new StudyMetaData::SubHeader;
      subHeader->setNumber(QString::number(pageReference->getNumberOfSubHeaders() + 1));
      pageReference->addSubHeader(subHeader);
   }
   StudySubHeaderWidget* subHeaderWidget = new StudySubHeaderWidget(subHeader, NULL, this);
   subHeadersLayout->addWidget(subHeaderWidget);
}

/**
 * called when add sub header button is pressed.
 */
void 
StudyPageReferenceWidget::slotAddSubHeaderPushButton()
{
   addSubHeader(NULL);
}
      
/**
 * called when delete this pageReference widget button is pressed.
 */
void 
StudyPageReferenceWidget::slotDeleteThisPageReferencePushButton()
{
   if (QMessageBox::question(this, "Confirm",
                               "Delete this pageReference", 
                               (QMessageBox::Yes | QMessageBox::No),
                               QMessageBox::Yes)
                                  == QMessageBox::Yes) {
      //
      // Remove this widget from its parent layout
      //
      parentStudyWidget->removePageReferenceWidget(this);
      
      //
      // Tell QT to delete this widget when it gets pack to its event loop
      //
      this->deleteLater();
   }
}

/**
 * remove a sub header widget.
 */
void 
StudyPageReferenceWidget::removeSubHeaderWidget(StudySubHeaderWidget* subHeaderWidget)
{
   subHeadersLayout->removeWidget(subHeaderWidget);
   StudyMetaData::SubHeader* subHeader = subHeaderWidget->getSubHeaderInThisWidget();
   pageReference->deleteSubHeader(subHeader);
}
      
/**
 * load data into the widget.
 */
void 
StudyPageReferenceWidget::loadData()
{
   allWidgetsGroup->blockSignals(true);

   pageReferenceNumberLineEdit->setText(pageReference->getPageNumber());
   pageReferenceHeaderLineEdit->setText(pageReference->getHeader());
   pageReferenceHeaderLineEdit->home(false);
   pageReferenceCommentTextEdit->setPlainText(pageReference->getComment());
   //QTextCursor tc;
   //tc.movePosition(QTextCursor::Start);
   //pageReferenceFooterTextEdit->setTextCursor(tc);
   pageReferenceSizeUnitsLineEdit->setText(pageReference->getSizeUnits());
   pageReferenceSizeUnitsLineEdit->home(false);
   pageReferenceVoxelSizeLineEdit->setText(pageReference->getVoxelDimensions());
   pageReferenceVoxelSizeLineEdit->home(false);
   pageReferenceStatisticLineEdit->setText(pageReference->getStatisticType());
   pageReferenceStatisticLineEdit->home(false);
   pageReferenceStatisticDescriptionLineEdit->setText(pageReference->getStatisticDescription());
   pageReferenceStatisticDescriptionLineEdit->home(false);

   for (int i = 0; i < pageReference->getNumberOfSubHeaders(); i++) {
      addSubHeader(pageReference->getSubHeader(i));
   }
   
   allWidgetsGroup->blockSignals(false);
}

/**
 * called when statistic push button pressed.
 */
void 
StudyPageReferenceWidget::slotPageReferenceStatisticPushButton()
{
   GuiNameSelectionDialog nsd(this,
                              GuiNameSelectionDialog::LIST_STATISTICS,
                              GuiNameSelectionDialog::LIST_STATISTICS);
   if (nsd.exec() == GuiNameSelectionDialog::Accepted) {
      pageReferenceStatisticLineEdit->setText(nsd.getNameSelected());
      slotPageReferenceStatisticLineEditChanged();
   }
}
      
/**
 * called when page number changed.
 */
void 
StudyPageReferenceWidget::slotPageReferencePageNumberLineEditChanged()
{
   pageReference->setPageNumber(pageReferenceNumberLineEdit->text());
}

/**
 * called when page reference header changed.
 */
void 
StudyPageReferenceWidget::slotPageReferenceHeaderLineEditChanged()
{
   pageReference->setHeader(pageReferenceHeaderLineEdit->text());
}

/**
 * called when page reference comment changed.
 */
void 
StudyPageReferenceWidget::slotPageReferenceCommentTextEditChanged()
{
   pageReference->setComment(pageReferenceCommentTextEdit->toPlainText());
}

/**
 * called when page reference size units changed.
 */
void 
StudyPageReferenceWidget::slotPageReferenceSizeUnitsLineEditChanged()
{
   pageReference->setSizeUnits(pageReferenceSizeUnitsLineEdit->text());
}

/**
 * called when page reference voxel size changed.
 */
void 
StudyPageReferenceWidget::slotPageReferenceVoxelSizeLineEditChanged()
{
   pageReference->setVoxelDimensions(pageReferenceVoxelSizeLineEdit->text());
}

/**
 * called when page reference statistic changed.
 */
void 
StudyPageReferenceWidget::slotPageReferenceStatisticLineEditChanged()
{
   pageReference->setStatisticType(pageReferenceStatisticLineEdit->text());
}

/**
 * called when page reference statistic description changed.
 */
void 
StudyPageReferenceWidget::slotPageReferenceStatisticDescriptionLineEditChanged()
{
   pageReference->setStatisticDescription(pageReferenceStatisticDescriptionLineEdit->text());
}
      
/**
 * save the data into the study meta data page reference.
 */
void 
StudyPageReferenceWidget::slotSaveData()
{
}            

//=====================================================================================
//
// Widget for displaying provenance
//
//=====================================================================================

/**
 * constructor.
 */
StudyProvenanceWidget::StudyProvenanceWidget(StudyMetaData::Provenance* provenanceIn,
                                             StudyWidget* parentStudyWidgetIn,
                                             QWidget* parentIn)
   : QGroupBox("Provenance", parentIn)
{
   provenance = provenanceIn;
   parentStudyWidget = parentStudyWidgetIn;
   
   QLabel* provenanceNameLabel = new QLabel("Name");
   provenanceNameLineEdit = new QLineEdit;
   provenanceNameLineEdit->setToolTip("Enter your name here.");
   QObject::connect(provenanceNameLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotNameLineEditChanged()));
                    
   QLabel* provenanceDateLabel = new QLabel("Date");
   provenanceDateLineEdit = new QLineEdit;
   provenanceDateLineEdit->setToolTip("Enter today's date here.");
   QObject::connect(provenanceDateLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotDateLineEditChanged()));
                    
   QLabel* provenanceCommentLabel = new QLabel("Comment");
   provenanceCommentLineEdit = new QLineEdit;
   provenanceCommentLineEdit->setToolTip("Enter comment here.");
   QObject::connect(provenanceCommentLineEdit, SIGNAL(textEdited(const QString&)),
                    this, SLOT(slotCommentLineEditChanged()));
                    
   int rowCounter = 0;
   const int nameRow = rowCounter++;
   const int dateRow = rowCounter++;
   const int commentRow = rowCounter++;

   QGridLayout* grid = new QGridLayout;
   grid->addWidget(provenanceNameLabel, nameRow, 0);
   grid->addWidget(provenanceNameLineEdit, nameRow, 1);
   grid->addWidget(provenanceDateLabel, dateRow, 0);
   grid->addWidget(provenanceDateLineEdit, dateRow, 1);
   grid->addWidget(provenanceCommentLabel, commentRow, 0);
   grid->addWidget(provenanceCommentLineEdit, commentRow, 1);

   QPushButton* deleteThisProvenancePushButton = new QPushButton("Delete This Provenance");
   deleteThisProvenancePushButton->setAutoDefault(false);
   QObject::connect(deleteThisProvenancePushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeleteThisProvenancePushButton()));
   
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(deleteThisProvenancePushButton);
   buttonsLayout->addStretch();
   
   layout = new QVBoxLayout(this);
   layout->addLayout(grid);
   layout->addLayout(buttonsLayout);

   allWidgetsGroup = new WuQWidgetGroup(this);
   allWidgetsGroup->addWidget(provenanceNameLineEdit);
   allWidgetsGroup->addWidget(provenanceDateLineEdit);
   allWidgetsGroup->addWidget(provenanceCommentLineEdit);
   
   loadData();
}

/**
 * destructor.
 */
StudyProvenanceWidget::~StudyProvenanceWidget()
{
}

/**
 * load data into the widget.
 */
void 
StudyProvenanceWidget::loadData()
{
   allWidgetsGroup->blockSignals(true);

   provenanceNameLineEdit->setText(provenance->getName());
   provenanceNameLineEdit->home(false);
   provenanceDateLineEdit->setText(provenance->getDate());
   provenanceDateLineEdit->home(false);
   provenanceCommentLineEdit->setText(provenance->getComment());
   provenanceCommentLineEdit->home(false);

   allWidgetsGroup->blockSignals(false);
}

/**
 * save the data into the study meta data provenance.
 */
void 
StudyProvenanceWidget::slotSaveData()
{
}

/**
 * called when delete this provenance widget button is pressed.
 */
void 
StudyProvenanceWidget::slotDeleteThisProvenancePushButton()
{
   if (QMessageBox::question(this, "Confirm",
                               "Delete this Provenance", 
                               (QMessageBox::Yes | QMessageBox::No),
                               QMessageBox::Yes)
                                  == QMessageBox::Yes) {
      //
      // Remove this widget from its parent layout
      //
      parentStudyWidget->removeProvenanceWidget(this);
      
      //
      // Tell QT to delete this widget when it gets pack to its event loop
      //
      this->deleteLater();
   }
}
      
/**
 * called when name changed.
 */
void 
StudyProvenanceWidget::slotNameLineEditChanged()
{
   provenance->setName(provenanceNameLineEdit->text());
}

/**
 * called when date changed.
 */
void 
StudyProvenanceWidget::slotDateLineEditChanged()
{
   provenance->setDate(provenanceDateLineEdit->text());
}

/**
 * called when comment changed.
 */
void 
StudyProvenanceWidget::slotCommentLineEditChanged()
{
   provenance->setComment(provenanceCommentLineEdit->text());
}

//=====================================================================================
//
// Widget for displaying a new study
//
//=====================================================================================

/**
 * constructor.
 */
GuiStudyMetaDataNewDialog::GuiStudyMetaDataNewDialog(StudyMetaData* currentStudyMetaDataIn,
                                                     QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("New Study");
   
   currentStudyMetaData = currentStudyMetaDataIn;
   
   //
   // Validator for PubMed ID which is an optional minus sign followed by digits
   // Also allow it to be a project ID which begins with "ProjID"
   //
   QRegExpValidator* pubMedIDValidator = new QRegExpValidator(
                        QRegExp("[P|0-9][r|0-9][o|0-9][j|0-9][I|0-9][D|0-9]\\d*"),
                                                              this);
                                                              
   QLabel* idMessageLabel = new QLabel("If you have a PubMed ID for the new study, "
                                       "enter it in the box below.  Otherwise, leave "
                                       "the box empty.");
   idMessageLabel->setWordWrap(true);
   
   newStudyPubMedIDLineEdit = new QLineEdit;
   newStudyPubMedIDLineEdit->setValidator(pubMedIDValidator);
   
   QGroupBox* studyGroupBox = new QGroupBox("PubMed ID");
   QVBoxLayout* studyGroupLayout = new QVBoxLayout(studyGroupBox);
   studyGroupLayout->addWidget(idMessageLabel);
   studyGroupLayout->addWidget(newStudyPubMedIDLineEdit);
   
   newEmptyStudyRadioButton = new QRadioButton("New Study (empty)");
   newCopyStudyRadioButton  = new QRadioButton("New Study (copy current study)");
   if (currentStudyMetaData == NULL) {
      newCopyStudyRadioButton->setEnabled(false);
   }
   
   newStudyButtonGroup = new QButtonGroup(this);
   newStudyButtonGroup->addButton(newEmptyStudyRadioButton, 0);
   newStudyButtonGroup->addButton(newCopyStudyRadioButton, 1);
   
   QWidget* newButtonWidget = new QWidget;
   QVBoxLayout* newButtonLayout = new QVBoxLayout(newButtonWidget);
   newButtonLayout->addWidget(newEmptyStudyRadioButton);
   newButtonLayout->addWidget(newCopyStudyRadioButton);
   
   // hide new/copy radio buttons
   newButtonWidget->hide();
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(studyGroupBox);
   layout->addWidget(newButtonWidget);
   
   newEmptyStudyRadioButton->setChecked(true);
   if (lastCheckedID == 0) {
      newEmptyStudyRadioButton->setChecked(true);
   }
   else if (lastCheckedID == 1) {
      if (newCopyStudyRadioButton->isEnabled()) {
         newCopyStudyRadioButton->setChecked(true);
      }
   }
   
   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok 
                                                      | QDialogButtonBox::Cancel);
   layout->addWidget(buttonBox);
   QObject::connect(buttonBox, SIGNAL(accepted()),
                    this, SLOT(accept()));
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(reject()));
}
      

/**
 * destructor.
 */
GuiStudyMetaDataNewDialog::~GuiStudyMetaDataNewDialog()
{
}

/**
 * get the PubMed ID.
 */
QString 
GuiStudyMetaDataNewDialog::getPubMedID() const 
{ 
   return newStudyPubMedIDLineEdit->text().trimmed(); 
}

/**
 * called when OK/Cancel button pressed.
 */
void 
GuiStudyMetaDataNewDialog::done(int r)
{
   if (r == WuQDialog::Accepted) {
      const QString pmid = getPubMedID();
      if (pmid.isEmpty() == false) {
         const StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
         const int studyIndex = smdf->getStudyIndexFromPubMedID(pmid);
         if (studyIndex >= 0) {
            const QString msg("ERROR: The PubMed ID you entered ("
                              + pmid
                              + ") is in use by study " 
                              + QString::number(studyIndex + 1)
                              + ".  Each study must have "
                              + "a unique PubMed ID.");
            QMessageBox::critical(this, "ERROR", msg);
            return;
         }
      }
      
      lastCheckedID = newStudyButtonGroup->checkedId();
      
      const bool anyButtonChecked = 
         newEmptyStudyRadioButton->isChecked() ||
         (newCopyStudyRadioButton->isChecked() &&
          newCopyStudyRadioButton->isEnabled());
         
      if (anyButtonChecked == false) {
         QMessageBox::critical(this, "ERROR", "No study type selected.");
         return;
      }
   }
   
   WuQDialog::done(r);
}
      
/**
 * get the new study.
 */
StudyMetaData* 
GuiStudyMetaDataNewDialog::getNewStudy() const
{
   //StudyMetaDataFile* smdf = theMainWindow->getBrainSet()->getStudyMetaDataFile();
   StudyMetaData* smd = NULL;
   if (newEmptyStudyRadioButton->isChecked()) {
      smd = new StudyMetaData;
   }
   else if (newCopyStudyRadioButton->isChecked()) {
      smd = new StudyMetaData;
      if (currentStudyMetaData != NULL) {
         const QString projID = smd->getProjectID();
         const QString pubMedID = smd->getPubMedID();
         *smd = *currentStudyMetaData;
         smd->setProjectID(projID);
         smd->setPubMedID(pubMedID);
      }
   }
   
   const QString pmid = getPubMedID();
   if (pmid.isEmpty() == false) {
      smd->setPubMedID(pmid);
   }
   
   return smd;
}      
