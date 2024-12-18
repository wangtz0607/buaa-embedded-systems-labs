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
1 2 3 4 5 6
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

## Lab 2-1

```sh
arm-linux-gnueabihf-gcc main.c
```

### Creating Virtual Serial Ports

```sh
sudo socat PTY,link=/dev/ttyV0,raw,echo=0 PTY,link=/dev/ttyV1,raw,echo=0
```

```sh
sudo stty -F /dev/ttyV0 9600
sudo stty -F /dev/ttyV1 9600
```

```sh
sudo screen /dev/ttyV0 9600
```

```sh
sudo screen /dev/ttyV1 9600
```

## Lab 2-2

```sh
arm-linux-gnueabihf-gcc main.c -lpthread
```

## Lab 3-1

### `out`

```sh
arm-linux-gnueabihf-gcc -o out out.c gpio.c
```

```sh
./out 24 26 27 37 32 29
```

### `in_out`

```sh
arm-linux-gnueabihf-gcc -o in_out in_out.c gpio.c
```

```sh
./in_out 36 24
```

### `in_out_2`

```sh
arm-linux-gnueabihf-gcc -o in_out_2 in_out_2.c gpio.c
```

```sh
./in_out_2 36 170 24 27
```

## Lab 3-2

```sh
export ARCH=arm
export CROSS_COMPILE=/path/to/arm-linux-gnueabihf-
make KDIR=/path/to/linux-3.14.52
```

```sh
insmod ./gpio_driver.ko
```

```sh
rmmod gpio_driver
```

## Lab 4


```sh
export ARCH=arm
export CROSS_COMPILE=/path/to/arm-linux-gnueabihf-
make KDIR=/path/to/linux-3.14.52
```

```sh
insmod ./zlg7290_driver.ko
```

```sh
echo "1234" > /dev/zlg7290_led
echo "abcd" > /dev/zlg7290_led
```

```sh
rmmod zlg7290_driver
```
