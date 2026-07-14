package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// pre-allocate once, re-use every iteration
	hist := make([]uint64, buckets)

	// iteration-local variables hoisted outside the loop
	var (
		finalHash uint64
		k, v, b   uint32
		idx       uint32
		hashes    = [4]uint64{0xcbf29ce484222d65, 0xcbf29ce484222d65, 0xcbf29ce484222d65, 0xcbf29ce484222d65}
	)
	const prime1 = 1469598103934665603
	const prime2 = 1099511628211

	for iter := 0; iter < iters; iter++ {
		// 1. fast reset-in-place (cheaper than make)
		for i := range hist {
			hist[i] = 0
		}

		// 2. aggregate in place *and* compute contributions to the rolling hash
		for j := 0; j < len(keys); j++ {
			k = keys[j]
			v = vals[j]
			hist[k] += uint64(v)
		}

		// 3. compute checksum from the populated buckets
		h := prime1
		for _, val := range hist {
			h ^= (val + 1)
			h *= prime2
		}
		finalHash = h
	}
	return finalHash
}