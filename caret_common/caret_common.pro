######################################################################
# Automatically generated by qmake (1.04a) Wed Jan 15 08:28:52 2003
######################################################################


TARGET	= CaretCommon
CONFIG	+= staticlib
INCLUDEPATH += .

include(../caret_qmake_include.pro)
   
TEMPLATE	= lib

# turn off unicode for displaying windows web browser
win32 {
   DEFINES	-= UNICODE
}

#macx {
#   INCLUDEPATH += /System/Library/Frameworks//ApplicationServices.framework/Versions/A/Frameworks/SpeechSynthesis.framework/Versions/A/Headers
#} 

# Input
HEADERS += Basename.h \
      CaretException.h \
      CaretLinkedList.h \
      CaretTips.h \
      Category.h \
	   CommandLineUtilities.h \
       DateAndTime.h \
	   DebugControl.h \
	   FileUtilities.h \
      GaussianComputation.h \
      HtmlColors.h \
	   HttpFileDownload.h \
	   MathUtilities.h \
      NameIndexSort.h \
      PointLocator.h \
      ProgramParameters.h \
      ProgramParametersException.h \
      StatisticsUtilities.h \
      StringTable.h \
      Species.h \
      StereotaxicSpace.h \
	   StringUtilities.h \
      Structure.h \
	   SystemUtilities.h \
      ValueIndexSort.h

SOURCES += Basename.cxx \
      CaretLinkedList.cxx \
      CaretTips.cxx \
      Category.cxx \
	   CommandLineUtilities.cxx \
      DateAndTime.cxx \
	   DebugControl.cxx \
	   FileUtilities.cxx \
      GaussianComputation.cxx \
      HtmlColors.cxx \
	   HttpFileDownload.cxx \
	   MathUtilities.cxx \
      NameIndexSort.cxx \
      PointLocator.cxx \
      ProgramParameters.cxx \
      ProgramParametersException.cxx \
      StatisticsUtilities.cxx \
      StringTable.cxx \
      Species.cxx \
      StereotaxicSpace.cxx \
	   StringUtilities.cxx \
      Structure.cxx \
	   SystemUtilities.cxx \
      ValueIndexSort.cxx

