#! /usr/bin/bash

##############################################################################
# Author: Liam Deacon                                                        #
#                                                                            #
# Contact: liam.deacon@diamond.ac.uk                                         #
#                                                                            #
# Created on 9 May 2014                                                      #
#                                                                            #
# Copyright: Copyright (C) 2014 Liam Deacon                                  #
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

# initialise variables
check=0
verbose=0
caoi_leed_exe="cleed_aoi"
caoi_rfac_exe="cleed_rfac"
cleed_nsym_exe="cleed_nsym"
cleed_sym_exe="cleed_sym"
crfac_exe="crfac"
csearch_exe="csearch"
phsh_exe="phsh.py"
input_file=""
output_file="cleed.ini"
ext=""
env=""
variables="CLEED_HOME CLEED_PHASE"

# program information
__version__=0.1
__date__='2014-05-09'
__updated__='2014-05-09'
__contact__='liam.deacon@diamond.ac.uk'

program_name="set_env.sh"
program_version="- $__versions__"
program_build_date="$__updated__"
program_version_message="$program_name $program_version ($program_build_date)"
program_shortdesc="set_env -- setup CLEED environment\n"
program_longdesc="set_env -- setup CLEED environment

set_env sets the environment in which CLEED is run. Program paths are checked 
in the order: command line METAVAR > SYSTEM PATH > CLEED_HOME."
program_license="
Created by Liam Deacon on $__date__.
Copyright 2014 Diamond Light Source Ltd. All rights reserved.

Licensed under the Apache License 2.0
http://www.apache.org/licenses/LICENSE-2.0

Distributed on an \"AS IS\" basis without warranties
or conditions of any kind, either express or implied.

Please send your feedback, including bug notifications 
and fixes to: $__contact__

usage:-
"
program_usage="usage: set_env.py [-h] [-c] [-x <ext> [<ext> ...]] [-e <env> [<env> ...]]
                  [-i <ini_file>] [-o <ini_file>] [--aoi-leed <AOI_LEED>]
                  [--aoi-rfac <AOI_RFAC>] [--leed <LEED>] [--search <SEARCH>]
                  [--sym <LEED_SYM>] [--rfac <RFAC>] [--phsh <PHSH>] [-v] [-V]
"
program_optional_arguments="optional arguments:
  -h, --help            show this help message and exit
  -c, --check           perform post-processing checks to ensure all paths and
                        executables are set correctly
  -x <ext> [<ext> ...], --ext <ext> [<ext> ...]
                        set enabled extensions; can be any of 'aoi', 'sym' or
                        'phsh'. Note: Multiple extensions can be enabled
                        simultaneously.
  -e <env> [<env> ...], --env <env> [<env> ...]
                        set custom environment values of the form:
                        \"<var>:<value>\"
  -i <ini_file>, --input <ini_file>
                        load custom environment from <ini_file>.
  -o <ini_file>, --output <ini_file>
                        save custom environment to <ini_file> [default:
                        \"cleed.ini\"]. Note <ini_file> is always written
  --aoi-leed <AOI_LEED>
                        path to angle of incidence LEED program wrapper
                        [default: \"$caoi_leed_exe\"]
  --aoi-rfac <AOI_RFAC>
                        path to angle of incidence R-Factor wrapper program
                        [default: \"$caoi_rfac_exe\"]
  --leed <LEED>         path to LEED program [default: \"$cleed_nsym_exe\"]
  --search <SEARCH>     path to search program [default: \"$csearch_exe\"]
  --sym <LEED_SYM>      path to symmetrised LEED program [default:
                        \"$cleed_sym_exe\"]
  --rfac <RFAC>         path to R-Factor program [default: \"$crfac_exe\"]
  --phsh <PHSH>         path to phase shifts program [default: \"$phsh_exe\"]
  -v, --verbose         print verbose output [default: $verbose]
  -V, --version         show program's version number and exit
"

# parse command line arguments
argc=0;
for arg in $@; do
  argv[$argc]=$arg
  let argc+=1
done

# functions
echoerr() { echo "***error (set_env): $@" 1>&2; }

cfg_parser ()
{
    ini="$(<$1)"                # read the file
    ini="${ini//[/\[}"          # escape [
    ini="${ini//]/\]}"          # escape ]
    IFS=$'\n' && ini=( ${ini} ) # convert to line-array
    ini=( ${ini[*]//;*/} )      # remove comments with ;
    ini=( ${ini[*]/\    =/=} )  # remove tabs before =
    ini=( ${ini[*]/=\   /=} )   # remove tabs be =
    ini=( ${ini[*]/\ =\ /=} )   # remove anything with a space around =
    ini=( ${ini[*]/#\\[/\}$'\n'cfg_section_} ) # set section prefix
    ini=( ${ini[*]/%\\]/ \(} )  # convert text2function (1)
    ini=( ${ini[*]/=/=\( } )    # convert item to array
    ini=( ${ini[*]/%/ \)} )     # close array parenthesis
    ini=( ${ini[*]/%\\ \)/ \\} )# the multiline trick
    ini=( ${ini[*]/%\( \)/\(\) \{} ) # convert text2function (2)
    ini=( ${ini[*]/%\} \)/\}} ) # remove extra parenthesis
    ini[0]=""                   # remove first element
    ini[${#ini[*]} + 1]='}'     # add the last brace
    eval "$(echo "${ini[*]}")"  # eval the result
}
 
cfg_writer ()
{
    IFS=' '$'\n'
    fun="$(declare -F)"
    fun="${fun//declare -f/}"
    for f in $fun; do
        [ "${f#cfg_section}" == "${f}" ] && continue
        item="$(declare -f ${f})"
        item="${item##*\{}"
        item="${item%\}}"
        item="${item//=*;/}"
        vars="${item//=*/}"
        eval $f
        echo "[${f#cfg_section_}]"
        for var in $vars; do
            echo $var=\"${!var}\"
        done
    done
}

# loop through args
i=0; while [ $i -lt $argc ]; do

    if [ ${argv[$i]} == "-c" ] || [ ${argv[$i]} == "--check" ]; then
        check=1
    fi

    if [ ${argv[$i]} == "-e" ] || [ ${argv[$i]} == "--env" ]; then
        let i+=1
        j=0
        while [ ${argv[$i]} != "-*" && $i -lt $argc ]; do 
            variable[$j]=`echo ${argv[$i]}|cut -d ':' -f1`
            value[$j]=`echo ${argv[$i]}|cut -d ':' -f2-`
            env="$env ${variable[$j]}"
            let i++
            let j++
        done
    fi
    
    if [ ${argv[$i]} == "-x" ] || [ ${argv[$i]} == "--ext" ]; then
        let i+=1
        while [ ${argv[$i]} != "-*" && $i -lt $argc ]; do 
            ext="$ext ${argv[$i]}"
            let i+=1
        done
    fi
    
    if [ ${argv[$i]} == "-i" ] || [ ${argv[$i]} == "--input" ]; then
        let i+=1
        if [ $i -lt $argc ]; then
            if [ -f ${argv[$i]} ]; then
                input_file="${argv[$i]}"
                cfg_parser=`cat "$input_file|sed 's/^[ \t]*//;s/[ \t]*$//'"`
                lines=`echo $cfg_parser|wc -l`
                if [ `echo $cfg_parser|grep "^\["|wc -l` ];then
                    if [ `echo $cfg_parser|grep "^[ENVIRONMENT]"|wc -l` ] ]; then
                        line_number=`echo $cfg_parser|grep -n "^[ENVIRONMENT]"|cut -d ":" -f1`
                        line_text="`echo $cfg_parser|sed -n -e ${line_number}p`"
                        let line_number++;
                        while [ $line_number -lt $lines ]; do
                            line_text="`echo $cfg_parser|sed -n -e ${line_number}p`"
                            if [ `echo $line_text | grep ^[^#\;\[]|wc -l` ]; then
                                cfg_parser="`echo $cfg_parser|sed -e "${line_number}d"`"
                                let lines--
                            else
                                let line_number++
                            fi
                        done
                    fi
                else  # assume all values are environment variables
                    for line in `echo $cfg_parser | grep ^[^#\;]`;do
                        var=`echo ${argv[$i]}|cut -d ':' -f1`
                        val=`echo ${argv[$i]}|cut -d ':' -f2-`
                        eval $var="$val"
                        variables="$variables $var"
                    done
                fi
            else
                echoerr "file '${argv[$i]}' not found"
            fi
        fi
    fi

    if [ ${argv[$i]} == "-o" ] || [ ${argv[$i]} == "--output" ]; then
        let i+=1
        if [ $i -lt $argc ]; then
            if [ -w ${argv[$i]} ]; then
                output_file="${argv[$i]}"
            else
                echoerr "'${argv[$i]}' is not writable"
            fi
        fi
    fi
    
    if [ ${argv[$i]} == "--aoi-leed" ]; then
        let i+=1
        if [ $i -lt $argc ]; then
            if [ ! -f "${argv[$i]}" ]; then
                echoerr "'${argv[$i]}' is not a valid file 
                         (using default: $caoi_leed_exe)"
            elif [ ! -x "${argv[$i]}" ]; then
                echoerr "'${argv[$i]}' is not executable 
                         (using default: $caoi_leed_exe)"
            else
                caoi_leed_exe="${argv[$i]}"
            fi
        fi
    fi
    
    if [ ${argv[$i]} == "--aoi-rfac" ]; then
        let i+=1
        if [ $i -lt $argc ]; then
            if [ ! -f "${argv[$i]}" ]; then
                echoerr "'${argv[$i]}' is not a valid file 
                         (using default: $caoi_rfac_exe)"
            elif [ ! -x "${argv[$i]}" ]; then
                echoerr "'${argv[$i]}' is not executable 
                         (using default: $caoi_rfac_exe)"
            else
                caoi_rfac_exe="${argv[$i]}"
            fi
        fi
    fi
    
    if [ ${argv[$i]} == "--leed" ]; then
        let i+=1
        if [ $i -lt $argc ]; then
            if [ ! -f "${argv[$i]}" ]; then
                echoerr "'${argv[$i]}' is not a valid file 
                         (using default: $cleed_nsym_exe)"
            elif [ ! -x "${argv[$i]}" ]; then
                echoerr "'${argv[$i]}' is not executable 
                         (using default: $cleed_nsym_exe)"
            else
                cleed_nsym_exe="${argv[$i]}"
            fi
        fi

    fi
    
    if [ ${argv[$i]} == "--search" ]; then
        let i+=1
        if [ $i -lt $argc ]; then
            if [ ! -f "${argv[$i]}" ]; then
                echoerr "'${argv[$i]}' is not a valid file 
                         (using default: $csearch_exe)"
            elif [ ! -x "${argv[$i]}" ]; then
                echoerr "'${argv[$i]}' is not executable 
                         (using default: $csearch_exe)"
            else
                csearch_exe="${argv[$i]}"
            fi
        fi
    fi
    
    if [ ${argv[$i]} == "--sym" ]; then
        let i+=1
        if [ $i -lt $argc ]; then
            if [ ! -f "${argv[$i]}" ]; then
                echoerr "'${argv[$i]}' is not a valid file 
                         (using default: $cleed_sym_exe)"
            elif [ ! -x "${argv[$i]}" ]; then
                echoerr "'${argv[$i]}' is not executable 
                         (using default: $cleed_sym_exe)"
            else
                cleed_sym_exe="${argv[$i]}"
            fi
        fi
    fi

    if [ ${argv[$i]} == "--rfac" ]; then
        let i+=1
        if [ $i -lt $argc ]; then
            if [ ! -f "${argv[$i]}" ]; then
                echoerr "'${argv[$i]}' is not a valid file 
                        (using default: $crfac_exe)"
            elif [ ! -x "${argv[$i]}" ]; then
                echoerr "'${argv[$i]}' is not executable 
                        (using default: $crfac_exe)"
            else
                crfac_exe="${argv[$i]}"
            fi
        fi
    fi
    
    if [ ${argv[$i]} == "-v" ] || [ ${argv[$i]} == "--verbose" ]; then
        verbose=1
    fi
    
    if [ ${argv[$i]} == "-V" ] || [ ${argv[$i]} == "--version" ]; then
        printf "%s\n" "$program_version_message"
        exit 0
    fi
    
    if [ ${argv[$i]} == "-h" ] || [ ${argv[$i]} == "--help" ]; then
        printf "%s\n%s\n%s\n%s\n" "$program_usage" "$program_longdesc" \
               "$program_license" "$program_optional_arguments"
        exit 0
    fi
    
    let i+=1
done 

# process env values
for (j=0, j <= ${#variable[@]}, j++);do
    eval ${variable[$j]}="${value[$j]}"
    variables="$variables ${variable[$j]}"
done
variables=`for var in $variables;do echo "${!var}";done | \
           awk '!a[$0]++' | tr "\n" " "`

# set default environment variables
if [ "$CLEED_HOME" == "" ]; then
    echoerr "CLEED_HOME not set (using default: '/usr/local/CLEED')"
    CLEED_HOME=/usr/local/CLEED
elif [ ! -d "$CLEED_HOME" ]; then
    echoerr "'$CLEED_HOME' not a valid CLEED_HOME directory 
             (using default: '/usr/local/CLEED')"
    CLEED_HOME=/usr/local/CLEED
fi
    
if [ "$CLEED_PHASE" == "" ]; then
    echoerr "CLEED_HOME not set (using default: '/usr/local/CLEED')"
    CLEED_PHASE="$CLEED_HOME/phase"
elif [ ! -d "$CLEED_PHASE" ]; then
    echoerr "'$CLEED_PHASE' not a valid CLEED_PHASE directory 
             (using default: '\$CLEED_HOME/phase')"
fi

# determine whether to enable extensions
ext=`for var in $ext;do echo "${!var}";done | awk '!a[$0]++' | tr "\n" " "`
if [ "$ext" != "" ]; then
    aoi=0
    sym=0
    phsh=0
    
    echo
    echo "===================================================================="
    echo "Testing extensions:"
    echo "===================================================================="
    
    exts=""
    for var in $ext; do
        if [ "$var" == "aoi" && ! aoi ]; then 
            aoi_leed=0
            aoi_rfac=0
            
            if [ -x "$caoi_leed_exe" ]; then 
                caoi_leed_exe="$caoi_leed_exe"
                echo "caoi_leed -> $caoi_leed_exe"  
                aoi_leed=1
            elif [ -x `which caoi_leed|head -1` ]; then
                caoi_leed_exe=`which caoi_leed|head -1`
                echo "caoi_leed -> $caoi_leed_exe"
                aoi_leed=1
            else
                echoerr "Unable to find 'caoi_leed' - 
                         angle of incidence extension disabled"
            fi
            
            if [ -x "$caoi_rfac_exe" ]; then 
                caoi_leed_exe="$caoi_rfac_exe"
                echo "caoi_rfac -> $caoi_rfac_exe"  
                aoi_rfac=1
            elif [ -x `which caoi_rfac|head -1` ]; then
                caoi_leed_exe=`which caoi_rfac|head -1`
                echo "caoi_rfac -> $caoi_rfac_exe"
                aoi_rfac=1
            else
                echoerr "Unable to find 'caoi_rfac' - 
                         angle of incidence extension disabled"
            fi
            
            if [ aoi_leed && aoi_rfac ]; then
                exts="$var $exts"
                aoi=1;
            fi
        fi
        if [ "$var" == "sym" && ! sym ]; then 
            if [ -x "$cleed_sym_exe" ]; then 
                cleed_sym_exe="$cleed_sym_exe"
                echo "cleed_sym -> $cleed_sym_exe"
                exts="$var $exts"
                sym=1;        
            elif [ -x "`which cleed_sym|head -1`" ]; then
                cleed_sym_exe="`which cleed_sym|head -1`"
                echo "cleed_sym -> $cleed_sym_exe"
                exts="$var $exts"
                sym=1;
            else
                echoerr "Unable to find 'cleed_sym' - 
                         symmetry extension disabled"
            fi
        fi
        if [ "$var" == "phsh" && ! phsh ]; then
            if [ -x "$phsh_exe" ]; then 
                phsh_exe="$phsh_exe"
                echo "phsh -> $phsh_exe"
                exts="$var $exts"
                phsh=1;        
            elif [ -x "`which phsh.py|head -1`" ]; then
                phsh_exe="`which phsh.py|head -1`"
                echo "phsh -> $phsh_exe"
                exts="$var $exts"
                phsh=1;
            else
                echoerr "Unable to find 'phsh.py' - 
                         phaseshifts extension disabled"
            fi
        fi
    done
    ext="$exts"
    
    if [ aoi && sym && phsh ]; then
        CSEARCH_LEED="$phsh_exe"
        CAOI_LEED="$cleed_sym_exe"
        CAOI_RFAC="$crfac_exe"
        CSEARCH_RFAC="$caoi_rfac_exe"
        PHASESHIFTS_LEED="$caoi_leed_exe"
        
        variables="$variables CSEARCH_LEED CAOI_LEED CAOI_RFAC 
                              CSEARCH_RFAC PHASESHIFTS_LEED"
        prog_cycle="CSEARCH_LEED PHASESHIFTS_LEED 
                    CAOI_LEED CAOI_RFAC CSEARCH_RFAC"
    elif [ aoi && sym ]; then
        CSEARCH_LEED="$caoi_leed_exe"
        CAOI_LEED="$cleed_sym_exe"
        CAOI_RFAC="$crfac_exe"
        CSEARCH_RFAC="$caoi_rfac_exe"
        
        variables="$variables CSEARCH_LEED CAOI_LEED CAOI_RFAC CSEARCH_RFAC"
        prog_cycle="CSEARCH_LEED CAOI_LEED CAOI_RFAC CSEARCH_RFAC"
    elif [ aoi && phsh ]; then
        CSEARCH_LEED="$phsh_exe"
        CAOI_LEED="$cleed_nsym_exe"
        CAOI_RFAC="$crfac_exe"
        CSEARCH_RFAC="$caoi_rfac_exe"
        PHASESHIFTS_LEED="$caoi_leed_exe"
        
        variables="$variables CSEARCH_LEED CAOI_LEED CAOI_RFAC 
                              CSEARCH_RFAC PHASESHIFTS_LEED"
        prog_cycle="CSEARCH_LEED PHASESHIFTS_LEED 
                    CAOI_LEED CAOI_RFAC CSEARCH_RFAC"       
    elif [ sym && phsh ]; then
        CSEARCH_LEED="$phsh_exe"
        CAOI_LEED="$cleed_sym_exe"
        CAOI_RFAC="$crfac_exe"
        CSEARCH_RFAC="$caoi_rfac_exe"
        PHASESHIFTS_LEED="$caoi_leed_exe"
        
        variables="$variables CSEARCH_LEED CAOI_LEED CAOI_RFAC 
                              CSEARCH_RFAC PHASESHIFTS_LEED"
        prog_cycle="CSEARCH_LEED PHASESHIFTS_LEED 
                    CAOI_LEED CAOI_RFAC CSEARCH_RFAC"    
    elif [ aoi ]; then
        CSEARCH_LEED="$caoi_leed_exe"
        CAOI_LEED="$cleed_nsym_exe"
        CAOI_RFAC="$crfac_exe"
        CSEARCH_RFAC="$caoi_rfac_exe"   
        
        variables="$variables CSEARCH_LEED CAOI_LEED CAOI_RFAC CSEARCH_RFAC"
        prog_cycle="CSEARCH_LEED CAOI_LEED CAOI_RFAC CSEARCH_RFAC"
    elif [ sym ]; then
        CSEARCH_LEED="$cleed_sym_exe"
        CSEARCH_RFAC="$crfac_exe"
        
        variables="$variables CSEARCH_LEED CSEARCH_RFAC"
        prog_cycle="CSEARCH_LEED CSEARCH_RFAC"
    elif [ phsh ]; then
        CSEARCH_LEED="$phsh_exe"
        CSEARCH_RFAC="$crfac_exe"
        PHASESHIFTS_LEED="$cleed_nsym_exe"
        
        variables="$variables CSEARCH_LEED PHASESHIFTS_LEED CSEARCH_RFAC"
        prog_cycle="CSEARCH_LEED PHASESHIFTS_LEED CSEARCH_RFAC"    
    else  # default
        CSEARCH_LEED="$cleed_nsym_exe"
        CSEARCH_RFAC="$crfac_exe"
    
        variables="$variables CSEARCH_LEED CSEARCH_RFAC"
        prog_cycle="CSEARCH_LEED CSEARCH_RFAC"    
    fi
    
fi

# remove duplicate variables
variables=`for var in $variables;do echo "${!var}";done | \
           awk '!a[$0]++' | tr "\n" " "`
echo
echo "===================================================================="
echo "CLEED Environment: $ext"
echo "===================================================================="
for var in $variables; do
    echo "$var=${!var}"
done
echo
echo "Note: For older versions of CLEED paths should be encapsulated
 in double quotation marks or else spaces, etc. need to be escaped"

 
if [ check ]; then  # check everything is valid

    echo
    echo "===================================================================="
    echo "Checking setup:"
    echo "===================================================================="
    echo "Program iteration loop is as follows:"
    i=1
    for var in $prog_cycle;do
        if [ "`echo "${!var}"`" == "" ];then
            echoerr "'$var' not in environment!"
        elif [ -x "`echo "${!var}"`" ]; then
            echo "   $i. $var (\"${!var}\")"
        else
            echoerr "'${!var}' not found"
        fi
        let i++
    done

fi

if [ verbose ]; then 
    printf "%s\n;CLEED environment values set by: set_env.sh\n[ENVIRONMENT]\n" "$config_parser" > "$output_file"
    for var in $variables; do echo "$var=${!var}" >> "$output_file";done
    echo "Wrote environment to INI file '$output_file'"
fi 
 
#export CLEED_HOME=~/CLEED                       # home directory for CLEED
#export CLEED_PHASE=$CLEED_HOME/phase            # path to phase shift files
#export CSEARCH_LEED=$CLEED_HOME/bin/cleed_nsym  # path to csym or cnsym executables
#export CSEARCH_RFAC=$CLEED_HOME/bin/crfac       # path to crfac executable

# export variables to parent shell
for var in $variables; do
    export $var="${!var}"
done

exit 0