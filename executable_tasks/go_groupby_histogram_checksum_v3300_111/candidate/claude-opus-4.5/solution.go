package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	n := len(keys)
	hist := make([]uint64, buckets)
	zero := make([]uint64, buckets)

	for iter := 0; iter < iters; iter++ {
		// Fast zero using copy
		copy(hist, zero)

		// Unrolled aggregation loop
		i := 0
		for ; i+3 < n; i += 4 {
			hist[keys[i]] += uint64(vals[i])
			hist[keys[i+1]] += uint64(vals[i+1])
			hist[keys[i+2]] += uint64(vals[i+2])
			hist[keys[i+3]] += uint64(vals[i+3])
		}
		for ; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}

		// FNV-1a style checksum with unrolling
		h = 1469598103934665603
		b := uint32(buckets)
		j := uint32(0)
		for ; j+3 < b; j += 4 {
			h ^= hist[j] + 1
			h *= 1099511628211
			h ^= hist[j+1] + 1
			h *= 1099511628211
			h ^= hist[j+2] + 1
			h *= 1099511628211
			h ^= hist[j+3] + 1
			h *= 1099511628211
		}
		for ; j < b; j++ {
			h ^= hist[j] + 1
			h *= 1099511628211
		}
	}
	return h
}