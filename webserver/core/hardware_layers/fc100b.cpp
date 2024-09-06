#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>
#include <poll.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>

#include <limits.h>

#include "ladder.h"
#include "custom_layer.h"

// Environment Variables

#define	ENV_DEBUG	NULL//"FOLKS_DEBUG"
#define	ENV_CODES	"FOLKS_CODES"
#define	ENV_GPIOMEM	"FOLKS_GPIOMEM"

#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(!TRUE)
#endif

#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

#define MAX_INPUT 		4
#define MAX_OUTPUT 		4

#define FOLKS_FC100B	1

#define SocModel FOLKS_FC100B

// modes
#define MODE_PINS            0
#define MODE_GPIO            1
#define MODE_GPIO_SYS        2
#define MODE_PHYS            3
#define MODE_PIFACE          4
#define MODE_UNINITIALISED  -1

// Pin modes
#define INPUT                    0
#define OUTPUT                   1
#define PWM_OUTPUT               2
#define GPIO_CLOCK               3
#define SOFT_PWM_OUTPUT          4
#define SOFT_TONE_OUTPUT         5
#define PWM_TONE_OUTPUT          6

#define	GPIO_MASK		(0xFFFFFFC0)
#define MODE_GPIO_SYS        2

#define	BLOCK_SIZE		(4*1024)

/*********** Folks Controller FC100B *************/
typedef struct fc100b_soc_info{
	unsigned int * gpio0_base;
	unsigned int * gpio1_base;
	unsigned int * gpio2_base;
	unsigned int * gpio3_base;
	unsigned int * gpio4_base;

	unsigned int * pmu_grf_base;
	unsigned int * sys_grf_base;

	unsigned int * cru_base;
	unsigned int * pmu_cru_base;

	unsigned int * pwm2_base;
	unsigned int * pwm3_base;
	unsigned int   cru_gate_con;
	unsigned int   cru_gate_con_offset;
	unsigned int   pwm_base;
	unsigned int   pwm_mux;
	unsigned int   pwm_mux_val;
	unsigned int   pwm_mux_offset;
	unsigned int   ch_period_hpr;
	unsigned int   ch_duty_lpr;
	unsigned int   ch_crtl;
};

fc100b_soc_info fc100b_soc_info_t;

//gpio0~gpio4 register base addr
#define SOC_GPIO0_BASE                       0xfdd60000U
#define SOC_GPIO1_BASE                       0xfe740000U
#define SOC_GPIO2_BASE                       0xfe750000U
#define SOC_GPIO3_BASE                       0xfe760000U
#define SOC_GPIO4_BASE                       0xfe770000U

//gpio offset
#define SOC_GPIO_SWPORT_DR_L_OFFSET          0x00U
#define SOC_GPIO_SWPORT_DR_H_OFFSET          0x04U
#define SOC_GPIO_SWPORT_DDR_L_OFFSET         0x08U
#define SOC_GPIO_SWPORT_DDR_H_OFFSET         0x0cU
#define SOC_GPIO_EXT_PORT_OFFSET             0x70U

#define SOC_PMU_GRF_BASE                     0xfdc20000U
#define SOC_SYS_GRF_BASE                     0xfdc60000U
#define SOC_PMU_CRU_BASE                     0xfdd00000U
#define SOC_CRU_BASE                         0xFdd20000U

//clock
#define SOC_CRU_GATE_CON31_OFFSET            0x37CU
#define SOC_CRU_GATE_CON31                   (SOC_CRU_BASE + 0x37CU)
#define SOC_CRU_GATE_CON32_OFFSET            0x380U
#define SOC_CRU_GATE_CON32                   (SOC_CRU_BASE + 0x380U)
#define SOC_PMUCRU_PMUGATE_CON01_OFFSET      0x184U  //bit 9 bit10  0 enable

//iomux + pull up/down
#define SOC_GRF_GPIO1A_IOMUX_L_OFFSET        0x00U
#define SOC_GRF_GPIO1A_P_OFFSET              0x80U
#define SOC_PMU_GRF_GPIO0A_IOMUX_L_OFFSET    0x00U
#define SOC_PMU_GRF_GPIO0A_P_OFFSET          0x20U

//pwm mux register
#define SOC_PWM_MUX_REG                      (SOC_SYS_GRF_BASE + 0x70)

//pwm register base addr
#define SOC_PWM2_BASE                        0xfe6f0000U
#define SOC_PWM3_BASE                        0xfe700000U

//CH3
#define SOC_CH3_PERIOD_HPR                   (SOC_PWM_BASE + 0x34)
#define SOC_CH3_DUTY_LPR                     (SOC_PWM_BASE + 0x38)
#define SOC_CH3_CTRL                         (SOC_PWM_BASE + 0x3C)

//for short——pwm
#define SOC_CRU_GATE_CON                     (fc100b_soc_info_t.cru_gate_con)
#define SOC_CRU_GATE_CON_OFFSET              (fc100b_soc_info_t.cru_gate_con_offset)
#define SOC_PWM_BASE                         (fc100b_soc_info_t.pwm_base)
#define SOC_PWM_MUX                          (fc100b_soc_info_t.pwm_mux)
#define SOC_PWM_MUX_VAL                      (fc100b_soc_info_t.pwm_mux_val)
#define SOC_PWM_MUX_OFFSET                   (fc100b_soc_info_t.pwm_mux_offset)
#define SOC_CH_PERIOD_HPR                    (fc100b_soc_info_t.ch_period_hpr)
#define SOC_CH_DUTY_LPR                      (fc100b_soc_info_t.ch_duty_lpr)
#define SOC_CH_CTRL                          (fc100b_soc_info_t.ch_crtl)

//pwm_ctrl_offset
#define SOC_RPT                              (24)    // 24 ~ 31
#define SOC_SCALE                            (16)    // 16 ~ 23
#define SOC_PRESCALE                         (12)    // 12 ~ 14
#define SOC_CLK_SRC_SEL                      (10)
#define SOC_CLK_SEL                          (9)
#define SOC_FORCE_CLK_EN                     (8)
#define SOC_CH_CNT_EN                        (7)
#define SOC_CONLOCK                          (6)
#define SOC_OUTPUT_MODE                      (5)
#define SOC_INACTIVE_POL                     (4)
#define SOC_DUTY_POL                         (3)
#define SOC_PWM_MODE                         (1)     // 1 ~ 2
#define SOC_PWM_EN                           (0)

#define MODE_PINS            0
#define MODE_PHYS            3
#define MODE_UNINITIALISED  -1

#define LOW                      0
#define HIGH                     1

// Failure modes

#define	FATAL	(1==1)
#define	ALMOST	(1==2)

struct fc100bNodeStruct
{
  int     pinBase ;
  int     pinMax ;

  int          fd ;	// Node specific
  unsigned int data0 ;	//  ditto
  unsigned int data1 ;	//  ditto
  unsigned int data2 ;	//  ditto
  unsigned int data3 ;	//  ditto

           void   (*pinMode)          (struct fc100bNodeStruct *node, int pin, int mode) ;
           void   (*pullUpDnControl)  (struct fc100bNodeStruct *node, int pin, int mode) ;
           int    (*digitalRead)      (struct fc100bNodeStruct *node, int pin) ;
//unsigned int    (*digitalRead8)     (struct fc100bNodeStruct *node, int pin) ;
           void   (*digitalWrite)     (struct fc100bNodeStruct *node, int pin, int value) ;
//         void   (*digitalWrite8)    (struct fc100bNodeStruct *node, int pin, int value) ;
           void   (*pwmWrite)         (struct fc100bNodeStruct *node, int pin, int value) ;
           int    (*analogRead)       (struct fc100bNodeStruct *node, int pin) ;
           void   (*analogWrite)      (struct fc100bNodeStruct *node, int pin, int value) ;

  struct fc100bNodeStruct *next ;
} ;

struct fc100bNodeStruct *fc100bPiNodes = NULL ;

/********************I/O PINS CONFIGURATION*********************
****************************************************************/
//inBufferPinMask: pin mask for each input, which
//means what pin is mapped to that OpenPLC input
int inBufferPinMask[MAX_INPUT] = { 10, 8, 9, 13 };

//outBufferPinMask: pin mask for each output, which
//means what pin is mapped to that OpenPLC output
int outBufferPinMask[MAX_OUTPUT] =	{ 27, 26, 25, 20 };

static unsigned int usingGpioMem    = FALSE ;
static          int boardSetuped = FALSE ;
static          int fc100bSysSetuped = FALSE ;

static int fc100bMode = MODE_UNINITIALISED ;

static int *pinToGpio ;
static int *physToGpio ;

// Debugging & Return codes
int SocDebug       = TRUE ;
int SocReturnCodes = FALSE ;

int (*PIN_MASK)[32];

static volatile unsigned int piGpioBase = 0 ;

static uint64_t epochMilli, epochMicro ;

int fc100b_pinToGpio[64] =
{
	140,141,      // 0, 1
	147, 25,      // 2, 3
	24, 118,      // 4  5
	119,128,      // 6, 7
	130,131,      // 8, 9
	129,138,      //10,11
	136,132,      //12,13
	139,134,      //14,15
	135, 32,      //16,17
	33, 133,      //18,19
	124,144,      //20,21
	127,120,      //22,23
	125,123,      //24,25
	122,121,      //26,27
	-1,  -1,      //28,29
	-1,  -1,      //30,31

	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // ... 47
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,// ... 63
};

int fc100b_physToGpio[64] =
{
	-1,        // 0
	-1,  -1,   // 1, 2
	140, -1,   // 3, 4
	141, -1,   // 5, 6
	147, 25,   // 7, 8
	-1,  24,   // 9, 10
	118,119,   // 11, 12
	128, -1,   // 13, 14
	130,131,   // 15, 16
	-1, 129,   // 17, 18
	138, -1,   // 19, 20
	136,132,   // 21, 22
	139,134,   // 23, 24
	-1, 135,   // 25, 26
	32,  33,   // 27, 28
	133, -1,   // 29, 30
	124,144,   // 31, 32
	127, -1,   // 33, 34
	120,125,   // 35, 36
	123,122,   // 37, 38
	-1, 121,   // 39, 40

	//Padding:
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   // ... 56
	-1, -1, -1, -1, -1, -1, -1,   // ... 63
};

static int fc100b_PIN_MASK[5][32] =  //[BANK]	[INDEX]
{
	{-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,  0, 1,-1,-1,-1,-1,-1,-1,},//GPIO0
	{ 0, 1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,},//GPIO1
	{-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,},//GPIO2
	{-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1, 6, 7,  0, 1, 2, 3, 4, 5,-1, 7,},//GPIO3
	{ 0, 1, 2, 3, 4, 5, 6, 7,  0,-1, 2, 3, 4, 5,-1,-1,  0,-1,-1, 3,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,},//GPIO4
};

// sysFds:
//	Map a file descriptor from the /sys/class/gpio/gpioX/value

static int sysFds[384] =
  {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1-1,-1,-1,-1
  };
/*
 * Functions
 *********************************************************************************
 */


/*
 * folksboardFailure:
 *	Fail. Or not.
 *********************************************************************************
 */
int folksboardFailure (int fatal, const char *message, ...)
{
  va_list argp ;
  char buffer [1024] ;

  if (!fatal && SocReturnCodes)
    return -1 ;

  va_start (argp, message) ;
    vsnprintf (buffer, 1023, message, argp) ;
  va_end (argp) ;

  fprintf (stderr, "%s", buffer) ;
  exit (EXIT_FAILURE) ;

  return 0 ;
}

/*
 * initialiseEpoch:
 *	Initialise our start-of-time variable to be the current unix
 *	time in milliseconds and microseconds.
 *********************************************************************************
 */

static void initialiseEpoch (void)
{
  struct timespec ts ;

  clock_gettime (CLOCK_MONOTONIC_RAW, &ts) ;
  epochMilli = (uint64_t)ts.tv_sec * (uint64_t)1000    + (uint64_t)(ts.tv_nsec / 1000000L) ;
  epochMicro = (uint64_t)ts.tv_sec * (uint64_t)1000000 + (uint64_t)(ts.tv_nsec /    1000L) ;
}

int BoardSetup (void)
{
	int fd;

	// It's actually a fatal error to call any of the BoardSetup routines more than once,
	//	(you run out of file handles!) but I'm fed-up with the useless twats who email
	//	me bleating that there is a bug in my code, so screw-em.

	if (boardSetuped)
		return 0 ;

	boardSetuped = TRUE ;

	if (getenv (ENV_DEBUG) != NULL)
		SocDebug = TRUE ;

	if (getenv (ENV_CODES) != NULL)
		SocReturnCodes = TRUE ;

	if (SocDebug)
		printf ("BoardSetup called\n") ;

	fc100bMode = MODE_PINS ;

	switch (SocModel)
	{
		case FOLKS_FC100B:
			pinToGpio =  fc100b_pinToGpio;
			physToGpio = fc100b_physToGpio;
			PIN_MASK = fc100b_PIN_MASK;
			break;
		default:
			printf ("Oops - unable to determine board type... model: %d\n", SocModel);
			break ;
	}

	// set_soc_info();

	// Open the master /dev/ memory control device
	// Device strategy: December 2016:
	//	Try /dev/mem. If that fails, then 
	//	try /dev/gpiomem. If that fails then game over.
	if ((fd = open ("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0){
		if ((fd = open ("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC) ) >= 0){	// We're using gpiomem
			piGpioBase   = 0 ;
			usingGpioMem = TRUE ;
		}
		else
			return folksboardFailure (ALMOST, "BoardSetup: Unable to open /dev/mem or /dev/gpiomem: %s.\n"
			"  Aborting your program because if it can not access the GPIO\n"
			"  hardware then it most certianly won't work\n"
			"  Try running with sudo?\n", strerror (errno)) ;
	}
	
	switch (SocModel)
	{
		case FOLKS_FC100B:
			/* GPIO Register */
			fc100b_soc_info_t.gpio0_base = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, SOC_GPIO0_BASE);
			if ((int32_t)(unsigned long)fc100b_soc_info_t.gpio0_base == -1)
				return folksboardFailure(ALMOST, "BoardSetup: mmap (SOC_GPIO0_BASE) failed: %s\n", strerror(errno));

			fc100b_soc_info_t.gpio1_base = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, SOC_GPIO1_BASE);
			if ((int32_t)(unsigned long)fc100b_soc_info_t.gpio1_base == -1)
				return folksboardFailure(ALMOST, "BoardSetup: mmap (SOC_GPIO1_BASE) failed: %s\n", strerror(errno));

			fc100b_soc_info_t.gpio2_base = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, SOC_GPIO2_BASE);
			if ((int32_t)(unsigned long)fc100b_soc_info_t.gpio2_base == -1)
				return folksboardFailure(ALMOST, "BoardSetup: mmap (SOC_GPIO2_BASE) failed: %s\n", strerror(errno));

			fc100b_soc_info_t.gpio3_base = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, SOC_GPIO3_BASE);
			if ((int32_t)(unsigned long)fc100b_soc_info_t.gpio3_base == -1)
				return folksboardFailure(ALMOST, "BoardSetup: mmap (SOC_GPIO3_BASE) failed: %s\n", strerror(errno));

			fc100b_soc_info_t.gpio4_base = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, SOC_GPIO4_BASE);
			if ((int32_t)(unsigned long)fc100b_soc_info_t.gpio4_base == -1)
				return folksboardFailure(ALMOST, "BoardSetup: mmap (SOC_GPIO4_BASE) failed: %s\n", strerror(errno));

			fc100b_soc_info_t.pmu_grf_base = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, SOC_PMU_GRF_BASE);
			if ((int32_t)(unsigned long)fc100b_soc_info_t.pmu_grf_base == -1)
				return folksboardFailure(ALMOST, "BoardSetup: mmap (SOC_PMU_GRF_BASE) failed: %s\n", strerror(errno));

			fc100b_soc_info_t.sys_grf_base = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, SOC_SYS_GRF_BASE);
			if ((int32_t)(unsigned long)fc100b_soc_info_t.sys_grf_base == -1)
				return folksboardFailure(ALMOST, "BoardSetup: mmap (SOC_SYS_GRF_BASE) failed: %s\n", strerror(errno));

			fc100b_soc_info_t.cru_base = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, SOC_CRU_BASE);
			if ((int32_t)(unsigned long)fc100b_soc_info_t.cru_base == -1)
				return folksboardFailure(ALMOST, "BoardSetup: mmap (SOC_CRU_BASE) failed: %s\n", strerror(errno));

			fc100b_soc_info_t.pmu_cru_base = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, SOC_PMU_CRU_BASE);
			if ((int32_t)(unsigned long)fc100b_soc_info_t.pmu_cru_base == -1)
				return folksboardFailure(ALMOST, "BoardSetup: mmap (SOC_PMU_CRU_BASE) failed: %s\n", strerror(errno));

			fc100b_soc_info_t.pwm2_base = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, SOC_PWM2_BASE);
			if ((int32_t)(unsigned long)fc100b_soc_info_t.pwm2_base == -1)
				return folksboardFailure(ALMOST, "BoardSetup: mmap (SOC_PWM2_BASE) failed: %s\n", strerror(errno));

			fc100b_soc_info_t.pwm3_base = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, SOC_PWM3_BASE);
			if ((int32_t)(unsigned long)fc100b_soc_info_t.pwm3_base == -1)
				return folksboardFailure(ALMOST, "BoardSetup: mmap (SOC_PWM3_BASE) failed: %s\n", strerror(errno));
			break;

		default:

			printf("model %d is error\n", SocModel);
			break ;
	}

	initialiseEpoch() ;

	return 0 ;
}

/*
 * setupCheck
 *	Another sanity check because some users forget to call the setup
 *	function. Mosty because they need feeding C drip by drip )-:
 *********************************************************************************
 */

static void setupCheck (const char *fName)
{
	if (!boardSetuped)
	{
		fprintf (stderr, "%s: You have not called one of the BoardSetup\n"
				"  functions, so I'm aborting your program before it crashes anyway.\n", fName) ;
		exit (EXIT_FAILURE) ;
	}
}

/*
 * Wirte value to register helper
 */
void writeR(unsigned int val, unsigned int addr)
{
	unsigned int mmap_base;
	unsigned int mmap_seek;

	switch (SocModel)
	{
		case FOLKS_FC100B:
			mmap_base = (addr & (~0xfff));
			mmap_seek = (addr - mmap_base);

			if(mmap_base == SOC_GPIO0_BASE)
				*((unsigned int *)((unsigned char *)fc100b_soc_info_t.gpio0_base + mmap_seek)) = val;
			else if(mmap_base == SOC_GPIO1_BASE)
				*((unsigned int *)((unsigned char *)fc100b_soc_info_t.gpio1_base + mmap_seek)) = val;
			else if(mmap_base == SOC_GPIO2_BASE)
				*((unsigned int *)((unsigned char *)fc100b_soc_info_t.gpio2_base + mmap_seek)) = val;
			else if(mmap_base == SOC_GPIO3_BASE)
				*((unsigned int *)((unsigned char *)fc100b_soc_info_t.gpio3_base + mmap_seek)) = val;
			else if(mmap_base == SOC_GPIO4_BASE)
				*((unsigned int *)((unsigned char *)fc100b_soc_info_t.gpio4_base + mmap_seek)) = val;
			else if(mmap_base == SOC_PMU_GRF_BASE)
				*((unsigned int *)((unsigned char *)fc100b_soc_info_t.pmu_grf_base + mmap_seek)) = val;
			else if(mmap_base == SOC_SYS_GRF_BASE)
				*((unsigned int *)((unsigned char *)fc100b_soc_info_t.sys_grf_base + mmap_seek)) = val;
			else if(mmap_base == SOC_CRU_BASE)
				*((unsigned int *)((unsigned char *)fc100b_soc_info_t.cru_base + mmap_seek)) = val;
			else if(mmap_base == SOC_PMU_CRU_BASE)
				*((unsigned int *)((unsigned char *)fc100b_soc_info_t.pmu_cru_base + mmap_seek)) = val;
			else if(mmap_base == SOC_PWM2_BASE)
				*((unsigned int *)((unsigned char *)fc100b_soc_info_t.pwm2_base + mmap_seek)) = val;
			else if(mmap_base == SOC_PWM3_BASE)
				*((unsigned int *)((unsigned char *)fc100b_soc_info_t.pwm3_base + mmap_seek)) = val;

			if (SocDebug)
				printf("write %#x to [%#x]\n",val,addr);

			break;

		default:
			break;
	}
}

/*
 * Read register value helper  
 */
unsigned int readR(unsigned int addr)
{
	unsigned int val = 0;
	unsigned int mmap_base;
	unsigned int mmap_seek;

	switch (SocModel)
	{
		case FOLKS_FC100B:
			val = 0;

			mmap_base = (addr & (0xfffff000));
			mmap_seek = (addr - mmap_base);

			if(mmap_base == SOC_GPIO0_BASE)
				val = *((unsigned int *)((unsigned char *)fc100b_soc_info_t.gpio0_base + mmap_seek));
			else if(mmap_base == SOC_GPIO1_BASE)
				val = *((unsigned int *)((unsigned char *)fc100b_soc_info_t.gpio1_base + mmap_seek));
			else if(mmap_base == SOC_GPIO2_BASE)
				val = *((unsigned int *)((unsigned char *)fc100b_soc_info_t.gpio2_base + mmap_seek));
			else if(mmap_base == SOC_GPIO3_BASE)
				val = *((unsigned int *)((unsigned char *)fc100b_soc_info_t.gpio3_base + mmap_seek));
			else if(mmap_base == SOC_GPIO4_BASE)
				val = *((unsigned int *)((unsigned char *)fc100b_soc_info_t.gpio4_base + mmap_seek));
			else if(mmap_base == SOC_PMU_GRF_BASE)
				val = *((unsigned int *)((unsigned char *)fc100b_soc_info_t.pmu_grf_base + mmap_seek));
			else if(mmap_base == SOC_SYS_GRF_BASE)
				val = *((unsigned int *)((unsigned char *)fc100b_soc_info_t.sys_grf_base + mmap_seek));
			else if(mmap_base == SOC_CRU_BASE)
				val = *((unsigned int *)((unsigned char *)fc100b_soc_info_t.cru_base + mmap_seek));
			else if(mmap_base == SOC_PMU_CRU_BASE)
				val = *((unsigned int *)((unsigned char *)fc100b_soc_info_t.pmu_cru_base + mmap_seek));
			else if(mmap_base == SOC_PWM2_BASE)
				val = *((unsigned int *)((unsigned char *)fc100b_soc_info_t.pwm2_base + mmap_seek));
			else if(mmap_base == SOC_PWM3_BASE)
				val = *((unsigned int *)((unsigned char *)fc100b_soc_info_t.pwm3_base + mmap_seek));

			if (SocDebug)
				printf("read %#x from [%#x]\n",val,addr);

			return val;

			break;

		default:
			break;
	}

	return -1;
}

/*
 * Set GPIO Mode
 */
int set_gpio_mode(int pin, int mode)
{
	unsigned int regval = 0;
	unsigned int bank   = pin >> 5;
	unsigned int index  = pin - (bank << 5);
	unsigned int phyaddr = 0;
	int offset;
	unsigned int cru_phyaddr =0, grf_phyaddr = 0, gpio_phyaddr = 0, ddr_phyaddr = 0;
	unsigned int cru_val = 0;
	unsigned int temp = 0;
	unsigned int bit_enable;
	unsigned int grf_val = 0;

	switch (SocModel)
	{
		case FOLKS_FC100B:
			if(bank == 0){
				cru_phyaddr = SOC_PMU_CRU_BASE + SOC_PMUCRU_PMUGATE_CON01_OFFSET;
				cru_val = ~((0x3 << 9) | (0x3 << (16 + 9)));
				grf_phyaddr = SOC_PMU_GRF_BASE + SOC_PMU_GRF_GPIO0A_IOMUX_L_OFFSET + ((pin >> 2) << 2);
				ddr_phyaddr = SOC_GPIO0_BASE + SOC_GPIO_SWPORT_DDR_L_OFFSET + ((index >> 4) << 2);
			}
			else{
				cru_phyaddr = SOC_CRU_BASE + SOC_CRU_GATE_CON31_OFFSET;
				cru_val = ~((0xff << 2) | (0xff << (16 + 2)));
				grf_phyaddr = SOC_SYS_GRF_BASE + SOC_GRF_GPIO1A_IOMUX_L_OFFSET + (((pin - 32) >> 2) << 2);
				ddr_phyaddr = SOC_GPIO1_BASE + ((bank - 1) << 16) + SOC_GPIO_SWPORT_DDR_L_OFFSET + ((index >> 4) << 2);
			}

			offset = (index % 4) << 2;
			bit_enable = 0x7 << (16 + offset);
			/* Ignore unused gpio */
			if (PIN_MASK[bank][index] != -1)
			{
				if (INPUT == mode)
				{
					regval = readR(cru_phyaddr);
					regval &= cru_val;
					writeR(regval, cru_phyaddr);
					regval = readR(cru_phyaddr);

					regval = readR(grf_phyaddr);
					regval |= bit_enable;
					regval &= ~(0x7 << offset);
					writeR(regval, grf_phyaddr);
					regval = readR(grf_phyaddr);

					regval = readR(ddr_phyaddr);
					regval |= 0x1 << (16 + (index % 16));
					regval &= ~(1 << (index % 16));
					writeR(regval, ddr_phyaddr);
					regval = readR(ddr_phyaddr);
				}
				else if (OUTPUT == mode)
				{
					regval = readR(cru_phyaddr);
					regval &= cru_val;
					writeR(regval, cru_phyaddr);
					regval = readR(cru_phyaddr);

					regval = readR(grf_phyaddr);
					regval |= bit_enable;
					regval &= ~(0x7 << offset);
					writeR(regval, grf_phyaddr);
					regval = readR(grf_phyaddr);

					regval = readR(ddr_phyaddr);
					regval |= 0x1 << (16 + (index % 16));
					regval |= (1 << (index % 16));
					writeR(regval, ddr_phyaddr);
					regval = readR(ddr_phyaddr);
				}
				else if (PWM_OUTPUT == mode)
				{
					//not support for fc100b
				}
				else
				{
					printf("Unknow mode\n");
				}

			}
			else
			{
				printf("Pin mode failed!\n");
			}
			break;

		default: break;
	}

	return 0;
}

struct fc100bNodeStruct *FindNode (int pin)
{
  struct fc100bNodeStruct *node = fc100bPiNodes ;

  while (node != NULL)
    if ((pin >= node->pinBase) && (pin <= node->pinMax))
      return node ;
    else
      node = node->next ;

  return NULL ;
}

/*
 * pinMode:
 *	Sets the mode of a pin to be input, output or PWM output
 *********************************************************************************
 */
void pinMode (int pin, int mode)
{
	struct fc100bNodeStruct *node = fc100bPiNodes ;

	setupCheck ("pinMode") ;

	if (SocDebug)
		printf("PinMode: pin:%d,mode:%d\n", pin, mode);

	if ((pin & GPIO_MASK) == 0) {
		if (fc100bMode == MODE_PINS) {
			pin = pinToGpio[pin];
		} else if (fc100bMode == MODE_PHYS)
			pin = physToGpio[pin];
		else if (fc100bMode != MODE_GPIO)
			return;

		if (-1 == pin) {
			printf("[%s:L%d] the pin:%d is invaild,please check it over!\n", 
					__func__,  __LINE__, pin);
			return;
		}

		if (mode == INPUT) {
			set_gpio_mode(pin, INPUT);
			return;
		} 
		else if (mode == OUTPUT) {
			set_gpio_mode(pin, OUTPUT);
			return ;
		}
		else if (mode == PWM_OUTPUT) {
			//set_gpio_mode(pin, PWM_OUTPUT);
			return;
		}
		else
			return;
	} 
	else
	{
		if ((node = FindNode (pin)) != NULL)
			node->pinMode(node, pin, mode);

		return ;
	}
}

int folksdigitalRead(int pin)
{
	int bank = pin >> 5;
	int index = pin - (bank << 5);
	int val;
	unsigned int phyaddr = 0;

	switch (SocModel)
	{
		case FOLKS_FC100B:
			if(bank == 0)
				phyaddr = SOC_GPIO0_BASE + SOC_GPIO_EXT_PORT_OFFSET;
			else
				phyaddr = SOC_GPIO1_BASE + ((bank - 1) << 16) + SOC_GPIO_EXT_PORT_OFFSET;

			break;

		default:
			break;

	}

	if (PIN_MASK[bank][index] != -1)
	{
		val = readR(phyaddr);


		val = val >> index;

		val &= 1;

		if (SocDebug)
			printf("Read reg val: 0x%#x, bank:%d, index:%d\n", val, bank, index);

		return val;
	}

	return 0;
}

/*
 * digitalRead:
 *	Read the value of a given Pin, returning HIGH or LOW
 *********************************************************************************
 */
int digitalRead (int pin)
{
	char c ;
	int ret;
	struct fc100bNodeStruct *node = fc100bPiNodes ;

	if ((pin & GPIO_MASK) == 0)
	{
		if (fc100bMode == MODE_GPIO_SYS)	// Sys mode
		{
			if (sysFds [pin] == -1)
				return LOW ;

			ret = lseek(sysFds [pin], 0L, SEEK_SET) ;
			ret = read(sysFds [pin], &c, 1);

			if (ret < 0)
				return -1;
			
			return (c == '0') ? LOW : HIGH ;
		} 
		else if (fc100bMode == MODE_PINS){
			pin = pinToGpio[pin];
		}
		else if (fc100bMode == MODE_PHYS)
			pin = physToGpio[pin];
		else if (fc100bMode != MODE_GPIO)
			return -1;

		if (pin == -1)
		{
			printf("[%s %d]Pin %d is invalid, please check it over!\n", __func__, __LINE__, pin);
			return LOW;
		}

		return folksdigitalRead(pin);   
	}
	else 
	{
		if ((node = FindNode (pin)) == NULL)
			return LOW ;
		
		return node->digitalRead (node, pin) ;
	}
}

int folksdigitalWrite(int pin, int value)
{
	unsigned int bank   = pin >> 5;
	unsigned int index  = pin - (bank << 5);
	unsigned int phyaddr = 0;
	unsigned int regval = 0;
	unsigned int cru_phyaddr =0, gpio_phyaddr = 0, dr_phyaddr = 0;
	unsigned int cru_val = 0;
	unsigned int temp = 0;
	unsigned int bit_enable = 0;
	unsigned int offset;

	switch (SocModel)
	{
		case FOLKS_FC100B:
			if(bank == 0){
				dr_phyaddr = SOC_GPIO0_BASE + SOC_GPIO_SWPORT_DR_L_OFFSET + ((index >> 4) << 2);
				cru_phyaddr = SOC_PMU_CRU_BASE + SOC_PMUCRU_PMUGATE_CON01_OFFSET;
				cru_val = ~((0x3 << 9) | (0x3 << (16 + 9)));
			}
			else{
				dr_phyaddr = SOC_GPIO1_BASE + ((bank - 1) << 16) + SOC_GPIO_SWPORT_DR_L_OFFSET + ((index >> 4) << 2);
				cru_phyaddr = SOC_CRU_BASE + SOC_CRU_GATE_CON31_OFFSET;
				cru_val = ~((0xff << 2) | (0xff << (16 + 2)));
			}

			offset = index % 16;
			bit_enable = 0x1 << (16 + offset);

			/* Ignore unused gpio */
			if (PIN_MASK[bank][index] != -1)
			{
				regval = readR(cru_phyaddr);
				regval &= cru_val;
				writeR(regval, cru_phyaddr);

				regval = readR(dr_phyaddr);

				if(SocDebug)
					printf("read val(%#x) from register[%#x]\n", regval, dr_phyaddr);

				regval |= bit_enable;

				if(0 == value)
					regval &= ~(1 << offset);
				else
					regval |= (1 << offset);

				writeR(regval, dr_phyaddr);
				if (SocDebug)
					printf("write val(%#x) to register[%#x]\n", regval, dr_phyaddr);

				regval = readR(dr_phyaddr);
				if (SocDebug)
					printf("set over reg val: 0x%x\n", regval);
			} 
			else
			{
				printf("Pin mode failed!\n");
			}

			break;

		default:
			break;
	}

	return 0;
}

/*
 * digitalWrite:
 *	Set an output bit
 *********************************************************************************
 */
void digitalWrite (int pin, int value)
{
	struct fc100bNodeStruct *node = fc100bPiNodes ;
	int ret;

	if ((pin & GPIO_MASK) == 0) 
	{
		if (fc100bMode == MODE_GPIO_SYS)
		{
			if (sysFds [pin] != -1) {
				if (value == LOW)
				{
					ret = write (sysFds [pin], "0\n", 2);
					if (ret < 0)
						return;
				}
				else
				{
					ret = write (sysFds [pin], "1\n", 2);
					if (ret < 0)
						return;
				}
			}
			return ;
		} 
		else if (fc100bMode == MODE_PINS) 
		{
			pin = pinToGpio[pin];
		}
		else if (fc100bMode == MODE_PHYS)
			pin = physToGpio[pin];
		else 
			return;

		if(-1 == pin)
		{
			printf("[%s:L%d] the pin:%d is invaild,please check it over!\n", __func__,  __LINE__, pin);
			printf("[%s:L%d] the mode is: %d, please check it over!\n", __func__,  __LINE__, fc100bMode);

			return;
		}

		folksdigitalWrite(pin, value);	  
	} 
	else 
	{
		if ((node = FindNode(pin)) != NULL)
			node->digitalWrite(node, pin, value);
	}

	return;
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
    BoardSetup();

	//set pins as input
	for (int i = 0; i < MAX_INPUT; i++)
	{
	    pinMode(inBufferPinMask[i], INPUT);
	}

	//set pins as output
	for (int i = 0; i < MAX_OUTPUT; i++)
	{
		pinMode(outBufferPinMask[i], OUTPUT);
	}
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is finalizing.
// Resource clearing procedures should be here.
//-----------------------------------------------------------------------------
void finalizeHardware()
{
	
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual state of the input pins. The mutex buffer_lock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersIn()
{
	pthread_mutex_lock(&bufferLock); //lock mutex

	//INPUT
	for (int i = 0; i < MAX_INPUT; i++)
	{
    	if (bool_input[i/8][i%8] != NULL) *bool_input[i/8][i%8] = digitalRead(inBufferPinMask[i]);
	}

	pthread_mutex_unlock(&bufferLock); //unlock mutex
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual state of the output pins. The mutex buffer_lock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersOut()
{
	pthread_mutex_lock(&bufferLock); //lock mutex

	//OUTPUT
	for (int i = 0; i < MAX_OUTPUT; i++)
	{
		if (bool_output[i/8][i%8] != NULL) digitalWrite(outBufferPinMask[i], *bool_output[i/8][i%8]);
	}

	pthread_mutex_unlock(&bufferLock); //unlock mutex
}