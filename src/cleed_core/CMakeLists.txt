# CLEED core library target

set(cleed_core_SRCS
    cleed_core.c
    cleed_curve.c
)

add_library(cleedCore SHARED ${cleed_core_SRCS})
add_library(cleedCoreStatic STATIC ${cleed_core_SRCS})

if (NOT WIN32)
    set_target_properties(cleedCoreStatic PROPERTIES OUTPUT_NAME cleedCore)
endif()

target_include_directories(cleedCore PUBLIC ${PROJECT_SOURCE_DIR}/src/include)
target_include_directories(cleedCoreStatic PUBLIC ${PROJECT_SOURCE_DIR}/src/include)

# Link to core computation libraries.
target_link_libraries(cleedCore PUBLIC search rfac)
target_link_libraries(cleedCoreStatic PUBLIC searchStatic rfacStatic)

if (WIN32)
    install(TARGETS cleedCore
        COMPONENT runtime
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
    )
else()
    install(TARGETS cleedCore
        COMPONENT runtime
        LIBRARY DESTINATION lib
    )
endif()

install(TARGETS cleedCoreStatic ARCHIVE DESTINATION lib COMPONENT libraries)
