@rem msbuild.exe dotNetMF.proj /v:d /t:build /p:flavor=release;memory=flash;platform=NUCLEOL476RG 2>&1 | tee build_flash_release.txt
msbuild.exe dotNetMF.proj /t:build /p:flavor=release;memory=flash;platform=NUCLEOL476RG 2>&1 | tee build_flash_release.txt
