#!/bin/bash
set -o errexit

c_file=$1
file_path="${c_file%.*}"

echo "Compiling code..."
avr-gcc -mmcu=atmega88 -Os $c_file -o $file_path.elf || {
    echo "Compilation failed"; exit 1;
}

echo "Uploading..."
avrdude -p m88p -P usb -c usbasp -Uflash:w:$file_path.elf || {
    echo "Programming failed"; exit 1;
}
