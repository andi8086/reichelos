#/bin/sh
qemu-system-i386 -s -S -kernel init.elf &
sleep 1
gdb -q -iex "add-auto-load-safe-path ." init.elf
