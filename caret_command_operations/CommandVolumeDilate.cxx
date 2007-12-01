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

#include "CommandVolumeDilate.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeDilate::CommandVolumeDilate()
   : CommandBase("-volume-dilate",
                 "VOLUME DILATE")
{
}

/**
 * destructor.
 */
CommandVolumeDilate::~CommandVolumeDilate()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeDilate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addInt("Iterations", 5, 0, 100000);
}

      
/**
 * get full help information.
 */
QString 
CommandVolumeDilate::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<iterations>\n"
       + indent9 + "\n"
       + indent9 + "Dilate the volume for the specified number of iterations.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeDilate::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   QString outputVolumeFileName, outputVolumeFileLabel;
   parameters->getNextParameterAsVolumeFileNameAndLabel("Output Volume File Name",
                                                        outputVolumeFileName,
                                                        outputVolumeFileLabel);
   const int numberOfIterations = 
      parameters->getNextParameterAsInt("Number of Iterations");
   checkForExcessiveParameters();
   
   //
   // Read the input volume file
   //
   VolumeFile vf;
   vf.readFile(inputVolumeFileName);
   
   //
   // Dilate the volume
   //
   vf.doVolMorphOps(numberOfIterations, 0);
   
   //
   // Write the volume file
   //
   writeVolumeFile(vf, outputVolumeFileName, outputVolumeFileLabel);
}


