# Global symbol without underscore.
set(FortranCInterface_GLOBAL_SYMBOL  "mysub_")
set(FortranCInterface_GLOBAL_PREFIX  "")
set(FortranCInterface_GLOBAL_SUFFIX  "_")
set(FortranCInterface_GLOBAL_CASE    "LOWER")
set(FortranCInterface_GLOBAL_MACRO   "(name,NAME) name##_")

# Global symbol with underscore.
set(FortranCInterface_GLOBAL__SYMBOL "my_sub_")
set(FortranCInterface_GLOBAL__PREFIX "")
set(FortranCInterface_GLOBAL__SUFFIX "_")
set(FortranCInterface_GLOBAL__CASE   "LOWER")
set(FortranCInterface_GLOBAL__MACRO  "(name,NAME) name##_")

# Module symbol without underscore.
set(FortranCInterface_MODULE_SYMBOL  "")
set(FortranCInterface_MODULE_PREFIX  "")
set(FortranCInterface_MODULE_MIDDLE  "")
set(FortranCInterface_MODULE_SUFFIX  "")
set(FortranCInterface_MODULE_CASE    "")
set(FortranCInterface_MODULE_MACRO   "")

# Module symbol with underscore.
set(FortranCInterface_MODULE__SYMBOL "")
set(FortranCInterface_MODULE__PREFIX "")
set(FortranCInterface_MODULE__MIDDLE "")
set(FortranCInterface_MODULE__SUFFIX "")
set(FortranCInterface_MODULE__CASE   "")
set(FortranCInterface_MODULE__MACRO  "")

# Summarize what was found.
set(FortranCInterface_GLOBAL_FOUND 1)
set(FortranCInterface_MODULE_FOUND 0)

