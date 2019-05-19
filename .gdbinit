set disassembly-flavor intel

set auto-load safe-path ~/reichelos
target remote localhost:1234
layout split
break startup
c
stepi
