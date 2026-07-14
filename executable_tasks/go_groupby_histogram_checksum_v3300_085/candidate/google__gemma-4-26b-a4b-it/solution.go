package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	if buckets == 0 {
		return 0
	}

	hist := make([]uint64, buckets)
	var h uint64

	for iter := 0; iter < iters; iter++ {
		// Reset histogram efficiently
		for i := range hist {
			hist[i] = 0
		}

		// Aggregation loop
		for i := 0; i < len(keys); i++ {
			k := keys[i]
			hist[k] += uint64(vals[i])
		}

		// FNV-1a style checksum calculation
		h = 1469598103934665603
		for i := 0; i < int(buckets); i++ {
			v := hist[i]
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}