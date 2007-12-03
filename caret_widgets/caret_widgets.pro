######################################################################
# Automatically generated by qmake (1.04a) Tue Mar 18 09:10:35 2003
######################################################################

TARGET	 = CaretWidgets
CONFIG	 += staticlib
INCLUDEPATH += . \
               ../caret_common

LIBS += -L../caret_common -lCaretCommon

include(../caret_qmake_include.pro)

win32 {
   TEMPLATE	= lib
}

macx {
   TEMPLATE	= lib
}

unix:!macx {
   TEMPLATE	= lib
}

# Input
HEADERS += \
      QtCheckBoxSelectionDialog.h \
      QtCheckBoxSelectionLineEditDialog.h \
      QtDataEntryDialog.h \
      QtDialog.h \
      QtDialogModal.h \
      QtDialogNonModal.h \
      QtDialogWizard.h \
      QtInputDialog.h \
	   QtListBoxSelectionDialog.h \
      QtMainWindow.h \
      QtMessageBox.h \
	   QtMultipleInputDialog.h \
	   QtRadioButtonSelectionDialog.h \
      QtSaveWidgetAsImagePushButton.h \
      QtTableDialog.h \
	   QtTextEditDialog.h \
      QtTextFileEditorDialog.h \
	   QtUtilities.h \
      WuQDataEntryDialog.h \
      WuQFileDialog.h \
      WuQWidgetGroup.h

SOURCES += \
      QtCheckBoxSelectionDialog.cxx \
      QtCheckBoxSelectionLineEditDialog.cxx \
      QtDataEntryDialog.cxx \
      QtDialog.cxx \
      QtDialogModal.cxx \
      QtDialogNonModal.cxx \
      QtDialogWizard.cxx \
      QtInputDialog.cxx \
	   QtListBoxSelectionDialog.cxx \
      QtMainWindow.cxx \
      QtMessageBox.cxx \
	   QtMultipleInputDialog.cxx \
	   QtRadioButtonSelectionDialog.cxx \
      QtSaveWidgetAsImagePushButton.cxx \
      QtTableDialog.cxx \
	   QtTextEditDialog.cxx \
      QtTextFileEditorDialog.cxx \
	   QtUtilities.cxx \
      WuQDataEntryDialog.cxx \
      WuQFileDialog.cxx \
      WuQWidgetGroup.cxx

