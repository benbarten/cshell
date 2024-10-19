#include <stdio.h>
#include <unistd.h>
#include "../builtin.h"
#include <CUnit/Basic.h>

void test_pwd() {
    // Redirect stdout to a buffer
    char buffer[1024];
    FILE *fp = freopen("/dev/null", "a", stdout);
    setvbuf(fp, buffer, _IOFBF, sizeof(buffer));

    pwd();

    // Flush the buffer and restore stdout
    fflush(fp);
    freopen("/dev/tty", "a", stdout);

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
	strcat(cwd, "\n");

	printf("\nbuffer %s\n", buffer);
	printf("cwd: %s\n", cwd);

    CU_ASSERT_STRING_EQUAL(buffer, cwd);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Test builtin commands", NULL, NULL);
    CU_add_test(suite, "pwd", test_pwd);
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}