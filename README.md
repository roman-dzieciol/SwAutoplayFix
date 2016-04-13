# SwAutoplayFix

This fix addresses following UnrealEd v3369 issue:

When using Mod Folder system, the "Test Level" button saves the map in UT2004\Maps folder instead of UT2004\ModName\Maps. If the mod doesn't use the UT2004 Maps folder, test map won't be found and game will crash.

This fix makes UED copy the map to the Mod's Maps folder before starting UT2004.


## INSTALLATION

 * Copy SwAutoplayFix.dll to UT2004/System
 * Copy SwAutoplayFix.u to UT2004/System
 * In your INI file, add SwAutoplayFix to EditPackages list in the [Editor.EditorEngine] section.
 * Don't forget to add -MOD=ModName to the GameCommandLine variable in the  [UnrealEd.UnrealEdEngine] section.
   
## COMPATIBILITY

 * Tested on UT2004 v3369, WinXP SP2
 * Should work with all UnrealEd 3 builds on all Windows versions.
 
