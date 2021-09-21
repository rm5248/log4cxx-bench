# Log4cxx Benchmarking Application

This is a benchmarking application for use with Apache Log4cxx.

## Profiling

In order to run a profile on the code, it's easy to use valgrind to
profile the code, and kcachegrind to view the results:

```
valgrind --tool=callgrind ./bench 100000
kcachegrind callgrind.out.<pid-number>
```

Using the default of 1,000,000 messages will take a very long time
and won't be too useful, so lowering that number probably makes the most
sense.
