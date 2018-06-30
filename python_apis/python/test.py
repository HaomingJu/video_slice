# -*- coding : utf-8 -*-
import os
import sys
import slice_video_py


if __name__ == "__main__":
    dms_cut_instance = slice_video_py.DMS_Slice()
    dms_cut_instance.Init("DMS", "dataslice.json")
    a = ' ' * 1024
    dms_cut_instance.Cut(1528684169181, 1000, a, 1024);
    print a
