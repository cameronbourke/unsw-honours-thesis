#include <stdlib.h>
#include <stdint.h>

#define PMU_SOFTWARE_INC_EVT_ID      0x00

#define PMUSERENR_EN_USR_MODE		 		(0x1 << 0)

#define PMCR_EN_CTRS 						 		(0x1 << 0)
#define PMCR_EN_RESET_CTRS 			 		(0x1 << 1)
#define PMCR_EN_RESET_CLK_CTR 	 		(0x1 << 2)
#define PMCR_EN_CLK_DIV 				 		(0x1 << 3)
#define PMCR_EN_EXPORT_EVTS 	 	 		(0x1 << 4)
#define PMCR_DD_CLK_CTR_PROB_REG 		(0x1 << 5)

#define PMCNTENSET_EN_PMCCNTR		 		(0x1 << 31)
#define PMCNTENSET_EN_CTRS	 		  	 0xF

#define PMOVSR_EN_PMCCNTR		 				(0x1 << 31)
#define PMOVSR_EN_CTRS	 		  			 0xF

typedef struct pmu_event_config {
	uint32_t counter;
	uint32_t event_id;
} pmu_event_config_t;

void pmu_init(uint32_t event_id)
{
	// Enable user-mode access to performance counters
	// https://developer.arm.com/documentation/ddi0406/b/Debug-Architecture/Debug-Registers-Reference/Performance-monitor-registers/c9--User-Enable-Register--PMUSERENR-?lang=en
	// MCR is Move to Coprocessor
	// p15 is the name of the coprocessor
	// 0 is coprocessor-specific opcode in the range 0 to 7
	// %0 = 1 is the core CPU register to transfer from
	// C9 is the destination coprocessor register
	// C14 is an additional destination coprocessor register
	// 0 is coprocessor-specific opcode in the range 0-7 (if omitted defaulted to 0)
	asm volatile ("MCR p15, 0, %0, C9, C14, 0\n\t" : : "r"(PMUSERENR_EN_USR_MODE));

	// Program the performance-counter control-register
	// https://developer.arm.com/documentation/ddi0406/b/Debug-Architecture/Debug-Registers-Reference/Performance-monitor-registers/c9--Performance-Monitor-Control-Register--PMCR-?lang=en
	// 0b0111
	//
	uint32_t pmcr_config = PMCR_EN_CTRS | PMCR_EN_RESET_CTRS | PMCR_EN_RESET_CLK_CTR;
	asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(pmrc_config));  

	// Enable all counters
	// https://developer.arm.com/documentation/ddi0406/b/Debug-Architecture/Debug-Registers-Reference/Performance-monitor-registers/c9--Count-Enable-Set-Register--PMCNTENSET-?lang=en
	uint32_t pmcntenset_config = PMCNTENSET_EN_CTRS | PMCNTENSET_EN_PMCCNTR;
	asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(pmcntenset_config));  

	// Clear overflows
	// https://developer.arm.com/documentation/ddi0406/b/Debug-Architecture/Debug-Registers-Reference/Performance-monitor-registers/c9--Overflow-Flag-Status-Register--PMOVSR-?lang=en
	uint32_t pmovsr_config = PMOVSR_EN_CTRS | PMOVSR_EN_PMCCNTR;
	asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(pmovsr_config));
}

void pmu_count_event(pmu_event_config_t config)
{
	//Now you must select which events you wish to track
	//https://developer.arm.com/documentation/ddi0406/b/Debug-Architecture/Debug-Registers-Reference/Performance-monitor-registers/c9--Event-Counter-Selection-Register--PMSELR-?lang=en
	asm volatile ("MCR p15, 0, %0, C9, C12, 5" :: "r"(config.counter));

	// Once you have selected this counter for programming, you may write into it which event you would like it to track.
	// https://developer.arm.com/documentation/ddi0406/b/Debug-Architecture/Debug-Registers-Reference/Performance-monitor-registers/c9--Event-Type-Select-Register--PMXEVTYPER-?lang=en
	asm volatile ("MCR p15, 0, %0, C9, C13, 1" :: "r"(config.event_id));
}

uint32_t pmu_read_counter(pmu_event_config_t config)
{
	uint32_t counter_value;
	// https://developer.arm.com/documentation/ddi0406/b/Debug-Architecture/Debug-Registers-Reference/Performance-monitor-registers/c9--Event-Counter-Selection-Register--PMSELR-?lang=en
	asm volatile ("MCR p15, 0, %0, C9, C12, 5" :: "r"(config.counter));
	// https://developer.arm.com/documentation/ddi0406/b/Debug-Architecture/Debug-Registers-Reference/Performance-monitor-registers/c9--Event-Count-Register--PMXEVCNTR-?lang=en
	// Move From Coprocessor
	asm volatile ("MRC p15, 0, %0, C9, C13, 2" : "=r"(counter_value));
	return counter_value;
}

int main() {
	pmu_event_config_t soft_inc = {
		.event_id = PMU_SOFTWARE_INC_EVT_ID,
		.counter = 0x0
	};

	pmu_init();
	pmu_count_event(soft_inc);

	for (int i = 0; i < 100; i++) {
		(void)i;
	}

	uint32_t ninstr = pmu_read_event(soft_inc);
	printf("ninstr: %d\n", ninstr);
}
