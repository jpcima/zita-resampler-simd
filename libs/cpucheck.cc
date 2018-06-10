#include "cpucheck.h"

#if !defined(_WIN32)
#include <signal.h>
#include <setjmp.h>
#include <pthread.h>

namespace ZitaResampler {

static pthread_mutex_t cpucheck_lock = PTHREAD_MUTEX_INITIALIZER;
static sigjmp_buf cpucheck_env;
static volatile int cpucheck_val;

static void cpucheck_sigaction(int, siginfo_t *info, void *)
{
    if (info->si_code != SI_USER) {
        cpucheck_val = 0;
        siglongjmp(cpucheck_env, 1);
    }
}

static void do_cpucheck(void(*fn)(cpucheck_datatype *))
{
    cpucheck_val = -1;
    sigsetjmp(cpucheck_env, 1);
    if (cpucheck_val == -1) {
        cpucheck_datatype buf[cpucheck_datasize] = {};
        fn(buf);
        cpucheck_val = 1;
    }
}

bool cpucheck(void(*fn)(cpucheck_datatype *))
{
    pthread_mutex_lock(&cpucheck_lock);
    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGILL);
    sigset_t oldsigs;
    pthread_sigmask(SIG_UNBLOCK, &sigs, &oldsigs);
    struct sigaction sa = {};
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &cpucheck_sigaction;
    sigfillset(&sa.sa_mask);
    struct sigaction oldsa;
    sigaction(SIGILL, &sa, &oldsa);
    do_cpucheck(fn);
    sigaction(SIGILL, &oldsa, NULL);
    pthread_sigmask(SIG_SETMASK, &oldsigs, NULL);
    bool result = (bool)cpucheck_val;
    pthread_mutex_unlock(&cpucheck_lock);
    return result;
}

}  // namespace ZitaResampler

#else
#include <stdexcept>
#include <windows.h>
#include <stllock.h>
#include <setjmp.h>

namespace ZitaResampler {

static CCritSec cpucheck_lock;
static jmp_buf cpucheck_env;
static volatile int cpucheck_val;

static LONG CALLBACK cpucheck_exc_handler(EXCEPTION_POINTERS *info)
{
    DWORD code = info->ExceptionRecord->ExceptionCode;
    if (code == EXCEPTION_ILLEGAL_INSTRUCTION) {
        cpucheck_val = 0;
        longjmp(cpucheck_env, 1);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

static void do_cpucheck(void(*fn)(cpucheck_datatype *))
{
    cpucheck_val = -1;
    setjmp(cpucheck_env);
    if (cpucheck_val == -1) {
        cpucheck_datatype buf[cpucheck_datasize] = {};
        fn(buf);
        cpucheck_val = 1;
    }
}

bool cpucheck(void(*fn)(cpucheck_datatype *))
{
    cpucheck_lock.Enter();
    void *veh = AddVectoredExceptionHandler(1, &cpucheck_exc_handler);
    if (!veh) {
        cpucheck_lock.Leave();
        throw std::runtime_error("AddVectoredExceptionHandler");
    }
    do_cpucheck(fn);
    RemoveVectoredExceptionHandler(veh);
    bool result = (bool)cpucheck_val;
    cpucheck_lock.Leave();
    return result;
}

}  // namespace ZitaResampler

#endif
