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

    p_size = 0x180
    p_start = 0

    file.seek(p_start)
    palette_data = file.read(p_size - p_start)
    palette = []
    # for i in range(0, len(palette_data), 4):
    for i in range(0, 128):
        # r = palette_data[i * 3 + 0]
        # g = palette_data[i * 3 + 1]
        # b = palette_data[i * 3 + 2]
        r = palette_data[i + 128*0]
        g = palette_data[i + 128*1]
        b = palette_data[i + 128*2]
        palette.append(r)
        palette.append(g)
        palette.append(b)
        print("i: {:02} (0x{:02X}) R: {:02X} G: {:02X} B: {:02X}".format(i, i, r, g, b))

        # if len(palette) == 768:
        #     break

# only has 0x280, not enough for RGB*0xFF

    # image.putpalette(palette, rawmode='RGB')

    file.seek(p_size)

    file_data = file.read(width*height)
    img_data = bytearray(width*height*4)
    for x in range(width):
        for y in range(height):
            i = (x + width * y)

            p_index = file_data[i]
            r = palette[p_index * 3 + 0]
            g = palette[p_index * 3 + 1]
            b = palette[p_index * 3 + 2]

            img_data[i * 4 + 0] = r
            img_data[i * 4 + 1] = g
            img_data[i * 4 + 2] = b
            img_data[i * 4 + 3] = 0xFF

    #image = Image.frombytes('P', (width, height), data, 'raw', 'P', 0, 1)
    new_image = Image.frombuffer('RGBA', (width, height), img_data, 'raw', 'RGBA', 0, 1)
    new_image.save("test2.png")
    #image.save("test2.png")
