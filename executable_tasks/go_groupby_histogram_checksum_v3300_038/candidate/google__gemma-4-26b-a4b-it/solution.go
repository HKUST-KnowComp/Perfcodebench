package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}

	// Pre-allocate the histogram once to avoid repeated allocations and GC pressure
	hist := make([]uint64, buckets)
	var h uint64

	for iter := 0; iter < iters; iter++ {
		// Reset the histogram for the current iteration
		// Using a loop for zeroing is often optimized by the compiler to memclr
		for i := range hist {
			hist[i] = 0
		}

		// Accumulate values into buckets
		// The bounds check elimination is aided by the range over keys
		for i := 0; i < len(keys); i++ {
			k := keys[i]
			hist[k] += uint64(vals[i])
		}

		// Compute FNV-1a hash over the histogram buckets
		h = 1469598103934665603
		for i := 0; i < len(hist); i++ {
			v := hist[i]
			h ^= v + 1
			h *= 1099511628211
		}
	}

	return h
}