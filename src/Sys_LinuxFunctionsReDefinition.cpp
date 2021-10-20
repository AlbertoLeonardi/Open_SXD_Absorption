// System :: Redefinition of standard library functions due to mismatch between unsafe and secure functions
//
// 

#include "Sys_LinuxFunctionsReDefinition.h"

#ifdef _SYS_LINUX_FUNCTIONS_REDEFINITION

int getch(void)
{
	int c = 0, res = 0;
	
	struct termios org_opts, new_opts;
	
	// ----- store olde setings ----------
	res = tcgetattr(STDIN_FILENO, &org_opts);
	assert(res == 0);
	
	// ----- set new terminal parms ------
	memcpy(&new_opts, &org_opts, sizeof(new_opts));
	new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
	c = getchar();
	
	// ----- restore old settings --------
	res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
	assert(res == 0);
	
	return(c);
}

int localtime_s(struct tm* buf, const time_t* timer)
{
	struct tm* time;
	
	time = localtime(timer);
	if (time == NULL) return 1;
	
	memcpy(buf, time, sizeof(struct tm));
	
	return 0;
}

#endif