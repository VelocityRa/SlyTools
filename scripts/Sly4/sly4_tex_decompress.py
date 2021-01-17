import sys
import os
import re
import struct
from time import sleep
from PIL import Image
from pathlib import Path

sys.path.append('../')
from DXTDecompress import DXTBuffer

tex_path_src = sys.argv[1]
tex_path_dst = sys.argv[2]

def gamma_correct(value):
    return value # disable
    value = int(pow(value / 255, 0.88) * 255)
    if value >= 255:
        value = 255
    return value

def decode_raw(path_src, path_dst, width, height):
    print("Decoding RAW at {}".format(path_src))

    with open(path_src, 'rb') as file:
        buf = bytearray(file.read())

        for i in range(0, len(buf), 4):
            buf[i+0] = gamma_correct(buf[i+1])
            buf[i+1] = gamma_correct(buf[i+2])
            buf[i+2] = gamma_correct(buf[i+3])
            buf[i+3] = 0xFF #buf[i+0]

        new_image = Image.frombuffer('RGBA', (width, height), buf, 'raw', 'RGBA', 0, 1)
        new_image.save(path_dst)

def decode_dxt(path_src, path_dst, width, height, is_ver_1):
    print("Decoding DXT at {}".format(path_src))

    with open(path_src, 'rb') as file:
        _buffer = DXTBuffer(width, height)  # Width and height of the image

        # Detect the compression type
        if is_ver_1:
            buf = bytearray(_buffer.DXT1Decompress(file))
        else:
            buf = bytearray(_buffer.DXT5Decompress(file))

        for i in range(0, len(buf), 4):
            buf[i+0] = gamma_correct(buf[i+0])
            buf[i+1] = gamma_correct(buf[i+1])
            buf[i+2] = gamma_correct(buf[i+2])
            buf[i+3] = 0xFF #_buffer[i+3]

        new_image = Image.frombuffer('RGBA', (width, height), buf, 'raw', 'RGBA', 0, 1)
        new_image.save(path_dst)

tex_dir = 'C:/Code/cpp/rpcs3/bin/'

p = re.compile('rpcs3_0x(.+?)_(.+?)x(.+?)_(.+?)\.(.+)')
for filename in os.listdir(tex_dir):
    m = p.match(filename)

    if m:
        addr = m.group(1)
        w = int(m.group(2))
        h = int(m.group(3))
        fmt = m.group(4)
        ext = m.group(5)

        if ext != 'tex':
            continue

        path_src = tex_dir + filename
        path_dst = tex_dir + filename[:-3] + "png"

        if fmt == 'A8R8G8B8':
            decode_raw(path_src, path_dst, w, h)
        elif fmt == 'COMPRESSED_DXT1' or fmt == 'COMPRESSED_DXT45':
            try:
                is_ver_1 = (fmt == 'COMPRESSED_DXT1')
                decode_dxt(path_src, path_dst, w, h, is_ver_1)
            except IndexError:
                print("IndexError")
            except ValueError:
                print("ValueError")