#!/usr/bin/env python
# encoding: utf-8
##############################################################################
# Author: Liam Deacon                                                        #
#                                                                            #
# Contact: liam.m.deacon@gmail.com                                           #
#                                                                            #
# Copyright: Copyright (C) 2013-2015 Liam Deacon                             #
#                                                                            #
# License: MIT License                                                       #
#                                                                            #
# Permission is hereby granted, free of charge, to any person obtaining a    #
# copy of this software and associated documentation files (the "Software"), #
# to deal in the Software without restriction, including without limitation  #
# the rights to use, copy, modify, merge, publish, distribute, sublicense,   #
# and/or sell copies of the Software, and to permit persons to whom the      #
# Software is furnished to do so, subject to the following conditions:       #
#                                                                            #
# The above copyright notice and this permission notice shall be included in #
# all copies or substantial portions of the Software.                        #
#                                                                            #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    #
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    #
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        #
# DEALINGS IN THE SOFTWARE.                                                  #
#                                                                            #
##############################################################################

import sys
import os
import subprocess
import fnmatch
import time

from datetime import datetime
from glob import glob
from tempfile import gettempdir
import shutil
from shutil import copy2 as copy
from os.path import (
    splitext,
    isfile,
    exists,
    expandvars,
    expanduser,
    basename,
    isdir,
    dirname,
)
from distutils.errors import CompileError

try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

try:
    from numpy.distutils.core import ccompiler, Extension
    import numpy as np

    # from distutils.extension import Extension
    have_numpy = True
except ImportError:
    from distutils import ccompiler
    from distutils.extension import Extension

    have_numpy = False

try:
    from Cython.Build import BuildExecutable, cythonize
    import cython

    have_cython = True
except ImportError:
    have_cython = False
    # cythonize = lambda x: x

BUILD_LIBS = True
FIXES = True

if len(sys.argv) == 1:
    sys.argv.append("build_ext")

if FIXES:
    os.environ["PATH"] = ";".join(
        [r"C:\Qt\Qt5.4.0\Tools\mingw491_32\bin"]
        + [
            item
            for item in os.environ["PATH"].split(";")
            if "mingw" not in item.lower()
        ]
    )


unix_path = lambda x: x.replace("\\", "/")
join = lambda *x: unix_path(os.path.join(*x).replace("\\", "/"))


def recursive_glob(treeroot, pattern):
    results = []
    for base, dirs, files in os.walk(treeroot):
        goodfiles = fnmatch.filter(files, pattern)
        results.extend(os.path.join(base, f) for f in goodfiles)
    return results


# get current checked out (or local) version
try:
    releases = {"develop": ".dev", "stable": ".post"}
    release = lambda x: releases[x] if x in releases else ""

    import git

    repo = git.Repo(".git")
    head = repo.heads[0]
    date = time.strftime("%Y-%m-%d", time.gmtime(head.commit.committed_date))
    __date__ = date
    __branch__ = head.name
    __version__ = "{}{}".format(time.strftime("%Y.%m"), release(head.name))

except ImportError:
    try:
        head = "git symbolic-ref --short HEAD"
        proc = subprocess.Popen(
            'git show -s --format="%at"', stdout=subprocess.PIPE, stderr=None
        )
        date = int(proc.stdout.read())
        __date__ = datetime.fromtimestamp(date).strftime("%Y-%m-%d")
        __branch__ = subprocess.check_output(head.split())
        __version__ = "{}{}".format(
            datetime.fromtimestamp(date).strftime("%Y-%m"), release(__branch__)
        )
    except:
        # alpha releases if local
        __version__ = "{}.a".format(datetime.now().strftime("%Y-%m"))
        __branch__ = None
        __date__ = None


def find_libs(
    lib_aliases,
    lookup_dirs=[],
    cc=ccompiler.new_compiler(),
    search_environment=["LD_LIBRARY_PATH", "DYLD_LIBRARY_PATH", "PATH"],
):
    from distutils import ccompiler

    delim = ":" if sys.platform[:3] != "win" else ";"
    libdirs = list(lookup_dirs) or []
    libdirs += ["/usr/lib", "/usr/local/lib"] if sys.platform[:3] != "win" else []
    for var in search_environment:
        libdirs += os.environ.get(var, "").split(delim)
    return (
        list(
            set(
                [
                    cc.find_library_file(libdirs, lib)
                    for lib in lib_aliases
                    if cc.find_library_file(libdirs, lib) != None
                ]
            )
        )
        or None
    )


def find_include_dirs(
    header,
    lookup_dirs=[],
    search_environment=["INCLUDE", "C_INCLUDE_PATH", "CPLUS_INCLUDE_PATH", "CPATH"],
):
    delim = ":" if sys.platform[:3] != "win" else ";"
    lookups = []
    parent = (
        lambda x: dirname(x)
        if not isdir(join(dirname(dirname(x)), "include"))
        else join(dirname(dirname(x)), "include")
    )
    for lookup in lookup_dirs:
        if isdir(lookup):
            lookups.append(lookup)
        else:
            lookups.append(dirname(lookup))
            lookups += (
                ["/usr/include", "/usr/local/include"]
                if sys.platform[:3] != "win"
                else []
            )
    for var in search_environment:
        lookups += os.environ.get(var, "").split(delim)
    lookups += [join(dir, "include") for dir in lookups if isdir(join(dir, "include"))]
    locations = [lookup for lookup in lookups if isfile(join(lookup, header))]
    return list(
        set(
            locations
            + [lookup for lookup in lookups if isfile(join(lookup, basename(header)))]
        )
    )


def cc_has_flag(cc, flag, quiet=True):
    dummy = join(gettempdir(), "test_omp.c")
    with open(dummy, "w") as f:
        f.write(
            "int main() {\n"
            "  int i, j=10;\n"
            "%s"
            "  for(i=0; i<10; i++) {j+=i;};\n"
            "  return j;\n"
            "}\n" % ("  #pragma omp parallel for\n" if "openmp" in flag else "")
        )

    try:
        cc.link_executable(
            cc.compile([dummy], gettempdir(), extra_preargs=[flag]),
            splitext(dummy)[0],
            gettempdir(),
            extra_postargs=[flag],
        )
        retval = True
    except CompileError:
        retval = False
    finally:
        if isfile(splitext(dummy)[0]):
            os.remove(splitext(dummy)[0])
    if not quiet:
        print("cc has flag {} ... {}".format(flag, retval))
    return retval


# setup some initial parameters for compiling C source files ----
debug = True if "debug" in sys.argv else False
readme = "Readme.md"
config = {
    "BLAS": None,
    "LAPACK": None,
    "GSL": None,
    "Cython": True if have_cython else None,
    "Cairo": None,
    "NumPy": have_numpy,
}
lib_lookup = "lib" if sys.platform[:3] != "win" else "bin"

# helper functions ----
is_testfile = lambda x: basename(x).startswith("test_")
has_macro = lambda name, macros: any([name == key[0] for key in macros])
src_glob = lambda *x: [unix_path(y) for y in glob(join(*x))]

# setup C compiler ----
os.environ["CC"] = "ccache gcc"
cc = ccompiler.CCompiler
cc = ccompiler.new_compiler(compiler="mingw32", verbose=True)
cc.add_include_dir(join("src", "include"))
cc.add_library("m")

cc._extra_compile_args = [
    flag
    for flag in ("-std=gnu11", "-pedantic", "-Wextra", "-Wall", "-Wconversion")
    if cc_has_flag(cc, flag)
]

cc._extra_link_args = ["-fopenmp"] if cc_has_flag(cc, "-fopenmp") else []
cc._debug_args = ["-g3", "-O0"]  # if debug else []
cc.verbose = True

# ==================================================================
# get sources for LEED program library (non-symmetry version) -----
# ==================================================================
leed_srcs = [
    unix_path(src)
    for src in glob(join("src", "leed_nsym", "*.c"))
    if "cleed" not in src
    and not is_testfile(src)
    and not any(
        [src == splitext(s)[0] + ".c" for s in src_glob("src", "leed_nsym", "*.pyx")]
    )
]
rfac_srcs = [
    unix_path(src)
    for src in glob(join("src", "rfac", "*.c"))
    if not src.endswith("crfac.c")
    and not is_testfile(src)
    and not any(
        [src == splitext(s)[0] + ".c" for s in src_glob("src", "rfac", "*.pyx")]
    )
]
search_srcs = [
    unix_path(src)
    for src in glob(join("src", "search", "*.c"))
    if not src.endswith("csearch.c")
    and not is_testfile(src)
    and not any(
        [src == splitext(s)[0] + ".c" for s in src_glob("src", "search", "*.pyx")]
    )
]
ftsmooth_srcs = [
    unix_path(src)
    for src in glob(join("src", "ftsmooth", "*.c"))
    if not src.endswith("ftsmooth_main.c")
    and not is_testfile(src)
    and not any(
        [src == splitext(s)[0] + ".c" for s in src_glob("src", "ftsmooth", "*.pyx")]
    )
]
latt_srcs = [
    unix_path(src)
    for src in glob(join("src", "latt", "*.c"))
    if not src.endswith("latt.c")
    and not is_testfile(src)
    and not any(
        [src == splitext(s)[0] + ".c" for s in src_glob("src", "latt", "*.pyx")]
    )
]
patt_srcs = [
    unix_path(src)
    for src in glob(join("src", "patt", "*.c"))
    if not src.endswith("patt.c")
    and not is_testfile(src)
    and "pdf" not in src
    and not any(
        [src == splitext(s)[0] + ".c" for s in src_glob("src", "patt", "*.pyx")]
    )
]
mkiv_srcs = [
    unix_path(src)
    for src in glob(join("src", "mkiv", "*.c"))
    if src not in glob(join("src", "mkiv", "mkiv.c"))
    and join("src", "mkiv", "mkiv.c") != src
    and not is_testfile(src)
    and not any(
        [src == splitext(s)[0] + ".c" for s in src_glob("src", "mkiv", "*.pyx")]
    )
]

build_libs = {
    "leed": {
        "src_dir": join("src", "leed"),
        "sources": leed_srcs,
        "exe_sources": [join("src", "leed_nsym", "cleed_nsym.c")],
        "ext_sources": src_glob("src", "leed_nsym", "*.pyx"),
        "scripts": [join("src", "leed_nsym", "cleed.py")],
        "shared_objects": [],  # intentionally blank
    },
    "rfac": {
        "src_dir": join("src", "rfac"),
        "sources": rfac_srcs,
        "exe_sources": [join("src", "rfac", "crfac.c")],
        "ext_sources": src_glob("src", "rfac", "*.pyx"),
        "scripts": [join("src", "rfac", "crfac.py")],
        "shared_objects": [],  # intentionally blank
    },
    "search": {
        "src_dir": join("src", "search"),
        "sources": search_srcs,
        "exe_sources": [join("src", "search", "csearch.c")],
        "ext_sources": src_glob("src", "search", "*c_search.pyx"),
        "scripts": [join("src", "search", "csearch.py")],
        "shared_objects": [],  # intentionally blank
    },
    "ftsmooth": {
        "src_dir": join("src", "ftsmooth"),
        "sources": ftsmooth_srcs,
        "exe_sources": [join("src", "ftsmooth", "ftsmooth_main.c")],
        "ext_sources": src_glob("src", "ftsmooth", "*.pyx"),
        "scripts": [join("src", "ftsmooth", "ftsmooth.py")],
        "shared_objects": [],  # intentionally blank
    },
    "latt": {
        "src_dir": join("src", "latt"),
        "sources": latt_srcs,
        "exe_sources": [join("src", "latt", "latt.c")],
        "ext_sources": src_glob("src", "latt", "*.pyx"),
        "scripts": [join("src", "latt", "latt.py")],
        "shared_objects": [],  # intentionally blank
    },
    "patt": {
        "src_dir": join("src", "patt"),
        "sources": patt_srcs,
        "exe_sources": [join("src", "patt", "patt.c")],
        "ext_sources": src_glob("src", "patt", "*.pyx"),
        "scripts": [join("src", "patt", "patt.py")],
        "shared_objects": [],  # intentionally blank
    },
    "mkiv": {
        "src_dir": join("src", "mkiv"),
        "sources": mkiv_srcs,
        "exe_sources": [join("src", "mkiv", "mkiv.c")],
        "ext_sources": src_glob("src", "mkiv", "*.pyx"),
        "scripts": [join("src", "mkiv", "mkiv.py")],
        "shared_objects": [],  # intentionally blank
    },
}

# =============================================================
# find support development libraries -------------------------
# =============================================================
if "--with-gsl" not in sys.argv or "--with-gsl=0" in sys.argv:
    for lib_name in build_libs:
        build_libs[lib_name]["sources"] = [
            src for src in build_libs[lib_name]["sources"] if "gsl" not in src
        ]
    cc.define_macro("USE_GSL", 0)
else:
    paths = [
        os.environ[env]
        for env in ("GSL_ROOT_DIR", "GSL_HOME", "GSL_DIR")
        if env in os.environ
    ]
    if not any([lambda x: exists(expandvars(expanduser(x))) for x in paths]):
        raise EnvironmentError(
            "$GSL_HOME environment variable not set "
            "in user environment - please remove "
            '"--with-gsl" or set $GSL_HOME'
        )
    else:
        gsl_root = [expandvars(expanduser(x)) for x in paths if exists(x)][0]

    cc.add_include_dir(join(gsl_root, "include"))
    cc.add_library_dir(join(gsl_root), lib_lookup)
    cc.define_macro("USE_GSL", 1)

if "--with-blas" in sys.argv:
    try:
        blas_info = np.__config__.show("blas")
        if blas_info == {}:
            raise Exception("Cannot get BLAS config")
        config["BLAS"] = blas_info
        cc.define_macro("USE_BLAS", 1)
    except:
        cc.define_macro("USE_BLAS", 0)
else:
    cc.define_macro("USE_BLAS", 0)

if "--with-cairo" in sys.argv or "with_cairo" in sys.argv or "with-cairo=1" in sys.argv:
    cc.define_macro("USE_CAIRO", 1)
else:
    cc.define_macro("USE_CAIRO", 0)

if ("USE_CAIRO", 0) in cc.macros:
    # remove cairo related source files
    if "patt" in build_libs:
        build_libs["patt"]["sources"] = [
            src for src in build_libs["patt"]["sources"] if "cairo" not in src
        ]


if sys.platform[:3] != "win":
    leed_srcs.pop("getrusage_win32.c")
else:
    cc.add_include_dir(join("src", "leed_nsym"))

# mkiv support needs libTIFF
if sys.platform[:3] == "win":
    cc.add_library("ws2_32")
    cc.add_library("psapi")
    cc.add_include_dir(join("lib", "win32", "libtiff", "include"))
    build_libs["mkiv"]["shared_objects"] += [
        unix_path(dll) for dll in glob(join("lib", "win32", "libtiff", "bin", "*.dll"))
    ]

    if has_macro("USE_BLAS", cc.macros):
        cc.undefine_macro("USE_BLAS")
    cc.add_include_dir(join("lib", "win32"))
    build_libs["search"]["shared_objects"] += [
        join("lib", "win32", "libblas.dll"),
        join("lib", "win32", "liblapacke.dll"),
        join("lib", "win32", "liblapack.dll"),
    ]
    cc.define_macro("USE_BLAS", 1)
    cc.define_macro("USE_LAPACKE", 1)
else:
    tiff_libs = find_libs(["tiff", "tiff3", "libtiff3", "libtiff"], [], cc)
    tiff_includes = find_include_dirs(
        "tiff.h", tiff_libs + [join("lib", "win32", "libtiff")]
    )
    if tiff_includes:
        cc.add_include_dir(tiff_includes[0])
        cc.add_link_object(tiff_libs[0])
    else:
        del build_libs["mkiv"]

build_dir = "build"
shared_libraries = []
executables = []

# build extras ----
if "build_extras" in sys.argv:
    print(
        "\n{}\nCLEED pre-build config:\n{}\n{}\n".format(
            "=" * 30,
            "=" * 30,
            "\n".join(["{}: {}".format(option, config[option]) for option in config]),
        )
    )
    print(
        30 * "-"
        + "\nCompiler setup:\n"
        + 30 * "-"
        + "\n"
        "c compiler: {}\n"
        "linker: {}\n"
        "cpp macros:\n  {}\n"
        "include dirs:\n  {}\n"
        "library_dirs:\n  {}\n"
        "libraries:\n  {}\n"
        "objects:\n  {}\n"
        "".format(
            cc.compiler[0],
            cc.linker,
            "\n  ".join(["{}={}".format(*macro) for macro in cc.macros])
            if cc.macros
            else None,
            "\n  ".join([inc for inc in cc.include_dirs]) if cc.include_dirs else None,
            "\n  ".join([lib_dir for lib_dir in cc.library_dirs])
            if cc.library_dirs
            else None,
            "\n  ".join([lib for lib in cc.libraries]) if cc.libraries else None,
            "\n  ".join([obj for obj in cc.objects]) if cc.objects else None,
        )
    )

    # build objects
    n = 0
    i = 1
    for lib_name in build_libs:
        n += len(build_libs[lib_name]["sources"])

    print("\nBuilding objects...")

    for lib_name in build_libs:
        # build_dir = join('build', lib_name)
        # cc.mkpath(build_dir)
        try:
            build_libs[lib_name]["objects"] = []
            for src in build_libs[lib_name]["sources"]:
                print("{} of {}: compiling {}...".format(i, n, src))
                obj = cc.compile(
                    sources=[src],
                    output_dir=build_dir,
                    debug=debug,
                    extra_preargs=None,
                    extra_postargs=cc._extra_compile_args,
                    depends=None,
                )
                build_libs[lib_name]["objects"] += obj
                i += 1
        except CompileError:
            exit()

    print("\nLinking shared libraries...")

    for lib_name in build_libs:
        lib_basename = str("lib" + lib_name + cc.shared_lib_extension)
        build_libs[lib_name]["library"] = join(build_dir, lib_basename)
        cc.link(
            cc.SHARED_LIBRARY,
            build_libs[lib_name]["objects"],
            lib_basename,
            output_dir=build_dir,
            debug=debug,
            libraries=None,
            library_dirs=None,
            runtime_library_dirs=None,
            export_symbols=None,
            extra_preargs=None,
            extra_postargs=None,
            build_temp=None,
            target_lang=None,
        )
        print(basename(build_libs[lib_name]["library"]))

    print("\nCreating executables...")

    for lib in build_libs:
        # cc.add_link_object(build_libs[lib]['library'])
        shared_libraries.append(build_libs[lib]["library"])

    for lib_name in build_libs:
        exe = lib_name + cc.exe_extension
        build_libs[lib_name]["library"] = join(build_dir, exe)
        objs = build_libs[lib_name]["objects"]
        cc.link_executable(
            [
                unix_path(o)
                for o in cc.compile(build_libs[lib_name]["exe_sources"], build_dir)
            ]
            + [unix_path(o) for o in objs],
            lib_name,
            build_dir,
            extra_preargs=build_libs[lib_name]["shared_libraries"],
            extra_postargs=["-Wl,--allow-multiple-definition"],
        )
        executables.append(build_libs[lib_name]["library"])
        print(exe)


if "build_extras" in sys.argv and "install" in sys.argv:
    # ================================================================
    # Add CLEED environment and update binaries to be executable ----
    # ================================================================
    cleed_home = os.path.join(sys.prefix, "cleed")
    cleed_env = {
        "CLEED_HOME": cleed_home,
        "CLEED_PHASE": os.path.join(cleed_home, "phase"),
        "CSEARCH_LEED": os.path.join(cleed_home, "bin", "leed"),
        "CSEARCH_RFAC": os.path.join(cleed_home, "bin", "rfac"),
    }
    if sys.platform[:3] == "win":
        print("\nSetting Registry values")
        for env_var in cleed_env:
            print("setting {}={}".format(env_var, cleed_env[env_var]))
            subprocess.Popen('SETX {0} "{1}"'.format(env_var, cleed_env[env_var]))

        paths = os.environ["PATH"].split(";")
        path = [
            p
            for p in paths
            if not p.startswith("%CLEED_HOME%") and "cleed" not in p.lower()
        ]
        # os.system('SETX PATH "{};%CLEED_HOME%\\bin"'.format(';'.join(path)))
        subprocess.Popen(
            'REG ADD "HKEY_CURRENT_USER\SYSTEM\CurrentControlSet\Control'
            '\Session Manager\Environment" /v PATH /t REG_SZ /f /d '
            '"{};%CLEED_HOME%"'.format(";".join(path))
        )

        for exe in executables:
            bat = os.path.join(build_dir, splitext(basename(exe))[0] + ".bat")
            with open(bat, "w") as f:
                f.write(
                    '"{}" %*\n\r'.format(
                        os.path.join(sys.prefix, "Scripts", basename(exe))
                    )
                )
            dst = os.path.join(cleed_home, "bin", basename(exe))
            print("moving {} -> {}".format(bat, dirname(dst)))
            if os.path.isfile(dst):
                os.remove(dst)
            try:
                shutil.move(bat, dirname(dst))
            except:
                copy(bat, dirname(dst))
    else:
        with open(expanduser("~/.bashrc"), "r") as f:
            lines = [line for line in f]

        for var in cleed_env:
            for i, line in enumerate(lines):
                if var in line:
                    segments = line.split(";")
                    for j, section in enumerate(segments):
                        if var in section:
                            segments[j] = "export {0}={1}".format(var, cleed_env[var])
                    lines[i] = ";".join(segments)
                    lines[i] += "\n" if "\n" not in lines[i] else ""

            if all([var not in line for line in lines]):
                lines.append("export {0}={1}\n".format(var, cleed_env[var]))

        with open(expanduser("~/.bashrc"), "w") as f:
            f.write("".join(lines))

        # add symbolic links instead of appending to PATH on linux
        if 0:  # probably not required here as shared libraries should all be okay
            for exe in executables:
                os.system(
                    "ln -s {0}/bin/{1} /usr/local/bin/{1}".format(
                        join(sys.prefix, "scripts"), basename(exe)
                    )
                )

        # stop argument error for distutils/setuptools script
        sys.argv.remove("build_extras")

# =================================
# python packaging ----
# =================================
extensions = []
python_scripts = []
for lib_name in build_libs:
    sources = (
        build_libs[lib_name]["sources"]
        + build_libs[lib_name]["exe_sources"]
        + build_libs[lib_name]["ext_sources"]
    )

    if lib_name == "search":
        sources.append(join("src", "leed_nsym", "linpdebtemp.c"))

    extensions.append(
        Extension(
            "cleed.c_" + lib_name,
            sources,
            include_dirs=cc.include_dirs  # *.pyd directories
            + [".", join("src", lib_name), join("src", "include")],
            libraries=cc.libraries,
            library_dirs=cc.library_dirs,
            define_macros=cc.macros + [("main", "{}_main".format(lib_name))],
            extra_objects=cc.objects,
            extra_compile_args=cc._extra_compile_args,
            extra_link_args=cc._extra_link_args
            + build_libs[lib_name]["shared_objects"]
            + ["-Wl,--allow-multiple-definition"],
            language="c",
        )
    )
    python_scripts.extend(build_libs[lib_name]["scripts"])

dlls = []
for obj in cc.objects:
    copy(obj, build_dir)
    dlls.append(join(build_dir, basename(obj)))

data_files = [
    ("cleed", cc.objects),
    ("cleed/bin", executables + glob("build/*.dll") + glob("build/*.so")),
    ("cleed/include", glob("src/include/*.h")),
    ("cleed/phase", glob("data/phase/*.phs")),
    ("cleed/man", glob("man/*")),
    ("cleed/man", glob("man/*")),
    ("cleed/scripts", glob("src/scripts/*")),
    (
        "cleed/doc",
        recursive_glob("doc/", "*CLEED*.pdf") + recursive_glob("doc/", "*.rst"),
    ),
    #'lib': [''],
    #'bin': ['']
]

dist = setup(
    name="cleed",
    package_dir={"cleed": "src"},
    packages=["cleed"],
    version=__version__,
    author="Liam Deacon, Georg Held and collaborators",
    author_email="liam.m.deacon@gmail.com",
    license="GPLv3+ License",
    url="https://pypi.python.org/pypi/cleed",
    description="LEED-IV calculation and analysis",
    long_description=(open(readme).read() if os.path.exists(readme) else None),
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Environment :: Console",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: GNU General Public " "License v3 or later (GPLv3+)",
        "Operating System :: OS Independent",
        "Programming Language :: C",
        "Programming Language :: Cython",
        "Programming Language :: Python :: " "Implementation :: CPython",
        "Topic :: Scientific/Engineering :: Chemistry",
        "Topic :: Scientific/Engineering :: Physics",
    ],
    keywords=["LEED IV", "physics", "chemistry", "materials", "diffraction"],
    # recursive-include *.py *.pyw *.pyx *.pyd
    include_package_data=True,
    # If any package contains *.txt or *.rst files, include them:
    # package_data=data_files,
    # data_files=data_files,
    scripts=python_scripts,
    # data_files = cython_exts,
    install_requires=["numpy >= 1.3", "phaseshifts", "cython", "gitpython"],
    ext_modules=cythonize(extensions),
)

sys.argv.append("build_ext")
sys.argv.append("--inplace")
