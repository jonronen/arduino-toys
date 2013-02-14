#!/usr/bin/python

import os
import struct


def create_mbr(num_blocks):
    return "\x00"*0x1be + \
        "000000006a000000".decode('hex') + \
        struct.pack("<II", 1, num_blocks) + \
        "\x00"*0x30 + "55aa".decode('hex')


PARTITION_MAGIC_NUM = 0x616e616d
partition_current_sector = 0

def create_partition_toc(code, num_files):
    global partition_current_sector

    code_blocks = (len(code)+511)/512
    partition_current_sector += (1+code_blocks) # code starts at sector #1
    ans = struct.pack(
        "<IHHHHH",
        PARTITION_MAGIC_NUM,
        1,
        1,
        code_blocks,
        1+code_blocks,
        num_files
    )
    ans += "\x00"*(512-len(ans))
    return ans

def create_sound_entries(data_list):
    global partition_current_sector

    ans = ""
    ind = 0

    num_entry_sectors = (len(data_list)*8 + 511)/512
    partition_current_sector += num_entry_sectors

    for data in data_list:
        data_blocks = (len(data)+511)/512
        ans += struct.pack("<HHI", ind, data_blocks, partition_current_sector)
        partition_current_sector += data_blocks
        ind += 1

    rem = 512 - (len(ans) % 512)
    rem %= 512
    ans += "\x00" * rem

    return ans


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) == 1:
        print "Usage: %s PCM_FILE"
        sys.exit()

    f = open(sys.argv[1], "rb")
    g = open(sys.argv[1]+".img", "wb")

    f.seek(0x2c) # start of PCM data

    file_len = os.stat(sys.argv[1]).st_size - 0x2c
    g.write(create_mbr((file_len+511)/512 + 3))
    
    g.write(create_partition_toc("", 1))
    
    data = f.read()
    g.write(create_sound_entries([data]))
    g.write(data)

    # pad with zeroes to the next block
    rem = 512-(g.tell() % 512)
    rem %= 512
    if rem:
        g.write("\x00"*rem)

    g.close()
    f.close()

