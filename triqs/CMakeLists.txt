# all .cpp files to get the dependencies
FILE(GLOB_RECURSE ALLSOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
FOREACH(CPP ${ALLSOURCES})
  set_property (GLOBAL APPEND PROPERTY TRIQS_LIB_CPP_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/${CPP}")
ENDFOREACH(CPP ${ALLSOURCES})
get_property(ALLSOURCES GLOBAL PROPERTY TRIQS_LIB_CPP_SOURCES_LIST)

# compile every subdirectory
add_all_subdirectories_with_cmakelist()

# Get all .mhpp, .mcpp
FILE(GLOB_RECURSE ALL_MHPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.mhpp)
FILE(GLOB_RECURSE ALL_MCPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.mcpp)

# Mako process mhpp, mcpp files
FOREACH(FI ${ALL_MHPP} ${ALL_MCPP})
 get_filename_component(P ${FI} PATH)
 get_filename_component(N ${FI} NAME_WE)
 # remove the m in the extension
 string(REGEX REPLACE "[.]m([hc])pp" ".\\1pp" FI2 ${FI})
 SET(fout ${CMAKE_BINARY_DIR}/include/triqs/${FI2})

 add_custom_command (OUTPUT ${fout} DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${FI}
  COMMAND ${PYTHON_INTERPRETER} ${CMAKE_SOURCE_DIR}/tools/mako_render.py
	  ${CMAKE_CURRENT_SOURCE_DIR}/${FI} ${CMAKE_BINARY_DIR}/include/triqs/${P})
 add_custom_target(generated_${N} ALL DEPENDS ${fout} )
ENDFOREACH(FI ${ALL_MHCPP})

# Build the list of generated CPP
FOREACH(FI ${ALL_MCPP})
 get_filename_component(N ${FI} NAME)
 SET(fout ${CMAKE_BINARY_DIR}/include/triqs/${FI})
 SET(GENERATED_CPP "${GENERATED_CPP} ${FI}")
ENDFOREACH(FI ${ALL_MHCPP})

# The lib will be build from the sources and the converters generated sources
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
SET(LINK_RT "")
else()
SET(LINK_RT "-lrt")
endif()
add_library(triqs ${ALLSOURCES} ${GENERATED_CPP} ${all_py_converters_sources}) # ${ALL_MHPP} ${ALL_MCPP})
target_link_libraries (triqs ${LINK_RT} ${TRIQS_LINK_LIBS}  )

# The TRIQS library depends on the generation of the .cpp wrapper and converters files
# which are produced first.
get_property(all_py_converters_targets GLOBAL PROPERTY TRIQS_PY_CONVERTERS_TARGETS)
foreach(T ${all_py_converters_targets})
 add_dependencies(triqs ${T})
endforeach()

# TRIQS library also depends on the generated sources
FOREACH(FI ${ALL_MHPP} ${ALL_MCPP})
 get_filename_component(N ${FI} NAME_WE)
 add_dependencies(triqs generated_${N})
ENDFOREACH(FI ${ALL_MHCPP})

# Install library
install(TARGETS triqs DESTINATION lib)

# Version file
configure_file(version.hpp.in version.hpp @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/version.hpp" DESTINATION "include/triqs/")

# all .hpp and .h files...
FILE(GLOB_RECURSE ALLHEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h)

# Install headers in good include dir
FOREACH(HEADER ${ALLHEADERS})
 get_filename_component(P ${HEADER} PATH)
 install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}" DESTINATION "include/triqs/${P}")
ENDFOREACH(HEADER ${ALLHEADERS})

# Install generated headers
FILE(GLOB_RECURSE GENERATED_HEADERS RELATIVE ${CMAKE_BINARY_DIR}/include ${CMAKE_BINARY_DIR}/include/triqs/*.hpp)
FOREACH(HEADER ${GENERATED_HEADERS})
 get_filename_component(P ${HEADER} PATH)
 get_filename_component(N ${HEADER} NAME)
 install(FILES "${CMAKE_BINARY_DIR}/include/${HEADER}" DESTINATION "include/${P}")
ENDFOREACH(HEADER ${GENERATED_HEADERS})


