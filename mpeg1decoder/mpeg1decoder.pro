TEMPLATE = app

HEADERS += \
    decoder.h \
    idct.h \
    inputbitstream.h \
    motionvector.h \
    plane.h \
    planeblock.h \
    utility.h \
    videopicture.h \
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
    plane.cpp \
    planeblock.cpp \
    videopicture.cpp \
    vlc.cpp \
    test/main.cpp \
    test/qmpegdecoderview.cpp \
    test/mpegbitmap.cpp \
    test/mpegviewer.cpp
	

FORMS += \
    test/qmpegdecoderview.ui









