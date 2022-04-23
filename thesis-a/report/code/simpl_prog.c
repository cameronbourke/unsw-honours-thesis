#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

void func_a (void);
void func_b (void);
void func_c (void);
void func_d (void);
void func_e (void);
void func_f (void);
void func_g (void);
void func_h (void);
void func_i (void);

int main (void) {
	srand(time(NULL));
	printf("main: %d\n", getpid());

	for (int i = 0; i < 1e8; ++i) {
		int r = rand();
		switch (r % 3) {
			case 0:
				func_a();
			case 1:
				func_b();
			default:
				func_c();
		}
	}
}

void func_a (void) {
	printf("func_a\n");
	int r = rand();
	switch (r % 2) {
		case 0:
			func_d();
		default:
			func_e();
	}
}

void func_b (void) {
	printf("func_b\n");
	int r = rand();
	switch (r % 2) {
		case 0:
			func_f();
		default:
			func_g();
	}
}

void func_c (void) {
	int r = rand();
	printf("func_c\n");
	switch (r % 2) {
		case 0:
			func_h();
		default:
			func_i();
	}
}

void func_d (void) {
	printf("func_d\n");
	int r = rand();
	if (r % 2 == 0) func_e();
}

void func_e (void) {
	printf("func_e\n");
	int r = rand();
	if (r % 2 == 0) func_f();
}

void func_f (void) {
	printf("func_f\n");
	int r = rand();
	if (r % 2 == 0) func_g();
}

void func_g (void) {
	printf("func_g\n");
	int r = rand();
	(void)r;
}

void func_h (void) {
	printf("func_h\n");
	int r = rand();
	(void)r;
}

void func_i (void) {
	printf("func_i\n");
	int r = rand();
	(void)r;
}
