#!/bin/bash
echo "Hello World"

# 1. Move "LUA" folder from "Community Patch" to main folder "CP LUA"
# 2. Move "LUA" folder from "Community Balance Patch" to main folder "CBO LUA".
# 3. Modify "CPPSetupData.iss" to reflect correct folder naming structure.
# 4. Test the installer locally.
# 5. Push changes to "github actions".
# 6. What about "UI_bc1" folder?


# 1. EUI license says we are not allowed to host it's files anywhere, so download it from official website.
curl https://forums.civfanatics.com/resources/24303/download?version=22637 --output ./eui.zip
unzip ./eui.zip -d ./UI_bc1
