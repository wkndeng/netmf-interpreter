@rem msbuild.exe dotNetMF.proj /v:d /t:clean /p:flavor=release;memory=flash;platform=NUCLEOL476RG 2>&1 | tee build_clean_release.txt
msbuild.exe dotNetMF.proj /t:clean /p:flavor=release;memory=flash;platform=NUCLEOL476RG
