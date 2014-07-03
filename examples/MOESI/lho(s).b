s = runtime;

thread:1.store(s);
# 1 is M
thread:0-1.barrier();
thread:0.load(s);
# 0 is S and 1 is O
thread:0-1.barrier();

time(
thread:1.load(s);
);
