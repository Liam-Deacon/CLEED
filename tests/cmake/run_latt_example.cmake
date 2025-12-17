if(NOT DEFINED PROGRAM)
  message(FATAL_ERROR "PROGRAM is required")
endif()
if(NOT DEFINED INPUT)
  message(FATAL_ERROR "INPUT is required")
endif()
if(NOT DEFINED H OR NOT DEFINED K OR NOT DEFINED L)
  message(FATAL_ERROR "H, K, L are required")
endif()
if(NOT DEFINED OUT_BASENAME)
  set(OUT_BASENAME "latt_example")
endif()

set(workdir "${CMAKE_CURRENT_BINARY_DIR}/e2e-${OUT_BASENAME}")
file(REMOVE_RECURSE "${workdir}")
file(MAKE_DIRECTORY "${workdir}")

set(out_xyz "${workdir}/${OUT_BASENAME}.xyz")

execute_process(
  COMMAND "${PROGRAM}"
          -h "${H}" -k "${K}" -l "${L}"
          --input "${INPUT}"
          --max-layers 1
          --max-cells 1
          --max-atoms 64
          -o "${out_xyz}"
  RESULT_VARIABLE rc
  OUTPUT_VARIABLE stdout
  ERROR_VARIABLE stderr
)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "latt failed (rc=${rc})\nstdout:\n${stdout}\nstderr:\n${stderr}")
endif()

set(out_inf "${out_xyz}.inf")
if(NOT EXISTS "${out_inf}")
  message(FATAL_ERROR "expected output file missing: ${out_inf}")
endif()

file(READ "${out_inf}" inf_contents)

set(needles "")
if(DEFINED NEEDLES_PIPE)
  string(REPLACE "|" ";" needles "${NEEDLES_PIPE}")
endif()

foreach(needle IN LISTS needles)
  if(needle STREQUAL "")
    continue()
  endif()
  string(FIND "${inf_contents}" "${needle}" pos)
  if(pos EQUAL -1)
    message(FATAL_ERROR "expected to find ${needle} in ${out_inf}")
  endif()
endforeach()

