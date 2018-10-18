ifdown eth0 ; ifup eth0
mount -t debugfs nodev /debug
echo 'function_graph' > /debug/tracing/current_tracer
