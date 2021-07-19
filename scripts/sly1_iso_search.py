# Author: https://github.com/VelocityRa

import os
from pathlib import Path

# from mmap import ACCESS_READ, mmap
# from DXTDecompress import DXTBuffer
# from PIL import Image
# import array

iso_path = Path("D:\\Roms\\PS2\\Games\\Sly\\SLY.mdf")

SECTOR_SIZE = 0x800

# data = np.fromfile(iso_path, 'uint8')

ff_byte_indices = []

with open(iso_path, "rb") as iso_f:
    filesize = os.path.getsize(iso_path)

    # data = array.array('B')
    # data.fromfile(iso_f, filesize // data.itemsize)

    for i in range(0, filesize, SECTOR_SIZE):
        iso_f.seek(i)

        if iso_f.read(1)[0] == 0xFF:
            iso_f.seek(i)
            data_preview = iso_f.read(16)
            data_preview_str = ''.join('%02x' % i for i in data_preview)
            print("0xFF at sector {} (byte 0x{:X})  {}".format(
                i // SECTOR_SIZE, i, data_preview_str))

            ff_byte_indices.append(i)

    ff_byte_indices_sec = []
    for i in ff_byte_indices:
        ff_byte_indices_sec.append(i // SECTOR_SIZE)

    ff_byte_indices_abs = []
    for i in ff_byte_indices:
        ff_byte_indices_abs.append(i | 0x20000000)

    ff_byte_indices.append(ff_byte_indices_abs)

    for j in range(0, filesize, 4):
        iso_f.seek(j)

        word = int.from_bytes(iso_f.read(4), byteorder='little')

        if word in ff_byte_indices:
            print("\tFound1 seg for offset {:08X} at 0x{:08X}".format(i, j))

        if word in ff_byte_indices_sec:
            print("\tFound2 seg for offset {:08X} at 0x{:08X}".format(i, j))
