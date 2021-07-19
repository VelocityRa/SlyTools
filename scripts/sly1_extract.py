# Author: https://github.com/VelocityRa

import sys
import os
from pathlib import Path
# from mmap import ACCESS_READ, mmap
# from DXTDecompress import DXTBuffer
# from PIL import Image
import struct
import io

# Config
EXTRACT_WAC_CONTENTS = True
PRINT_WAC_ENTRIES = False
WAC_NAME = "SLY.WAC"
#

if len(sys.argv) < 1:
    print("Enter the dir to SLY.WAC/WAL as command-line argument")

base_path = Path(sys.argv[1])

wal_path = base_path / "SLY.WAL"
wac_path = base_path / WAC_NAME
out_dir_path = base_path / "{}_contents".format(WAC_NAME)

out_dir_path.mkdir(exist_ok=True)

class WACEntry:
    def __init__(self, name, type_, offset, size):
        self.name = name
        self.type_ = type_
        self.offset = offset
        self.size = size

    def __str__(self):
        return '{:08X} {:08X} {:08X} {} {}'.format(self.offset, self.offset*2048, self.size, self.type_, self.name)


wac_entries = []

with open(wac_path, 'rb') as wac_f:
    wac_entry_count = struct.unpack("i", wac_f.read(4))[0]

    # File has 2 extra bytes at the end for some reason
    for i in range(wac_entry_count):
        name, type_, offset, size = struct.unpack("23sBii", wac_f.read(0x20))

        # Strip out null chars at the end - nothing else worked
        name_stripped = ""
        for n in name:
            if n == 0:
                break
            name_stripped += chr(n)

        wac_entries.append(WACEntry(name_stripped, chr(type_), offset, size))

def print_wac_entries():
    print("Offset(sector) Offset(bytes) Size Type Name")
    for wac_entry in wac_entries:
        print(wac_entry)

if PRINT_WAC_ENTRIES:
    print_wac_entries()

SECTOR_SIZE = 2048

def lookup_sector(sector_to_find):
    for wac_entry in wac_entries:
        start = wac_entry.offset
        end = wac_entry.offset + (wac_entry.size // SECTOR_SIZE)

        if start <= sector_to_find < end:
            print("Found sector 0x{:X} in [{}], offset 0x{:X} sectors".format(sector_to_find, wac_entry,
                                                                              sector_to_find - wac_entry.offset))

def lookup_byte(byte_to_find):
    for wac_entry in wac_entries:
        start = wac_entry.offset
        end = (wac_entry.offset * SECTOR_SIZE) + wac_entry.size

        if start <= byte_to_find < end:
            print("Found byte 0x{:X} at [{}], offset 0x{:X} bytes".format(byte_to_find, wac_entry,
                                                                          byte_to_find - wac_entry.offset * SECTOR_SIZE))

# lookup_sector(820372 - 0x3AFCE)

def extract_wac_contents():
    # with open(wal_path, 'rb') as wal_f, mmap(wal_f.fileno(), 0, access=ACCESS_READ) as wal_data:
    with open(wal_path, 'rb') as wal_f:
        for wac_entry in wac_entries:
            wac_offset = wac_entry.offset * SECTOR_SIZE

            extension = "bin"

            wal_f.seek(wac_offset)

            if wal_f.read(4) == b"VAGp":
                extension = "vag"
            else:
                wal_f.seek(wac_offset + 0x20)

                if wal_f.read(4) == b"klBS":
                    extension = "bnk"

            filename = "{}_{}_0x{:X}_0x{:X}.{}".format(wac_entry.name, wac_entry.type_, wac_entry.offset, wac_entry.size, extension)
            out_dir_path_final = out_dir_path / filename # TODO

            with open(out_dir_path_final, 'wb') as out_f:
                wal_f.seek(wac_offset)
                out_f.write(wal_f.read(wac_entry.size))

if EXTRACT_WAC_CONTENTS:
    extract_wac_contents()
