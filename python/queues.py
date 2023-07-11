import asyncio
import operator

from five_one_one_q.c import LOWEST, bucketq


class PriorityQueue(asyncio.Queue):
    def __init__(
        self,
        key=operator.itemgetter(0),
        first_out=LOWEST,
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
        self._queue = bucketq(key=key, first_out=first_out)

    @property
    def first_out(self):
        return self._queue.first_out

    def empty(self):
        return self._queue.empty()

    def _init(self, maxsize: int):
        pass

    def _put(self, item):
        self._queue.push(item)

    def _get(self):
        return self._queue.pop()
