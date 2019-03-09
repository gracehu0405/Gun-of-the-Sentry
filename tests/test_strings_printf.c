#include "assert.h"
#include "printf.h"
#include <stddef.h>
#include "strings.h"
#include "uart.h"

static void test_memset(void)
{
    int numA = 0xefefefef;
    int numB = 2;

    memset(&numB, 0xef, sizeof(int));
    assert(numA == numB);

    numA = 0;
    numB = 0;

    memset(&numB, 0, sizeof(int));
    assert(numA == numB);
}

static void test_memcpy(void)
{
    int numA = 0x12345678;
    int numB = 2;

    memcpy(&numB, &numA, sizeof(int));
    assert(numA == numB);
}

static void test_strlen(void)
{
    assert(strlen("green") == 5);
    assert(strlen("")==0);
}

static void test_strcmp(void)
{
    assert(strcmp("apples", "apples") == 0);
    assert(strcmp("apples", "apple") == 1);
    assert(strcmp("apple", "apples") == -1);
    assert(strcmp("a", "b") == -1);
    assert(strcmp("b", "a") == 1);
}

static void test_strlcat(void)
{
    char buf[20];
    memset(buf, 0x77, sizeof(buf)); // init contents with known value

    buf[0] = '\0'; // start with empty string
    strlcat(buf, "CS", sizeof(buf));
    strlcat(buf, "107e", sizeof(buf));
    assert(strcmp(buf, "CS107e") == 0);

    buf[0] = '\0'; // start with empty string
    strlcat(buf, "123", sizeof(buf));
    strlcat(buf, "4567", 8);
    assert(strcmp(buf, "1234567") == 0);

    buf[0] = '\0'; // start with empty string
    strlcat(buf, "123", sizeof(buf));
    strlcat(buf, "4567", 2);
    assert(strcmp(buf, "123") == 0);

    buf[0] = '\0'; // start with empty string
    strlcat(buf, "123", sizeof(buf));
    strlcat(buf, "4567", 7);
    assert(strcmp(buf, "123456") == 0);
    buf[0] = '\0'; // start with empty string
    strlcat(buf, "123", sizeof(buf));
    strlcat(buf, "4567", 11);
    assert(strcmp(buf, "1234567") == 0);
}

static void test_strtonum(void)
{
    const char *input = "013", *rest = NULL;

    int val = strtonum(input, &rest);
    assert(val == 13 && rest == (input + strlen(input)));

    const char *input1 = "01A", *rest1 = NULL;
    val = strtonum(input1, &rest1);
    assert(val == 1 && rest1 == (input1 + 2));

    const char *input2 = "0xA", *rest2 = NULL;
    val = strtonum(input2, &rest2);
    assert(val == 10);

    const char *input3 = "105s", *rest3 = NULL;
    val = strtonum(input3, &rest3);
    assert(val == 105);

    const char *input4 = "0x0010", *rest4 = NULL;
    val = strtonum(input4, &rest4);
    assert(val == 16);

    const char *input5 = "0", *rest5 = NULL;
    val = strtonum(input5, &rest5);
    assert(val == 0);

    const char *input6 = "0xa", *rest6 = NULL;
    val = strtonum(input6, &rest6);
    assert(val == 10);

    const char *input7 = ".4", *rest7 = NULL;
    val = strtonum(input7, &rest7);
    assert(val == 0 && rest7 == input7);
}


// These aren't part of printf public interface, we must declare them here to
// be able to use them in this test file.
int unsigned_to_base(char *buf, size_t bufsize, unsigned int val, int base, int min_width);
int signed_to_base(char *buf, size_t bufsize, int val, int base, int min_width);

static void test_to_base(void)
{
    char buf1[5];
    memset(buf1, 0x77, sizeof(buf1)); // init contents with known value

    int n = signed_to_base(buf1, 5, -9999, 10, 6);
    assert(strcmp(buf1, "-099") == 0 && n == 6);

    n = signed_to_base(buf1, 5, 9999, 10, 5);
    assert(strcmp(buf1, "0999") == 0&& n == 5);

    n = signed_to_base(buf1, 5, 0xef, 16, 0);
    assert(strcmp(buf1, "ef")==0);

    n = signed_to_base(buf1, 5, 0x6b, 10, 0);
    assert(strcmp(buf1, "107")==0);

    char buf[20];
    memset(buf, 0x77, sizeof(buf));

    n = signed_to_base(buf, 20, 94305, 10, 0);
    assert(strcmp(buf, "94305") == 0 && n == 5);

    n = unsigned_to_base(buf, 20, 94305, 10, 6);
    assert(strcmp(buf, "094305") == 0 && n == 6);

    n = unsigned_to_base(buf, 4, 94305, 10, 6);
    assert(strcmp(buf, "094") == 0 && n == 6);

    n = unsigned_to_base(buf, 20, 33, 16, 3);
    assert(strcmp(buf, "021") == 0 && n == 3);

    n = unsigned_to_base(buf, 20, 15, 16, 3);
    assert(strcmp(buf, "00f") == 0 && n == 3);


    //debug
    n = signed_to_base(buf1, 100, 0x0, 10, 0);
    assert(strcmp(buf1, "0") == 0);

    n = unsigned_to_base(buf1, 100, 0x80000000, 10, 0);
    assert(strcmp(buf1, "2147483648") == 0);

}

static void test_snprintf(void)
{
    char buf[100];
    memset(buf, 0x77, sizeof(buf)); // init contents with known value

    int bufsize = sizeof(buf);

    // Start off simple...
    snprintf(buf, bufsize, "Hello, world!");
    assert(strcmp(buf, "Hello, world!") == 0);

    // Decimal
    snprintf(buf, bufsize, "%d", 45);
    assert(strcmp(buf, "45") == 0);

    snprintf(buf, bufsize, "%d", 450);
    assert(strcmp(buf, "450") == 0);

    //negative decimal
    snprintf(buf, bufsize, "%d", -450);
    assert(strcmp(buf, "-450") == 0);

    snprintf(buf, bufsize, "%04d", 450);
    assert(strcmp(buf, "0450") == 0);

    // Hexadecimal
    snprintf(buf, bufsize, "%04x", 0xef);
    assert(strcmp(buf, "00ef") == 0);

    snprintf(buf, bufsize, "%x", 0xef);
    assert(strcmp(buf, "ef") == 0);

    // Pointer
    snprintf(buf, bufsize, "%p", (void *) 0x20200004);
    assert(strcmp(buf, "0x20200004") == 0);


    snprintf(buf, bufsize, "%p", (void *) 0x7ffff74);
    assert(strcmp(buf, "0x07ffff74") == 0);


    snprintf(buf, bufsize, "%04d%c", 450, 'A');
    assert(strcmp(buf, "0450A") == 0);

    snprintf(buf, bufsize, "%04x%c", 0xef, 'a');
    assert(strcmp(buf, "00efa") == 0);


    snprintf(buf, bufsize, "%04x%c%d", 0xef, 'a', 49);
    assert(strcmp(buf, "00efa49") == 0);


    snprintf(buf, bufsize, "%04x%d", 0xef, 15);
    assert(strcmp(buf, "00ef15") == 0);

    // Character
    snprintf(buf, bufsize, "%c", 'A');
    assert(strcmp(buf, "A") == 0);

    snprintf(buf, bufsize, "%c", 'z');
    assert(strcmp(buf, "z") == 0);

    snprintf(buf, bufsize, "%c %d", 'z', 5);
    assert(strcmp(buf, "z 5") == 0);

    //percent
    snprintf(buf, bufsize, "%%");
    assert(strcmp(buf, "%") == 0);

    // String
    snprintf(buf, bufsize, "%s", "binky");
    assert(strcmp(buf, "binky") == 0);

    snprintf(buf, bufsize, "%s", "hey there");
    assert(strcmp(buf, "hey there") == 0);

    // Format string with intermixed codes
    snprintf(buf, bufsize, "CS%d%c!", 107, 'e');
    assert(strcmp(buf, "CS107e!") == 0);

    snprintf(buf, bufsize, "CS%d", 107);
    assert(strcmp(buf, "CS107") == 0);

    snprintf(buf, bufsize, "CS %c", 'a');
    assert(strcmp(buf, "CS a") == 0);

    // Test return value
    assert(snprintf(buf, bufsize, "Hello") == 5);
    assert(snprintf(buf, 2, "Hello") == 5);


    snprintf(buf, bufsize, "%04d", -5);
    assert(strcmp(buf, "-005") == 0);

    snprintf(buf, 10, "%020x", 5);
    assert(strcmp(buf, "000000000") == 0);

}


void main(void)
{
    uart_init();
    test_memset();
    test_memcpy();
    test_strlen();
    test_strcmp();
    test_strlcat();
    test_strtonum();
    test_to_base();
    test_snprintf();
}
