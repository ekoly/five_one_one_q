import asyncio
import operator
import threading

import five_one_one_q.c

_global_lock = threading.Lock()


class PriorityQueue(asyncio.Queue):

    def __init__(
        self,
        key=operator.itemgetter(0),
        first_out=five_one_one_q.c.LOWEST,
        **kwargs,
    ):
        """
        Implementation of a PriorityQueue with support for a `key` operator.

        When `item` is `put()` into the queue, its priority will be determined
        by calling `key(item)`. `get()` will remove and return the item for
        which `key(item)` is the lowest or highest, depending on whether the
        `first_out` kwarg is set to `five_one_one_q.LOWEST` or
        `first_out=five_one_one_q.HIGHEST`. If items have a tied priority, the
        first item put into the queue will be the first out.

        Args:
            maxsize (int, optional): Maximum size of the queue. If None, the
                queue will have unlimited size. Defaults to None.
            key (callable, optional): Function called to determine items'
                priority. Defaults to `operator.itemgetter(0)`.
            first_out (int): Must be `five_one_one_q.LOWEST` or
                `five_one_one_q.HIGHEST`. Defaults to LOWEST.
        """
        super().__init__(**kwargs)
        self._first_out = first_out
        self._queue = five_one_one_q.c.bucketq(key=key, first_out=first_out)

    def _init(self, maxsize: int):
        pass

    def _put(self, item):
        self._queue.push(item)

    def _get(self):
        return self._queue.pop()

    def _get_loop(self):
        loop = asyncio.get_running_loop()
        if self._loop is None:
            with _global_lock:
                if self._loop is None:
                    self._loop = asyncio.get_running_loop()
        if self._loop is not loop:
            raise RuntimeError("Event loop changed!")
        return loop

    @property
    def first_out(self):
        return self._first_out
