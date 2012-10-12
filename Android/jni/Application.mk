#
#By default, the NDK build system will generate machine code for the
#'armeabi' ABI. This corresponds to an ARMv5TE based CPU with software
#floating point operations. You can use APP_ABI to select a different
#ABI.
#Or even better, since NDK r7, you can also use the special value
#'all' which means "all ABIs supported by this NDK release":
#
#    APP_ABI := all
#
#For the list of all supported ABIs and details about their usage and
#limitations, please read docs/CPU-ARCH-ABIS.html	
#
APP_ABI := armeabi armeabi-v7a

#
#By default, the NDK build system provides C++ headers for the minimal
#C++ runtime library (/system/lib/libstdc++.so) provided by the Android
#system.
#
#However, the NDK comes with alternative C++ implementations that you can
#use or link to in your own applications. Define APP_STL to select one of
#them. Examples are:
#
#   APP_STL := stlport_static    --> static STLport library
#   APP_STL := stlport_shared    --> shared STLport library
#   APP_STL := system            --> default C++ runtime library
#
#For more information on the subject, please read docs/CPLUSPLUS-SUPPORT.html
#
APP_STL := stlport_static

#
#This optional variable can be defined to either 'release' or
#'debug'. This is used to alter the optimization level when
#building your application's modules.
#
#A 'release' mode is the default, and will generate highly
#optimized binaries. The 'debug' mode will generate un-optimized
#binaries which are much easier to debug.
#
#Note that if your application is debuggable (i.e. if your manifest
#sets the android:debuggable attribute to "true" in its <application>
#tag), the default will be 'debug' instead of 'release'. This can
#be overridden by setting APP_OPTIM to 'release'.
#
APP_OPTIM := release
