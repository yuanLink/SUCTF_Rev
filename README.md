# SUCTF_Rev
[Rev] WinRev

The source code for WinRev.

This Project is for SUCTF_2019.

## directory structure

```
 +-- WinRev
        /// Event Module
        AEvent.cpp
        AEvent.h
        Common.h
        gdi32.dll
        /// Main module
        main.cpp
        /// Reverse Protection, like "Process dectetion" and others
        RevProtect.cpp
        RevProtect.h
        stdafx.cpp
        stdafx.h
        targetver.h
        /// [TEST] structure for TEB
        ThreadHeader.h
        /// UnitTest for Module
        /// [TODO]:detech to another project
        UnitTestcpp.cpp
        WinRev.cpp
        WinRev.vcxproj
        WinRev.vcxproj.filters
```



