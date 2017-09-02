/*
 * topology
 * Copyright (C) 2017 CYRIL INGENIERIE
 * Licenses: GPL2
 */

#include <linux/module.h>
#include <linux/slab.h>

#include "topology.h"

MODULE_AUTHOR ("CYRIL INGENIERIE <labs[at]cyring[dot]fr>");
MODULE_DESCRIPTION ("Processor Topology Driver");
MODULE_SUPPORTED_DEVICE ("x86_64");
MODULE_LICENSE ("GPL");


static PROC *Proc = NULL;
static KPUBLIC *KPublic = NULL;

typedef struct {
	FEATURES	features;
	unsigned int	count;
} ARG;

unsigned int Intel_Brand(char *pBrand)
{
	char idString[64] = {0x20};
	unsigned long ix = 0, jx = 0, px = 0;
	unsigned int frequency = 0, multiplier = 0;
	BRAND Brand;

	for (ix = 0; ix < 3; ix++) {
		asm volatile
		(
			"movq	%4,    %%rax	\n\t"
			"xorq	%%rbx, %%rbx	\n\t"
			"xorq	%%rcx, %%rcx	\n\t"
			"xorq	%%rdx, %%rdx	\n\t"
			"cpuid			\n\t"
			"mov	%%eax, %0	\n\t"
			"mov	%%ebx, %1	\n\t"
			"mov	%%ecx, %2	\n\t"
			"mov	%%edx, %3"
			: "=r"  (Brand.AX),
			  "=r"  (Brand.BX),
			  "=r"  (Brand.CX),
			  "=r"  (Brand.DX)
			: "r"   (0x80000002 + ix)
			: "%rax", "%rbx", "%rcx", "%rdx"
		);
		for (jx = 0; jx < 4; jx++, px++)
			idString[px] = Brand.AX.Chr[jx];
		for (jx = 0; jx < 4; jx++, px++)
			idString[px] = Brand.BX.Chr[jx];
		for (jx = 0; jx < 4; jx++, px++)
			idString[px] = Brand.CX.Chr[jx];
		for (jx = 0; jx < 4; jx++, px++)
			idString[px] = Brand.DX.Chr[jx];
	}
	for (ix = 0; ix < 46; ix++)
		if ((idString[ix+1] == 'H') && (idString[ix+2] == 'z')) {
			switch (idString[ix]) {
			case 'M':
					multiplier = 1;
				break;
			case 'G':
					multiplier = 1000;
				break;
			case 'T':
					multiplier = 1000000;
				break;
			}
			break;
		}
	if (multiplier > 0) {
	    if (idString[ix-3] == '.') {
		frequency  = (int) (idString[ix-4] - '0') * multiplier;
		frequency += (int) (idString[ix-2] - '0') * (multiplier / 10);
		frequency += (int) (idString[ix-1] - '0') * (multiplier / 100);
	    } else {
		frequency  = (int) (idString[ix-4] - '0') * 1000;
		frequency += (int) (idString[ix-3] - '0') * 100;
		frequency += (int) (idString[ix-2] - '0') * 10;
		frequency += (int) (idString[ix-1] - '0');
		frequency *= frequency;
	    }
	}
	for (ix = jx = 0; jx < 48; jx++)
		if (!(idString[jx] == 0x20 && idString[jx+1] == 0x20))
			pBrand[ix++] = idString[jx];

	return(frequency);
}

void AMD_Brand(char *pBrand)
{
	char idString[64] = {0x20};
	unsigned long ix = 0, jx = 0, px = 0;
	BRAND Brand;

	for (ix = 0; ix < 3; ix++) {
		asm volatile
		(
			"movq	%4,    %%rax	\n\t"
			"xorq	%%rbx, %%rbx	\n\t"
			"xorq	%%rcx, %%rcx	\n\t"
			"xorq	%%rdx, %%rdx	\n\t"
			"cpuid			\n\t"
			"mov	%%eax, %0	\n\t"
			"mov	%%ebx, %1	\n\t"
			"mov	%%ecx, %2	\n\t"
			"mov	%%edx, %3"
			: "=r"  (Brand.AX),
			  "=r"  (Brand.BX),
			  "=r"  (Brand.CX),
			  "=r"  (Brand.DX)
			: "r"   (0x80000002 + ix)
			: "%rax", "%rbx", "%rcx", "%rdx"
		);
		for (jx = 0; jx < 4; jx++, px++)
			idString[px] = Brand.AX.Chr[jx];
		for (jx = 0; jx < 4; jx++, px++)
			idString[px] = Brand.BX.Chr[jx];
		for (jx = 0; jx < 4; jx++, px++)
			idString[px] = Brand.CX.Chr[jx];
		for (jx = 0; jx < 4; jx++, px++)
			idString[px] = Brand.DX.Chr[jx];
	}
	for (ix = jx = 0; jx < 48; jx++)
		if (!(idString[jx] == 0x20 && idString[jx+1] == 0x20))
			pBrand[ix++] = idString[jx];
}

// Retreive the Processor(BSP) features through calls to the CPUID instruction.
void Query_Features(void *pArg)
{
	ARG *arg = (ARG *) pArg;

	unsigned int eax = 0x0, ebx = 0x0, ecx = 0x0, edx = 0x0; // DWORD Only!

	// Must have x86 CPUID 0x0, 0x1, and Intel CPUID 0x4
	asm volatile
	(
		"xorq	%%rax, %%rax	\n\t"
		"xorq	%%rbx, %%rbx	\n\t"
		"xorq	%%rcx, %%rcx	\n\t"
		"xorq	%%rdx, %%rdx	\n\t"
		"cpuid			\n\t"
		"mov	%%eax, %0	\n\t"
		"mov	%%ebx, %1	\n\t"
		"mov	%%ecx, %2	\n\t"
		"mov	%%edx, %3"
		: "=r" (arg->features.Info.LargestStdFunc),
		  "=r" (ebx),
		  "=r" (ecx),
		  "=r" (edx)
		:
		: "%rax", "%rbx", "%rcx", "%rdx"
	);
	arg->features.Info.VendorID[ 0] = ebx;
	arg->features.Info.VendorID[ 1] = (ebx >> 8);
	arg->features.Info.VendorID[ 2] = (ebx >> 16);
	arg->features.Info.VendorID[ 3] = (ebx >> 24);
	arg->features.Info.VendorID[ 4] = edx;
	arg->features.Info.VendorID[ 5] = (edx >> 8);
	arg->features.Info.VendorID[ 6] = (edx >> 16);
	arg->features.Info.VendorID[ 7] = (edx >> 24);
	arg->features.Info.VendorID[ 8] = ecx;
	arg->features.Info.VendorID[ 9] = (ecx >> 8);
	arg->features.Info.VendorID[10] = (ecx >> 16);
	arg->features.Info.VendorID[11] = (ecx >> 24);
	arg->features.Info.VendorID[12] = '\0';

	asm volatile
	(
		"movq	$0x1,  %%rax	\n\t"
		"xorq	%%rbx, %%rbx	\n\t"
		"xorq	%%rcx, %%rcx	\n\t"
		"xorq	%%rdx, %%rdx	\n\t"
		"cpuid			\n\t"
		"mov	%%eax, %0	\n\t"
		"mov	%%ebx, %1	\n\t"
		"mov	%%ecx, %2	\n\t"
		"mov	%%edx, %3"
		: "=r" (arg->features.Std.AX),
		  "=r" (arg->features.Std.BX),
		  "=r" (arg->features.Std.CX),
		  "=r" (arg->features.Std.DX)
		:
		: "%rax", "%rbx", "%rcx", "%rdx"
	);
	if (arg->features.Info.LargestStdFunc >= 0x5) {
		asm volatile
		(
			"movq	$0x5,  %%rax	\n\t"
			"xorq	%%rbx, %%rbx	\n\t"
			"xorq	%%rcx, %%rcx	\n\t"
			"xorq	%%rdx, %%rdx	\n\t"
			"cpuid			\n\t"
			"mov	%%eax, %0	\n\t"
			"mov	%%ebx, %1	\n\t"
			"mov	%%ecx, %2	\n\t"
			"mov	%%edx, %3"
			: "=r" (arg->features.MWait.AX),
			  "=r" (arg->features.MWait.BX),
			  "=r" (arg->features.MWait.CX),
			  "=r" (arg->features.MWait.DX)
			:
			: "%rax", "%rbx", "%rcx", "%rdx"
		);
	}
	if (arg->features.Info.LargestStdFunc >= 0x6) {
		asm volatile
		(
			"movq	$0x6,  %%rax	\n\t"
			"xorq	%%rbx, %%rbx	\n\t"
			"xorq	%%rcx, %%rcx	\n\t"
			"xorq	%%rdx, %%rdx	\n\t"
			"cpuid			\n\t"
			"mov	%%eax, %0	\n\t"
			"mov	%%ebx, %1	\n\t"
			"mov	%%ecx, %2	\n\t"
			"mov	%%edx, %3"
			: "=r" (arg->features.Power.AX),
			  "=r" (arg->features.Power.BX),
			  "=r" (arg->features.Power.CX),
			  "=r" (arg->features.Power.DX)
			:
			: "%rax", "%rbx", "%rcx", "%rdx"
		);
	}
	if (arg->features.Info.LargestStdFunc >= 0x7) {
		asm volatile
		(
			"movq	$0x7, %%rax	\n\t"
			"xorq	%%rbx, %%rbx    \n\t"
			"xorq	%%rcx, %%rcx    \n\t"
			"xorq	%%rdx, %%rdx    \n\t"
			"cpuid			\n\t"
			"mov	%%eax, %0	\n\t"
			"mov	%%ebx, %1	\n\t"
			"mov	%%ecx, %2	\n\t"
			"mov	%%edx, %3"
			: "=r" (arg->features.ExtFeature.AX),
			  "=r" (arg->features.ExtFeature.BX),
			  "=r" (arg->features.ExtFeature.CX),
			  "=r" (arg->features.ExtFeature.DX)
			:
			: "%rax", "%rbx", "%rcx", "%rdx"
		);
	}
	// Must have 0x80000000, 0x80000001, 0x80000002, 0x80000003, 0x80000004
	asm volatile
	(
		"movq	$0x80000000, %%rax	\n\t"
		"xorq	%%rbx, %%rbx		\n\t"
		"xorq	%%rcx, %%rcx		\n\t"
		"xorq	%%rdx, %%rdx		\n\t"
		"cpuid				\n\t"
		"mov	%%eax, %0		\n\t"
		"mov	%%ebx, %1		\n\t"
		"mov	%%ecx, %2		\n\t"
		"mov	%%edx, %3"
		: "=r" (arg->features.Info.LargestExtFunc),
		  "=r" (ebx),
		  "=r" (ecx),
		  "=r" (edx)
		:
		: "%rax", "%rbx", "%rcx", "%rdx"
	);
	asm volatile
	(
		"movq	$0x80000001, %%rax	\n\t"
		"xorq	%%rbx, %%rbx		\n\t"
		"xorq	%%rcx, %%rcx		\n\t"
		"xorq	%%rdx, %%rdx		\n\t"
		"cpuid				\n\t"
		"mov	%%eax, %0		\n\t"
		"mov	%%ebx, %1		\n\t"
		"mov	%%ecx, %2		\n\t"
		"mov	%%edx, %3"
		: "=r" (eax),
		  "=r" (ebx),
		  "=r" (arg->features.ExtInfo.CX),
		  "=r" (arg->features.ExtInfo.DX)
		:
		: "%rax", "%rbx", "%rcx", "%rdx"
	);
	if (arg->features.Info.LargestExtFunc >= 0x80000007) {
		asm volatile
		(
			"cpuid"
			: "=a" (arg->features.AdvPower.AX),
			  "=b" (arg->features.AdvPower.BX),
			  "=c" (arg->features.AdvPower.CX),
			  "=d" (arg->features.AdvPower.DX)
			:  "a" (0x80000007)
		);
	}

	// Reset the performance features bits (present is zero)
	arg->features.PerfMon.BX.CoreCycles    = 1;
	arg->features.PerfMon.BX.InstrRetired  = 1;
	arg->features.PerfMon.BX.RefCycles     = 1;
	arg->features.PerfMon.BX.LLC_Ref       = 1;
	arg->features.PerfMon.BX.LLC_Misses    = 1;
	arg->features.PerfMon.BX.BranchRetired = 1;
	arg->features.PerfMon.BX.BranchMispred = 1;

	// Per Vendor features
	if (!strncmp(arg->features.Info.VendorID, VENDOR_INTEL, 12)) {
		unsigned int MaxCore = 0; //arg->features.Std.BX.MaxThread;

		asm volatile
		(
			"movq	$0x4,  %%rax	\n\t"
			"xorq	%%rbx, %%rbx	\n\t"
			"xorq	%%rcx, %%rcx	\n\t"
			"xorq	%%rdx, %%rdx	\n\t"
			"cpuid			\n\t"
			"mov	%%eax, %0	\n\t"
			"mov	%%ebx, %1	\n\t"
			"mov	%%ecx, %2	\n\t"
			"mov	%%edx, %3"
			: "=r" (eax),
			  "=r" (ebx),
			  "=r" (ecx),
			  "=r" (edx)
			:
			: "%rax", "%rbx", "%rcx", "%rdx"
		);
		MaxCore = (eax >> 26) & 0x3f;
		MaxCore++;
		arg->count = MaxCore;

	    if (arg->features.Info.LargestStdFunc >= 0xa) {
		asm volatile
		(
			"movq	$0xa,  %%rax	\n\t"
			"xorq	%%rbx, %%rbx	\n\t"
			"xorq	%%rcx, %%rcx	\n\t"
			"xorq	%%rdx, %%rdx	\n\t"
			"cpuid			\n\t"
			"mov	%%eax, %0	\n\t"
			"mov	%%ebx, %1	\n\t"
			"mov	%%ecx, %2	\n\t"
			"mov	%%edx, %3"
			: "=r" (arg->features.PerfMon.AX),
			  "=r" (arg->features.PerfMon.BX),
			  "=r" (arg->features.PerfMon.CX),
			  "=r" (arg->features.PerfMon.DX)
			:
			: "%rax", "%rbx", "%rcx", "%rdx"
		);
	    }
	    arg->features.FactoryFreq = Intel_Brand(arg->features.Info.Brand);

	} else if (!strncmp(arg->features.Info.VendorID, VENDOR_AMD, 12)) {

		if (arg->features.Std.DX.HTT)
			arg->count = arg->features.Std.BX.MaxThread;
		else {
			if (arg->features.Info.LargestExtFunc >= 0x80000008) {
				asm volatile
				(
					"movq	$0x80000008, %%rax	\n\t"
					"xorq	%%rbx, %%rbx		\n\t"
					"xorq	%%rcx, %%rcx		\n\t"
					"xorq	%%rdx, %%rdx		\n\t"
					"cpuid				\n\t"
					"mov	%%eax, %0		\n\t"
					"mov	%%ebx, %1		\n\t"
					"mov	%%ecx, %2		\n\t"
					"mov	%%edx, %3"
					: "=r" (eax),
					  "=r" (ebx),
					  "=r" (ecx),
					  "=r" (edx)
					:
					: "%rax", "%rbx", "%rcx", "%rdx"
				);
				arg->count = (ecx & 0xf) + 1;
			}
		}
		AMD_Brand(arg->features.Info.Brand);
	}
}

void Cache_Topology(CORE *Core)
{
	unsigned long level = 0x0;
	if (!strncmp(Proc->Features.Info.VendorID, VENDOR_INTEL, 12)) {
	    for (level = 0; level < CACHE_MAX_LEVEL; level++) {
		asm volatile
		(
			"movq	$0x4,  %%rax	\n\t"
			"xorq	%%rbx, %%rbx	\n\t"
			"movq	%4,    %%rcx	\n\t"
			"xorq	%%rdx, %%rdx	\n\t"
			"cpuid			\n\t"
			"mov	%%eax, %0	\n\t"
			"mov	%%ebx, %1	\n\t"
			"mov	%%ecx, %2	\n\t"
			"mov	%%edx, %3"
			: "=r" (Core->T.Cache[level].AX),
			  "=r" (Core->T.Cache[level].BX),
			  "=r" (Core->T.Cache[level].Set),
			  "=r" (Core->T.Cache[level].DX)
			: "r" (level)
			: "%rax", "%rbx", "%rcx", "%rdx"
		);
		if (!Core->T.Cache[level].Type)
			break;
	    }
	}
	else if (!strncmp(Proc->Features.Info.VendorID, VENDOR_AMD, 12)) {
	    struct CACHE_INFO CacheInfo; // Employ the Intel algorithm.

	    if (Proc->Features.Info.LargestExtFunc >= 0x80000005) {
		Core->T.Cache[0].Level = 1;
		Core->T.Cache[0].Type  = 2;		// Inst.
		Core->T.Cache[1].Level = 1;
		Core->T.Cache[1].Type  = 1;		// Data

		// Fn8000_0005 L1 Data and Inst. caches
		asm volatile
		(
			"movq	$0x80000005, %%rax	\n\t"
			"xorq	%%rbx, %%rbx		\n\t"
			"xorq	%%rcx, %%rcx		\n\t"
			"xorq	%%rdx, %%rdx		\n\t"
			"cpuid				\n\t"
			"mov	%%eax, %0		\n\t"
			"mov	%%ebx, %1		\n\t"
			"mov	%%ecx, %2		\n\t"
			"mov	%%edx, %3"
			: "=r" (CacheInfo.AX),
			  "=r" (CacheInfo.BX),
			  "=r" (CacheInfo.CX),
			  "=r" (CacheInfo.DX)
			:
			: "%rax", "%rbx", "%rcx", "%rdx"
		);
		// L1 Inst.
		Core->T.Cache[0].Way  = CacheInfo.CPUID_0x80000005_L1I.Assoc;
		Core->T.Cache[0].Size = CacheInfo.CPUID_0x80000005_L1I.Size;
		// L1 Data
		Core->T.Cache[1].Way  = CacheInfo.CPUID_0x80000005_L1D.Assoc;
		Core->T.Cache[1].Size = CacheInfo.CPUID_0x80000005_L1D.Size;
	    }
	    if (Proc->Features.Info.LargestExtFunc >= 0x80000006) {
		Core->T.Cache[2].Level = 2;
		Core->T.Cache[2].Type  = 3;		// Unified!
		Core->T.Cache[3].Level = 3;
		Core->T.Cache[3].Type  = 3;

		// Fn8000_0006 L2 and L3 caches
		asm volatile
		(
			"movq	$0x80000006, %%rax	\n\t"
			"xorq	%%rbx, %%rbx		\n\t"
			"xorq	%%rcx, %%rcx		\n\t"
			"xorq	%%rdx, %%rdx		\n\t"
			"cpuid				\n\t"
			"mov	%%eax, %0		\n\t"
			"mov	%%ebx, %1		\n\t"
			"mov	%%ecx, %2		\n\t"
			"mov	%%edx, %3"
			: "=r" (CacheInfo.AX),
			  "=r" (CacheInfo.BX),
			  "=r" (CacheInfo.CX),
			  "=r" (CacheInfo.DX)
			:
			: "%rax", "%rbx", "%rcx", "%rdx"
		);
		// L2
		Core->T.Cache[2].Way  = CacheInfo.CPUID_0x80000006_L2.Assoc;
		Core->T.Cache[2].Size = CacheInfo.CPUID_0x80000006_L2.Size;
		// L3
		Core->T.Cache[3].Way  = CacheInfo.CPUID_0x80000006_L3.Assoc;
		Core->T.Cache[3].Size = CacheInfo.CPUID_0x80000006_L3.Size;
	    }
	}
}

// Enumerate the Processor's Cores and Threads topology.
void Map_Topology(void *arg)
{
    if (arg != NULL) {
	unsigned int eax = 0x0, ecx = 0x0, edx = 0x0;
	CORE *Core = (CORE *) arg;
	FEATURES features;

	RDMSR(Core->T.Base, MSR_IA32_APICBASE);

	asm volatile
	(
		"movq	$0x1,  %%rax	\n\t"
		"xorq	%%rbx, %%rbx	\n\t"
		"xorq	%%rcx, %%rcx	\n\t"
		"xorq	%%rdx, %%rdx	\n\t"
		"cpuid			\n\t"
		"mov	%%eax, %0	\n\t"
		"mov	%%ebx, %1	\n\t"
		"mov	%%ecx, %2	\n\t"
		"mov	%%edx, %3"
		: "=r" (eax),
		  "=r" (features.Std.BX),
		  "=r" (ecx),
		  "=r" (edx)
		:
		: "%rax", "%rbx", "%rcx", "%rdx"
	);

	Core->T.CoreID = Core->T.ApicID=features.Std.BX.Apic_ID;

	Cache_Topology(Core);
    }
}

void Map_Extended_Topology(void *arg)
{
    if (arg != NULL) {
	CORE *Core = (CORE *) arg;

	long	InputLevel = 0;
	int	NoMoreLevels = 0,
		SMT_Mask_Width = 0, SMT_Select_Mask = 0,
		CorePlus_Mask_Width = 0, CoreOnly_Select_Mask = 0,
		Pkg_Select_Mask = 0;

	CPUID_TOPOLOGY_LEAF ExtTopology = {
		.AX.Register = 0,
		.BX.Register = 0,
		.CX.Register = 0,
		.DX.Register = 0
	};

	RDMSR(Core->T.Base, MSR_IA32_APICBASE);

	do {
	    asm volatile
	    (
		"movq	$0xb,  %%rax	\n\t"
		"xorq	%%rbx, %%rbx	\n\t"
		"movq	%4,    %%rcx	\n\t"
		"xorq	%%rdx, %%rdx	\n\t"
		"cpuid			\n\t"
		"mov	%%eax, %0	\n\t"
		"mov	%%ebx, %1	\n\t"
		"mov	%%ecx, %2	\n\t"
		"mov	%%edx, %3"
		: "=r" (ExtTopology.AX),
		  "=r" (ExtTopology.BX),
		  "=r" (ExtTopology.CX),
		  "=r" (ExtTopology.DX)
		: "r" (InputLevel)
		: "%rax", "%rbx", "%rcx", "%rdx"
	    );
	// Exit from the loop if the BX register equals 0 or
	// if the requested level exceeds the level of a Core.
	    if (!ExtTopology.BX.Register || (InputLevel > LEVEL_CORE))
		NoMoreLevels = 1;
	    else {
	      switch (ExtTopology.CX.Type) {
	      case LEVEL_THREAD: {
		SMT_Mask_Width   = ExtTopology.AX.SHRbits;

		SMT_Select_Mask  = ~((-1) << SMT_Mask_Width);

		Core->T.ThreadID = ExtTopology.DX.x2ApicID & SMT_Select_Mask;
		}
		break;
	      case LEVEL_CORE: {
		CorePlus_Mask_Width  = ExtTopology.AX.SHRbits;

		CoreOnly_Select_Mask = (~((-1) << CorePlus_Mask_Width))
					^ SMT_Select_Mask;

		Core->T.CoreID =(ExtTopology.DX.x2ApicID & CoreOnly_Select_Mask)
				>> SMT_Mask_Width;

		Pkg_Select_Mask = (~((-1) << CorePlus_Mask_Width));

		Core->T.PkgID = (ExtTopology.DX.x2ApicID & Pkg_Select_Mask)
				>> CorePlus_Mask_Width;
		}
		break;
	      }
	    }
	    InputLevel++;
	} while (!NoMoreLevels);

	Core->T.ApicID = ExtTopology.DX.x2ApicID;

	Cache_Topology(Core);
    }
}

int Core_Topology(unsigned int cpu)
{
	int rc = smp_call_function_single(cpu,
				(Proc->Features.Info.LargestStdFunc >= 0xb) ?
					Map_Extended_Topology : Map_Topology,
				KPublic->Core[cpu],
				1); // Synchronous call.

	if (	!rc
		&& !Proc->Features.HTT_Enable
		&& (KPublic->Core[cpu]->T.ThreadID > 0))
			Proc->Features.HTT_Enable = 1;

	return(rc);
}

unsigned int Proc_Topology(void)
{
	unsigned int cpu, CountEnabledCPU = 0;

	for (cpu = 0; cpu < Proc->CPU.Count; cpu++) {
		KPublic->Core[cpu]->T.Base.value = 0;
		KPublic->Core[cpu]->T.ApicID     = -1;
		KPublic->Core[cpu]->T.PkgID      = -1;
		KPublic->Core[cpu]->T.CoreID     = -1;
		KPublic->Core[cpu]->T.ThreadID   = -1;

		// CPU state based on the OS
		if (	!(KPublic->Core[cpu]->OffLine.OS = !cpu_online(cpu)
			|| !cpu_active(cpu)) ) {

			if (!Core_Topology(cpu)) {
				// CPU state based on the hardware
				if (KPublic->Core[cpu]->T.ApicID >= 0) {
					KPublic->Core[cpu]->OffLine.HW = 0;
					CountEnabledCPU++;
				}
				else
					KPublic->Core[cpu]->OffLine.HW = 1;
			}
		}
	}
	return(CountEnabledCPU);
}

void Print_Topology(void)
{
    char *line = NULL, *buffer = NULL;
    unsigned int cpu, loop, CountEnabledCPU = Proc_Topology();

    if (Proc->Features.Std.DX.HTT)
	Proc->CPU.OnLine = CountEnabledCPU;
    else
	Proc->CPU.OnLine = Proc->CPU.Count;

    line = kmalloc(128, GFP_KERNEL);
    buffer = kmalloc(256, GFP_KERNEL);
    if ((line != NULL) && (buffer != NULL)) {
	printk(	"Topology: %s\n"					\
		"Processor [%1X%1X_%1X%1X]"				\
		" [%s] CPU [%u/%u]\n"					\
		"Topology map [%s]"					\
		"                         (w)rite-Back (i)nclusive\n"	\
		" cpu#    Addr   ApicID   PkgID  CoreID ThreadID"	\
		" L1-Inst Way  L1-Data Way     L2  Way      L3  Way\n",
		Proc->Features.Info.Brand,
		Proc->Features.Std.AX.ExtFamily,
		Proc->Features.Std.AX.Family,
		Proc->Features.Std.AX.ExtModel,
		Proc->Features.Std.AX.Model,
		Proc->Features.Info.VendorID,
		Proc->CPU.OnLine,
		Proc->CPU.Count,
		(Proc->Features.Info.LargestStdFunc >= 0xb) ?
			"Extended" : "Standard");

	for(cpu = 0; cpu < Proc->CPU.OnLine; cpu++) {
	  unsigned long apic_base_addr = KPublic->Core[cpu]->T.Base.Addr << 12;
		char apic_class = 0x20;
		if (KPublic->Core[cpu]->T.Base.BSP == 1) {
			apic_class = '*';
		} else if (KPublic->Core[cpu]->T.Base.EN == 0) {
			apic_class = '!';
		}
		sprintf(line, "%3u%c %8lx%8d%8d%8d%8d", cpu,
			apic_class,
			apic_base_addr,
			KPublic->Core[cpu]->T.ApicID,
			KPublic->Core[cpu]->T.PkgID,
			KPublic->Core[cpu]->T.CoreID,
			KPublic->Core[cpu]->T.ThreadID);
		strcpy(buffer, line);

	  for (loop = 0; loop < CACHE_MAX_LEVEL; loop++) {
	    if (KPublic->Core[cpu]->T.Cache[loop].Type > 0) {
		unsigned int level = KPublic->Core[cpu]->T.Cache[loop].Level;

		if (KPublic->Core[cpu]->T.Cache[loop].Type == 2) // Instruction
			level = 0;

		if(!strncmp(Proc->Features.Info.VendorID, VENDOR_INTEL, 12)) {
			KPublic->Core[cpu]->T.Cache[level].Size =
			 (KPublic->Core[cpu]->T.Cache[loop].Set + 1)
			*(KPublic->Core[cpu]->T.Cache[loop].LineSz + 1)
			*(KPublic->Core[cpu]->T.Cache[loop].Part + 1)
			*(KPublic->Core[cpu]->T.Cache[loop].Way + 1);

		  sprintf(line, "%8u%3u%c%c",
			KPublic->Core[cpu]->T.Cache[level].Size,
			KPublic->Core[cpu]->T.Cache[level].Way + 1,
			KPublic->Core[cpu]->T.Cache[level].WrBack ? 'w' : '.',
			KPublic->Core[cpu]->T.Cache[level].Inclus ? 'i' : '.');
		}
		else {
		  if(!strncmp(Proc->Features.Info.VendorID, VENDOR_AMD, 12)) {

			unsigned int Compute_Way(unsigned int value)
			{
				switch (value) {
				case 0x6:
					return(8);
				case 0x8:
					return(16);
				case 0xa:
					return(32);
				case 0xb:
					return(48);
				case 0xc:
					return(64);
				case 0xd:
					return(96);
				case 0xe:
					return(128);
				default:
					return(value);
				}
			}

			if (loop == 2)
			  KPublic->Core[cpu]->T.Cache[level].Way =
			    Compute_Way(KPublic->Core[cpu]->T.Cache[loop].Way);

			sprintf(line, "%8u%3u%c%c",
			  KPublic->Core[cpu]->T.Cache[loop].Size,
			  KPublic->Core[cpu]->T.Cache[loop].Way,
			  KPublic->Core[cpu]->T.Cache[loop].WrBack ? 'w' : '.',
			  KPublic->Core[cpu]->T.Cache[loop].Inclus ? 'i' : '.');
		  }
		}
	      strcat(buffer, line);
	    }
	  }
	  strcat(buffer, "\n");
	  printk(buffer);
	}
    } else
	printk("Topology: Out of memory\n");

    if (line != NULL)
	kfree(line);
    if (buffer != NULL)
	kfree(buffer);
}

static int __init topology_init(void)
{
	int rc = 0;
	ARG Arg = {.count = 0};
	unsigned int oscount[4] = {
		num_online_cpus(),
		num_possible_cpus(),
		num_present_cpus(),
		num_active_cpus()
	};

	printk("Topology: OS CPU Count [%u/%u/%u/%u]\n",
		oscount[0], oscount[1], oscount[2], oscount[3]);

	// Query features on the presumed BSP processor.
	memset(&Arg.features, 0, sizeof(FEATURES));

	rc = smp_call_function_single(0, Query_Features, &Arg, 1);
	rc = (rc == 0) ? ((Arg.count > 0) ? 0 : -ENXIO) : rc;
	if (rc == 0)
	{
		unsigned int cpu = 0;
		unsigned long publicSize = 0, packageSize = 0;

		publicSize = sizeof(KPUBLIC) + sizeof(CORE *) * Arg.count;

		if ((KPublic = kmalloc(publicSize, GFP_KERNEL)) != NULL) {
			memset(KPublic, 0, publicSize);

			packageSize = ROUND_TO_PAGES(sizeof(PROC));
			if ((Proc = kmalloc(packageSize, GFP_KERNEL)) != NULL) {
			    memset(Proc, 0, packageSize);

			    Proc->CPU.Count = (oscount[2] > 0) ?
						oscount[2] : Arg.count;

			    memcpy(&Proc->Features, &Arg.features,
					sizeof(FEATURES) );

			    publicSize  = ROUND_TO_PAGES(sizeof(CORE));
			    if ((KPublic->Cache=kmem_cache_create(
					"topology-pub",
					publicSize, 0,
					SLAB_HWCACHE_ALIGN, NULL)) != NULL) {

				for (cpu = 0; cpu < Proc->CPU.Count; cpu++) {
				  void *kcache=kmem_cache_alloc(KPublic->Cache,
								GFP_KERNEL);
				  memset(kcache, 0, publicSize);
				  KPublic->Core[cpu] = kcache;
				}

				Print_Topology();

			    } else {
				kfree(Proc);
				kfree(KPublic);
				rc = -ENOMEM;
			    }
			} else {
			    kfree(KPublic);
			    rc = -ENOMEM;
			}
		}
	}
	return(rc);
}

static void __exit topology_cleanup(void)
{
	unsigned int cpu = 0;

	for (cpu = 0;(KPublic->Cache != NULL) && (cpu < Proc->CPU.Count); cpu++)
	{
		if (KPublic->Core[cpu] != NULL)
			kmem_cache_free(KPublic->Cache,	KPublic->Core[cpu]);
	}
	if (KPublic->Cache != NULL)
		kmem_cache_destroy(KPublic->Cache);
	if (Proc != NULL) {
		kfree(Proc);
	}
	if (KPublic != NULL)
		kfree(KPublic);

	printk(KERN_NOTICE "Topology: Unload\n");
}

module_init(topology_init);
module_exit(topology_cleanup);
