if(NOT DEFINED PROGRAM)
  message(FATAL_ERROR "PROGRAM is required")
endif()
if(NOT DEFINED LEED_PROGRAM)
  message(FATAL_ERROR "LEED_PROGRAM is required")
endif()
if(NOT DEFINED RFAC_PROGRAM)
  message(FATAL_ERROR "RFAC_PROGRAM is required")
endif()
if(NOT DEFINED INPUT)
  message(FATAL_ERROR "INPUT is required")
endif()
if(NOT DEFINED BULK)
  message(FATAL_ERROR "BULK is required")
endif()
if(NOT DEFINED CTR)
  message(FATAL_ERROR "CTR is required")
endif()
if(NOT DEFINED OUT_BASENAME)
  set(OUT_BASENAME "csearch_e2e")
endif()

set(workdir "${CMAKE_CURRENT_BINARY_DIR}/e2e-${OUT_BASENAME}")
file(REMOVE_RECURSE "${workdir}")
file(MAKE_DIRECTORY "${workdir}")

set(inp_dst "${workdir}/${OUT_BASENAME}.inp")
set(bul_dst "${workdir}/${OUT_BASENAME}.bul")
set(ctr_dst "${workdir}/${OUT_BASENAME}.ctr")

file(COPY_FILE "${INPUT}" "${inp_dst}")
file(COPY_FILE "${BULK}" "${bul_dst}")
file(COPY_FILE "${CTR}" "${ctr_dst}")

get_filename_component(leed_dir "${LEED_PROGRAM}" DIRECTORY)
get_filename_component(leed_name "${LEED_PROGRAM}" NAME)
get_filename_component(rfac_dir "${RFAC_PROGRAM}" DIRECTORY)
get_filename_component(rfac_name "${RFAC_PROGRAM}" NAME)

if(WIN32)
  set(path_sep ";")
else()
  set(path_sep ":")
endif()

set(path_prefix "${leed_dir}")
if(NOT rfac_dir STREQUAL leed_dir)
  set(path_prefix "${path_prefix}${path_sep}${rfac_dir}")
endif()
if(DEFINED ENV{PATH})
  set(path_value "${path_prefix}${path_sep}$ENV{PATH}")
else()
  set(path_value "${path_prefix}")
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" -E env
          "PATH=${path_value}"
          "CSEARCH_LEED=${leed_name}"
          "CSEARCH_RFAC=${rfac_name}"
          "${PROGRAM}" -i "${inp_dst}" -s sx -d 0.1
  WORKING_DIRECTORY "${workdir}"
  RESULT_VARIABLE rc
  OUTPUT_VARIABLE stdout
  ERROR_VARIABLE stderr
)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "csearch failed (rc=${rc})\nstdout:\n${stdout}\nstderr:\n${stderr}")
endif()

set(expected_files
  "${workdir}/${OUT_BASENAME}.log"
  "${workdir}/${OUT_BASENAME}.par"
  "${workdir}/${OUT_BASENAME}.bsr"
  "${workdir}/${OUT_BASENAME}.res"
  "${workdir}/${OUT_BASENAME}.dum"
  "${workdir}/${OUT_BASENAME}.pmin"
  "${workdir}/${OUT_BASENAME}.rmin"
  "${workdir}/${OUT_BASENAME}.bmin"
  "${workdir}/${OUT_BASENAME}.ver"
  "${workdir}/${OUT_BASENAME}.out"
)

foreach(path IN LISTS expected_files)
  if(NOT EXISTS "${path}")
    message(FATAL_ERROR "expected output file missing: ${path}\nstdout:\n${stdout}\nstderr:\n${stderr}")
  endif()
endforeach()

file(READ "${workdir}/${OUT_BASENAME}.log" log_contents)
foreach(needle IN ITEMS "CSEARCH - version" "=> SIMPLEX SEARCH" "rf:")
  string(FIND "${log_contents}" "${needle}" pos)
  if(pos EQUAL -1)
    message(FATAL_ERROR "expected to find ${needle} in ${OUT_BASENAME}.log")
  endif()
endforeach()

file(READ "${workdir}/${OUT_BASENAME}.ver" ver_contents)
string(REGEX MATCH "^[0-9]+[ ]+[0-9]+[ ]+${OUT_BASENAME}" ver_header_match "${ver_contents}")
if(ver_header_match STREQUAL "")
  message(FATAL_ERROR "unexpected ${OUT_BASENAME}.ver header (expected: \"ndim mpts ${OUT_BASENAME}\")")
endif()
