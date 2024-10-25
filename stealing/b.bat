@ECHO OFF

clang main.c --debug -DPLATFORM_WIN32 -luser32.lib -lgdi32.lib

