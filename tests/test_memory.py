import asyncio  # noqa
import warnings

import pytest  # noqa

from five_one_one_q import LOWEST
from tests.utils import producer_factory

NUM_OPERATIONS = 10_000_000

message = (
    "test_memory.py takes a long time to run and is intended to be left "
    "running while the developer monitors memory usage (for example with "
    "htop). It is recommended to run this test file at least once if "
    "changes to the C code has been made, but is otherwise probably not "
    "necessary. "
    "See the Makefile for common pytest commands."
)
warnings.warn(message)


@pytest.mark.parametrize(
    "first_out",
    [
        LOWEST,
    ],  # no need to change default
)
@pytest.mark.parametrize(
    "maxsize",
    [
        0,
    ],  # unlimited
)
@pytest.mark.parametrize(
    "test_number",
    list(range(10_000)),
)
@pytest.mark.asyncio
async def test_priority_leaks_put_only(q, test_number):
    # unlimited number of priorities
    prod = producer_factory()
    for _ in range(NUM_OPERATIONS):
        await asyncio.wait_for(
            q.put(prod()),
            2,
        )


@pytest.mark.parametrize(
    "first_out",
    [
        LOWEST,
    ],  # no need to change default
)
@pytest.mark.parametrize(
    "maxsize",
    [
        0,
    ],  # unlimited
)
@pytest.mark.parametrize(
    "test_number",
    list(range(10_000)),
)
@pytest.mark.asyncio
async def test_value_leaks_put_only(q, test_number):
    # limited number of priorities
    prod = producer_factory(num_unique_priorities=3)
    for _ in range(NUM_OPERATIONS):
        await asyncio.wait_for(
            q.put(prod()),
            2,
        )


@pytest.mark.parametrize(
    "first_out",
    [
        LOWEST,
    ],  # no need to change default
)
@pytest.mark.parametrize(
    "maxsize",
    [
        0,
    ],  # unlimited
)
@pytest.mark.parametrize(
    "test_number",
    list(range(10_000)),
)
@pytest.mark.asyncio
async def test_priority_leaks_put_get(q, test_number):
    # unlimited number of priorities
    prod = producer_factory()
    for _ in range(NUM_OPERATIONS):
        await asyncio.wait_for(
            q.put(prod()),
            2,
        )
    for _ in range(NUM_OPERATIONS):
        await asyncio.wait_for(
            q.get(),
            2,
        )


@pytest.mark.parametrize(
    "first_out",
    [
        LOWEST,
    ],  # no need to change default
)
@pytest.mark.parametrize(
    "maxsize",
    [
        0,
    ],  # unlimited
)
@pytest.mark.parametrize(
    "test_number",
    list(range(10_000)),
)
@pytest.mark.asyncio
async def test_value_leaks_put_get(q, test_number):
    # unlimited number of priorities
    prod = producer_factory(num_unique_priorities=3)
    for _ in range(NUM_OPERATIONS):
        await asyncio.wait_for(
            q.put(prod()),
            2,
        )
    for _ in range(NUM_OPERATIONS):
        await asyncio.wait_for(
            q.get(),
            2,
        )
