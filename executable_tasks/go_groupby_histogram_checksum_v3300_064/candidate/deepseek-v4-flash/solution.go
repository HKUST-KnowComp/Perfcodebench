package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	hist := make([]uint64, buckets)
	zero := make([]uint64, buckets)

	for iter := 0; iter < iters; iter++ {
		copy(hist, zero)

		for i, k := range keys {
			hist[k] += uint64(vals[i])
		}

		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}
