import pytest

import five_one_one_q


parametrized_first_out = pytest.mark.parametrize(
    "first_out",
    [five_one_one_q.LOWEST, five_one_one_q.HIGHEST],
)

parametrized_maxsize = pytest.mark.parametrize(
    "maxsize",
    [5, 10, 50, 100, 500, 1000, None],
)
