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

#include "CommandVolumeMaskVolume.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeMaskVolume::CommandVolumeMaskVolume()
   : CommandBase("-volume-mask-volume",
                 "VOLUME MASK VOLUME")
{
}

/**
 * destructor.
 */
CommandVolumeMaskVolume::~CommandVolumeMaskVolume()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeMaskVolume::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addInt("i-minimum");
   paramsOut.addInt("i-maximum");
   paramsOut.addInt("j-minimum");
   paramsOut.addInt("j-maximum");
   paramsOut.addInt("k-minimum");
   paramsOut.addInt("k-maximum");
}

/**
 * get full help information.
 */
QString 
CommandVolumeMaskVolume::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<i-minimum>\n"
       + indent9 + "<i-maximum>\n"
       + indent9 + "<j-minimum>\n"
       + indent9 + "<j-maximum>\n"
       + indent9 + "<k-minimum>\n"
       + indent9 + "<k-maximum>\n"
       + indent9 + "\n"
       + indent9 + "Clear all of the voxels outside the specified extent.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeMaskVolume::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");
   QString outputVolumeFileName, outputVolumeFileLabel;
   parameters->getNextParameterAsVolumeFileNameAndLabel("Output Volume File Name/Label",
                                                        outputVolumeFileName, 
                                                        outputVolumeFileLabel);
   const int extent[6] = {
      parameters->getNextParameterAsInt("i-minimum"),
      parameters->getNextParameterAsInt("i-maximum"),
      parameters->getNextParameterAsInt("j-minimum"),
      parameters->getNextParameterAsInt("j-maximum"),
      parameters->getNextParameterAsInt("k-minimum"),
      parameters->getNextParameterAsInt("k-maximum")
   };
   checkForExcessiveParameters();

   //
   // Read the input file
   //
   VolumeFile volume;
   volume.readFile(inputVolumeFileName);
   
   //
   // make the volume
   //
   volume.maskVolume(extent);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

