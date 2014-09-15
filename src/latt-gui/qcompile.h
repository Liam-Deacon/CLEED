#ifndef QCOMPILE_H
#define QCOMPILE_H

#include <QtCore/QString>
#include <QtNetwork/QHostInfo>
#include <QtCore/QSysInfo>

// compile info
#ifndef __USER__
    #if defined(Q_OS_WIN)
        #define __USER__ getenv("USERNAME")
    #else
        #define __USER__ getenv("USER")
    #endif
#endif

#ifndef __HOST__
#define __HOST__ QHostInfo::localHostName()
#endif

#ifndef __COMPILER__
    #if defined(Q_CC_BOR)
        #define __COMPILER__ "BOR"
    #elif defined(Q_CC_CDS)
        #define __COMPILER__ "CDS"
    #elif defined(Q_CC_COMEAU)
        #define __COMPILER__ "COMEAU"
    #elif defined(Q_CC_DEC)
        #define __COMPILER__ "DEC"
    #elif defined(Q_CC_EDG)
        #define __COMPILER__ "EDG"
    #elif defined(Q_CC_GHS)
        #define __COMPILER__ "GHS"
    #elif defined(Q_CC_GNU) || defined(Q_CC_MINGW)
        #ifdef Q_CC_MINGW
            #define __COMPILER__ "MinGW"
        #else
            #define __COMPILER__ "GNU"
        #endif
    #elif defined(Q_CC_HIGHC)
        #define __COMPILER__ "HIGHC"
    #elif defined(Q_CC_HPACC)
        #define __COMPILER__ "HPACC"
    #elif defined(Q_CC_INTEL)
        #define __COMPILER__ "INTEL"
    #elif defined(Q_CC_KAI)
        #define __COMPILER__ "KAI"
    #elif defined(Q_CC_MIPS)
        #define __COMPILER__ "MIPS"
    #elif defined(Q_CC_MSVC)
        #define __COMPILER__ "MSVC"
    #elif defined(Q_CC_MWERKS)
        #define __COMPILER__ "MWERKS"
    #elif defined(Q_CC_OC)
        #define __COMPILER__ "OC"
    #elif defined(Q_CC_PGI)
        #define __COMPILER__ "PGI"
    #elif defined(Q_CC_SUN)
        #define __COMPILER__ "SUN"
    #elif defined(Q_CC_SYM)
        #define __COMPILER__ "SYM"
    #elif defined(Q_CC_USLC)
        #define __COMPILER__ "USLC"
    #elif defined(Q_CC_WAT)
        #define __COMPILER__ "WAT"
    #else
        #define __COMPILER__ "compiler-unknown"
    #endif
#endif

#ifndef __OS__
    #if defined(Q_OS_AIX)
        #define __OS__ "AIX"
    #elif defined(Q_OS_BSD4)
        #define __OS__ "BSD4"
    #elif defined(Q_OS_BSDI)
        #define __OS__ "BSDI"
    #elif defined(Q_OS_CYGWIN)
        #define __OS__ "CYGWIN"
    #elif defined(Q_OS_DARWIN)
        #define __OS__ "DARWIN"
    #elif defined(Q_OS_DGUX)
        #define __OS__ "DGUX"
    #elif defined(Q_OS_DYNIX)
        #define __OS__ "DYNIX"
    #elif defined(Q_OS_FREEBSD)
        #define __OS__ "FREEBSD"
    #elif defined(Q_OS_HPUX)
        #define __OS__ "HPUX"
    #elif defined(Q_OS_HURD)
        #define __OS__ "HURD"
    #elif defined(Q_OS_IRIX)
        #define __OS__ "IRIX"
    #elif defined(Q_OS_LINUX)
        #define __OS__ "LINUX"
    #elif defined(Q_OS_LYNX)
        #define __OS__ "LYNX"
    #elif defined(Q_OS_MAC)
        #define __OS__ "MAC"
    #elif defined(Q_OS_MSDOS)
        #define __OS__ "MSDOS"
    #elif defined(Q_OS_NETBSD)
        #define __OS__ "NETBSD"
    #elif defined(Q_OS_OS2)
        #define __OS__ "OS2"
    #elif defined(Q_OS_OPENBSD)
        #define __OS__ "OPENBSD"
    #elif defined(Q_OS_OS2EMX)
        #define __OS__ "OS2EMX"
    #elif defined(Q_OS_OSF)
        #define __OS__ "OSF"
    #elif defined(Q_OS_QNX)
        #define __OS__ "QNX"
    #elif defined(Q_OS_RELIANT)
        #define __OS__ "RELIANT"
    #elif defined(Q_OS_SCO)
        #define __OS__ "SCO"
    #elif defined(Q_OS_SOLARIS)
        #define __OS__ "SOLARIS"
    #elif defined(Q_OS_SYMBIAN)
        #define __OS__ "SYMBIAN"
    #elif defined(Q_OS_ULTRIX)
        #define __OS__ "ULTIX"
    #elif defined(Q_OS_UNIX)
        #define __OS__ "UNIX"
    #elif defined(Q_OS_UNIXWARE)
        #define __OS__ "UNIXWARE"
    #elif defined(Q_OS_WIN32)
        #define __OS__ "WIN32"
    #elif defined(Q_OS_WINCE)
        #define __OS__ "WINCE"
    #else
        #define __OS__ "OS-unknown"
    #endif
#endif


#ifndef __PROCESSOR__
#if defined(Q_PROCESSOR_X86) || defined(Q_PROCESSOR_X86_32)
    // x86 family
    #ifdef Q_PROCESSOR_X86_32
        #define __PROCESSOR__ "x86_32"
    #else
        #ifdef Q_PROCESSOR_X86_64
            #define __PROCESSOR__ "x86_64"
        #else
            #define __PROCESSOR__ "x86"
        #endif
    #endif
#elif defined(Q_PROCESSOR_IA64)
    // intel itanium
    #define __PROCESSOR__ "IA64"
#elif defined(Q_PROCESSOR_S390)
        #ifdef Q_PROCESSOR_S390_X
            #define __PROCESSOR__ "S390_X"
        #else
            #define __PROCESSOR__ "S390"
        #endif
#elif defined(Q_PROCESSOR_ALPHA)
    // alpha
    #define __PROCESSOR__ "alpha"
#elif defined(Q_PROCESSOR_ARM) || defined (Q_PROCESSOR_ARM_V5)
    // arm family
    #ifdef Q_PROCESSOR_ARM_V5
        #ifdef Q_PROCESSOR_ARM_V6
            #ifdef Q_PROCESSOR_ARM_V7
                #define __PROCESSOR__ "ARM_v7"
            #else
                #define __PROCESSOR__ "ARM_v6"
            #endif
        #else
            #define __PROCESSOR__ "ARM_v5"
        #endif
    #else
        #define __PROCESSOR__ "ARM"
    #endif
#elif defined (Q_PROCESSOR_AVR32)
    #define __PROCESSOR__ "AVR32"
#elif defined(Q_PROCESSOR_BLACKFIN)
    #define __PROCESSOR__ "Blackfin"
#elif defined(Q_PROCESSOR_MIPS) || defined(Q_PROCESSOR_MIPS_32) || defined(QPROCESSOR_MIPS_I)
    // MIPS family
    #ifdef Q_PROCESSOR_MIPS_I
        #ifdef Q_PROCESSOR_MIPS_II
            #ifdef Q_PROCESSOR_MIPS_III
                #ifdef Q_PROCESSOR_MIPS_IV
                    #ifdef Q_PROCESSOR_MIPS_V
                        #ifdef Q_PROCESSOR_MIPS_64
                            #define __PROCESSOR__ "MIPS_V_64"
                        #else
                            #define __PROCESSOR__ "MIPS_V"
                        #endif
                    #else
                        #ifdef Q_PROCESSOR_MIPS_64
                            #define __PROCESSOR__ "MIPS_IV_64"
                        #else
                            #define __PROCESSOR__ "MIPS_IV"
                        #endif
                    #endif
                #else
                    #ifdef Q_PROCESSOR_MIPS_64
                        #define __PROCESSOR__ "MIPS_III_64"
                    #else
                        #define __PROCESSOR__ "MIPS_III"
                    #endif
                #endif
            #else
                #define __PROCESSOR__ "MIPS_II"
            #endif
        #else
            #define __PROCESSOR__ "MIPS_I"(
        #endif
    #else
        #define __PROCESSOR__ "MIPS-unknown"
    #endif
#elif defined(Q_PROCESSOR_POWER)
    // PowerPC, CELL, etc
    #ifdef Q_PROCESSOR_POWER_32
        #ifdef Q_PROCESSOR_POWER_64
            #define __PROCESSOR__ "POWER_64"
        #else
            #define __PROCESSOR__ "POWER_32"
        #endif
    #else
        #define __PROCESSOR__ "POWER"
    #endif
#elif defined(Q_PROCESSOR_SH) || defined(Q_PROCESSOR_SH_4A)
    #ifdef Q_PROCESSOR_SH_4A
        #define __PROCESSOR__ "SH_4A"
    #else
        #define __PROCESSOR__ "SH"
    #endif
#elif defined(Q_PROCESSOR_SPARC) || defined(Q_PROCESSOR_SPARC_V9)
    // SPARC architechture
    #ifdef Q_PROCESSOR_SPARC_V9
        #define __PROCESSOR__ "SPARC_V9"
    #else
        #define __PROCESSOR__ "SPARC"
    #endif
#else
#define __PROCESSOR__ "processor-unknown"
#endif
#endif

// compile info
#ifndef __USER__
#define __USER__ getenv("USERNAME")
#endif

#ifndef __HOST__
#define __HOST__ QHostInfo::localHostName()
#endif

#ifndef __COMPILER__
    #ifdef Q_CC_BOR
        #define __COMPILER__ "BOR"
    #endif

    #ifdef Q_CC_CDS
        #define __COMPILER__ "CDS"
    #endif

    #ifdef Q_CC_COMEAU
        #define __COMPILER__ "COMEAU"
    #endif

    #ifdef Q_CC_DEC
        #define __COMPILER__ "DEC"
    #endif

    #ifdef Q_CC_EDG
        #define __COMPILER__ "EDG"
    #endif

    #ifdef Q_CC_GHS
        #define __COMPILER__ "GHS"
    #endif

    #ifdef Q_CC_GNU
        #ifdef Q_CC_MINGW
            #define __COMPILER__ "MinGW" __MINGW32_VERSION
        #else
            #define __COMPILER__ "GNU"
        #endif
    #endif

    #ifdef Q_CC_HIGHC
        #define __COMPILER__ "HIGHC"
    #endif

    #ifdef Q_CC_HPACC
        #define __COMPILER__ "HPACC"
    #endif

    #ifdef Q_CC_INTEL
        #define __COMPILER__ "INTEL"
    #endif

    #ifdef Q_CC_KAI
        #define __COMPILER__ "KAI"
    #endif

    #ifdef Q_CC_MIPS
        #define __COMPILER__ "MIPS"
    #endif

    #ifdef Q_CC_MINGW
        #define __COMPILER__ "MINGW"
    #endif

    #ifdef Q_CC_MSVC
        #define __COMPILER__ "MSVC"
    #endif

    #ifdef Q_CC_MWERKS
        #define __COMPILER__ "MWERKS"
    #endif

    #ifdef Q_CC_OC
        #define __COMPILER__ "OC"
    #endif

    #ifdef Q_CC_PGI
        #define __COMPILER__ "PGI"
    #endif

    #ifdef Q_CC_SUN
        #define __COMPILER__ "SUN"
    #endif

    #ifdef Q_CC_SYM
        #define __COMPILER__ "SYM"
    #endif

    #ifdef Q_CC_USLC
        #define __COMPILER__ "USLC"
    #endif

    #ifdef Q_CC_WAT
        #define __COMPILER__ "WAT"
    #endif
#endif

class qCompileInfo {

public:
    qCompileInfo();
    ~qCompileInfo();

    QString printOS();
    QString printCompiler();
    QString printProcessor();

private:
    void findOs();
    void findCompiler();
    void findProcessor();

    QString getOs();
    QString getOsName();
    QString getOsVersion();
    QString getOsVersion(QString &os);
    QString getCompilerName();
    QString getCompilerVersion();
    QString getCompilerVersion(QString &compiler);
    QString getProcessorName();
    QString getProcessorFamily();
    QString getProcessorExtensions();

    QString printCompilerInfo();
    QString printProcessorInfo();


    QString osName;
    QString osVersion;
    QString osBits;
    QString compilerName;
    QString compilerVersion;
    QString processorName;
    QString processorFamily;
    QString processorExtensions;

};

#endif // QCOMPILE_H
