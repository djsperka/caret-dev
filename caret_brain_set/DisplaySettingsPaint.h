#ifndef __DISPLAY_SETTINGS_PAINT_H__
#define __DISPLAY_SETTINGS_PAINT_H__

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

#include <vector>

#include "DisplaySettingsNodeAttributeFile.h"

/// DisplaySettingsPaint is a class that maintains parameters for controlling
/// the display of paint data files.
class DisplaySettingsPaint : public DisplaySettingsNodeAttributeFile {
   public:
      
      /// Constructor
      DisplaySettingsPaint(BrainSet* bs);
      
      /// Destructor
      ~DisplaySettingsPaint();
      
      /// Reinitialize all display settings
      void reset();
      
      /// Update any selections due to changes in loaded paint file
      void update();
      
      /// get the selected medial wall override column
      int getMedialWallOverrideColumn() const { return medialWallOverrideColumn; }
      
      /// set the medial wall override column
      void setMedialWallOverrideColumn(const int col) { medialWallOverrideColumn = col; }
      
      /// get the medial wall override column enabled
      bool getMedialWallOverrideColumnEnabled() const { return medialWallOverrideEnabled; }
      
      /// set the medial wall override column enabled
      void setMedialWallOverrideColumnEnabled(const bool b) { medialWallOverrideEnabled = b; }
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage);
                       
      /// get the geography blending
      float getGeographyBlending() const { return geographyBlending; }
      
      /// set the geography blending
      void setGeographyBlending(const float gb) { geographyBlending = gb; }
      
   private:
      /// geography blending
      float geographyBlending;
      
      /// selected medial wall override column
      int medialWallOverrideColumn;
      
      /// medial wall override column enabled
      bool medialWallOverrideEnabled;
};

#endif // __DISPLAY_SETTINGS_PAINT_H__


