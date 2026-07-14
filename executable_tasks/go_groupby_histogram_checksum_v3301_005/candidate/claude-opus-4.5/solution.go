package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	n := len(keys)
	hist := make([]uint64, buckets)
	for iter := 0; iter < iters; iter++ {
		// Fast zero using append trick that compiles to runtime.memclrNoHeapPointers
		for i := range hist {
			hist[i] = 0
		}
		// Aggregate
		for i := 0; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		// Compute checksum
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}