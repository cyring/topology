/*
 * topology
 * Copyright (C) 2017 CYRIL INGENIERIE
 * Licenses: GPL2
 */

#define ROUND_TO_PAGES(Size)	PAGE_SIZE * ((Size / PAGE_SIZE) 	\
				+ ((Size % PAGE_SIZE)? 1:0))

#define LEVEL_INVALID	0
#define LEVEL_THREAD	1
#define LEVEL_CORE	2
#define CACHE_MAX_LEVEL	(3 + 1)

#define VENDOR_INTEL	"GenuineIntel"
#define VENDOR_AMD	"AuthenticAMD"

typedef struct
{
	struct
	{
		unsigned char Chr[4];
	} AX, BX, CX, DX;
} BRAND;

typedef struct
{		// Common x86
	unsigned int	LargestStdFunc, // Largest CPUID Standard Function.
			LargestExtFunc;	// Largest CPUID Extended Function.
	char		Brand[48],
			_pad48[2],
			VendorID[13],
			_pad62[1];
} CPUID_FUNCTION;

typedef struct	// Basic CPUID Function.
{
		unsigned int LargestStdFunc, BX, CX, DX;
} CPUID_0x00000000;

typedef struct
{
	union
	{
	    struct SIGNATURE
	    {
		unsigned int
		Stepping	:  4-0,
		Model		:  8-4,
		Family		: 12-8,
		ProcType	: 14-12,
		Unused1		: 16-14,
		ExtModel	: 20-16,
		ExtFamily	: 28-20,
		Unused2		: 32-28;
	    } AX;
		unsigned int Signature;
	};
	struct
	{
		unsigned int
		Brand_ID	:  8-0,
		CLFSH_Size	: 16-8,
		MaxThread	: 24-16,
		Apic_ID		: 32-24;
	} BX;
	struct
	{
		unsigned int
		SSE3	:  1-0,  // AMD Family 0Fh
		PCLMULDQ:  2-1,
		DTES64	:  3-2,
		MONITOR	:  4-3,
		DS_CPL	:  5-4,
		VMX	:  6-5,
		SMX	:  7-6,
		EIST	:  8-7,
		TM2	:  9-8,
		SSSE3	: 10-9,  // AMD Family 0Fh
		CNXT_ID	: 11-10,
		Unused1	: 12-11,
		FMA	: 13-12,
		CMPXCH16: 14-13,
		xTPR	: 15-14,
		PDCM	: 16-15,
		Unused2	: 17-16,
		PCID	: 18-17,
		DCA	: 19-18,
		SSE41	: 20-19,
		SSE42	: 21-20,
		x2APIC	: 22-21,
		MOVBE	: 23-22,
		POPCNT	: 24-23,
		TSCDEAD	: 25-24,
		AES	: 26-25,
		XSAVE	: 27-26,
		OSXSAVE	: 28-27,
		AVX	: 29-28,
		F16C	: 30-29,
		RDRAND	: 31-30,
		Unused3	: 32-31;
	} CX;
	struct
	{	// Most common x86
		unsigned int
		FPU	:  1-0,
		VME	:  2-1,
		DE	:  3-2,
		PSE	:  4-3,
		TSC	:  5-4,
		MSR	:  6-5,
		PAE	:  7-6,
		MCE	:  8-7,
		CMPXCH8	:  9-8,
		APIC	: 10-9,
		Unused1	: 11-10,
		SEP	: 12-11,
		MTRR	: 13-12,
		PGE	: 14-13,
		MCA	: 15-14,
		CMOV	: 16-15,
		PAT	: 17-16,
		PSE36	: 18-17,
		PSN	: 19-18, // Intel
		CLFSH	: 20-19,
		Unused2	: 21-20,
		DS_PEBS	: 22-21,
		ACPI	: 23-22,
		MMX	: 24-23,
		FXSR	: 25-24, // FXSAVE and FXRSTOR instructions.
		SSE	: 26-25,
		SSE2	: 27-26,
		SS	: 28-27, // Intel
		HTT	: 29-28,
		TM1	: 30-29, // Intel
		Unused3	: 31-30,
		PBE	: 32-31; // Intel
	} DX;
} CPUID_0x00000001;

typedef struct	// MONITOR & MWAIT Leaf.
{		// Common x86
	struct
	{
		unsigned int
		SmallestSize	: 16-0,
		ReservedBits	: 32-16;
	} AX;
	struct
	{
		unsigned int
		LargestSize	: 16-0,
		ReservedBits	: 32-16;
	} BX;
	struct
	{
		unsigned int
		EMX_MWAIT	:  1-0,
		IBE_MWAIT	:  2-1,
		ReservedBits	: 32-2;
	} CX;
	struct
	{	// Intel reseved.
		unsigned int
		Num_C0_MWAIT	:  4-0,
		Num_C1_MWAIT	:  8-4,
		Num_C2_MWAIT	: 12-8,
		Num_C3_MWAIT	: 16-12,
		Num_C4_MWAIT	: 20-16,
		ReservedBits	: 32-20;
	} DX;
}  CPUID_0x00000005;

typedef struct THERMAL_POWER_LEAF
{	// Thermal and Power Management Leaf.
	struct
	{	// Most Intel reserved.
		unsigned int
		DTS	:  1-0,
		TurboIDA:  2-1,
		ARAT	:  3-2, // Common x86
		Unused1	:  4-3,
		PLN	:  5-4,
		ECMD	:  6-5,
		PTM	:  7-6,
		HWP_Reg	:  8-7,	// Hardware Performance registers
		HWP_Int	:  9-8,	// IA32_HWP_INTERRUPT HWP_Notification.
		HWP_Act	: 10-9,	// IA32_HWP_REQUEST Activity_Window
		HWP_Prf	: 11-10,// IA32_HWP_REQUEST Performance_Pref.
		HWP_Lvl	: 12-11,// IA32_HWP_REQUEST_PKG
		Unused2	: 13-12,
		HDC_Reg	: 15-13,// Hardware Duty Cycling registers
		Unused3	: 32-15;
	} AX;
	struct
	{	// Intel reserved.
		unsigned int
		Threshld:  4-0,
		Unused1	: 32-4;
	} BX;
    union
    {
	struct
	{	// Intel reserved.
		unsigned int
		HCF_Cap	:  1-0, // MSR: IA32_MPERF (E7H) & IA32_APERF (E8H)
		ACNT_Cap:  2-1,
		Unused1	:  3-2,
		SETBH	:  4-3,
		Unused2	: 32-4;
	};
	struct
	{	// AMD reserved.
		unsigned int
		EffFreq	:  1-0, // MSR0000_00E7 (MPERF) & MSR0000_00E8 (APERF)
		NotUsed : 32-1;
	};
    } CX;
	struct
	{	// Intel reserved.
		unsigned int
		Unused1	: 32-0;
	} DX;
} CPUID_0x00000006;

typedef struct	// Extended Feature Flags Enumeration Leaf.
{
	struct
	{	// Common x86
		unsigned int
		MaxSubLeaf	: 32-0;
	} AX;
	struct
	{
		unsigned int
		FSGSBASE	:  1-0, // Common x86
		TSC_ADJUST	:  2-1,
		Unused1		:  3-2,
		BMI1		:  4-3, // Common x86
		HLE		:  5-4,
		AVX2		:  6-5, // Common x86
		Unused2		:  7-6,
		SMEP		:  8-7, // Common x86
		BMI2		:  9-8, // Common x86
		FastStrings	: 10-9,
		INVPCID		: 11-10,
		RTM		: 12-11,
		PQM		: 13-12,
		FPU_CS_DS	: 14-13,
		MPX		: 15-14,
		PQE		: 16-15,
		Unused3		: 18-16,
		RDSEED		: 19-18,
		ADX		: 20-19,
		SMAP		: 21-20,
		Unused4		: 25-21,
		ProcessorTrace	: 26-25,
		Unused5		: 32-26;
	} BX;
	struct
	{	// Intel reserved.
		unsigned int
		PREFETCHWT1	:  1-0,
		Unused1		:  3-1,
		PKU		:  4-3,
		OSPKE		:  5-4,
		Unused2		: 32-5;
	} CX;
		unsigned int
	DX			: 32-0; // Intel reserved.
} CPUID_0x00000007;

typedef struct	// Architectural Performance Monitoring Leaf.
{	// Intel reserved.
	struct
	{
		unsigned int
		Version	:  8-0,
		MonCtrs	: 16-8,
		MonWidth: 24-16,
		VectorSz: 32-24;
	} AX;
	struct
	{
		unsigned int
		CoreCycles	:  1-0,
		InstrRetired	:  2-1,
		RefCycles	:  3-2,
		LLC_Ref		:  4-3,
		LLC_Misses	:  5-4,
		BranchRetired	:  6-5,
		BranchMispred	:  7-6,
		ReservedBits	: 32-7;
	} BX;
	struct
	{
		unsigned int
		Unused1	: 32-0;
	} CX;
	struct
	{
		unsigned int
		FixCtrs	:  5-0,
		FixWidth: 13-5,
		Unused1	: 32-13;
	} DX;
} CPUID_0x0000000a;

typedef struct	// Extended CPUID Function.
{
		unsigned int LargestExtFunc, BX, CX, DX;
} CPUID_0x80000000;

typedef	struct
{
    union
    {
	struct	{ // Intel reserved.
		unsigned int
		LAHFSAHF:  1-0,  // LAHF and SAHF instruction support.
		Unused1	: 32-1;
	};
	struct	{ // AMD reserved.
		unsigned int
		// Family 0Fh :
		LahfSahf:  1-0,
		MP_Mode	:  2-1,  // Core multi-processing legacy mode.
		SVM	:  3-2,  // Secure virtual machine.
		Ext_APIC:  4-3,  // Extended APIC space.
		AltMov	:  5-4,	 // AltMovCr8
		ABM	:  6-5,  // LZCNT instruction support.
		SSE4A	:  7-6,
		AlignSSE:  8-7,  // Misaligned SSE mode.
		PREFETCH:  9-8,  // 3DNow PREFETCH, PREFETCHW instruction.
		// Family 15h :
		OSVW	: 10-9,  // OS-visible workaround support.
		IBS	: 11-10, // Instruction based sampling.
		XOP	: 12-11, // Extended operation support.
		SKINIT	: 13-12, // SKINIT and STGI support.
		WDT	: 14-13, // Watchdog timer support.
		NotUsed1: 15-14,
		LWP	: 16-15, // Lightweight profiling support.
		FMA4	: 17-16, // Four-operand FMA instruction.
		TCE	: 18-17, // Translation Cache Extension.
		NotUsed2: 21-18,
		TBM	: 22-21, // Trailing bit manipulation.
		TopoExt	: 23-22, // Topology extensions support.
		PerfCore: 24-23, // PerfCtrExtCore MSR.
		PerfNB	: 25-24, // PerfCtrExtNB MSR.
		NotUsed3: 26-25,
		Data_BP	: 27-26, // Data access breakpoint extension.
		PerfTSC	: 28-27, // Performance TSC MSR.
		PerfL2I	: 29-28, // L2I performance counter extensions support.
		MWaitExt: 30-29, // MWAITX/MONITORX support.
		NotUsed4: 32-30;
	};
    } CX;
    union
    {
	struct { // Intel reserved.
		unsigned int
		Unused1	: 11-0,
		SYSCALL	: 12-11,
		Unused2	: 20-12,
		XD_Bit	: 21-20,
		Unused3	: 26-21,
		PG_1GB	: 27-26,
		RdTSCP	: 28-27,
		Unused4	: 29-28,
		IA64	: 30-29,
		Unused5	: 32-30;
	};
	struct { // AMD reserved.
		unsigned int	 // Most bits equal to CPUID 0x01
		FPU	:  1-0,
		VME	:  2-1,  // Virtual-mode enhancements.
		DE	:  3-2,  // Debugging extensions.
		PSE	:  4-3,  // Page-size extensions.
		TSC	:  5-4,
		MSR	:  6-5,  // AMD MSR.
		PAE	:  7-6,
		MCE	:  8-7,
		CMPXCH8	:  9-8,
		APIC	: 10-9,
		NotUsed1: 11-10,
		SEP	: 12-11,
		MTRR	: 13-12,
		PGE	: 14-13,
		MCA	: 15-14,
		CMOV	: 16-15,
		PAT	: 17-16,
		PSE36	: 18-17,
		NotUsed2: 20-18,
		NX	: 21-20, // No-execute page protection.
		NotUsed3: 22-21,
		MMX_Ext : 23-22, // MMX extensions.
		MMX	: 24-23,
		FXSR	: 25-24,
		FFXSR	: 26-25, // FXSAVE and FXRSTOR optimizations.
		Page1GB	: 27-26,
		RDTSCP	: 28-27,
		NotUsed4: 29-28,
		LM	: 30-29, // Long mode.
		_3DNowEx: 31-30, // Extensions to 3DNow!
		_3DNow	: 32-31; // 3DNow! instructions.
	};
    } DX;
} CPUID_0x80000001;

typedef struct	// Architectural Performance Monitoring Leaf.
{
	struct
	{
		unsigned int
		Unused1	: 32-0;
	} AX, BX, CX;
    union
    {
	struct { // Intel reserved.
		unsigned int
		Unused1	:  8-0,
		Inv_TSC	:  9-8, // Invariant TSC available if 1
		Unused2	: 32-9;
	};
      union
      {		// AMD Family 0Fh
	struct {
		unsigned int
		TS	:  1-0,  // Temperature sensor
		FID	:  2-1,  // Frequency ID control is supported.
		VID	:  3-2,  // Voltage ID control is supported.
		TTP	:  4-3,  // THERMTRIP is supported = 1.
		TM	:  5-4,  // Hardware thermal control (HTC).
		STC	:  6-5,  // K7-K8: Software thermal control (STC)
		_100MHz	:  7-6,  // 100 MHz multiplier Control.
		NotUsed	: 32-7;
	};
	struct { // AMD Family 15h
		unsigned int
		Fam_0Fh	:  7-0,  // Family 0Fh features.
		HwPstate:  8-7,  // Hardware P-state control msr exist ?
		TscInv	:  9-8,  // Invariant TSC ?
		CPB	: 10-9,  // Core performance boost.
		EffFrqRO: 11-10, // Read-only effective frequency interf. msr ?
		ProcFb	: 12-11, // Processor feedback interface available if 1
		ProcPwr	: 13-12, // Core power reporting interface supported.
		Reserved: 32-13;
	};
      };
    } DX;
} CPUID_0x80000007;

typedef struct	// BSP CPUID features.
{
	CPUID_FUNCTION Info;

	CPUID_0x00000001 Std;
	CPUID_0x00000005 MWait;
	CPUID_0x00000006 Power;
	CPUID_0x00000007 ExtFeature;
	CPUID_0x0000000a PerfMon;
	CPUID_0x80000001 ExtInfo;
	CPUID_0x80000007 AdvPower;

	unsigned int	HTT_Enable,
			FactoryFreq;
} FEATURES;

typedef union
{
	unsigned long long	value;
	struct
	{
		unsigned long long
		ReservedBits1	:  8-0,
		BSP		:  9-8,
		ReservedBits2	: 10-9,
		EXTD		: 11-10,
		EN		: 12-11,
		Addr		: 64-12;
	};
} LOCAL_APIC;

typedef struct {
	union {
		struct
		{
			unsigned int
			SHRbits	:  5-0,
			Unused1	: 32-5;
		};
		unsigned int Register;
	} AX;
	union {
		struct
		{
			unsigned int
			Threads	: 16-0,
			Unused1	: 32-16;
		};
		unsigned int Register;
	} BX;
	union {
		struct
		{
			unsigned int
			Level	:  8-0,
			Type	: 16-8,
			Unused1 : 32-16;
		};
		unsigned int Register;
	} CX;
	union {
		struct
		{
			unsigned int
			x2ApicID: 32-0;
		};
		unsigned int Register;
	} DX;
} CPUID_TOPOLOGY_LEAF;

typedef struct
{
	LOCAL_APIC	Base;
	int		ApicID,
			CoreID,
			ThreadID;

	struct CACHE_INFO
	{
		union
		{
			struct
			{	// Intel
				unsigned int
				Type	:  5-0,  // Cache Type *
				Level	:  8-5,  // Cache Level (starts at 1)
				Init	:  9-8,  // Self Init. cache level
				Assoc	: 10-9,  // Fully Associative cache
				Unused	: 14-10,
				MxThrdID: 26-14, // Max threads for this cache
				MxCoreID: 32-26; // Max cores for this cache
			};
			struct
			{	// AMD L1
				unsigned int
				ISize	:  8-0,  // Inst. TLB number of entries
				IAssoc	: 16-8,  // Inst. TLB associativity
				DSize	: 24-16, // Data TLB number of entries
				DAssoc	: 32-24; // Data TLB associativity
			} CPUID_0x80000005_L1Tlb2and4M; // for 2 MB & 4 MB pages
			struct
			{	// AMD L2
				unsigned int
				ISize	: 12-0,
				IAssoc	: 16-12,
				DSize	: 28-16,
				DAssoc	: 32-28;
			} CPUID_0x80000006_L2ITlb2and4M;
			unsigned int AX;
		};
		union
		{
			struct
			{	// Intel
				unsigned int
				LineSz	: 12-0,  // L=System Coherency Line Size
				Part	: 22-12, // P=Physical Line partitions
				Way	: 32-22; // W=Ways of associativity
			};
			struct
			{	// AMD L1
				unsigned int
				ISize	:  8-0,  // Inst. TLB number of entries
				IAssoc	: 16-8,  // Inst. TLB associativity *
				DSize	: 24-16, // Data TLB number of entries
				DAssoc	: 32-24; // Data TLB associativity *
			} CPUID_0x80000005_L1Tlb4K; // for 4 KB pages
			struct
			{	// AMD L2
				unsigned int
				ISize	: 12-0,
				IAssoc	: 16-12,
				DSize	: 28-16,
				DAssoc	: 32-28;
			} CPUID_0x80000006_L2Tlb4K;
			unsigned int BX;
		};
		union
		{		// Intel
			unsigned int Set;	// S=Number of Sets
			struct
			{	// AMD L1
				unsigned int
				LineSz	:  8-0,  // L1 data cache line size (B)
				ClPerTag: 16-8,  // L1 data cache lines per tag
				Assoc	: 24-16, // L1 data cache associativity*
				Size	: 32-24; // L1 data cache size (KB)
			} CPUID_0x80000005_L1D;
			struct
			{	// AMD L2
				unsigned int
				LineSz	:  8-0,  // L2 cache line size (B)
				ClPerTag: 12-8,  // L2 cache lines per tag
				Assoc	: 16-12, // L2 cache associativity **
				Size	: 32-16; // L2 cache size (KB) ***
			} CPUID_0x80000006_L2;
			unsigned int CX;
		};
		union
		{
			struct
			{	// Intel
				unsigned int
				WrBack	: 1-0,  // Write-Back **
				Inclus	: 2-1,  // Cache Inclusiveness ***
				Direct	: 3-2,  // Complex Cache Indexing ****
				Resrvd	: 32-3;
			};
			struct
			{	// AMD L1
				unsigned int
				LineSz	:  8-0,  // L1 inst. cache line size (B)
				ClPerTag: 16-8,  // L1 inst. cache lines per tag
				Assoc	: 24-16, // L1 inst. cache associativity
				Size	: 32-24; // L1 inst. cache size (KB) 
			} CPUID_0x80000005_L1I;
			struct
			{	// AMD L3
				unsigned int
				LineSz	:  8-0,  // L3 cache line (B)
				ClPerTag: 12-8,  // L3 cache lines per tag
				Assoc	: 16-12, // L3 cache associativity
				Reserved: 18-16,
				Size	: 32-18; // L3 cache size
			} CPUID_0x80000006_L3;
			unsigned int DX;
		};
		unsigned int	Size;
	} Cache[CACHE_MAX_LEVEL];
} CACHE_TOPOLOGY;

/*
--- Intel Cache Parameters Leaf ---

* Cache Type Field
	0 = Null - No more caches
	1 = Data Cache
	2 = Instruction Cache
	3 = Unified Cache
	4-31 = Reserved

** Write-Back Invalidate/Invalidate
	0 = WBINVD/INVD from threads sharing this cache
		acts upon lower level caches for threads sharing this cache.
	1 = WBINVD/INVD is not guaranteed to act upon lower level caches
		of non-originating threads sharing this cache.

*** Cache Inclusiveness
	0 = Cache is not inclusive of lower cache levels.
	1 = Cache is inclusive of lower cache levels.

**** Complex Cache Indexing
	0 = Direct mapped cache.
	1 = A complex function is used to index the cache,
		potentially using all address bits.


--- AMD Cache Identifiers ---

* L1 data cache associativity
	Bits	Description
	00h	Reserved
	01h	1 way (direct mapped)
	02h	2 way
	03h	3 way
	FEh-04h	[L1IcAssoc] way
	FFh	Fully associative

** L2 cache associativity
	Bits	Description		Bits	Description
	0h	Disabled.		8h	16 ways
	1h	1 way (direct mapped)	9h	Reserved
	2h	2 ways			Ah	32 ways
	3h	Reserved		Bh	48 ways
	4h	4 ways			Ch	64 ways
	5h	Reserved		Dh	96 ways
	6h	8 ways			Eh	128 ways
	7h	Reserved		Fh	Fully associative

*** L2 cache size
	Bits		Description
	03FFh-0000h	Reserved
	0400h		1 MB
	07FFh-0401h	Reserved
	0800h		2 MB
	FFFFh-0801h	Reserved
*/

typedef struct
{
	unsigned long long	HW,
				OS;
} OFFLINE;

typedef struct
{
	OFFLINE				OffLine;

	struct
	{
		unsigned long long
					EIST	:  1-0,	// Package
					C1E	:  2-1,	// Package
					Turbo	:  3-2,	// Thread
					C3A	:  4-3,	// Core
					C1A	:  5-4,	// Core
					C3U	:  6-5,	// Sandy Bridge
					C1U	:  7-6,	// Sandy Bridge
					Unused	: 64-7;

		CPUID_0x00000000	StdFunc;
		CPUID_0x80000000	ExtFunc;
	} Query;

	CACHE_TOPOLOGY			T;

	unsigned int			Bind;
} CORE;

typedef struct
{
	FEATURES		Features;

	struct {
		unsigned int	Count,
				OnLine;
	} CPU;
} PROC;

typedef struct
{
	struct kmem_cache	*Cache;
	CORE			*Core[];
} KPUBLIC;

#define RDMSR(_data, _reg)						\
({									\
	unsigned int _lo, _hi;						\
									\
	asm volatile							\
	(								\
		"rdmsr"							\
		: "=a" (_lo),						\
		  "=d" (_hi)						\
		: "c" (_reg)						\
	);								\
	_data.value=_lo | ((unsigned long long) _hi << 32);		\
})
