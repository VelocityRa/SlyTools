import sys
import os
from pathlib import Path
import struct


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


mem_dump_path = Path(sys.argv[1])
out_dir_p = Path(sys.argv[2])

out_dir_p.mkdir(exist_ok=True)

with open(mem_dump_path, 'rb') as f, open(out_dir_p.joinpath("{}.obj".format("ALL")), 'w') as f_out:
    f_data = bytearray(f.read())
    f_len = len(f_data)

    mesh_count = 0
    # i = 0xCDD6D8
    i = 0
    model_i = 0
    models_count = 0

    while i < f_len:
        if read_32(f_data, i) == 0x6D03C00A:
            print("found at 0x{:X}".format(i))
            f.seek(i + 0x1E)

            vertex_count = f.read(1)[0]
            vertex_data_size = vertex_count * 3 * 4

            two_test = f.read(1)[0]

            if two_test != 0x68:
                print("two_test is {:X}, skipping seg at 0x{:X}".format(two_test, i))
                i += 4
                continue

            vertex_data = f.read(vertex_data_size)

            print("vertex_count: {} vertex_data_size: {}".format(vertex_count, vertex_data_size))

            f.seek(i + 0x20 + vertex_data_size + 2)
            quad_count = f.read(1)[0]

            # f.seek(1, os.SEEK_CUR)
            # test_first_vtx = f.read(4)
            # f.seek(-5, os.SEEK_CUR)

            # if quad_count == 0:

            # Attempt to skip unknown segment by assuming that the first quad's indices will be of a specific pattern
            f.seek(1, os.SEEK_CUR)
            words_later = 0
            while True:
                word = f.read(4)
                # if (word[0] == 0 and word[1] == 1) or (word[0] == 1 and word[1] == 0) \
                #         and word[2] < 128 and word[3] < 128:
                if word[0] == 0 and word[1] == 1 and word[2] == 2 and word[3] < 128:
                    f.seek(-4, os.SEEK_CUR)
                    break
                words_later += 1
            f.seek(-2, os.SEEK_CUR)
            quad_count = f.read(1)[0]

            # f.seek(1 + unk_size + 2, os.SEEK_CUR)
            # quad_count = f.read(1)[0]
            # else test_first_vtx[0] != 0 and test_first_vtx[1] != 1 and test_first_vtx[2] != 2 and test_first_vtx[3] != 3:

            if quad_count == 0:
                raise Exception("quad count is 0 at 0x{:X}".format(i))

            # print("error! skipping bytes")
            # unk_size = 0
            # if i == 0xD4CF28:
            #     unk_size = 0x70
            # elif i == 0xD4DB38:
            #     unk_size = 0x84
            # elif i == 0xD50118:
            #     unk_size = 0x88
            # elif i == 0xD50DA8:
            #     unk_size = 0x40
            # elif i == 0xD51BEC:
            #     unk_size = 0xCC
            # elif i == 0xD55888:
            #     unk_size = 0x40
            # elif i == 0xD4DEBC:
            #     unk_size = 0xA0
            # elif i == 0xD4E290:
            #     unk_size = 0x84
            # elif quad_count == 0:
            #     raise Exception("offset not in db")
            # f.seek(1 + unk_size + 2, os.SEEK_CUR)
            # quad_count = f.read(1)[0]

            index_count = quad_count * 4
            index_data_size = index_count
            f.seek(1, os.SEEK_CUR)
            index_data = f.read(index_data_size)

            print("quad_count: {} index_count: {}".format(quad_count, index_count))

            if i - model_i > 20000 or models_count == 0:
                f_out.write("g 0x{:X}_0x{:X}\n".format(model_i, i))
                models_count += 1
                model_i = i
                model_i += 0x20 + vertex_data_size + index_data_size # roughly the size of current model

            for j in range(0, vertex_data_size, 12):
                vert_pos = struct.unpack('3f', vertex_data[j: j + 12])

                f_out.write("v {} {} {}\n".format(vert_pos[0], vert_pos[1], vert_pos[2]))

            for j in range(0, index_data_size, 4):
                quad_indices = list(struct.unpack('4B', index_data[j: j + 4]))

                # if quad_indices[0] > 127 or quad_indices[1] > 127 or quad_indices[2] > 127 or quad_indices[3] > 127:
                #     continue

                for k in range(4):
                    quad_indices[k] = vertex_count - quad_indices[k]

                q = quad_indices  # shorthand
                f_out.write("f -{} -{} -{}\nf -{} -{} -{}\n".format(q[0], q[1], q[2], q[3], q[2], q[1]))

                # if i == 0xD574A8:
                #     break # todo remove

                # buf_size = 0x20 + f_data[i + 0x1E] * 12
                # buf_size += 4 + f_data[i + buf_size + 2] * 4
                # with open(out_dir_p.joinpath("0x{:X}.mesh".format(i)), 'wb') as f_out:
                #     f_out.write(f_data[i:i+buf_size])

        i += 4
