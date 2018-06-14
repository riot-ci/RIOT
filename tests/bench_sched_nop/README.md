# About

This test calls "thread_yield()" in a loop. As there is no other thread with a
higher or same priority, this measures the raw context save / restore
performance plus the (short) time the scheduler need to realize there's no
other active thread.
The result amounts to the number of thread_yield() calls per second.
