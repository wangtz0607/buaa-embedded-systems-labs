# buaa-embedded-systems-labs

## Lab 0

### Troubleshooting

Problem:

```plaintext
arm-linux-gnueabihf-gcc: cannot execute: required file not found
```

Solution:

```sh
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install libc6:i386
```

Problem:

```plaintext
 *** Unable to find the ncurses libraries or the
 *** required header files.
 *** 'make menuconfig' requires the ncurses libraries.
 *** 
 *** Install ncurses (ncurses-devel) and try again.
 *** 
```

Solution:

```sh
sudo apt-get install libncurses-dev
```

Problem:

```plaintext
Your display is too small to run Menuconfig!
It must be at least 19 lines by 80 columns.
```

Solution: Enlarge the terminal window.

Problem:

```plaintext
/usr/bin/ld: scripts/dtc/dtc-parser.tab.o:(.bss+0x50): multiple definition of `yylloc'; scripts/dtc/dtc-lexer.lex.o:(.bss+0x0): first defined here
```

Solution:

```patch
--- scripts/dtc/dtc-lexer.l.bak
+++ scripts/dtc/dtc-lexer.l
@@ -39,7 +39,7 @@
 #include "srcpos.h"
 #include "dtc-parser.tab.h"
 
-YYLTYPE yylloc;
+extern YYLTYPE yylloc;
 
 /* CAUTION: this will stop working if we ever use yyless() or yyunput() */
 #define	YY_USER_ACTION \
```

```patch
--- scripts/dtc/dtc-lexer.lex.c_shipped.bak
+++ scripts/dtc/dtc-lexer.lex.c_shipped
@@ -637,7 +637,7 @@
 #include "srcpos.h"
 #include "dtc-parser.tab.h"
 
-YYLTYPE yylloc;
+extern YYLTYPE yylloc;
 
 /* CAUTION: this will stop working if we ever use yyless() or yyunput() */
 #define	YY_USER_ACTION \
```

Problem:

```plaintext
arm-linux-gnueabihf/bin/as: error while loading shared libraries: libz.so.1: cannot open shared object file: No such file or directory
```

Solution:

```sh
sudo apt-get install lib32z1
```

## Lab 1

```sh
arm-linux-gnueabihf-gcc main.c sort.s
```

Example input:

```plaintext
5 2 4 6 1 3 -1
```

Example output:

```plaintext
5 2 4 6 1 3
6 5 4 3 2 1
```

### Troubleshooting

Problem:

```plaintext
unknown filesystem type 'ntfs'
```

```plaintext
unknown filesystem type 'exfat'
```

Solution: Format the USB drive as ext4.

Problem:

```plaintext
version `GLIBC_2.39' not found
```

Solution: Link the binary statically:

```patch
-arm-linux-gnueabihf-gcc main.c sort.s
+arm-linux-gnueabihf-gcc -static main.c sort.s
```
