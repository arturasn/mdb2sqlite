cd "%CD%"
cd..
devenv "%CD%\wxwidgets\build\msw\wx_vc10.sln" /build Release
devenv "%CD%\bin\mdb2sqlite.sln" /rebuild Debug