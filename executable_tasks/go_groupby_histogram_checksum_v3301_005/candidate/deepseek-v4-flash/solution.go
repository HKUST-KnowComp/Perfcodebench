package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	nb := int(buckets)
	hist := make([]uint64, nb)
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211
	var finalH uint64
	for iter := 0; iter < iters; iter++ {
		// zero histogram
		for i := 0; i < nb; i++ {
			hist[i] = 0
		}
		// accumulate
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		// checksum
		h := fnvOffset
		for _, v := range hist {
			h ^= v + 1
			h *= fnvPrime
		}
		finalH = h
	}
	return finalH
}