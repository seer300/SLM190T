
file(GLOB_RECURSE C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c") #先包含全部.c.s文件
file(GLOB_RECURSE C_EXCLUDE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/xy_lib/*.c") #去除封库或者不使用的.c.s
file(GLOB_RECURSE C_EXCLUDE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/mcu_adapt/other/*.c")
list(REMOVE_ITEM C_SOURCES ${C_EXCLUDE_SOURCES})

get_sub_header_dirs(INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory_if_exist(xy_lib)

foreach(def ${PRJ_DEF_LIST})
    if(${def} MATCHES "GNSS_EN")
    string(REGEX MATCH "[0-9]+" GNSS_EN ${def})
    endif()
endforeach()

if(${GNSS_EN} EQUAL 1)
    change_section_config(${TARGET_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/HAL/src/hal_csp.c" FALSE)
endif()

set(LIBRARIES ${LIBRARIES} PARENT_SCOPE)
set(PRJ_SRC_LIST ${PRJ_SRC_LIST} ${C_SOURCES} PARENT_SCOPE)

set(BASE_INCLUDE_DIRS ${BASE_INCLUDE_DIRS} ${INCLUDE_DIRS} PARENT_SCOPE)