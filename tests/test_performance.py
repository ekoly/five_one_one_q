import asyncio
import logging
import logging.handlers
import random
import sys
import time
import warnings

import pytest  # noqa

from five_one_one_q import LOWEST
from tests.utils import (
    counter_factory,
    parametrized_first_out,
    parametrized_first_out_lowest,
    parametrized_maxsize,
    parametrized_maxsize_and_unlimited,
    parametrized_unlimited,
    producer_factory,
)

logger = logging.getLogger(__name__)
formatter = logging.Formatter("%(asctime)s | %(levelname)s | %(message)s")
wfh = logging.handlers.WatchedFileHandler("performance.log")
wfh.setFormatter(formatter)
logger.addHandler(wfh)
logger.setLevel(logging.INFO)

message = (
    "This test will compare the performance of `five_one_one_q.PiorityQueue` "
    "against its main competitor, the `asyncio.PriorityQueue`. I note that "
    "the behavior/order of these queues will be different (see readme), but "
    "it is assumed that the five one one queue works 'as advertised' based "
    "on test_order.py. The results will appear in performance.log."
)

warnings.warn(message)

NUM_OPERATIONS = 100_000


@parametrized_first_out_lowest
@parametrized_unlimited
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
        sys.maxsize,
    ),
)
@pytest.mark.asyncio
async def test_order_many_put(
    q,
    num_unique_priorities,
):
    prod = counter_factory(num_unique_priorities=num_unique_priorities)
    aq = asyncio.PriorityQueue()
    control = []

    for _ in range(NUM_OPERATIONS):
        control.append(prod())

    foo_t_start = time.time()
    for item in control:
        await q.put(item)
    foo_t_end = time.time()

    foo_t_delta = foo_t_end - foo_t_start

    aq_t_start = time.time()
    for item in control:
        await aq.put(item)
    aq_t_end = time.time()

    aq_t_delta = aq_t_end - aq_t_start

    if foo_t_delta < aq_t_delta:
        faster = "five one one"
        slower = "asyncio"
        faster_delta = foo_t_delta
        slower_delta = aq_t_delta
    else:
        faster = "asyncio"
        slower = "five one one"
        faster_delta = aq_t_delta
        slower_delta = foo_t_delta
    pdiff = (slower_delta - faster_delta) / slower_delta * 100.0

    logger.info(
        "Test: %s put operations, %s unique priorities:"
        f"\n\t%s is faster with {faster_delta:.3f} seconds"
        f"\n\t%s is slower with {slower_delta:.3f} seconds"
        f"\n\t{pdiff:.3f}%% difference",
        NUM_OPERATIONS,
        num_unique_priorities,
        faster,
        slower,
    )


@parametrized_first_out_lowest
@parametrized_unlimited
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
        sys.maxsize,
    ),
)
@pytest.mark.asyncio
async def test_order_many_get(
    q,
    num_unique_priorities,
):
    prod = counter_factory(num_unique_priorities=num_unique_priorities)
    aq = asyncio.PriorityQueue()

    for _ in range(NUM_OPERATIONS):
        item = prod()
        await q.put(item)
        await aq.put(item)

    foo_t_start = time.time()
    for _ in range(NUM_OPERATIONS):
        await q.get()
    foo_t_end = time.time()

    foo_t_delta = foo_t_end - foo_t_start

    aq_t_start = time.time()
    for _ in range(NUM_OPERATIONS):
        await aq.get()
    aq_t_end = time.time()

    aq_t_delta = aq_t_end - aq_t_start

    if foo_t_delta < aq_t_delta:
        faster = "five one one"
        slower = "asyncio"
        faster_delta = foo_t_delta
        slower_delta = aq_t_delta
    else:
        faster = "asyncio"
        slower = "five one one"
        faster_delta = aq_t_delta
        slower_delta = foo_t_delta
    pdiff = (slower_delta - faster_delta) / slower_delta * 100.0

    logger.info(
        "Test: %s get operations, %s unique priorities:"
        f"\n\t%s is faster with {faster_delta:.3f} seconds"
        f"\n\t%s is slower with {slower_delta:.3f} seconds"
        f"\n\t{pdiff:.3f}%% difference",
        NUM_OPERATIONS,
        num_unique_priorities,
        faster,
        slower,
    )


@parametrized_first_out_lowest
@parametrized_unlimited
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
        sys.maxsize,
    ),
)
@pytest.mark.asyncio
async def test_order_randomized_put_get(
    q,
    num_unique_priorities,
):
    prod = counter_factory(num_unique_priorities=num_unique_priorities)
    aq = asyncio.PriorityQueue()
    control = []
    qsize = 0

    for _ in range(NUM_OPERATIONS):
        # get operations are more likely if qsize is large
        # put operations are more likely if qsize is small
        if random.randint(0, qsize // 10) == 0:
            control.append(("put", prod()))
            qsize += 1
        else:
            control.append(("get", None))
            qsize -= 1

    foo_t_start = time.time()
    for op, item in control:
        if op == "put":
            await q.put(item)
        else:
            await q.get()
    foo_t_end = time.time()

    foo_t_delta = foo_t_end - foo_t_start

    aq_t_start = time.time()
    for op, item in control:
        if op == "put":
            await aq.put(item)
        else:
            await aq.get()
    aq_t_end = time.time()

    aq_t_delta = aq_t_end - aq_t_start

    if foo_t_delta < aq_t_delta:
        faster = "five one one"
        slower = "asyncio"
        faster_delta = foo_t_delta
        slower_delta = aq_t_delta
    else:
        faster = "asyncio"
        slower = "five one one"
        faster_delta = aq_t_delta
        slower_delta = foo_t_delta
    pdiff = (slower_delta - faster_delta) / slower_delta * 100.0

    logger.info(
        "Test: %s randomize put/get operations, %s unique priorities:"
        f"\n\t%s is faster with {faster_delta:.3f} seconds"
        f"\n\t%s is slower with {slower_delta:.3f} seconds"
        f"\n\t{pdiff:.3f}%% difference",
        NUM_OPERATIONS,
        num_unique_priorities,
        faster,
        slower,
    )
