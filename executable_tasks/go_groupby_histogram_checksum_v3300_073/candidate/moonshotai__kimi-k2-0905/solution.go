package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	if buckets == 0 {
		return 0
	}
	hist := make([]uint64, buckets)
	var h uint64

	for iter := 0; iter < iters; iter++ {
		// fast zero of the slice we will actually touch
		for i := range keys {
			hist[keys[i]] = 0
		}

		// histogram accumulation
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}

		// 4-unrolled FNV-1a checksum
		h = 1469598103934665603
		i := 0
		for ; i+4 <= int(buckets); i += 4 {
			h ^= hist[i] + 1
			h *= 1099511628211
			h ^= hist[i+1] + 1
			h *= 1099511628211
			h ^= hist[i+2] + 1
			h *= 1099511628211
			h ^= hist[i+3] + 1
			h *= 1099511628211
		}
		for ; i < int(buckets); i++ {
			h ^= hist[i] + 1
			h *= 1099511628211
		}
	}
	return h
}