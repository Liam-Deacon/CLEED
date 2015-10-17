/*********************************************************************
 *													attributes.h
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/11.02.2015 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 * \date 2015-02-11
 *
 * Defines attributes for functions using GCC's \c __attribute__ extension.
 *
 * \note On other compilers that do not define \c __GNUC__ then either the
 * nearest approximation is used or else the attributes are ignored.
 */


#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

/* expands to the first argument */
#define VA_FIRST(...) VA_FIRST_HELPER(__VA_ARGS__, throwaway)
#define VA_FIRST_HELPER(first, ...) first
#define REST(...) REST_HELPER(NUM(__VA_ARGS__), __VA_ARGS__)
#define REST_HELPER(qty, ...) REST_HELPER2(qty, __VA_ARGS__)
#define REST_HELPER2(qty, ...) REST_HELPER_##qty(__VA_ARGS__)
#define REST_HELPER_ONE(first)
#define REST_HELPER_TWOORMORE(first, ...) , __VA_ARGS__
#define NUM(...) \
    SELECT_10TH(__VA_ARGS__, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE,\
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, ONE, throwaway)
#define SELECT_10TH(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ...) a10


#if (!__GNUC__)
# define __attribute__(x) /* empty to disable GCC attribute extension */
#define GCC_VERSION 0
#else
#define GCC_VERSION (  __GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)
#endif

/* The following attributes are defined in almost all GCC versions (>=2.95.3) */
#define NORETURN __attribute__((noreturn))
#define CONST __attribute__((const))
#define FORMAT(...) __attribute__((format(__VA_ARGS__)))
#define FORMAT_ARG(x) __attribute__((format_args(x)))
#define NO_INSTRUMENT_FUNCTION __attribute__((no_instrument_function))
#define SECTION(name) __attribute__((section(name)))
#define UNUSED __attribute__((unused))
#define WEAK __attribute__((weak))
#define ALIAS(x) __attribute__((alias(x))
#define NO_CHECK_MEMORY_USAGE __attribute__((no_check_memory_usage))
#define REGPARM(x) __attribute__((regparm(x))) /* Intel 386 */
#define STDCALL __attribute__((stdcall)) /* Intel 386 */
#define CDECL __attribute__((cdecl)) /* Intel 386 */
#define LONGCALL __attribute__((longcall)) /* RS/6000 & PowerPC */
#define DLLIMPORT __declspec((dllimport))
#define DLLEXPORT __declspec((dllexport))
#define EXCEPTION(x, ...) __attribute__((exception(x, ##__VA_ARGS__)))
#define FUNCTION_VECTOR(x) __attribute__((function_vector(x))) /* H8/300[,H] */
#define INTERRUPT_HANDLER __attribute__((interrupt_handler)) /* H8/300[,H] */
#define EIGHTBIT_DATA __attribute__((eightbit_data)) /* H8/300[,H] */
#define TINY_DATA __attribute__((tiny_data)) /* M32R/D */
#define INTERRUPT __attribute__((interrupt)) /* M32R/D */
#define MODEL(x) __attribute__((model(x)))  /* M32R/D */

#define ALIGNED(alignment) __attribute__((aligned(alignment)))
#define MODE(x) __attribute__((mode(x)))
#define NOCOMMON __attribute__((nocommon))
#define PACKED __attribute__((packed))
#define TRANSPARENT_UNION __attribute__((transparent_union)

/* following have optional arguments depending on GCC version */
#if (GCC_VERSION >= 40204)
#define CONSTRUCTOR(...) __attribute__((constructor))
#define DESTRUCTOR(...) __attribute__((destructor))
#else
#define CONSTRUCTOR(...) __attribute__((constructor(__VA_ARGS__)))
#define DESTRUCTOR(...) __attribute__((destructor(__VA_ARGS__)))
#endif

#if (GCC_VERSION >= 40503)
#define DEPRECATED(...) __attribute__((deprecated(__VA_ARGS__)))
#else
#define DEPRECATED(...) __attribute__((deprecated)) /* msg added in GCC 4.5.3 */
#endif

/* Attributes for GCC 3.0.4 or later */
#if (GCC_VERSION >= 30004)
#define PURE __attribute__((pure))
#define MALLOC __attribute__((malloc))
#define SP_SWITCH __attribute__((sp_switch))
#define TRAP_EXIT __attribute__((trap_exit))
#define SIGNAL __attribute__((signal)) /* AVR */
#define NAKED __attribute__((naked)) /* ARM or AVR */
#define SHARED __attribute__((shared)) /* Windows NT */
#else
#define PURE
#define MALLOC
#define SP_SWITCH
#define TRAP_EXIT
#define SIGNAL
#define NAKED
#define SHARED
#endif

/* Attributes for GCC 3.1.1 or later */
#if (GCC_VERSION >= 30101)
#define USED __attribute__((used)
#define VECTOR_SIZE(bytes) __attribute__((vector_size(bytes)))
#else
#define USED
#define VECTOR_SIZE(bytes)
#endif

/* Attributes for GCC 3.2.3 or later */
#if (GCC_VERSION >= 30203)
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE
#endif

/* Attributes for GCC 3.3.6 or later */
#if (GCC_VERSION >= 30306)
#define NOTHROW __attribute__((nothrow))
#define NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define VISIBILITY(type) __attribute__((visibility(type)))
#define SHORTCALL __attribute__((shortcall)) /* ARM */
#define NEAR __attribute__((near)) /* 68HC11 */
#define FAR __attribute__((far)) /* 68HC11 */
#define COMMON __attribute__((common))
#define TLS_MODEL(model) __attribute__((tls_model(model)))
#define MAY_ALIAS __attribute__((may_alias))
#else
#define NOTHROW
#define NONNULL(...)
#define VISIBILITY(type)
#define SHORTCALL
#define NEAR
#define FAR
#define COMMON
#define TLS_MODEL(model)
#define MAY_ALIAS
#endif

/* Attributes for GCC 3.4.6 or later */
#if (GCC_VERSION >= 30406)
#define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#define SAVEALL __attribute__((saveall))
#define CLEANUP(function) __attribute__((cleanup(function)))
#define MS_STRUCT __attribute__((ms_struct)) /* i386 */
#define GCC_STRUCT __attribute__((gcc_struct)) /* i386 */
#else
#define WARN_UNUSED_RESULT
#define SAVEALL
#define CLEANUP(function)
#define MS_STRUCT
#define GCC_STRUCT
#endif

/* Attributes for GCC 4.0.4 or later */
#if (GCC_VERSION >= 40004)
#define FASTCALL __attribute__((fastcall)) /* i386 */
#define KSPSUSP __attribute__((kspisusp))
#define NESTING __attribute__((nesting))
#define NMI_HANDLER __attribute__((nmi_handler)) /* Blackfin */
#define SENTINEL __attribute__((sentinel))
#define BELOW100 __attribute__((below100)) /* xstormy16 */
#define NOTSHARED __declspec((notshared)) /* ARM */
#else
#define FASTCALL
#define KSPSUSP
#define NESTING
#define NMI_HANDLER
#define SENTINEL
#define BELOW100
#define NOTSHARED
#endif

/* Attributes for GCC 4.1.2 or later */
#if (GCC_VERSION >= 40102)
#define FLATTEN __attribute__((flatten))
#define SSEREGPARM __attribute__((sseregparm)) /* i386 with SSE */
#define RETURNS_TWICE __attribute__((returns_twice))
#define EXTERNALLY_VISIBLE __attribute__((externally_visible))
#define SELECTANY __attribute__((selectany))
#else
#define FLATTEN
#define SSEREGPARM
#define RETURNS_TWICE
#define EXTERNALLY_VISIBLE
#define SELECTANY
#endif

/* Attributes for GCC 4.2.4 or later */
#if (GCC_VERSION >= 40204)
#define GNU_INLINE __attribute__((gnu_inline))
#define FORCE_ALIGN_ARG_PTR __attribute__((force_align_arg_pointer)) /* x86 */
#define WEAKREF(...) __attribute__((weakref(__VA_ARGS__)))
#define ALTIVEC(x) __attribute__((altivec(x))) /* PowerPC */
#else
#define GNU_INLINE
#define FORCE_ALIGN_ARG_PTR
#define WEAKREF(...)
#define ALTIVEC(x)
#endif

/* Attributes for GCC 4.3.6 or later */
#if (GCC_VERSION >= 40306)
#define ARTIFICIAL __attribute__((artificial))
#define ERROR(msg) __attribute__((error(msg)))
#define WARNING(msg) __attribute__((warnining(msg)))
#define L1_TEXT __attribute__((l1_text))
#define MIPS16 __attribute__((mips16)) /* MIPS */
#define NOMIPS16 __attribute__((nomips16)) /* MIPS */
#define HOT __attribute__((hot))
#define COLD __attribute__((cold))
#define VERSION_ID(x) __attribute__((version_id(x)))
#define PROGMEM __attribute__((progmem)) /* AVR */
#else
#define ARTIFICIAL
#define ERROR(msg)
#define WARNING(msg)
#define L1_TEXT
#define MIPS16
#define NOMIPS16
#define HOT
#define COLD
#define VERSION_ID(x)
#define PROGMEM
#endif

/* Attributes for GCC 4.4.7 or later */
#if (GCC_VERSION >= 40407)
#define ALLOC_SIZE(...) __attribute__((alloc_size(__VA_ARGS__)))
#define ISR __attribute__((isr)) /* ARM */
#define MS_ABI __attribute__((ms_abi))
#define SYSV_ABI __attribute__((sysv_abi))
#define OPTIMIZE(x) __attribute__((optimize(x)))
#define RESBANK __attribute__((resbank))
#define SYSCALL_LINKAGE __attribute__((syscall_linkage)) /* IA64 */
#define TARGET(x) __attribute__((target(x)))
#else
#define ALLOC_SIZE(x, ...)
#define ISR
#define MS_ABI
#define SYSV_ABI
#define OPTIMIZE(x)
#define RESBANK
#define SYSCALL_LINKAGE
#define TARGET(x)
#endif

/* Attributes for GCC 4.5.4 or later */
#if (GCC_VERSION >= 40504)
#define BANK_SWITCH __attribute__((bank_switch))
#define DISINTERRUPT __attribute__((disinterrupt)) /* MeP */
#define MS_HOOK_PROLOGUE __attribute__((ms_hook_prologue)) /* Windows XP SP2 or later */
#define NOCLONE __attribute__((noclone))
#define PCS(x) __attribute__((pcs(x)))
#define VLIW __attribute__((vliw)) /* MeP with VLIW coprocessor */
#else
#define BANK_SWITCH
#define DISINTERRUPT
#define MS_HOOK_PROLOGUE
#define NOCLONE
#define PCS(x)
#define VLIW
#endif

/* Attributes for GCC 4.6.4 or later */
#if (GCC_VERSION >= 40604)
#define THISCALL __attribute__((thiscall))
#define IFUNC(resolver) __attribute__((ifunc(resolver)))
#define LEAF __attribute__((leaf))
#define SAVE_VOLATILES __attribute__((save_volatiles)) /* MicroBlaze */
#define CALLEE_POP_AGGREGATE_RETURN(num) __attribute__((callee_pop_aggregate_return(num)))
#define NO_SPLIT_STACK __attribute__((no_split_stack))
#define OS_MAIN __attribute__((os_main)) /* AVR */
#define OS_TASK __attribute__((os_task)) /* AVR */
#else
#define THISCALL
#define IFUNC(resolver)
#define LEAF
#define SAVE_VOLATILES
#define CALLEE_POP_AGGREGATE_RETURN(num)
#define NO_SPLIT_STACK
#define OS_MAIN
#define OS_TASK
#endif

/* Attributes for GCC 4.8.4 or later */
#if (GCC_VERSION >= 40804)
#define NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))
#else
#define NO_SANITIZE_ADDRESS
#endif

/* Attributes for GCC 4.9.2 or later */
#if (GCC_VERSION >= 40902)
#define  RETURNS_NONNULL __attribute__((returns_nonnull))
#else
#define  RETURNS_NONNULL
#endif

#endif /* ATTRIBUTES_H */
