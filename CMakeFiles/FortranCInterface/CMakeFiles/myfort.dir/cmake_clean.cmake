FILE(REMOVE_RECURSE
  "CMakeFiles/myfort.dir/mysub.f.obj"
  "CMakeFiles/myfort.dir/my_sub.f.obj"
  "libmyfort.pdb"
  "libmyfort.a"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang Fortran)
  INCLUDE(CMakeFiles/myfort.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
