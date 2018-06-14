@ECHO OFF

SETLOCAL

SET CONFIGURATION=Release
IF NOT [%1]==[] (SET CONFIGURATION=%1)
SET PLATFORM=Win32
IF NOT [%2]==[] (SET PLATFORM=%2)

where cmake 1>nul
IF ERRORLEVEL 1 (
	ECHO cmake could not be found
	GOTO :QUIT
)
IF ["%OSSL_LIB32_DIR%"]==[] (
	ECHO OpenSSL could not be found
	GOTO :QUIT
)
ECHO OSSL_LIB32_DIR is %OSSL_LIB32_DIR%
IF ["%VS140COMNTOOLS%"]==[] (
	ECHO VS2015 could not be found
	GOTO: QUIT
)
ECHO VS140COMNTOOLS is %VS140COMNTOOLS%

:BUILD
CALL "%VS140COMNTOOLS%VsDevCmd.bat"

IF EXIST build RMDIR build /s /q
MKDIR build\lib
CD build

SET OPENDNP3_DIR=%CD%\lib
ECHO OPENDNP3_DIR is %OPENDNP3_DIR%

cmake .. -DCMAKE_INSTALL_PREFIX=lib -DDNP3_TEST=ON -DDNP3_TLS=ON -DCMAKE_BUILD_TYPE=%CONFIGURATION% -G"Visual Studio 14 2015"
msbuild opendnp3.sln /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM%
msbuild INSTALL.vcxproj /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM%
msbuild ..\dotnet\bindings.sln /target:Clean
msbuild ..\dotnet\bindings.sln /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM%

:QUIT
ENDLOCAL