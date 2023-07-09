# five-one-one-q
C-backed priority queue implementation, with support for a sort-like `key`
argument.

### Behavior
The order of items returned by `await q.get()` is as follows:
 1. Lowest value of `key(item)` is first out.
 2. If multiple items have a tied value of `key(item)`, FIFO is followed.

If the `first_out=five_one_one_q.HIGHEST` keyword argument is given to the
constructor, the highest `key(item)` is first out.

For items `put` into the queue, `key(item)` must support the `<` operator, or
the `>` operator if `first_out` is set to `HIGHEST`.

### Examples

The following assume an environment that supports `await`.

First:
```
import operator
import five_one_one_q

my_q = five_one_one_q.PriorityQueue(key=operator.itemgetter(0), first_out=five_one_one_q.LOWEST)

await my_q.put((3, "Alice"))
await my_q.put((1, "Eve"))
await my_q.put((2, "Bob"))

results = []
while not my_q.empty():
    item = await my_q.get()
    results.append(item)

print(results)
```
Will output the following:
```
[(1, 'Eve'), (2, 'Bob'), (3, 'Alice')]
```

Second:
```
import operator
import random
import five_one_one_q

my_q = five_one_one_q.PriorityQueue(key=operator.itemgetter(0), first_out=five_one_one_q.LOWEST)


for item_counter in range(20):
    # first item is a randomized priority
    # second item would be the place in a non-priority queue
    await my_q.put((random.randint(1, 3), item_counter))

results = []
while not my_q.empty():
    item = await my_q.get()
    results.append(item)

print(results)
```
Will output the following:
```
# Lowest priority first out
# Tied priority, first in first out
[(1, 1), (1, 2), (1, 5), (1, 7), (1, 8), (1, 14), (1, 18), (2, 3), (2, 4), (2, 9), (2, 11), (2, 12), (3, 0), (3, 6), (3, 10), (3, 13), (3, 15), (3, 16), (3, 17), (3, 19)]
```

Third:
```
import operator
import random
import five_one_one_q

my_q = five_one_one_q.PriorityQueue(key=operator.itemgetter(0), first_out=five_one_one_q.LOWEST)


for item_counter in range(20, 0, -1):
    # first item is a randomized priority
    # second item would be the REVERSED place in a non-priority queue
    await my_q.put((random.randint(1, 3), item_counter))

results = []
while not my_q.empty():
    item = await my_q.get()
    results.append(item)

print(results)
```
Will output the following:
```
# Lowest priority first out
# Tied priority, first in first out
[(1, 19), (1, 10), (1, 7), (1, 6), (1, 4), (1, 3), (2, 20), (2, 17), (2, 16), (2, 15), (2, 9), (2, 5), (2, 1), (3, 18), (3, 14), (3, 13), (3, 12), (3, 11), (3, 8), (3, 2)]
```


### Current Status

I have not seen unexpected exceptions being raised by `await get()` or
`await put(item)` in the current iteration. The order out is as expected.

This code currently does not have a unit test suite. This should be coming
soon.

This is a C-backed library meaning that object reference counts need to be
handled manually. The library needs to be rigorously checked for memory leaks.
