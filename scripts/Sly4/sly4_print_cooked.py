import sys
import os
import struct
import string
from pathlib import Path
from mmap import ACCESS_READ, mmap

def unpack(_bytes):
    STRUCT_SIGNS = {
    1 : 'B',
    2 : 'H',
    4 : 'I',
    8 : 'Q'
    }
    return struct.unpack('<' + STRUCT_SIGNS[len(_bytes)], _bytes)[0]

def unpack_be(_bytes):
    STRUCT_SIGNS = {
    1 : 'B',
    2 : 'H',
    4 : 'I',
    8 : 'Q'
    }
    return struct.unpack('>' + STRUCT_SIGNS[len(_bytes)], _bytes)[0]

def check_magic(data, offset, magic):
    return data[offset + 0] == ord(magic[0]) and \
           data[offset + 1] == ord(magic[1]) and \
           data[offset + 2] == ord(magic[2]) and \
           data[offset + 3] == ord(magic[3])


if len(sys.argv) < 2:
    print("wrong args")

cooked_path = sys.argv[1]
print("Cooked path: {}".format(cooked_path))

# tags = ["FORM", "PLAT", "PROF", "PRFN", "TEXR", "TXRH", "TGTF", "GMTF", "GMTH"]

leaf_tags = ['PLAT', 'PRFN', 'TXRH', 'TGTF', 'XMLB', 'ASTH', 'ASTD', 'PRFE', 'MTLH', 'MTLC', 'MSHH', 'MHDR', 'MVTX',
             'MIDX', 'MEDG', 'OTLH', 'OUTA', 'OUTV', 'OUTE', 'OUTO', 'OUTW', 'GEOH', 'GLOD', 'SKHD', 'BONS', 'LCRH',
             'LCRD', 'LITH', 'EMIH', 'EMPH', 'EPIB', 'EPNM', 'EPUV', 'EPCT', 'EPTS', 'EPTR', 'PNDH', 'CAMH', 'TAG2',
             'SPDH', 'SPDP', 'EINH', 'GINH', 'LINH', 'SPIH', 'PNTH', 'PVSH', 'PVSR', 'OCCH', 'OCCV', 'OCHT', 'OCFT',
             'OCCL', 'OCNT', 'HVKH', 'HVKD', 'GBED', 'HKAH', 'HKAD', 'HKSH', 'HKSD', 'HKMH', 'HKMD', 'SBKH', 'SBXH',
             'SBXD', 'SPMH', 'SPMD', 'DIRH', 'DIRD', 'ENTH', 'ENTD', 'SETH', 'SETD', 'LVLH', 'LVLD', 'ACLH', 'ACLD'
             'GMTH', 'ACLD', 'GMTH', 'GMTD']

# todo: XMLB needs special handling

def read_str(data, offset, sz):
    pos = data.tell()
    data.seek(offset, os.SEEK_CUR)
    st = data.read(sz).decode("ascii")
    st = ''.join([x for x in st if x in string.printable]) # Filter out null characters
    data.seek(pos)
    return st

def read_u64(data, offset):
    pos = data.tell()
    data.seek(offset, os.SEEK_CUR)
    n = unpack(data.read(8))
    data.seek(pos)
    return n

def read_u16_be(data, offset):
    pos = data.tell()
    data.seek(offset, os.SEEK_CUR)
    n = unpack_be(data.read(2))
    data.seek(pos)
    return n

with open(cooked_path, 'rb') as cooked_file, mmap(cooked_file.fileno(), 0, access=ACCESS_READ) as cooked_data:
    i = 0
    prev_i = 0
    prev_size = len(cooked_data)
    level = -1

    stack = [(0, 0)]
    stack.clear()

    extra_info_pre = " " * 16

    print("")
    print("Address   <Size     >  (Description     ) Tag Structure                Additional information")
    print("_" * 120)

    while i < len(cooked_data):
        extra_info_post = ""

        assert(i % 4 == 0)

        cooked_data.seek(i)

        tag = str(cooked_data.read(4), "utf-8")
        size = int.from_bytes(cooked_data.read(4), byteorder='little', signed=False)
        i += 8

        level += 1
        for s in range(len(stack) - 1, -1, -1):
            ss = stack[s][0] + stack[s][1]
            if i >= ss:
                level -= 1
                del stack[s]

        if tag == 'PRFN':
            prfn = read_str(cooked_data, 0, size - 4)
            extra_info_pre = "{:<16}".format(prfn)
            extra_info_post = "Set type: " + prfn
        elif tag == "TXRH":
            txrh_id = read_u64(cooked_data, 0x4)
            extra_info_post += str(txrh_id) + " | "
            txrh_name = read_str(cooked_data, 0xC, 0x28)
            extra_info_post += '"' + txrh_name + '"'
        elif tag == "TGTF":
            tgtf_width = read_u16_be(cooked_data, 0x20)
            tgtf_height = read_u16_be(cooked_data, 0x22)
            extra_info_post += str(tgtf_width) + "x" + str(tgtf_height)
        elif tag == 'MTLH':
            mtlh_name = read_str(cooked_data, 0x19, 0x1F)
            extra_info_post += '"' + mtlh_name + '"'

        if len(extra_info_post) > 0:
            extra_info_post = " " * (24 - (level * 4)) + " " + extra_info_post

        print("0x{:07X} <0x{:07X}>  ({}) {}{}{}".format(i, size, extra_info_pre, "    " * level, tag, extra_info_post))

        assert (tag.isupper())

        stack.append((i, size))

        if tag in leaf_tags:
            i += size - 4



# def visit_tag(cooked_data, range_start, range_end, level):
#     # exit conditions
#     # if i == len(cooked_data):
#     #     return
#
#     print("{}({:X}, {:X})".format("\t" * level, range_start, range_end))
#
#     # if range_start + 4 == range_end:
#     #     return True
#
#     i = range_start
#
#     prev_size = 0
#
#     while True:
#         cooked_data.seek(i)
#
#         tag = str(cooked_data.read(4), "utf-8")
#         size = int.from_bytes(cooked_data.read(4), byteorder='little', signed=False)
#         i += 8
#
#         print("{}{}[0x{:X}] at 0x{:X}".format("\t" * level, tag, size, i))
#
#         if tag in leaf_tags:
#             i += size - 4
#             continue
#
#         # print("{}i bef: {:X}".format("\t" * level, i))
#         visit_tag(cooked_data, i, i + size - 4, level)
#
#         print("{}0x{:X} += 0x{:X} =  {:X}".format("\t" * level, i, size, i + size))
#
#         i += size
#
#         if i + 4 == range_end:
#             print("ending range: (0x{:X}, 0x{:X})".format("\t" * level, range_start, range_end))
#             break
#
# with open(cooked_path, 'rb') as cooked_file, mmap(cooked_file.fileno(), 0, access=ACCESS_READ) as cooked_data:
#     visit_tag(cooked_data, 0, len(cooked_data), 0)
