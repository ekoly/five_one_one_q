import asyncio
import operator
import random
import sys
from bisect import insort_right

import pytest  # noqa

from five_one_one_q import HIGHEST, LOWEST
from tests.utils import (
    counter_factory,
    parametrized_first_out,
    parametrized_first_out_lowest,
    parametrized_maxsize,
    parametrized_maxsize_and_unlimited,
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
    (
        1,
        2,
        3,
        5,
        10,
        20,
        sys.maxsize,
    ),
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
    (
        1,
        2,
        3,
        5,
        10,
        20,
        sys.maxsize,
    ),
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
    (
        1,
        2,
        3,
        5,
        10,
        20,
        sys.maxsize,
    ),
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
    (
        1,
        2,
        3,
        5,
        10,
        20,
        sys.maxsize,
    ),
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


@parametrized_first_out_lowest
@parametrized_maxsize_and_unlimited
@pytest.mark.parametrize(
    "num_unique_priorities",
    (
        1,
        2,
        3,
        5,
        8,
        13,
        21,
        34,
        55,
    ),
)
@pytest.mark.asyncio
async def test_order_randomized_put_get(
    q,
    num_unique_priorities,
):
    prod = counter_factory(num_unique_priorities=num_unique_priorities)
    control = []
    k = operator.itemgetter(0)

    for _ in range(100_000):
        if q.empty():
            # must push if we are empty
            item = prod()
            await q.put(item)
            insort_right(control, item, key=k)
        elif q.full():
            # must pop if we are full
            q_item = await q.get()
            c_item = control.pop(0)
            assert q_item == c_item
        elif random.randint(0, 2) != 0:
            # if we get here, we can either push or pop
            item = prod()
            await q.put(item)
            insort_right(control, item, key=k)
        else:
            q_item = await q.get()
            c_item = control.pop(0)
            assert q_item == c_item

    while not q.empty():
        q_item = await q.get()
        c_item = control.pop(0)
        assert q_item == c_item
