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

void pmu_init()
{
	// Enable counters, reset current value, reset clock counter
	uint32_t pmcr_config = PMCR_EN_CTRS | PMCR_EN_RESET_CTRS | PMCR_EN_RESET_CLK_CTR;
	asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(pmrc_config));  

	// Enable all counters
	uint32_t pmcntenset_config = PMCNTENSET_EN_CTRS | PMCNTENSET_EN_PMCCNTR;
	asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(pmcntenset_config));  

	// Clear overflows
	uint32_t pmovsr_config = PMCNTENSET_EN_CTRS | PMCNTENSET_EN_PMCCNTR;
	asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(pmovsr_config));
}

void pmu_count_event(pmu_event_config_t config)
{
	asm volatile ("MCR p15, 0, %0, C9, C12, 5" :: "r"(0x00));
	asm volatile ("MCR p15, 0, %0, C9, C13, 1" :: "r"(PMU_SOFTWARE_INC_EVT_ID));
}

uint32_t pmu_read_counter(pmu_event_config_t config)
{
	uint32_t counter_value;
	asm volatile ("MCR p15, 0, %0, C9, C12, 5" :: "r"(config.counter));
	asm volatile ("MRC p15, 0, %0, C9, C13, 2" : "=r"(counter_value));
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
