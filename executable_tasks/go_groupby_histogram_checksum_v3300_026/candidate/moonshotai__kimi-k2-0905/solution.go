package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// allocate once, reuse every iteration
	hist := make([]uint64, buckets)

	var h uint64
	for iter := 0; iter < iters; iter++ {
		// zero histogram: 8 bytes at a time, compiler emits DUFFCOPY-style move
		for i := range hist {
			hist[i] = 0
		}

		// populate histogram and accumulate FNV-1a checksum in a single pass
		// buckets is a power of two → masking is equivalent to modulo
		h = 1469598103934665603
		for i := range keys {
			b := keys[i] & (buckets - 1)
			v := uint64(vals[i])
			hist[b] += v

			h ^= hist[b] + 1
			h *= 1099511628211
		}
	}
	return h
}