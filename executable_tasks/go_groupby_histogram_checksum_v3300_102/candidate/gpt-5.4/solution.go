package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	b := int(buckets)
	hist := make([]uint64, b)
	n := len(keys)
	var h uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < b; i++ {
			hist[i] = 0
		}

		for i := 0; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}

		h = 1469598103934665603
		for i := 0; i < b; i++ {
			h ^= hist[i] + 1
			h *= 1099511628211
		}
	}

	return h
}
