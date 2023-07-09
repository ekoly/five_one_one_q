# five_one_one_q
C-backed priority queue implementation, with support for a sort-like `key` argument.

### Current Status
While `await q.put(item)` and `await q.get()` do not throw errors, items returned by `get()` are not in the expected order.

The intended order is:
 1. Items for which `key(item)` is lowest are first out (or highest if `first_out=five_one_one_q.HIGHEST` is given).
 2. If multiple items have a tied value of `key(item)`, the first in should be the first out.

In other words, if we run the following (in an environment where `await` is usable):
```
import operator
import random

import five_one_one_q

my_q = five_one_one_q.PriorityQueue(key=operator.itemgetter(0), first_out=five_one_one_q.LOWEST)


for counter in range(10):
    # first value is the priority, second indicates the place in the queue
    await my_q.put((random.randint(1, 10), counter))

results = []

while not my_q.empty():
    res = await my_q.get()
    results.append(res)

print(results)
```
We would expect the following output (lowest priority first, and first in if there is a priority tie):
```
[(2, 1),
 (2, 4),
 (2, 8),
 (3, 0),
 (3, 6),
 (3, 7),
 (4, 5),
 (5, 3),
 (8, 2),
 (9, 9)]
```
What we see in the current state is:
```
[(2, 1),
 (2, 4),
 (2, 8),
 (3, 6), # 6th in, yet comes out before 0th in
 (3, 0), # 0th in, yet comes out after 6th in
 (3, 7),
 (4, 5),
 (5, 3),
 (8, 2),
 (9, 9)]
```

This problem is due to my own misunderstanding of the behavior of heaps and will require significant changes to fix.
