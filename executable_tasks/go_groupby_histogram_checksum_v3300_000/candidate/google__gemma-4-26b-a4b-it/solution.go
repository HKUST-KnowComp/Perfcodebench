package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	hist := make([]uint64, buckets)
	var h uint64
	
	for iter := 0; iter < iters; iter++ {
		// Reset histogram efficiently
		for i := range hist {
			hist[i] = 0
		}

		// Accumulate values
		// Using local variables to help compiler with bounds check elimination
		k := keys
		v := vals
		for i := 0; i < len(k); i++ {
			hist[k[i]] += uint64(v[i])
		}

		// FNV-1a hash calculation
		h = 1469598103934665603
		for i := 0; i < len(hist); i++ {
			v_val := hist[i]
			h ^= v_val + 1
			h *= 1099511628211
		}
	}
	return h
}