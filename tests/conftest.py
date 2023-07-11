import operator

import pytest

import five_one_one_q
import five_one_one_q.c


@pytest.fixture(scope="function")
def q(first_out, maxsize):
    yield five_one_one_q.PriorityQueue(
        key=operator.itemgetter(0),
        first_out=first_out,
        maxsize=maxsize,
    )


@pytest.fixture(scope="function")
def bq(first_out):
    yield five_one_one_q.c.bucketq(
        key=operator.itemgetter(0),
        first_out=first_out,
    )
