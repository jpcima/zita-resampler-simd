#ifndef ZR_CPUCHECK_H
#define ZR_CPUCHECK_H

namespace ZitaResampler {
    typedef float cpucheck_datatype;
    enum { cpucheck_datasize = 32 };
    bool cpucheck(void(*fn)(cpucheck_datatype *));
}

#endif  // ZR_CPUCHECK_H
