/* SPDX-License-Identifier: Apache-2.0
 *
 * Copyright(c) 2018 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 *         Keyon Jie <yang.jie@linux.intel.com>
 *         Rander Wang <rander.wang@intel.com>
 *         Janusz Jankowski <janusz.jankowski@linux.intel.com>
 */
#include <device.h>
#include <init.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(sof);

#include <ipc.h>
#include <soc/shim.h>
#include <adsp/io.h>

/*
 * Sets up the host windows so that the host can see the memory
 * content on the DSP SRAM.
 */
static void prepare_host_windows(void)
{
	/* window0, for fw status */
	sys_write32((HP_SRAM_WIN0_SIZE | 0x7), DMWLO(0));
	sys_write32((HP_SRAM_WIN0_BASE | DMWBA_READONLY | DMWBA_ENABLE),
		    DMWBA(0));
	memset((void *)(HP_SRAM_WIN0_BASE + SRAM_REG_FW_END), 0,
	      HP_SRAM_WIN0_SIZE - SRAM_REG_FW_END);
	SOC_DCACHE_FLUSH((void *)(HP_SRAM_WIN0_BASE + SRAM_REG_FW_END),
			 HP_SRAM_WIN0_SIZE - SRAM_REG_FW_END);

	/* window3, for trace
	 * initialized in trace_out.c
	 */
	sys_write32((HP_SRAM_WIN3_SIZE | 0x7), DMWLO(3));
	sys_write32((HP_SRAM_WIN3_BASE | DMWBA_READONLY | DMWBA_ENABLE),
		    DMWBA(3));
	SOC_DCACHE_FLUSH((void *)HP_SRAM_WIN3_BASE, HP_SRAM_WIN3_SIZE);
}

static int adsp_init(const struct device *dev)
{
	prepare_host_windows();

	return 0;
}

/* Note priority zero: this is required for trace output to appear to
 * the host (we can log to the buffer earlier, but the host needs our
 * registers to see it), so we want it done FIRST.
 */
SYS_INIT(adsp_init, PRE_KERNEL_1, 0);
