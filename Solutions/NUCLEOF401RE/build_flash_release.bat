@rem msbuild.exe dotNetMF.proj /v:d /t:build /p:flavor=release;memory=flash;platform=NUCLEOF401RE 2>&1 | tee build_flash_release.txt
msbuild.exe dotNetMF.proj /t:build /p:flavor=release;memory=flash;platform=NUCLEOF401RE 2>&1 | tee build_flash_release.txt
