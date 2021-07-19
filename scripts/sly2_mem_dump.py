import sys
import os
from pathlib import Path
import struct

for level_id in [1]: #range(2, 8+1):
    out_path_p = Path("{}/sly2_lvl_ep{}.obj".format(sys.argv[2], level_id))

    mem_dump_path = Path("D:\Emulators\pcsx2\sstates\eeMemory_sly2_ep{}.bin".format(level_id))

    with open(mem_dump_path, 'rb') as f, open(out_path_p, 'w') as f_out:
        f_data = bytearray(f.read())
        f_len = len(f_data)

        i = 0
        group_i = 0
        face_count = 0
        while i < f_len:
            if (f_data[i+1] == 0xC0 or f_data[i+1] == 0xC1) and f_data[i+3] == 0x6C:
                vert_count = f_data[i+2]
                #print("Segment {:02X}{:02X} of vcount {:>3} at 0x{:X}".format(f_data[i], f_data[i+1], vert_count, i))
                i += 4

                if vert_count > 9:
                    f_out.write("g seg_{}_0x{:X}\n".format(group_i, i))

                    vert_i = 0
                    while vert_i < vert_count:
                        vert_pos = struct.unpack("3f", f_data[i:i+12])
                        f_out.write("v {} {} {}\n".format(vert_pos[0], vert_pos[1], vert_pos[2]))
                        i += 12
                        #print("i now at {:X}".format(i))

                        #f_out.write("f -3 -2 -1\n")
                        #print("{:X}".format(f_data[i+1] >> 4))
                        if (f_data[i+1] >> 4) <= 0x7:
                            f_out.write("f -3 -2 -1\n")
                            face_count += 1
                        # else:
                        #     if f_data[i+1] != 0x8B:
                        #         print("error at {:X}".format(i))
                        #         #assert(f_data[i+1] == 0x8B)

                        i += 4
                        vert_i += 1
                group_i += 1

            i += 4

    print("Wrote {} faces for level_id {}".format(face_count, level_id))


#facebyte = 0x7C
#
# for ep_id in [1]: # range(1, 8+1):
#     out_dir_p = Path(sys.argv[2] + "/ep{}".format(ep_id))
#     out_dir_p.mkdir(exist_ok=True)
#
#     mem_dump_path = Path("D:\Emulators\pcsx2\sstates\eeMemory_sly2_ep{}.bin".format(ep_id))
#
# #    for facebyte in range(0, 255):
#
#     #outfile = out_dir_p.joinpath("sly2_lvl_ep{}_0x{:X}.obj".format(ep_id, facebyte))
#     outfile = out_dir_p.joinpath("sly2_lvl_ep{}_0x{:X}.obj".format(ep_id))
#
#     with open(mem_dump_path, 'rb') as f, open(outfile, 'w') as f_out:
#         f_data = bytearray(f.read())
#         f_len = len(f_data)
#
#         i = 0 #0x01F6D140
#         group_i = 0
#         face_count = 0
#         while i < f_len:
#             if (f_data[i+1] == 0xC0 or f_data[i+1] == 0xC1) and f_data[i+3] == 0x6C:
#                 vert_count = f_data[i+2]
#                 #print("Segment {:02X}{:02X} of vcount {:>3} at 0x{:X}".format(f_data[i], f_data[i+1], vert_count, i))
#                 i += 4
#
#                 if vert_count > 9:
#                     f_out.write("g seg_{}_0x{:X}\n".format(group_i, i))
#
#                     vert_i = 0
#                     while vert_i < vert_count:
#                         vert_pos = struct.unpack("3f", f_data[i:i+12])
#                         f_out.write("v {} {} {}\n".format(vert_pos[0], vert_pos[1], vert_pos[2]))
#                         i += 12
#                         #print("i now at {:X}".format(i))
#
#                         # f_out.write("f -3 -2 -1\n")
#                         # if f_data[i+1] == facebyte:
#                         if f_data[i+1] & 0xF0 <= 0x70:
#                             f_out.write("f -3 -2 -1\n")
#                             face_count += 1
#                         # else:
#                         #     if f_data[i+1] != 0x8B:
#                         #         print("error at {:X}".format(i))
#                         #         #assert(f_data[i+1] == 0x8B)
#
#                         i += 4
#                         vert_i += 1
#                 group_i += 1
#
#             i += 4
#
#         #print("Wrote {} faces for 0x{:X}".format(face_count, facebyte))
#     print("Wrote {} faces for level_id {}".format(face_count, level_id))

            # if face_count > 10:
    #     os.rename(str(outfile), str(out_dir_p.joinpath("sly2_lvl_ep{}_0x{:X}_{}.obj".format(ep_id, facebyte, face_count))))
    # else:
    #     os.remove(str(outfile))

        # i = 0x19c8ae0
        # vertex_count = 0x43D60/16
        #
        # # Tri-strips
        # for j in range(i, i+2*16, 16):
        #     vert_pos = struct.unpack("3f", f_data[i:i+12])
        #     f_out.write("v {} {} {}\n".format(vert_pos[0], vert_pos[1], vert_pos[2]))
        #     i += 16
        #     vertex_count -= 1
        # while vertex_count > 0:
        #     vert_pos = struct.unpack("3f", f_data[i:i+12])
        #     f_out.write("v {} {} {}\n".format(vert_pos[0], vert_pos[1], vert_pos[2]))
        #     f_out.write("f -2 -3 -1\n")
        #     i += 16
        #     vertex_count -= 1
        #     # if i>=0x19c8ae0+16*40:
        #     #     break

        # Tris
        # while vertex_count > 0:
        #     for k in range(3):
        #         vert_pos = struct.unpack("3f", f_data[i:i+12])
        #         f_out.write("v {} {} {}\n".format(vert_pos[0], vert_pos[1], vert_pos[2]))
        #         i += 16
        #         vertex_count -= 1
        #         if vertex_count == 0:
        #             break
        #
        #     f_out.write("f -3 -2 -1 \n")
        #
        #     if i >= 0x19c8ae0 + 16 * 40:
        #         break

        # # Quads
        # while vertex_count > 0:
        #     for k in range(4):
        #         vert_pos = struct.unpack("3f", f_data[i:i+12])
        #         f_out.write("v {} {} {}\n".format(vert_pos[0], vert_pos[1], vert_pos[2]))
        #         i += 16
        #         vertex_count -= 1
        #     f_out.write("f -4 -3 -2 -1\n")
