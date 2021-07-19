# Author: https://github.com/VelocityRa

import sys
import os
from pathlib import Path
import struct
import io
import operator

tracefile_path = Path("D:\\Nikos\\Programming\\cpp\\pcsx2\\pcsx2\\windows\\VCprojects\\pcsx2_trace2.bin")
coveragefile_path = Path("D:\\Nikos\\Programming\\cpp\\pcsx2\\pcsx2\\windows\\VCprojects\\pcsx2_trace2.trace")
exe_path = Path("D:\\Roms\\PS2\\Games\\Sly Cooper and the Thievius Raccoonus (USA)\\SCUS_971.98")
exe_text_offset = 0x1000
exe_text_size = 0x17B294
module_base = 0x00100000
exclude_vaddr_list = [ 0x9488 ]
count_hits = True
trace_data = {}

def is_inst_that_splits_block(instruction):
    opcode = instruction >> 26

    splits_standard = [2, 3, 4, 5, 6, 7, 20, 21, 22, 23]
    if opcode in splits_standard:
        return True

    if opcode == 0: # SPECIAL
        opcode = instruction & 0x3F

        splits_special = [8, 9]

        if opcode in splits_special:
            return True

    if opcode == 1: # REGIMM
        opcode = (instruction >> 16) & 0x1F

        splits_regimm = [0, 1, 2, 3, 16, 17, 18, 19]

        if opcode in splits_regimm:
            return True

    return False


with open(tracefile_path , 'rb') as trace_f:
    trace_f.seek(0, io.SEEK_END)
    entry_filesize = trace_f.tell()
    trace_f.seek(0, io.SEEK_SET)

    entry_count = entry_filesize // 4 - len(exclude_vaddr_list)

    with open(coveragefile_path , 'wb') as cov_f:
        cov_f.write(b"DDPH-PINTOOL\n")
        cov_f.write(bytes("EntryCount: {}, ModuleCount: {}\n".format(entry_count, 1), encoding='utf8'))
        cov_f.write(b"Module table row names (left to right): Module Id,  Module Base, Module End, Module Path\n\n")

        cov_f.write(b"MODULE_TABLE\n")
        cov_f.write(bytes("0, {:X}, {:X}, none\n".format(module_base, module_base + exe_text_size), encoding='utf8'))

        cov_f.write(b"ENTRY_TABLE\n")

        max_range_size = 0
        with open(exe_path, 'rb') as exe_f:
            for i in range(entry_count):
                # if i==392:
                #     what=4

                # Write range start offset

                pc = struct.unpack('I', trace_f.read(4))[0]
                if pc < module_base or pc >= module_base + exe_text_size:
                    continue

                pc_rel = pc - module_base

                if pc_rel in exclude_vaddr_list:
                    continue

                if count_hits:
                    hits = struct.unpack('I', trace_f.read(4))[0]
                    trace_data[pc_rel] = hits

                cov_f.write(struct.pack('I', pc_rel))

                # Write range size

                # Walk instructions till we find one that would split the current block (
                range_size = 4 + 4 # +4 for branch delay slot

                exe_f.seek(exe_text_offset + pc_rel)
                while True:
                    instruction = int.from_bytes(exe_f.read(4), byteorder='little')

                    # if pc_rel == 0x9488 and range_size // 4 ==  14:
                    #     print("ins: {:X}".format(instruction))
                    #     what=1

                    if is_inst_that_splits_block(instruction):
                        break

                    range_size += 4

                cov_f.write(struct.pack('H', range_size))

                # Write module id
                cov_f.write(b'\x00\x00')

                # Write instruction count
                inst_count = range_size // 4
                cov_f.write(struct.pack('I', inst_count))

                # print("Wrote start=0x{:X} icount=0x{:X}".format(pc_rel, inst_count))

                max_range_size = max(range_size, max_range_size)

            print("Done. entry count: {} max icount: 0x{:X}".format(entry_count, max_range_size//4))


trace_data = dict(sorted(trace_data.items(), key=operator.itemgetter(1), reverse=True))

if count_hits:
    print("Addr Hits")
    for k, v in trace_data:
        print("{:08X} {}".format(k, v))