// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
/*************************************************************************************/
/*                                   StressLog.h                                     */
/*************************************************************************************/

/* StressLog is a binary, memory based circular queue of logging messages.  It is 
   intended to be used in retail, non-golden builds during stress runs (activated
   by registry key), so to help find bugs that only turn up during stress runs.  

   It is meant to have very low overhead and can not cause deadlocks, etc.  It is
   however thread safe */

/* The log has a very simple structure, and it meant to be dumped from a NTSD 
   extention (eg. strike). There is no memory allocation system calls etc to purtub things */

/* see the tools/strike/stressdump.cpp for the dumper utility that parses this log */

/*************************************************************************************/

#ifndef StressLog_h 
#define StressLog_h  1

#ifdef STRESS_LOG

#include "holder.h"
#include "log.h"
#include "staticcontract.h"
#include "mscoree.h"
#include "clrinternal.h"
#ifdef STRESS_LOG_READONLY
#include <stddef.h> // offsetof
#else //STRESS_LOG_READONLY
#include "clrhost.h"
#endif //STRESS_LOG_READONLY

#ifndef _ASSERTE
#define _ASSERTE(expr)
#endif

/* The STRESS_LOG* macros work like printf.  In fact the use printf in their implementation
   so all printf format specifications work.  In addition the Stress log dumper knows 
   about certain suffixes for the %p format specification (normally used to print a pointer)

            %pM     // The pointer is a MethodDesc
            %pT     // The pointer is a type (MethodTable)
            %pV     // The pointer is a C++ Vtable pointer (useful for distinguishing different types of frames
            %pK     // The pointer is a code address (used for stack track)
*/

/*  STRESS_LOG_VA was added to allow sendign GC trace output to the stress log. msg must be enclosed 
      in ()'s and contain a format string followed by 0 - 4 arguments.  The arguments must be numbers or 
      string literals.  LogMsgOL is overloaded so that all of the possible sets of parameters are covered.  
      This was done becasue GC Trace uses dprintf which dosen't contain info on how many arguments are  
      getting passed in and using va_args would require parsing the format string during the GC
*/
#define STRESS_LOG_VA(msg) do {                                               \
            if (StressLog::StressLogOn(LF_GC, LL_ALWAYS))                     \
                StressLog::LogMsgOL msg;                                      \
            LogSpewAlways  msg;                                               \
            } while(0)

#define STRESS_LOG0(facility, level, msg) do {                                \
            if (StressLog::StressLogOn(facility, level))                      \
                StressLog::LogMsg(facility, 0, msg);                          \
            LOG((facility, level, msg));                                      \
            } while(0)

#define STRESS_LOG1(facility, level, msg, data1) do {                         \
            if (StressLog::StressLogOn(facility, level))                      \
                StressLog::LogMsg(facility, 1, msg, (void*)(size_t)(data1));  \
            LOG((facility, level, msg, data1));                               \
            } while(0)

#define STRESS_LOG2(facility, level, msg, data1, data2) do {                  \
            if (StressLog::StressLogOn(facility, level))                      \
                StressLog::LogMsg(facility, 2, msg,                           \
                    (void*)(size_t)(data1), (void*)(size_t)(data2), 0, 0);    \
            LOG((facility, level, msg, data1, data2));                        \
            } while(0)

#define STRESS_LOG2_CHECK_EE_STARTED(facility, level, msg, data1, data2) do { \
            if (g_fEEStarted)                                                 \
                STRESS_LOG2(facility, level, msg, data1, data2);              \
            else                                                              \
                LOG((facility, level, msg, data1, data2));                    \
            } while(0)

#define STRESS_LOG3(facility, level, msg, data1, data2, data3) do {           \
            if (StressLog::StressLogOn(facility, level))                      \
                StressLog::LogMsg(facility, 3, msg,                           \
                    (void*)(size_t)(data1),(void*)(size_t)(data2),(void*)(size_t)(data3), 0); \
            LOG((facility, level, msg, data1, data2, data3));                 \
            } while(0)

#define STRESS_LOG4(facility, level, msg, data1, data2, data3, data4) do {    \
            if (StressLog::StressLogOn(facility, level))                      \
                StressLog::LogMsg(facility, 4, msg, (void*)(size_t)(data1),   \
                    (void*)(size_t)(data2),(void*)(size_t)(data3),(void*)(size_t)(data4));\
            LOG((facility, level, msg, data1, data2, data3, data4));          \
            } while(0)

#define STRESS_LOG5(facility, level, msg, data1, data2, data3, data4, data5) do {        \
            if (StressLog::StressLogOn(facility, level))                      \
                StressLog::LogMsg(facility, 5, msg, (void*)(size_t)(data1),   \
                    (void*)(size_t)(data2),(void*)(size_t)(data3),(void*)(size_t)(data4), \
                    (void*)(size_t)(data5));                                  \
            LOG((facility, level, msg, data1, data2, data3, data4, data5));   \
            } while(0)

#define STRESS_LOG6(facility, level, msg, data1, data2, data3, data4, data5, data6) do { \
            if (StressLog::StressLogOn(facility, level))                      \
                StressLog::LogMsg(facility, 6, msg, (void*)(size_t)(data1),   \
                    (void*)(size_t)(data2),(void*)(size_t)(data3),(void*)(size_t)(data4), \
                    (void*)(size_t)(data5), (void*)(size_t)(data6));          \
            LOG((facility, level, msg, data1, data2, data3, data4, data5, data6));   \
            } while(0)

#define STRESS_LOG7(facility, level, msg, data1, data2, data3, data4, data5, data6, data7) do {    \
            if (StressLog::StressLogOn(facility, level))                      \
                StressLog::LogMsg(facility, 7, msg, (void*)(size_t)(data1),   \
                    (void*)(size_t)(data2),(void*)(size_t)(data3),(void*)(size_t)(data4), \
                    (void*)(size_t)(data5), (void*)(size_t)(data6), (void*)(size_t)(data7)); \
            LOG((facility, level, msg, data1, data2, data3, data4, data5, data6, data7));\
            } while(0)

#define STRESS_LOG_RESERVE_MEM(numChunks) do { \
            if (StressLog::StressLogOn (LF_ALL, LL_ALWAYS))  \
                {StressLog::ReserveStressLogChunks (numChunks);} \
            } while(0)
// !!! WARNING !!!
// !!! DO NOT ADD STRESS_LOG8, as the stress log infrastructure supports a maximum of 7 arguments
// !!! WARNING !!!

#define STRESS_LOG_PLUG_MOVE(plug_start, plug_end, plug_delta) do {                                     \
            if (StressLog::StressLogOn(LF_GC, LL_INFO1000))                                             \
                StressLog::LogMsg(LF_GC, 3, ThreadStressLog::gcPlugMoveMsg(),                           \
                (void*)(size_t)(plug_start), (void*)(size_t)(plug_end), (void*)(size_t)(plug_delta));   \
            LOG((LF_GC, LL_INFO10000, ThreadStressLog::gcPlugMoveMsg(), (plug_start), (plug_end), (plug_delta))); \
            } while(0)

#define STRESS_LOG_ROOT_PROMOTE(root_addr, objPtr, methodTable) do {                                    \
            if (StressLog::StressLogOn(LF_GC|LF_GCROOTS, LL_INFO1000))                                  \
                StressLog::LogMsg(LF_GC|LF_GCROOTS, 3, ThreadStressLog::gcRootPromoteMsg(),             \
                    (void*)(size_t)(root_addr), (void*)(size_t)(objPtr), (void*)(size_t)(methodTable)); \
            LOG((LF_GC|LF_GCROOTS, LL_INFO1000000, ThreadStressLog::gcRootPromoteMsg(), (root_addr), (objPtr), (methodTable))); \
            } while(0)

#define STRESS_LOG_ROOT_RELOCATE(root_addr, old_value, new_value, methodTable) do {                     \
            if (StressLog::StressLogOn(LF_GC|LF_GCROOTS, LL_INFO1000))                                  \
                StressLog::LogMsg(LF_GC|LF_GCROOTS, 4, ThreadStressLog::gcRootMsg(),                    \
                    (void*)(size_t)(root_addr), (void*)(size_t)(old_value),                             \
                    (void*)(size_t)(new_value), (void*)(size_t)(methodTable));                          \
            LOG((LF_GC|LF_GCROOTS, LL_INFO10000, ThreadStressLog::gcRootMsg(), (root_addr), (old_value), (new_value), (methodTable)));\
            } while(0)

#define STRESS_LOG_GC_START(gcCount, Gen, collectClasses) do {                                          \
            if (StressLog::StressLogOn(LF_GCROOTS|LF_GC|LF_GCALLOC, LL_INFO10))                         \
                StressLog::LogMsg(LF_GCROOTS|LF_GC|LF_GCALLOC, 3, ThreadStressLog::gcStartMsg(),        \
                    (void*)(size_t)(gcCount), (void*)(size_t)(Gen), (void*)(size_t)(collectClasses));   \
            LOG((LF_GCROOTS|LF_GC|LF_GCALLOC, LL_INFO10, ThreadStressLog::gcStartMsg(), (gcCount), (Gen), (collectClasses))); \
            } while(0)

#define STRESS_LOG_GC_END(gcCount, Gen, collectClasses) do {                                            \
            if (StressLog::StressLogOn(LF_GCROOTS|LF_GC|LF_GCALLOC, LL_INFO10))                         \
                StressLog::LogMsg(LF_GCROOTS|LF_GC|LF_GCALLOC, 3, ThreadStressLog::gcEndMsg(),          \
                    (void*)(size_t)(gcCount), (void*)(size_t)(Gen), (void*)(size_t)(collectClasses), 0);\
            LOG((LF_GCROOTS|LF_GC|LF_GCALLOC, LL_INFO10, ThreadStressLog::gcEndMsg(), (gcCount), (Gen), (collectClasses)));  \
            } while(0)
            
#define STRESS_LOG_OOM_STACK
#define STRESS_LOG_GC_STACK

class ThreadStressLog;

/*************************************************************************************/
/* a log is a circular queue of messages */
    
class StressLog {
public:
    static void Initialize(unsigned facilities, unsigned level, unsigned maxBytesPerThread, 
                    unsigned maxBytesTotal, HMODULE hMod);
    static void Terminate(BOOL fProcessDetach=FALSE);
    static void ThreadDetach(ThreadStressLog *msgs);         // call at DllMain  THREAD_DETACH if you want to recycle thread logs
    static long NewChunk ()
    {
        return InterlockedIncrement (&theLog.totalChunk);
    }
    static long ChunkDeleted ()
    {
        return InterlockedDecrement (&theLog.totalChunk);
    }

    //the result is not 100% accurate. If multiple threads call this funciton at the same time, 
    //we could allow the total size be bigger than required. But the memory won't grow forever
    //and this is not critical so we don't try to fix the race
    static BOOL AllowNewChunk (LONG numChunksInCurThread);

    //preallocate Stress log chunks for current thread. The memory we could preallocate is still
    //bounded by per thread size limit and total size limit. If chunksToReserve is 0, we will try to
    //preallocate up to per thread size limit
    static BOOL ReserveStressLogChunks (unsigned chunksToReserve);

    // used by out of process debugger to dump the stress log to 'fileName'
    // IDebugDataSpaces is the NTSD execution callback for getting process memory.  
    // This function is defined in the tools\strike\stressLogDump.cpp file
    static HRESULT Dump(ULONG64 logAddr, const char* fileName, struct IDebugDataSpaces* memCallBack);    
    static BOOL StressLogOn(unsigned facility, unsigned level) 
    { 
        STATIC_CONTRACT_LEAF;
        return (theLog.facilitiesToLog & facility) && (level <= theLog.levelToLog); 
    }

// private:
    unsigned facilitiesToLog;               // Bitvector of facilities to log (see loglf.h)
    unsigned levelToLog;                    // log level (see log.h)
    unsigned MaxSizePerThread;              // maximum number of bytes each thread should have before wrapping
    unsigned MaxSizeTotal;               //maximum memory allowed for stress log
    volatile LONG totalChunk;                       //current number of total chunks allocated
    ThreadStressLog* volatile logs;   // the list of logs for every thread.
    volatile unsigned TLSslot;              // Each thread gets a log this is used to fetch each threads log
    volatile LONG deadCount;                // count of dead threads in the log
    CRITSEC_COOKIE lock;                    // lock
    unsigned __int64 tickFrequency;         // number of ticks per second
    unsigned __int64 startTimeStamp;        // start time from when tick counter started
    FILETIME startTime;                     // time the application started
    HMODULE  hMod;                          // Module this is linked in.  Used to compute format strings' offset

// private:
    static void Enter(BOOL dummy = TRUE);
    static void Leave(BOOL dummy = TRUE);
    static ThreadStressLog* CreateThreadStressLog();

    static void LogMsg(DWORD_PTR facility, int cArgs, const char* format, ... );

// Support functions for STRESS_LOG_VA
// We disable the warning "conversion from 'type' to 'type' of greater size" since everything will
// end up on the stack, and LogMsg will know the size of the variable based on the format string.
#ifdef _MSC_VER
#pragma warning( disable : 4312 )
#endif
    static void LogMsgOL(const char* format)
    { LogMsg(LF_GC, 0, format); }

    template < typename T1 >
    static void LogMsgOL(const char* format, T1 data1)
    {
        C_ASSERT(sizeof(T1) <= sizeof(void*)); 
        LogMsg(LF_GC, 1, format, (void*)data1); 
    }

    template < typename T1, typename T2 >
    static void LogMsgOL(const char* format, T1 data1, T2 data2)
    {
        C_ASSERT(sizeof(T1) <= sizeof(void*) && sizeof(T2) <= sizeof(void*)); 
        LogMsg(LF_GC, 2, format, (void*)data1, (void*)data2); 
    }

    template < typename T1, typename T2, typename T3 >
    static void LogMsgOL(const char* format, T1 data1, T2 data2, T3 data3)
    { 
        C_ASSERT(sizeof(T1) <= sizeof(void*) && sizeof(T2) <= sizeof(void*) && sizeof(T3) <= sizeof(void*)); 
        LogMsg(LF_GC, 3, format, (void*)data1, (void*)data2, (void*)data3); 
    }

    template < typename T1, typename T2, typename T3, typename T4 >
    static void LogMsgOL(const char* format, T1 data1, T2 data2, T3 data3, T4 data4)
    { 
        C_ASSERT(sizeof(T1) <= sizeof(void*) && sizeof(T2) <= sizeof(void*) && sizeof(T3) <= sizeof(void*) && sizeof(T4) <= sizeof(void*)); 
        LogMsg(LF_GC, 4, format, (void*)data1, (void*)data2, (void*)data3, (void*)data4); 
    }

    template < typename T1, typename T2, typename T3, typename T4, typename T5 >
    static void LogMsgOL(const char* format, T1 data1, T2 data2, T3 data3, T4 data4, T5 data5)
    { 
        C_ASSERT(sizeof(T1) <= sizeof(void*) && sizeof(T2) <= sizeof(void*) && sizeof(T3) <= sizeof(void*) && sizeof(T4) <= sizeof(void*) && sizeof(T5) <= sizeof(void*)); 
        LogMsg(LF_GC, 5, format, (void*)data1, (void*)data2, (void*)data3, (void*)data4, (void*)data5); 
    }

    template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
    static void LogMsgOL(const char* format, T1 data1, T2 data2, T3 data3, T4 data4, T5 data5, T6 data6)
    { 
        C_ASSERT(sizeof(T1) <= sizeof(void*) && sizeof(T2) <= sizeof(void*) && sizeof(T3) <= sizeof(void*) && sizeof(T4) <= sizeof(void*) && sizeof(T5) <= sizeof(void*) && sizeof(T6) <= sizeof(void*)); 
        LogMsg(LF_GC, 6, format, (void*)data1, (void*)data2, (void*)data3, (void*)data4, (void*)data5, (void*)data6); 
    }

    template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7 >
    static void LogMsgOL(const char* format, T1 data1, T2 data2, T3 data3, T4 data4, T5 data5, T6 data6, T7 data7)
    { 
        C_ASSERT(sizeof(T1) <= sizeof(void*) && sizeof(T2) <= sizeof(void*) && sizeof(T3) <= sizeof(void*) && sizeof(T4) <= sizeof(void*) && sizeof(T5) <= sizeof(void*) && sizeof(T6) <= sizeof(void*) && sizeof(T7) <= sizeof(void*)); 
        LogMsg(LF_GC, 7, format, (void*)data1, (void*)data2, (void*)data3, (void*)data4, (void*)data5, (void*)data6, (void*)data7); 
    }

#ifdef _MSC_VER
#pragma warning( default : 4312 )
#endif

// We can only log the stacktrace on DEBUG builds!
#ifdef _DEBUG
#endif //_DEBUG

// private: // static variables
    static StressLog theLog;    // We only have one log, and this is it
};

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
typedef Holder<BOOL, StressLog::Enter, StressLog::Leave, 0, CompareDefault,       HSV_NoValidation> StressLogLockHolder;
#else
typedef Holder<BOOL, StressLog::Enter, StressLog::Leave, 0, CompareDefault<BOOL>, HSV_NoValidation> StressLogLockHolder;
#endif


/*************************************************************************************/
/* private classes */

#if defined(_MSC_VER)
#pragma warning(disable:4200 4201)					// don't warn about 0 sized array below or unnamed structures
#endif

// The order of fields is important.  Keep the prefix length as the first field.
// And make sure the timeStamp field is naturally alligned, so we don't waste 
// space on 32-bit platforms
struct StressMsg {
    union {
        struct {
            DWORD_PTR numberOfArgs  : 3;   // at most 7 arguments
            DWORD_PTR formatOffset  : 29;  // offset of string in mscorwks
        };
        DWORD_PTR fmtOffsCArgs;            // for optimized access
    };
    DWORD_PTR facility;                    // facility used to log the entry
    unsigned __int64 timeStamp;            // time when mssg was logged
    void*     args[0];                     // size given by numberOfArgs

    static const size_t maxArgCnt = 7;
    static const size_t maxOffset = 0x80000000;
    static size_t maxMsgSize () 
    { return sizeof(StressMsg) + maxArgCnt*sizeof(void*); }

    friend class ThreadStressLog;
    friend class StressLog;
};
#define STRESSLOG_CHUNK_SIZE (16 * 1024)
#define GC_STRESSLOG_MULTIPLY 5

// a chunk of memory for stress log
struct StressLogChunk
{
    StressLogChunk * prev;
    StressLogChunk * next;
    char buf[STRESSLOG_CHUNK_SIZE];
    DWORD dwSig1;
    DWORD dwSig2;         


    StressLogChunk (StressLogChunk * p = NULL, StressLogChunk * n = NULL)
        :prev (p), next (n), dwSig1 (0xCFCFCFCF), dwSig2 (0xCFCFCFCF)    
    {} 

    char * StartPtr ()
    {
        return buf;
    }

    char * EndPtr ()
    {
        return buf + STRESSLOG_CHUNK_SIZE;
    }

    BOOL IsValid () const
    {
        return dwSig1 == 0xCFCFCFCF && dwSig2 == 0xCFCFCFCF;
    }
};

// This class implements a circular stack of variable sized elements
//    .The buffer between startPtr-endPtr is used in a circular manner
//     to store instances of the variable-sized struct StressMsg.
//     The StressMsg are always aligned to endPtr, while the space
//     left between startPtr and the last element is 0-padded.
//    .curPtr points to the most recently written log message
//    .readPtr points to the next log message to be dumped
//    .hasWrapped is TRUE while dumping the log, if we had wrapped
//     past the endPtr marker, back to startPtr
// The AdvanceRead/AdvanceWrite operations simply update the 
//     readPtr / curPtr fields. thecaller is responsible for reading/writing 
//     to the corresponding field
class ThreadStressLog {
    ThreadStressLog* next;      // we keep a linked list of these
    unsigned   threadId;        // the id for the thread using this buffer
    BOOL       isDead;          // Is this thread dead 
    StressMsg* curPtr;          // where packets are being put on the queue
    StressMsg* readPtr;         // where we are reading off the queue (used during dumping)
    BOOL       readHasWrapped;      // set when read ptr has passed chunListTail
    BOOL       writeHasWrapped;     // set when write ptr has passed chunListHead
    StressLogChunk * chunkListHead; //head of a list of stress log chunks
    StressLogChunk * chunkListTail; //tail of a list of stress log chunks
    StressLogChunk * curReadChunk; //the stress log chunk we are currently reading
    StressLogChunk * curWriteChunk; //the stress log chunk we are currently writing
    LONG chunkListLength; // how many stress log chunks are in this stress log

#ifdef STRESS_LOG_READONLY
    FORCEINLINE StressMsg* AdvanceRead();
#endif //STRESS_LOG_READONLY
    FORCEINLINE StressMsg* AdvanceWrite(int cArgs);

#ifdef STRESS_LOG_READONLY
    inline StressMsg* AdvReadPastBoundary();
#endif //STRESS_LOG_READONLY
    inline StressMsg* AdvWritePastBoundary(int cArgs);

#ifdef STRESS_LOG_READONLY
    ThreadStressLog* FindLatestThreadLog() const;
#endif //STRESS_LOG_READONLY
    friend class StressLog;

#ifndef STRESS_LOG_READONLY
    FORCEINLINE BOOL GrowChunkList ()
    {
        _ASSERTE (chunkListLength >= 1);
        if (!StressLog::AllowNewChunk (chunkListLength))
        {
            return FALSE;
        }
        StressLogChunk * newChunk = new StressLogChunk (chunkListTail, chunkListHead);
        if (newChunk == NULL)
        {
            return FALSE;
        }
        StressLog::NewChunk ();
        chunkListLength++;
        chunkListHead->prev = newChunk;
        chunkListTail->next = newChunk;
        chunkListHead = newChunk;

        return TRUE;
    }
#endif //!STRESS_LOG_READONLY

public:
#ifndef STRESS_LOG_READONLY
    ThreadStressLog ()
    {  
        chunkListHead = chunkListTail = curWriteChunk = NULL;
        StressLogChunk * newChunk =new StressLogChunk;        
        //OOM or in cantalloc region
        if (newChunk == NULL)
        {
            return;
        }     
        StressLog::NewChunk ();

        newChunk->prev = newChunk;
        newChunk->next = newChunk;
            
        chunkListHead = chunkListTail = newChunk;

        next = NULL;
        threadId = 0;
        isDead = TRUE;
        curPtr = NULL;
        readPtr = NULL;        
        writeHasWrapped = FALSE;        
        curReadChunk = NULL;
        curWriteChunk = NULL;
        chunkListLength = 1;            
    }

#endif //!STRESS_LOG_READONLY
    ~ThreadStressLog ()
    {
        //no thing to do if the list is empty (failed to initialize)
        if (chunkListHead == NULL)
        {
            return;
        }
#ifndef STRESS_LOG_READONLY          
        _ASSERTE (chunkListLength >= 1 && chunkListLength <= StressLog::theLog.totalChunk);
#endif //!STRESS_LOG_READONLY          
        StressLogChunk * chunk = chunkListHead;      
        
        do
        {
            StressLogChunk * tmp = chunk;
            chunk = chunk->next;
            delete tmp;
#ifndef STRESS_LOG_READONLY            
            StressLog::ChunkDeleted ();
#endif //!STRESS_LOG_READONLY
        } while (chunk != chunkListHead);
    }

    void Activate ()
    {
#ifndef STRESS_LOG_READONLY
        //there is no need to zero buffers because we could handle garbage contents
        StressLogChunk * chunk = chunkListHead;
        threadId = GetCurrentThreadId ();       
        isDead = FALSE;        
        curWriteChunk = chunkListTail;
        curPtr = (StressMsg *)curWriteChunk->EndPtr ();
        writeHasWrapped = FALSE;
#else //STRESS_LOG_READONLY
        curReadChunk = curWriteChunk;
        readPtr = curPtr;
        readHasWrapped = FALSE;
        // the last written log, if it wrapped around may have partially overwritten
        // a previous record.  Update curPtr to reflect the last safe beginning of a record,
        // but curPtr shouldn't wrap around, otherwise it'll break our assumptions about stress 
        // log
        curPtr = (StressMsg*)((char*)curPtr - StressMsg::maxMsgSize());
        if (curPtr < (StressMsg*)curWriteChunk->StartPtr())
        {
            curPtr = (StressMsg *)curWriteChunk->StartPtr();
        }
        //corner case: the log is empty
        if (readPtr == (StressMsg *)curReadChunk->EndPtr ())
        {
            AdvReadPastBoundary();
        }
#endif //!STRESS_LOG_READONLY
    }    

    BOOL IsValid () const
    {
        return chunkListHead != NULL && (!curWriteChunk || curWriteChunk->IsValid ());
    }
    
#ifdef STRESS_LOG_READONLY    
    // Called while dumping.  Returns true after all messages in log were dumped
    FORCEINLINE BOOL CompletedDump ()
    {
        return readPtr->timeStamp == 0 
                //if read has passed end of list but write has not passed head of list yet, we are done
                //if write has also wrapped, we are at the end if read pointer passed write pointer
                || (readHasWrapped && 
                        (!writeHasWrapped || (curReadChunk == curWriteChunk && readPtr >= curPtr)));
    }
#endif //STRESS_LOG_READONLY

    static const char* gcStartMsg()
    {
        STATIC_CONTRACT_LEAF;
        return "{ =========== BEGINGC %d, (requested generation = %lu, collect_classes = %lu) ==========\n";
    }

    static const char* gcEndMsg()
    {
        STATIC_CONTRACT_LEAF;
        return "========== ENDGC %d (gen = %lu, collect_classes = %lu) ===========}\n";
    }
    
    static const char* gcRootMsg()
    {
        STATIC_CONTRACT_LEAF;
        return "    GC Root %p RELOCATED %p -> %p  MT = %pT\n";
    }

    static const char* gcRootPromoteMsg()
    {
        STATIC_CONTRACT_LEAF;
        return "    GCHeap::Promote: Promote GC Root *%p = %p MT = %pT\n";
    }

    static const char* gcPlugMoveMsg()
    {
        STATIC_CONTRACT_LEAF;
        return "GC_HEAP RELOCATING Objects in heap within range [%p %p) by -0x%x bytes\n";
    }    

    static const char* TaskSwitchMsg()
    {
        STATIC_CONTRACT_LEAF;
        return "StressLog TaskSwitch Marker\n";
    }

    void LogMsg ( DWORD_PTR facility, int cArgs, const char* format, ... )
    {
        va_list Args;
        va_start(Args, format);
        LogMsg (facility, cArgs, format, Args);
        va_end(Args);
    }
    void LogMsg ( DWORD_PTR facility, int cArgs, const char* format, va_list Args);
#ifdef STRESS_LOG_READONLY
    static size_t OffsetOfNext () {return offsetof (ThreadStressLog, next);}
    static size_t OffsetOfListHead () {return offsetof (ThreadStressLog, chunkListHead);}
#endif //STRESS_LOG_READONLY    
};

#ifdef STRESS_LOG_READONLY
/*********************************************************************************/
// Called when dumping the log (by StressLog::Dump())
// Updates readPtr to point to next stress messaage to be dumped
// For convenience it returns the new value of readPtr
inline StressMsg* ThreadStressLog::AdvanceRead() {
    STATIC_CONTRACT_LEAF;
    // advance the marker
    readPtr = (StressMsg*)((char*)readPtr + sizeof(StressMsg) + readPtr->numberOfArgs*sizeof(void*));
    // wrap around if we need to
    if (readPtr >= (StressMsg *)curReadChunk->EndPtr ())
    {
        AdvReadPastBoundary();
    }
    return readPtr;
}

// It's the factored-out slow codepath for AdvanceRead() and
// is only called by AdvanceRead().  
// Updates readPtr to and returns the first stress message >= startPtr
inline StressMsg* ThreadStressLog::AdvReadPastBoundary() {
    STATIC_CONTRACT_LEAF;
    //if we pass boundary of tail list, we need to set has Wrapped
    if (curReadChunk == chunkListTail)
    {
        readHasWrapped = TRUE;
        //If write has not wrapped, we know the contents from list head to 
        //cur pointer is garbage, we don't need to read them
        if (!writeHasWrapped)
        {
            return readPtr;
        }
    }
    curReadChunk = curReadChunk->next;
    void** p = (void**)curReadChunk->StartPtr();
    while (*p == NULL && (size_t)(p-(void**)curReadChunk->StartPtr ()) < (StressMsg::maxMsgSize()/sizeof(void*)))
    {
        ++p;
    }
    // if we failed to find a valid start of a StressMsg fallback to startPtr (since timeStamp==0)
    if (*p == NULL)
    {
        p = (void**) curReadChunk->StartPtr ();
    }
    readPtr = (StressMsg*)p;

    return readPtr;
}
#endif //STRESS_LOG_READONLY
/*********************************************************************************/
// Called at runtime when writing the log (by StressLog::LogMsg())
// Updates curPtr to point to the next spot in the log where we can write
// a stress message with cArgs arguments
// For convenience it returns a pointer to the empty slot where we can 
// write the next stress message.
// cArgs is the number of arguments in the message to be written.
inline StressMsg* ThreadStressLog::AdvanceWrite(int cArgs) {
    STATIC_CONTRACT_LEAF;
    // _ASSERTE(cArgs <= StressMsg::maxArgCnt);
    // advance the marker
    StressMsg* p = (StressMsg*)((char*)curPtr - sizeof(StressMsg) - cArgs*sizeof(void*));

    //past start of current chunk
    //wrap around if we need to
    if (p < (StressMsg*)curWriteChunk->StartPtr ())
    {
       curPtr = AdvWritePastBoundary(cArgs);
    }
    else
    {
        curPtr = p;
    }
    
    return curPtr;
}

// It's the factored-out slow codepath for AdvanceWrite() and
// is only called by AdvanceWrite().  
// Returns the stress message flushed against endPtr
// In addition it writes NULLs b/w the startPtr and curPtr
inline StressMsg* ThreadStressLog::AdvWritePastBoundary(int cArgs) {
    STATIC_CONTRACT_LEAF;
#ifndef STRESS_LOG_READONLY    
     //zeroed out remaining buffer
    memset (curWriteChunk->StartPtr (), 0, (BYTE *)curPtr - (BYTE *)curWriteChunk->StartPtr ());
        
    //if we are already at head of the list, try to grow the list
    if (curWriteChunk == chunkListHead)
    {
        GrowChunkList ();            
    }
#endif //!STRESS_LOG_READONLY        

    curWriteChunk = curWriteChunk->prev;
#ifndef STRESS_LOG_READONLY
   if (curWriteChunk == chunkListTail)
   {
        writeHasWrapped = TRUE;
   }
#endif //STRESS_LOG_READONLY
    curPtr = (StressMsg*)((char*)curWriteChunk->EndPtr () - sizeof(StressMsg) - cArgs * sizeof(void*));    
    return curPtr;
}

#else   // STRESS_LOG

#define STRESS_LOG_VA(msg)                                              do { } while(0)
#define STRESS_LOG0(facility, level, msg)                               do { } while(0)
#define STRESS_LOG1(facility, level, msg, data1)                        do { } while(0)
#define STRESS_LOG2(facility, level, msg, data1, data2)                 do { } while(0)
#define STRESS_LOG2_CHECK_EE_STARTED(facility, level, msg, data1, data2)do { } while(0)
#define STRESS_LOG3(facility, level, msg, data1, data2, data3)          do { } while(0)
#define STRESS_LOG4(facility, level, msg, data1, data2, data3, data4)   do { } while(0)
#define STRESS_LOG5(facility, level, msg, data1, data2, data3, data4, data5)   do { } while(0)
#define STRESS_LOG6(facility, level, msg, data1, data2, data3, data4, data5, data6)   do { } while(0)
#define STRESS_LOG7(facility, level, msg, data1, data2, data3, data4, data5, data6, data7)   do { } while(0)
#define STRESS_LOG_PLUG_MOVE(plug_start, plug_end, plug_delta)          do { } while(0)
#define STRESS_LOG_ROOT_PROMOTE(root_addr, objPtr, methodTable)         do { } while(0)
#define STRESS_LOG_ROOT_RELOCATE(root_addr, old_value, new_value, methodTable) do { } while(0)
#define STRESS_LOG_GC_START(gcCount, Gen, collectClasses)               do { } while(0)
#define STRESS_LOG_GC_END(gcCount, Gen, collectClasses)                 do { } while(0)
#define STRESS_LOG_OOM_STACK(size)   do { } while(0)
#define STRESS_LOG_GC_STACK(size)   do { } while(0)
#define STRESS_LOG_RESERVE_MEM(numChunks) do {} while (0)
#endif // STRESS_LOG

#endif // StressLog_h 
