#include "qcompile.h"

qCompileInfo::qCompileInfo() {
    findOs();
    findCompiler();
    findProcessor();
}

qCompileInfo::~qCompileInfo() {

}


QString qCompileInfo::printCompilerInfo() {
    QString print;
    print.append("Compiler: ").append(compilerName).append("\n");
    print.append("Version: ").append(compilerVersion).append("\n");
    return print;
}

QString qCompileInfo::printProcessorInfo() {
    QString print;
    print.append("CPU: ").append(processorName).append("\n");
    print.append("Family: ").append(processorFamily).append("\n");
    print.append("Extensions: ").append(processorExtensions).append("\n");
    return print;
}

/* work out os */
void qCompileInfo::findOs() {

}

/* work out compiler */
void qCompileInfo::findCompiler() {

}

/* work out processor */
void qCompileInfo::findProcessor() {

}

QString qCompileInfo::getOs() {
    QString os = osName;
    os.append(" ").append(osVersion);
    if (osBits.isNull() || osBits.isEmpty()) {
        return os;
    } else {
        os.append(" ").append(osBits);
    }
    return os;
}

QString qCompileInfo::getOsName() {
    return osName;
}

QString qCompileInfo::getOsVersion() {
    return osVersion;
}

QString qCompileInfo::getCompilerName() {
    return compilerName;
}

QString qCompileInfo::getCompilerVersion() {
    return compilerVersion;
}

QString qCompileInfo::getProcessorName() {
    return processorName;
}

QString qCompileInfo::getProcessorFamily() {
    return processorFamily;
}

QString qCompileInfo::getProcessorExtensions() {
    return processorExtensions;
}

#include "moc_qcompile.cpp"