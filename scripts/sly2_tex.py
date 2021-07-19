import sys
import struct
from time import sleep
from PIL import Image
from pathlib import Path

sys.path.append('../')

def unpack(_bytes):
    STRUCT_SIGNS = {
    1 : 'B',
    2 : 'H',
    4 : 'I',
    8 : 'Q'
    }
    return struct.unpack('<' + STRUCT_SIGNS[len(_bytes)], _bytes)[0]

tex_path = sys.argv[1]

with open(tex_path, 'rb') as file:
    width = 128
    height = 118

    image = Image.new('P', (width, height))

    p_start = 0
    p_size = 0x280

    palette_data = file.read(p_size - p_start)
    palette = []
    for i in range(0, len(palette_data), 4):
        r = palette_data[i + 0]
        g = palette_data[i + 1]
        b = palette_data[i + 2]
        palette.append(r)
        palette.append(g)
        palette.append(b)
        print("i: {:02} (0x{:02X}) R: {:02X} G: {:02X} B: {:02X}".format(i//4, i//4, r, g, b))

        if len(palette) == 768:
            break

    image.putpalette(palette, rawmode='RGB')

    file.seek(p_size)

    data = file.read(width*height)
    image = Image.frombytes('P', (width, height), data, 'raw', 'RGB')
    image.save("test2.png")
