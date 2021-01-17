# Author: https://github.com/VelocityRa

import sys
import os
from pathlib import Path
from mmap import ACCESS_READ, mmap
from DXTDecompress import DXTBuffer
from PIL import Image

if len(sys.argv) < 2:
    print("wrong args")

cooked_path = sys.argv[1]
print("Cooked path: {}".format(cooked_path))

if len(sys.argv) == 3:
    out_dir = sys.argv[2]
else:
    out_dir = os.path.split(cooked_path)[0] + "/extracted"
    out_dir_p = Path(out_dir)
    out_dir_p.mkdir(exist_ok=True)

meshes_dir = Path(out_dir + "/meshes")
meshes_dir.mkdir(exist_ok=True)
textures_dir = Path(out_dir + "/textures")
textures_dir.mkdir(exist_ok=True)

DUMP_MESHES = True
DUMP_TEXTURES = True

def check_magic(data, offset, magic):
    return data[offset + 0] == ord(magic[0]) and \
           data[offset + 1] == ord(magic[1]) and \
           data[offset + 2] == ord(magic[2]) and \
           data[offset + 3] == ord(magic[3])

def read_16_be(data, offset):
    return data[offset + 1] << 0 | data[offset + 0] << 8

def read_16(data, offset):
    return data[offset + 0] << 0 | data[offset + 1] << 8
def read_32(data, offset):
    return data[offset + 0] << 0 | data[offset + 1] << 8 | data[offset + 2] << 16 | data[offset + 3] << 24


def dump_meshes(cooked_data):
    cooked_len = len(cooked_data)

    mesh_count = 0
    i = 0

    while i < cooked_len:
        if check_magic(cooked_data, i, "MESH"):
            # print("MESH at {:08X}".format(i))
            mesh_len = read_32(cooked_data, i + 4) + 4 # 4 is for 'MESH' tag

            # Determine if it's Type 2
            is_type_2 = False
            for j in range(i, i + mesh_len, 4):
                if check_magic(cooked_data, j, "MEDG"):
                    is_type_2 = True
                    break

            out_filename = "{:08X}.{}.mesh".format(i, "2" if is_type_2  else "1")

            with open(meshes_dir.joinpath(out_filename), "wb") as mesh_file:
                mesh_file.write(cooked_data[i:i+mesh_len])
                mesh_count += 1

        i += 4
    print("Dumped {} meshes.".format(mesh_count))

def dump_textures(cooked_data):
    cooked_len = len(cooked_data)

    texture_count = 0
    i = 0

    while i < cooked_len:
        if check_magic(cooked_data, i, "TEXR"):
            texture_len = read_32(cooked_data, i + 4) + 4 # 4 is for 'TEXR' tag

            name = cooked_data[i + 0x1C : i + 0x44].decode('utf-8').rstrip('\x00')[:-4]

            width = read_16_be(cooked_data, i + 0x6C)
            height = read_16_be(cooked_data, i + 0x6E)

            out_compr_filename = "{}.gxt".format(name)
            out_uncompr_filename = "{}.png".format(name)

            # 4 is for 'TGTF' tag
            texture_data_len = read_32(cooked_data, i + 0x48) - 0x88

            print("texture {} ({}x{}) at 0x{:08X} of size 0x{:X}".format(name, width, height, i, texture_data_len))

            # Dump compressed texture
            with open(textures_dir.joinpath(out_compr_filename), "w+b") as compr_tex_file:
                compr_tex_file.write(cooked_data[i : i+texture_len])

                with open(textures_dir.joinpath(out_uncompr_filename), "wb") as uncompr_tex_file:
                    # Decompress texture
                    _buffer = DXTBuffer(width, height)

                    compr_tex_file.seek(0x64)
                    format_ = compr_tex_file.read(1)[0]
                    # assert(format_ == 0x86 or format_ == 0x88)
                    if format_ != 0x86 and format_ != 0x88:
                        print("format: {:X}\n".format(format_))

                    is_format_dxt_1 = (format_ == 0x86)

                    compr_tex_file.seek(0xCC)
                    try:
                        if is_format_dxt_1:
                            _buffer = _buffer.DXT1Decompress(compr_tex_file)
                        else:
                            _buffer = _buffer.DXT5Decompress(compr_tex_file)

                        # Dump decompressed texture
                        new_image = Image.frombuffer('RGBA', (width, height), _buffer, 'raw', 'RGBA', 0, 1)
                        new_image.save(uncompr_tex_file)
                    except IndexError:
                        print("IndexError")
                    except ValueError:
                        print("ValueError")
                    texture_count += 1

            i += texture_len
            continue

        i += 4

    print("Dumped {} textures.".format(texture_count))

with open(cooked_path, 'rb') as cooked_file, mmap(cooked_file.fileno(), 0, access=ACCESS_READ) as cooked_data:
    if DUMP_MESHES:
        dump_meshes(cooked_data)

    if DUMP_TEXTURES:
        dump_textures(cooked_data)

    print("Done.")
