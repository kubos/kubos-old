/*
 * Copyright (C) 2017 Kubos Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "radio-controller/packet.h"
#include <cmocka.h>

void test_parse_packet(void ** arg)
{
    packet  packet;
    uint8_t data[PACKET_SIZE];

    assert_true(packet_parse(data, &packet));
}

void test_parse_version(void ** arg)
{
    packet  packet;
    uint8_t data[PACKET_SIZE];

    data[0] = 1;

    packet_parse(data, &packet);

    assert_int_equal(packet.header.version, 1);
}

void test_process_packet(void ** arg)
{
    uint8_t data[PACKET_SIZE];

    packet_process(data, PACKET_SIZE);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_packet),
        cmocka_unit_test(test_parse_version),
        cmocka_unit_test(test_process_packet),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
