package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	for iter := 0; iter < iters; iter++ {
		hist := make([]uint64, buckets)
		n := len(keys)
		// Manual 4-way unroll to remove bounds checks and reduce loop overhead.
		for i := 0; i <= n-4; i += 4 {
			hist[keys[i]] += uint64(vals[i])
			hist[keys[i+1]] += uint64(vals[i+1])
			hist[keys[i+2]] += uint64(vals[i+2])
			hist[keys[i+3]] += uint64(vals[i+3])
		}
		// Tail for remaining 0-3 elements.
		for i := n &^ 3; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		// FNV-1a checksum over buckets.
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}