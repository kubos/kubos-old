#include "radio-controller/telecommand.h"
#include <cmocka.h>

void test_parse_enable_line(void ** arg)
{
    uint8_t buffer[] = { 0xfd, 0x55, 0xd5, 0x55, 0x0, 0x3,  0xff,
                         0xff, 0x80, 0x1,  0x1,  0x1, 0x55, 0x55 };
    telecommand_packet p;
    telecommand_parse(buffer, &p);
    telecommand_debug(p);

    assert_int_equal(p.data.payload[0], 1);
    assert_int_equal(p.data.payload[1], 1);
    assert_int_equal(p.data.payload[2], 1);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_enable_line),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
