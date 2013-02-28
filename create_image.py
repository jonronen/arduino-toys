#!/usr/bin/python

import os
import struct



SAMPLE_CODE = ("100050"+"100150"+"100250"+"100350"+"71").decode("hex")


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
        print "Usage: %s PCM_FILE_PREFIX [CODE]"
        sys.exit()

    if len(sys.argv) > 2:
        code = sys.argv[2].decode('hex')
    else:
        code = SAMPLE_CODE

    # get all the files that start with the same prefix
    dirname = os.path.dirname(sys.argv[1]) or "."
    file_prefix = os.path.basename(sys.argv[1])
    dirlist = os.listdir(dirname)
    dirlist = filter(
        lambda x: x.startswith(file_prefix) and x.endswith(".wav"),
        dirlist
    )
    dirlist = map(lambda x: dirname + "/" + x, dirlist)

    dest_image = open(sys.argv[1]+".img", "wb")
    sound_data = []
    
    for fname in dirlist:
        file_len = os.stat(fname).st_size - 0x2c
        f = open(fname, "rb")
        f.seek(0x2c) # start of PCM data
        fdata = f.read()
        fdata += "\x00" * ((-len(fdata)) & 0x1ff)
        sound_data.append(fdata)
        f.close()

    #
    # TODO:
    # change the 3 to something better,
    # taking into account the number of sound entries
    #
    total_partition_blocks = \
        3 + \
        (len(code)+511)/512 + \
        reduce(lambda x,y: x+len(y), sound_data, 0)
    dest_image.write(create_mbr(total_partition_blocks))
    
    dest_image.write(create_partition_toc(code, len(dirlist)))

    dest_image.write(code + "\x00" * (-len(code) & 0x1ff))
    
    dest_image.write(create_sound_entries(sound_data))

    for data in sound_data:
        dest_image.write(data)

    dest_image.close()
    f.close()

