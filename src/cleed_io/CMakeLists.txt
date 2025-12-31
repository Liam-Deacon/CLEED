# CLEED IO library target (stub)

set(cleed_io_SRCS
    cleed_io.c
)

add_library(cleedIO SHARED ${cleed_io_SRCS})
add_library(cleedIOStatic STATIC ${cleed_io_SRCS})

if (NOT WIN32)
    set_target_properties(cleedIOStatic PROPERTIES OUTPUT_NAME cleedIO)
endif()

target_include_directories(cleedIO PUBLIC ${PROJECT_SOURCE_DIR}/src/include)
target_include_directories(cleedIOStatic PUBLIC ${PROJECT_SOURCE_DIR}/src/include)

target_link_libraries(cleedIO PUBLIC cleedCore)
target_link_libraries(cleedIOStatic PUBLIC cleedCoreStatic)

if (WIN32)
    install(TARGETS cleedIO
        COMPONENT runtime
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
    )
else()
    install(TARGETS cleedIO
        COMPONENT runtime
        LIBRARY DESTINATION lib
    )
endif()

install(TARGETS cleedIOStatic ARCHIVE DESTINATION lib COMPONENT libraries)
