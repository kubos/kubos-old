#include "radio-controller/telecommand.h"
#include <cmocka.h>

void test_parse_enable_line(void ** arg)
{
    uint8_t buffer[] = { 0xfd, 0x55, 0xd5, 0x55, 0x0, 0x3,  0xff,
                         0xff, 0x80, 0x1,  0x1,  0x1, 0x55, 0x55 };
    telecommand_packet p;
    telecommand_parse(buffer, &p);
    telecommand_debug(p);

    assert_int_equal(p.id.version, 7);
    assert_int_equal(p.id.type, 1);
    assert_int_equal(p.id.dfh, 1);
    assert_int_equal(p.id.app_id, 1365);
    assert_int_equal(p.sequence.flags, 3);
    assert_int_equal(p.sequence.count, 5461);
    assert_int_equal(p.data_length, 3);
    assert_int_equal(p.data.header.csh, 1);
    assert_int_equal(p.data.header.version, 7);
    assert_int_equal(p.data.header.ack, 15);
    assert_int_equal(p.data.header.service_type, 255);
    assert_int_equal(p.data.header.service_subtype, 128);
    assert_int_equal(p.data.payload[0], 1);
    assert_int_equal(p.data.payload[1], 1);
    assert_int_equal(p.data.payload[2], 1);
    assert_int_equal(p.data.error_control, 21845);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_enable_line),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
