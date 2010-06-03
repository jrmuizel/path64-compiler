#!/bin/sh
LIB_DIR=/opt/path64/lib/3.9.99

# Apache stdcxx - Do we at some point want to switch to LLVM libc++
cd ..
# Currently svn is broked
wget http://www.apache.org/dist/stdcxx/stdcxx-4.2.1.tar.gz
tar -xf stdcxx-4.2.1.tar.gz
cd stdcxx-4.2.1
# svn co http://svn.apache.org/repos/asf/stdcxx/trunk stdcxx
# cd stdcxx/
make BUILDDIR=build-12D BUILDTYPE=12D CONFIG=gcc.config config -j20
cd build-12D
make lib -j20
make locales -j20
cp lib/libstd12D.so $LIB_DIR/libstdcxx.so
mkdir /opt/path64/include
mkdir /opt/path64/include/3.9.99
mkdir /opt/path64/include/3.9.99/stdcxx
cp -r ../include/* /opt/path64/include/3.9.99/stdcxx


# libunwind
# Could also pull from git clone git://git.sv.gnu.org/libunwind.git
cd ../..
wget http://ftp.twaren.net/Unix/NonGNU/libunwind/libunwind-0.99-beta.tar.gz
tar -xf libunwind-0.99-beta.tar.gz
mkdir libunwind-0.99-beta/build
cd libunwind-0.99-beta/build
../configure --prefix=/opt/path64 --enable-cxx-exceptions
make -j20
cp src/.libs/libunwind*.so $LIB_DIR

# compiler-rt (aka libgcc)
cd ../..
svn co http://llvm.org/svn/llvm-project/compiler-rt/trunk compiler-rt
mkdir compiler-rt/build
cd compiler-rt
# may need to patch this crap
patch -p1 < /mnt/psc/src/patches/compiler-rt-x86_64.diff
cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt/path64 -DCMAKE_BUILD_TYPE=Release ..
make -j20
cp lib/libCompilerRT-x86_64.a $LIB_DIR/libgcc.a
