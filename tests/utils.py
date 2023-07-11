import logging
import logging.handlers
import random
import string
import sys
from typing import List

import pytest

import five_one_one_q

logger = logging.getLogger("five_one_one_q.c")
formatter = logging.Formatter("%(asctime)s | %(levelname)s | %(message)s")
wfh = logging.handlers.WatchedFileHandler("tests.log")
wfh.setFormatter(formatter)
logger.addHandler(wfh)
logger.setLevel(logging.DEBUG)


parametrized_first_out = pytest.mark.parametrize(
    "first_out",
    [five_one_one_q.LOWEST, five_one_one_q.HIGHEST],
)

parametrized_first_out_lowest = pytest.mark.parametrize(
    "first_out",
    [five_one_one_q.LOWEST],
)

parametrized_invalid_first_out = pytest.mark.parametrize(
    "first_out",
    [255, 511, 1023],
)

parametrized_maxsize = pytest.mark.parametrize(
    "maxsize",
    [5, 10, 50, 100, 500, 1000, 5000, 10_000],
)

parametrized_maxsize_short = pytest.mark.parametrize(
    "maxsize",
    [5, 10, 50, 100, 500],
)

parametrized_unlimited = pytest.mark.parametrize(
    "maxsize",
    [0],  # 0 means unlimited
)

parametrized_maxsize_and_unlimited = pytest.mark.parametrize(
    "maxsize",
    [5, 10, 50, 100, 500, 1000, 5000, 10_000, 0],  # 0 means unlimited
)


def producer_factory(num_unique_priorities: int = sys.maxsize):
    def _producer():
        priority = hex(random.randint(0, num_unique_priorities - 1))
        value = "".join(random.choice(string.ascii_lowercase) for _ in range(64))
        return (priority, value)

    return _producer


def counter_factory(num_unique_priorities: int = sys.maxsize):
    _count = 0

    def _producer():
        nonlocal _count
        priority = hex(random.randint(0, num_unique_priorities - 1))
        value = _count
        _count += 1
        return (priority, value)

    return _producer


def avg(values: List[float]) -> float:
    return sum(values) / len(values)


def std(values: List[float]) -> float:
    mean = avg(values)
    return (sum((val - mean) ** 2 for val in values) / len(values)) ** 0.5
