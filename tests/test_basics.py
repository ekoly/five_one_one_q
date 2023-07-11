import operator

import pytest  # noqa

from five_one_one_q import PriorityQueue
from five_one_one_q.c import bucketq
from tests.utils import (
    parametrized_first_out,
    parametrized_invalid_first_out,
    parametrized_maxsize,
    producer_factory,
)

"""
Basic sanity tests- make sure constructors work
"""


@parametrized_first_out
@parametrized_maxsize
def test_basic_q_constructor(q):
    assert isinstance(q, PriorityQueue)


@parametrized_first_out
def test_basic_bucketq_constructor(bq):
    assert isinstance(bq, bucketq)


@parametrized_first_out
@parametrized_maxsize
def test_basic_q_empty(q):
    assert q.empty()


@parametrized_first_out
def test_basic_bucketq_empty(bq):
    assert bq.empty()


@parametrized_first_out
@parametrized_maxsize
@pytest.mark.asyncio
async def test_basic_q_full_empty(q):
    prod = producer_factory(num_unique_priorities=3)
    counter = 0
    while not q.full():
        await q.put(prod())
        counter += 1
    assert not q.empty()
    for _ in range(counter):
        await q.get()
    assert q.empty()


@parametrized_first_out
def test_basic_bucketq_fill_empty(bq):
    prod = producer_factory(num_unique_priorities=3)
    counter = 0
    for _ in range(50):
        bq.push(prod())
        counter += 1
    assert not bq.empty()
    for _ in range(counter):
        bq.pop()
    assert bq.empty()


@parametrized_invalid_first_out
def test_basic_q_constructor_invalid(first_out):
    with pytest.raises(ValueError):
        PriorityQueue(key=operator.itemgetter(0), first_out=first_out)


@parametrized_invalid_first_out
def test_basic_bucketq_constructor_invalid(first_out):
    with pytest.raises(ValueError):
        bucketq(key=operator.itemgetter(0), first_out=first_out)
