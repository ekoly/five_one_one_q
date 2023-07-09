import pytest  # noqa

from tests.utils import parametrized_first_out, parametrized_maxsize

from five_one_one_q import PriorityQueue
from five_one_one_q.c import bucketq


@parametrized_first_out
@parametrized_maxsize
def test_basic_q_constructor(q):
    assert isinstance(q, PriorityQueue)


@parametrized_first_out
def test_basic_bucketq_constructor(bq):
    assert isinstance(bq, bucketq)
