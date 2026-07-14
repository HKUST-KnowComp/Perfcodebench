package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	// Preallocate histogram once to eliminate repeated per-iteration heap allocations
	hist := make([]uint64, buckets)
	for iter := 0; iter < iters; iter++ {
		// Accumulate values into pre-zeroed histogram
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		// Compute checksum AND reset histogram to zero in one pass, avoids separate zeroing loop
		h = 1469598103934665603
		for i := range hist {
			v := hist[i]
			h ^= v + 1
			h *= 1099511628211
			hist[i] = 0 // Reset for next iteration, merged into checksum loop
		}
	}
	return h
}