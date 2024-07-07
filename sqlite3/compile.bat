@echo off

call vcvars64
cl ^
	/O2 /arch:AVX512 ^
  /I. /Fe:sqlite3.exe shell.c sqlite3.c
del /f shell.obj sqlite3.obj
