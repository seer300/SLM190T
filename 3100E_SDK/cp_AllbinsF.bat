 copy .\Allbins\arm.img 		..\V3100EB10001R00C0002\allbins\ /B /Y
 copy .\Allbins\cp.img 		..\V3100EB10001R00C0002\allbins\ /B /Y
 copy .\Allbins\loginfo.info 	..\V3100EB10001R00C0002\allbins\ /B /Y
@REM  复制elf
 xcopy .\Allbins\xinyiNBIot_AP\elf ..\V3100EB10001R00C0002\elf\ap-elf /E /I /Y
 xcopy .\Allbins\xinyiNBIot_CP\elf ..\V3100EB10001R00C0002\elf\cp-elf /E /I /Y
 del ..\V3100EB10001R00C0002\elf\ap-elf\xinyiNBSoC.asm
 del ..\V3100EB10001R00C0002\elf\cp-elf\xinyiNBSoC.asm
   