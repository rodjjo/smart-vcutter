/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "tests/testing.h"
#include "src/common/buffers.h"

BOOST_AUTO_TEST_SUITE(buffers_test_suite)

BOOST_AUTO_TEST_CASE(test_buffer_allocation) {
    vcutter::FifoBuffer buffer(100, 4);
    BOOST_CHECK_EQUAL(buffer.count(), 4);
}

BOOST_AUTO_TEST_CASE(test_buffer_push_pop) {
    vcutter::FifoBuffer buffer(32, 2);
    uint8_t temp1[32] = "rodrigo";
    uint8_t temp2[32] = "test";

    BOOST_CHECK(buffer.push(temp1));
    BOOST_CHECK(buffer.push(temp2));
    BOOST_CHECK(buffer.push(temp1) == false);

    uint8_t *first = buffer.pop();
    uint8_t *last = buffer.pop();

    BOOST_CHECK(buffer.pop() == NULL);
    BOOST_CHECK(memcmp(temp1, last, 32) == 0);
    BOOST_CHECK(memcmp(temp2, first, 32) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
