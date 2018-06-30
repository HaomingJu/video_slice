# 生成wrap和py文件
swig -c++ -python ./slice_video_py.i


export LD_LIBRARY_PATH=/home/haoming/code/video-slice/python_apis/python/

dll

g++-4.9   -shared -fPIC  -std=c++11  ./slice_video_py_wrap.cxx -o _slice_video_py.so -I/usr/include/python2.7 -lpython2.7 -ldataslice -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale

g++-4.9   -shared -fPIC  -std=c++11  ./slice_video_py_wrap.cxx -o _slice_video_py.so -I/usr/include/python2.7 -lpython2.7 -L/home/haoming/code/video-slice/python_apis/python/  -ldataslice -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale


g++-4.9   -shared -fPIC  -std=c++11 -Wl,-rpath=./ ./slice_video_py_wrap.cxx -o _slice_video_py.so -I/usr/include/python2.7 -lpython2.7 -L./  -ldataslice -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale
