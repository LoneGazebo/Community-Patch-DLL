-- Paper
-- Author: Jeff - Thanks to Kyoss (Tutorial)
-- DateCreated: 1/6/2014 8:46:59 PM
--------------------------------------------------------------
-- Added by Kyoss
if GameInfo.Resources[ResourceType].IconAtlas ~= "RESOURCE_ATLAS" then 
-- means if the Resource is NOT using the default IconAtlas, then
   --get right Texture Atlas
   local customAtlas = false;
   for iTA in GameInfo.IconTextureAtlases() do
     --do this for each IconAtlas:
       local iconAtlasRef, iconAtlasIconSize = iTA.Atlas, iTA.IconSize;
       if iconAtlasRef == GameInfo.Resources[ResourceType].IconAtlas and iconAtlasIconSize == 64 then
       -- if this Atlas is the Atlas we specified for the current Resource and has a width of 64 (WorldMap icons are 64x64px). then
            customAtlas = iTA.Filename;
            -- YAY, we found the Atlas for our custom resource
            instance.ResourceIcon:SetTexture(customAtlas);
            -- set the background Image of the IconResource to the IconAtlas we found
            break;
       end
   end
 end
 -- End: Added by Kyoss