s = runtime;

thread:2.store(s);
# 2 is M
thread:0-2.barrier();
thread:1.load(s);
# 1 is S, 2 is O
thread:0-2.barrier();

time(
thread:0.store(s);
);
