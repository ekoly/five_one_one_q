import asyncio
import sys

import pytest  # noqa

from five_one_one_q import HIGHEST, LOWEST
from tests.utils import (
    counter_factory,
    parametrized_first_out,
    parametrized_maxsize,
    parametrized_unlimited,
    producer_factory,
)

NUM_OPERATIONS = 10_000


"""
This test file is the most important in terms of ensuring the queue order works
"as advertised". We use asyncio.wait_for so that the test automatically fails
if something unexpected happens, if a `get` or `put` is done that causes a long
wait.
"""


def check_order(actual, first_out):
    # test that priorities are in the expected order
    if first_out == LOWEST:
        assert all(actual[ix][0] <= actual[ix + 1][0] for ix in range(len(actual) - 1))
    elif first_out == HIGHEST:
        assert all(actual[ix][0] >= actual[ix + 1][0] for ix in range(len(actual) - 1))
    else:
        raise AssertionError("invalid value for first_out")


def check_expected(expected, actual):
    assert sorted(expected) == sorted(actual)


@parametrized_first_out
@parametrized_maxsize
@pytest.mark.parametrize(
    "num_unique_priorities",
    [1, 2, 3, 5, 10, 20, sys.maxsize],
)
@pytest.mark.asyncio
async def test_order_variable_priorities(q, num_unique_priorities):
    prod = producer_factory(num_unique_priorities=num_unique_priorities)
    expected = []
    actual = []

    while not q.full():
        item = prod()
        expected.append(item)
        await asyncio.wait_for(
            q.put(item),
            2,
        )

    while not q.empty():
        item = await asyncio.wait_for(
            q.get(),
            2,
        )
        actual.append(item)

    assert q.empty()

    check_order(actual, q.first_out)
    check_expected(expected, actual)


@parametrized_first_out
@parametrized_unlimited
@pytest.mark.parametrize(
    "num_unique_priorities",
    [1, 2, 3, 5, 10, 20, sys.maxsize],
)
@pytest.mark.asyncio
async def test_order_variable_priorities_unlimited_maxsize(
    q,
    num_unique_priorities,
):
    prod = producer_factory(num_unique_priorities=num_unique_priorities)
    expected = []
    actual = []

    for _ in range(NUM_OPERATIONS):
        item = prod()
        expected.append(item)
        await asyncio.wait_for(
            q.put(item),
            2,
        )

    for _ in range(NUM_OPERATIONS):
        item = await asyncio.wait_for(
            q.get(),
            2,
        )
        actual.append(item)

    assert q.empty()

    check_order(actual, q.first_out)
    check_expected(expected, actual)


@parametrized_first_out
@parametrized_maxsize
@pytest.mark.parametrize(
    "num_unique_priorities",
    [1, 2, 3, 5, 10, 20, sys.maxsize],
)
@pytest.mark.asyncio
async def test_order_variable_priorities_limited_maxsize_fifo(
    q,
    num_unique_priorities,
):
    prod = counter_factory(num_unique_priorities=num_unique_priorities)
    expected = []
    actual = []

    while not q.full():
        item = prod()
        expected.append(item)
        await asyncio.wait_for(
            q.put(item),
            2,
        )

    while not q.empty():
        item = await asyncio.wait_for(
            q.get(),
            2,
        )
        actual.append(item)

    assert q.empty()

    check_order(actual, q.first_out)
    check_expected(expected, actual)


@parametrized_first_out
@parametrized_unlimited
@pytest.mark.parametrize(
    "num_unique_priorities",
    [1, 2, 3, 5, 10, 20, sys.maxsize],
)
@pytest.mark.asyncio
async def test_order_variable_priorities_unlimited_maxsize_fifo(
    q,
    num_unique_priorities,
):
    prod = counter_factory(num_unique_priorities=num_unique_priorities)
    expected = []
    actual = []

    for _ in range(NUM_OPERATIONS):
        item = prod()
        expected.append(item)
        await asyncio.wait_for(
            q.put(item),
            2,
        )

    for _ in range(NUM_OPERATIONS):
        item = await asyncio.wait_for(
            q.get(),
            2,
        )
        actual.append(item)

    assert q.empty()

    check_order(actual, q.first_out)
    check_expected(expected, actual)
