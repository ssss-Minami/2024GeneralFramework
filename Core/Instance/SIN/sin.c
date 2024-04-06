#include "sin.h"
#include "math.h"


const float Sin_Tab[128] = {0.0 ,0.04906767 ,0.09801714 ,0.14673047 ,0.19509032 ,0.24298018 ,0.29028468 ,0.33688985 ,
				 0.38268343 ,0.42755509 ,0.47139674 ,0.51410274 ,0.55557023 ,0.5956993 ,0.63439328 ,0.67155895 ,
				 0.70710678 ,0.74095112 ,0.77301045 ,0.80320753 ,0.83146961 ,0.85772861 ,0.88192126 ,0.90398929 ,
				 0.92387953 ,0.94154406 ,0.95694034 ,0.97003125 ,0.98078528 ,0.98917651 ,0.99518473 ,0.99879546 ,
				 1.0 ,0.99879546 ,0.99518473 ,0.98917651 ,0.98078528 ,0.97003125 ,0.95694034 ,0.94154407 ,
				 0.92387953 ,0.90398929 ,0.88192127 ,0.85772861 ,0.83146961 ,0.80320753 ,0.77301045 ,0.74095113 ,
				 0.70710678 ,0.67155896 ,0.63439329 ,0.59569931 ,0.55557024 ,0.51410275 ,0.47139674 ,0.4275551 ,
				 0.38268344 ,0.33688986 ,0.29028468 ,0.24298018 ,0.19509033 ,0.14673048 ,0.09801714 ,0.04906768 ,
				 0.0 ,-0.04906767 ,-0.09801714 ,-0.14673047 ,-0.19509032 ,-0.24298018 ,-0.29028467 ,-0.33688985 ,
				 -0.38268343 ,-0.42755509 ,-0.47139673 ,-0.51410274 ,-0.55557023 ,-0.5956993 ,-0.63439328 ,-0.67155895 ,
				 -0.70710678 ,-0.74095112 ,-0.77301045 ,-0.80320753 ,-0.83146961 ,-0.85772861 ,-0.88192126 ,-0.90398929 ,
				 -0.92387953 ,-0.94154406 ,-0.95694033 ,-0.97003125 ,-0.98078528 ,-0.98917651 ,-0.99518473 ,-0.99879546 ,
				 -1.0 ,-0.99879546 ,-0.99518473 ,-0.98917651 ,-0.98078528 ,-0.97003125 ,-0.95694034 ,-0.94154407 ,
				 -0.92387953 ,-0.9039893 ,-0.88192127 ,-0.85772861 ,-0.83146962 ,-0.80320754 ,-0.77301046 ,-0.74095113 ,
				 -0.70710679 ,-0.67155896 ,-0.63439329 ,-0.59569931 ,-0.55557024 ,-0.51410275 ,-0.47139674 ,-0.4275551 ,
				 -0.38268344 ,-0.33688986 ,-0.29028468 ,-0.24298019 ,-0.19509033 ,-0.14673048 ,-0.09801715};

float Freq_Tab[64] = {0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4,
							4.5, 5, 5.5, 6, 6.5, 7, 7.5, 8,
							8.5, 9, 9.5, 10, 10.5, 11, 11.5, 12,
							13, 14, 15, 16, 17, 18, 19, 20,
							22, 24, 26, 28, 30, 32, 34, 36,
							40, 44, 48, 52, 56, 60, 64, 68,
							74, 80, 86, 92, 98, 104, 110, 116,
							124, 124, 124, 124, 124, 124, 124, 124};

Sin_Typedef sin_struct = {0, 0, 0, 1, Sin_Tab, Freq_Tab};

float SinWave(Sin_Typedef *sin)
{
	sin->sin_time ++;
	float freq = sin->freq_tab_pt[sin->freq_idx];
	float time = sin->sin_time * TIMER_PERIOD;
	uint8_t idx_next = round(SINTAB_LEN * fmod((2*PI*freq * time), 2*PI) / (2*PI));
	if((sin->sin_idx > idx_next))
		sin->sin_period ++;
	sin->sin_idx = idx_next;
	if(sin->sin_period >= PERIOD_NUM)
	{
		sin->freq_idx = sin->freq_idx >=FREQTAB_LEN ? 0 : sin->freq_idx+1;
		sin->sin_period = 0;
		sin->sin_time = 0;
	}

	return sin->sin_tab_pt[sin->sin_idx];
}

void SinInit()
{
	
}

