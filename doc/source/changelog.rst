.. _changelog:

Changelog
=========

V0.2.3
------

This is release ``0.2.3`` of Nuclei SDK.

* OS

   - Add **RT-Thread 3.1.3** as a new RTOS service of Nuclei SDK, the kernel source
     code is from RT-Thread Nano project.
   - Update UCOSII source code from version ``V2.91`` to ``V2.93``
   - The source code of UCOSII is fetched from https://github.com/SiliconLabs/uC-OS2/
   - **Warning**: Now for UCOSII application development, the ``app_cfg.h``, ``os_cfg.h``
     and ``app_hooks.c`` are required, which can be also found in
     https://github.com/SiliconLabs/uC-OS2/tree/master/Cfg/Template

* Application

   - Add **RT-Thread** demo application.
   - Don't use the ``get_cpu_freq`` function in application code, which currently is only
     for internal usage, and not all SoC implementations are required to provide this function.
   - Use ``SystemCoreClock`` to get the CPU frequency instead of using ``get_cpu_freq()`` in
     ``whetstone`` application.
   - Update UCOSII applications due to UCOSII version upgrade, and application development
     for UCOSII also required little changes, please refer to :ref:`design_rtos_ucosii`
   - Fix ``time_in_secs`` function error in ``coremark``, and cleanup ``coremark`` application.

* Documentation

   - Add documentation about RT-Thread and its application development.
   - Update documentation about UCOSII and its application development.
   - Update ``coremark`` application documentation.

* Build System

   - Add build system support for RT-Thread support.
   - Build system is updated due to UCOSII version upgrade, the ``OS/UCOSII/cfg`` folder
     no longer existed, so no need to include it.

* SoC

    - Update SoC startup and linkscript files to support RT-Thread

* Misc

    - Add ``SConscript`` file in Nuclei SDK root, this file is used by RT-Thread package.

V0.2.2
------

This is release ``0.2.2`` of Nuclei SDK.

* OS

  - Update UCOSII portable code
  - Now both FreeRTOS and UCOSII are using similar portable code,
    which both use ``SysTimer Interrupt`` and ``SysTimer Software Interrupt``.

* Documentation

  - Update documentation about RTOS

V0.2.1
------

This is release ``0.2.1`` of Nuclei SDK.

* Build System

  - Add extra linker options ``-u _isatty -u _write -u _sbrk -u _read -u _close -u _fstat -u _lseek``
    in Makefile.conf to make sure if you pass extra ``-flto`` compile option, link phase will not fail

* Documentation

  - Add documentation about how to optimize for code size in application development, using ``demo_eclic``
    as example.

* OS

  - Update FreeRTOS to version V10.3.1
  - Update FreeRTOS portable code

* NMSIS

  - Update NMSIS to release ``v1.0.0-beta1``


V0.2.0-alpha
------------

This is release ``0.2.0-alpha`` of Nuclei SDK.

* Documentation

  - Initial verison of Nuclei SDK documentation
  - Update Nuclei-SDK README.md

* Application

  - Add ``demo_eclic`` application
  - Add ``demo_dsp`` application
  - ``timer_test`` application renamed to ``demo_timer``

* Build System

  - Add comments for build System
  - Small bug fixes

* **NMSIS**

  - Change ``NMSIS/Include`` to ``NMSIS/Core/Include``
  - Add ``NMSIS/DSP`` and ``NMSIS/NN`` header files
  - Add **NMSIS-DSP** and **NMSIS-NN** pre-built libraries


V0.1.1
------

This is release ``0.1.1`` of Nuclei SDK.

Here are the main features of this release:

* Support Windows and Linux development in command line using Make

* Support development using PlatformIO, see https://github.com/Nuclei-Software/platform-nuclei

* Support Humming Bird FPGA evaluation Board and GD32VF103 boards

  - The **Humming Bird FPGA evaluation Board** is used to run evaluation FPGA bitstream
    of Nuclei N200, N300, N600 and NX600 processor cores
  - The **GD32VF103 boards** are running using a real MCU from Gigadevice which is using
    Nuclei N200 RISC-V processor core

* Support different download modes flashxip, ilm, flash for our FPGA evaluation board


.. _Nuclei-SDK: https://github.com/Nuclei-Software/nuclei-sdk
