@ECHO OFF


set LIBS=-lkernel32.lib -luser32.lib -ldxgi.lib -ldxguid.lib -lole32 -lavrt

PUSHD bin
clang --debug ../src/main.c -I../inc -l../lib/*.lib %LIBS% -Xlinker /IGNORE:4099

POPD

