This test measures the amount of context switches between two threads of the same priority.
The result amounts to the number of thread_yield() calls in *one* thread (half
the number of actual context switches).
