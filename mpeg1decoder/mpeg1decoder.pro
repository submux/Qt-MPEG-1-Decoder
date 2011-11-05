TEMPLATE = app

HEADERS += \
    decoder.h \
    idct.h \
    inputbitstream.h \
    motionvector.h \
    picture.h \
    picturequeue.h \
    utility.h \
    videorenderer.h \
    vlc.h \
    test/qmpegdecoderview.h \
    test/mpegbitmap.h \
    test/mpegviewer.h

SOURCES += \
    decoder.cpp \
    idct.cpp \
    inputbitstream.cpp \
    motionvector.cpp \
    picture.cpp \
    vlc.cpp \
    test/main.cpp \
    test/qmpegdecoderview.cpp \
    test/mpegbitmap.cpp \
    test/mpegviewer.cpp
	

FORMS += \
    test/qmpegdecoderview.ui









