PROJECT(ITKNrrdIO)

#
# This CMake file configures the NrrdIO library build.  NrrdIO
# is used by Insight/Code/IO/itkNrrdIO for reading/writing 
# "Nearly Raw Raster Data" within the open-source Teem software 
# package. See http://teem.sourceforge.net for more information.
#

SET(nrrdio_SRCS
754.c            comment.c      endianNrrd.c 
gzio.c           parseNrrd.c    defaultsNrrd.c   
enum.c           keyvalue.c     dio.c
enumsNrrd.c      methodsNrrd.c  accessors.c   
encoding.c       format.c       miscAir.c      
encodingAscii.c  formatEPS.c    mop.c           
read.c           array.c        encodingBzip2.c 
formatNRRD.c     reorder.c      arraysNrrd.c    
encodingGzip.c   formatPNG.c    sane.c
axis.c           encodingHex.c  formatPNM.c
simple.c         write.c        biffbiff.c
encodingRaw.c    formatText.c   string.c
endianAir.c      formatVTK.c    parseAir.c
)

# Set compiler flags for endian-ness.
IF(CMAKE_WORDS_BIGENDIAN)
  ADD_DEFINITIONS(-DTEEM_ENDIAN=4321)
ELSE(CMAKE_WORDS_BIGENDIAN)
  ADD_DEFINITIONS(-DTEEM_ENDIAN=1234)
ENDIF(CMAKE_WORDS_BIGENDIAN)

# Set compiler flags for 32 or 64 bit architecture (based on the size
# of a void pointer).
IF(CMAKE_SIZEOF_VOID_P=8)
  ADD_DEFINITIONS(-DTEEM_32BIT=0)
#  MESSAGE(STATUS "NrrdIO configuration: -DTEEM_32BIT=0")
ELSE(CMAKE_SIZEOF_VOID_P=8)
  ADD_DEFINITIONS(-DTEEM_32BIT=1)
#  MESSAGE(STATUS "NrrdIO configuration: -DTEEM_32BIT=1")
ENDIF(CMAKE_SIZEOF_VOID_P=8)

# The 22nd bit of a 32-bit quiet-NaN is either 1 (1) or 0 (0).  This distinction 
# is needed in handling of IEEE floating point special values.  This quantity 
# is independent of endian-ness.
TRY_RUN(nrrd_QNANHIBIT nrrd_QNANHIBIT_COMPILE_RESULT
  ${ITKNrrdIO_BINARY_DIR} 
  ${ITKNrrdIO_SOURCE_DIR}/qnanhibit.c )

IF(NOT nrrd_QNANHIBIT_COMPILE_RESULT)
  MESSAGE(ERROR " Failed to compile a test (qnanhibit.c) necessary for NrrdIO configuration. NrrdIO may not properly handle NaN's.")
ENDIF(NOT nrrd_QNANHIBIT_COMPILE_RESULT)

IF(nrrd_QNANHIBIT LESS 0)
  MESSAGE(ERROR " A test (qnanhibit.c) necessary for NrrdIO configuration returned error code. NrrdIO may not properly handle NaN's.")
ENDIF(nrrd_QNANHIBIT LESS 0)

IF(nrrd_QNANHIBIT)
  ADD_DEFINITIONS(-DTEEM_QNANHIBIT=1)
#  MESSAGE(STATUS "NrrdIO configuration: -DTEEM_QNANHIBIT=1")
ELSE(nrrd_QNANHIBIT)
  ADD_DEFINITIONS(-DTEEM_QNANHIBIT=0)
#  MESSAGE(STATUS "NrrdIO configuration: -DTEEM_QNANHIBIT=0")
ENDIF(nrrd_QNANHIBIT)

# This platform can (1) or cannot(0) do bit-fields greater than 32 bits.  This is 
# necessary for IEEE floating point special values.
# TO DO: Can I automatically configure this or do we need a CMAKE_OPTION?
TRY_COMPILE(nrrd_BIGBITFIELD  ${ITKNrrdIO_BINARY_DIR}       
                        ${ITKNrrdIO_SOURCE_DIR}/bigbitfield.c )
IF(nrrd_BIGBITFIELD)
  ADD_DEFINITIONS(-DTEEM_BIGBITFIELD=1)
ELSE(nrrd_BIGBITFIELD)
  ADD_DEFINITIONS(-DTEEM_BIGBITFIELD=0)
ENDIF(nrrd_BIGBITFIELD)

#DirectIO is the fast way to do multi-gigabyte I/O and currently only available
#for SGI platforms.  Use of DirectIO is enabled manually for now.
OPTION(USE_DIRECTIO "Use DirectIO for Nrrd file IO.  Only valid on SGI systems." 0)
MARK_AS_ADVANCED(USE_DIRECTIO)
IF(USE_DIRECTIO)
  ADD_DEFINITIONS(-DTEEM_DIO=1)
#  MESSAGE(STATUS "NrrdIO configuration: -DTEEM_DIO=1")
ELSE(USE_DIRECTIO)
  ADD_DEFINITIONS(-DTEEM_DIO=0)
#  MESSAGE(STATUS "NrrdIO configuration: -DTEEM_DIO=0")
ENDIF(USE_DIRECTIO)

ADD_LIBRARY(ITKNrrdIO ${nrrdio_SRCS} )
TARGET_LINK_LIBRARIES(ITKNrrdIO ${ITK_ZLIB_LIBRARIES} )

INSTALL_TARGETS(/lib/InsightToolkit ITKNrrdIO)
INSTALL_FILES(/include/InsightToolkit/Utilities/NrrdIO "(\\.h)$")

