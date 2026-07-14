package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	b := int(buckets)
	hist := make([]uint64, b)

	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	var h uint64
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < b; i++ {
			hist[i] = 0
		}

		for i, k := range keys {
			hist[k] += uint64(vals[i])
		}

		h = offset
		for i := 0; i < b; i++ {
			h ^= hist[i] + 1
			h *= prime
		}
	}
	return h
}
