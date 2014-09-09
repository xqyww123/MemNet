AUX_SOURCE_DIRECTORY(. srcs)
AUX_SOURCE_DIRECTORY(freq srcs)
AUX_SOURCE_DIRECTORY(vecdir srcs)
AUX_SOURCE_DIRECTORY(tools srcs)
ADD_EXECUTABLE ( memnet ${srcs})

ADD_SUBDIRECTORY ( others ../bin/others )
