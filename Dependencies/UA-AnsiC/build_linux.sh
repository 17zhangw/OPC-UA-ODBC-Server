#! /bin/bash
set -e
make -f linux_gcc.mak BUILD_TARGET=debug "$@"
rm -rf $(pwd)/Headers
rm -rf /usr/include/opcua.h
rm -rf /usr/include/opcua_*
mkdir $(pwd)/Headers
cp Stack/core/*.h $(pwd)/Headers
cp Stack/proxystub/clientproxy/*.h $(pwd)/Headers
cp Stack/proxystub/serverstub/*.h $(pwd)/Headers
cp Stack/securechannel/*.h $(pwd)/Headers
cp Stack/stackcore/*.h $(pwd)/Headers
cp Stack/transport/https/*.h $(pwd)/Headers
cp Stack/transport/tcp/*.h $(pwd)/Headers
cp Stack/platforms/linux/*.h $(pwd)/Headers
#chmod -R 777 Headers
#ln -s $(pwd)/Headers/* /usr/include/
rm -rf /usr/lib64/libuastack.a
#ln -s $(pwd)/lib/linux/x86_64/gcc/debug/libuastack.a /usr/lib64/libuastack.a
cp $(pwd)/Headers/* /usr/include/
cp $(pwd)/lib/linux/x86_64/gcc/debug/libuastack.a /usr/lib64/libuastack.a
chmod -R 644 /usr/include/opcua*
