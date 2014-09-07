AUX_SOURCE_DIRECTORY(. srcs)
AUX_SOURCE_DIRECTORY(freq srcs)
AUX_SOURCE_DIRECTORY(vecdir srcs)
ADD_EXECUTABLE ( memnet ${srcs})

ADD_SUBDIRECTORY ( others ../bin/others )
