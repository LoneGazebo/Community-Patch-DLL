# Civilopedia Html Generator

This project exports all Civilopedia data from Civilization V: Vox Populi and generates a searchable HTML version that can be uploaded on GitHub.

## How to use

1. Make sure the version number in ExportCivilopedia/Text/en_US/UI.json matches the VP version number.
2. Copy "ExportCivilopedia" to your mods folder and start a game of Civ5 with VP and the ExportCivilopedia mod enabled. Make sure logging is turned on.
3. After clicking on "Begin your Journey", the game data will be written to the lua log. In Lua.log, "END EXPORT DATA" will be printed when the export is complete.
4. In ExportCivilopedia: run `python extract_from_log.py`. This script will export the data from Lua.log into a file "civilopedia_export.json" 
5. In ExportCivilopedia: run `pip install -r requirements.txt` and `python generate_civilopedia.py`. This script will generate the HTML files in "civilopedia_html".
6. Open `civilopedia_html/index.html` in your browser to inspect the results.

