package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	// Allocate histogram once and reuse across iterations.
	hist := make([]uint64, buckets)
	n := len(keys)
	b := int(buckets)

	for iter := 0; iter < iters; iter++ {
		// Accumulate values into histogram (hist is guaranteed zero here).
		for i := 0; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}

		// Compute checksum and clear histogram for next iteration in the same pass.
		h = 1469598103934665603
		for i := 0; i < b; i++ {
			v := hist[i]
			h ^= v + 1
			h *= 1099511628211
			hist[i] = 0
		}
	}
	return h
}
