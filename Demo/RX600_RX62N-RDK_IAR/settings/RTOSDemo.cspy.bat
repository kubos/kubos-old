@REM This batch file has been generated by the IAR Embedded Workbench
@REM C-SPY Debugger, as an aid to preparing a command line for running
@REM the cspybat command line utility using the appropriate settings.
@REM
@REM You can launch cspybat by typing the name of this batch file followed
@REM by the name of the debug file (usually an ELF/DWARF or UBROF file).
@REM Note that this file is generated every time a new debug session
@REM is initialized, so you may want to move or rename the file before
@REM making changes.
@REM 


"C:\devtools\IAR Systems\Embedded Workbench 6.0\common\bin\cspybat" "C:\devtools\IAR Systems\Embedded Workbench 6.0\rx\bin\rxproc.dll" "C:\devtools\IAR Systems\Embedded Workbench 6.0\rx\bin\rxemue20.dll"  %1 --plugin "C:\devtools\IAR Systems\Embedded Workbench 6.0\rx\bin\rxbat.dll" --backend -B "--core" "RX600" "-p" "C:\devtools\IAR Systems\Embedded Workbench 6.0\rx\config\debugger\ior5f562n8.ddf" "--double" "32" "--endian" "l" "--int" "32" "-d" "emue20" "--emu" "jlink" "--drv_communication" "USB" "--verify_download" "all" 


