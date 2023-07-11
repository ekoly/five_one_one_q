import asyncio
import time
import warnings

import pytest  # noqa

from tests.utils import (
    avg,
    parametrized_first_out_lowest,
    parametrized_maxsize,
    parametrized_maxsize_and_unlimited,
    parametrized_maxsize_short,
    producer_factory,
    std,
)

"""
The asyncio mechanisms of `five_one_one_q.PriorityQueue` are inherrited from
`asyncio.Queue` and I generally think testing these mechanisms is outside the
scope of this project.

The tests in this file are just for sanity.
"""

message = (
    "test_async.py takes a very long time to run. "
    "Unless changes have been made to the `asyncio.Queue` inheritance, "
    "or the methods inherited from `asyncio.Queue` have been overridden, "
    "there is no reason to believe that any of these tests will fail. "
    "See the Makefile for common pytest commands."
)
warnings.warn(message)


async def producer_worker(my_q, prod, num, bar, fut):
    await bar.wait()

    t_start = time.time()
    for _ in range(num):
        # The sleep will encourage python's asyncio machinery to yield
        # processor time to another worker after a single iteration.
        await asyncio.sleep(0.01)
        try:
            # The test(s) that use this are mainly checking that there is not
            # too much variation in activity among the workers. Performance is
            # checked in other tests.
            # We thus allow for a liberal amount of wait time.
            await asyncio.wait_for(
                my_q.put(prod()),
                10,
            )
        except asyncio.TimeoutError as exc:
            fut.set_exception(exc)
            return
        except Exception as exc:
            fut.set_exception(exc)
            return
    t_end = time.time()

    fut.set_result(t_end - t_start)


async def consumer_worker(my_q, bar, done_eve, fut):
    await bar.wait()

    counter = 0
    while True:
        # The sleep will encourage python's asyncio machinery to yield
        # processor time to another worker after a single iteration.
        await asyncio.sleep(0.01)
        try:
            if my_q.empty() and done_eve.is_set():
                break
            # The test(s) that use this are mainly checking that there is not
            # too much variation in activity among the workers. Performance is
            # checked in other tests.
            # We thus allow for a liberal amount of wait time.
            await asyncio.wait_for(
                my_q.get(),
                10,
            )
            counter += 1
        except asyncio.TimeoutError as exc:
            if not done_eve.is_set():
                fut.set_exception(exc)
                return
            break
        except Exception as exc:
            fut.set_exception(exc)
            return

    fut.set_result(counter)


@parametrized_first_out_lowest
@parametrized_maxsize
@pytest.mark.asyncio
async def test_await_put_full_queue_times_out(q):
    prod = producer_factory()

    while not q.full():
        await asyncio.wait_for(
            q.put(prod()),
            1,
        )

    with pytest.raises(asyncio.TimeoutError):
        await asyncio.wait_for(
            q.put(prod()),
            1,
        )


@parametrized_first_out_lowest
@parametrized_maxsize
@pytest.mark.asyncio
async def test_await_get_empty_queue_times_out(q):
    prod = producer_factory()

    while not q.full():
        await asyncio.wait_for(
            q.put(prod()),
            1,
        )

    while not q.empty():
        await asyncio.wait_for(
            q.get(),
            1,
        )

    with pytest.raises(asyncio.TimeoutError):
        await asyncio.wait_for(
            q.get(),
            1,
        )


@parametrized_first_out_lowest
@parametrized_maxsize_short
@pytest.mark.parametrize(
    "num_unique_priorities",
    (
        1,
        2,
        3,
        5,
        10,
        20,
    ),
)
@pytest.mark.parametrize(
    "num_producers",
    (
        2,
        5,
    ),
)
@pytest.mark.parametrize(
    "num_consumers",
    (2,),
)
@pytest.mark.asyncio
async def test_multiple_tasks(
    q,
    num_unique_priorities,
    num_producers,
    num_consumers,
):
    """
    This test is not "realistic" in that in a production environment you
    would not want to have mutliple async tasks running at once whose limiting
    factor is processor time.
    Even still, I wanted to make sure the async machinery holds up under heavy
    load.
    """
    loop = asyncio.get_running_loop()
    prod = producer_factory(num_unique_priorities=num_unique_priorities)
    clear_to_start_bar = asyncio.Barrier(num_producers + num_consumers)
    prod_done_eve = asyncio.Event()
    num_items = 1000

    prod_futures = []
    for _ in range(num_producers):
        fut = loop.create_future()
        prod_futures.append(fut)
        loop.create_task(
            producer_worker(
                q,
                prod,
                num_items // num_producers,
                clear_to_start_bar,
                fut,
            )
        )

    consumer_futures = []
    for _ in range(num_consumers):
        fut = loop.create_future()
        consumer_futures.append(fut)
        loop.create_task(
            consumer_worker(
                q,
                clear_to_start_bar,
                prod_done_eve,
                fut,
            )
        )

    # pytest-asyncio will actually close the event loop when the test exits.
    # If there are still tasks pending at that time, it will cause
    # scary looking tracebacks.
    # Need to wait for all tasks to complete before introducing the
    # possibility of exiting the test.
    prod_done, _ = await asyncio.wait(
        prod_futures,
        return_when=asyncio.ALL_COMPLETED,
    )

    # Let our consumers know prod is done
    prod_done_eve.set()

    # so far so good, on to consumer results
    consumer_done, _ = await asyncio.wait(
        consumer_futures,
        return_when=asyncio.ALL_COMPLETED,
    )

    # pytest-asyncio will actually close the event loop when the test exits.
    # If there are future exceptions that have not been retrieved at this time,
    # it will cause scary looking tracebacks.
    prod_exceptions = [fut.exception() for fut in prod_done]
    consumer_exceptions = [fut.exception() for fut in consumer_done]

    for exc in prod_exceptions:
        if exc is not None:
            raise exc

    for exc in consumer_exceptions:
        if exc is not None:
            raise exc

    prod_results = [f.result() for f in prod_done]
    consumer_results = [f.result() for f in consumer_done]

    # Our tests will show that there is not too much variation (using
    # "coefficient of variation"), hence if there was a bottleneck on one end,
    # the asyncio waiting machinery still worked.
    # Producer results is the amount of time in seconds it took to `put` a
    # constant amount of items into the queue.
    assert (std(prod_results) / avg(prod_results)) < 0.55

    # Consumer results is the number of `get` operations the consumer was
    # able to complete.
    assert (std(consumer_results) / avg(consumer_results)) < 0.55
