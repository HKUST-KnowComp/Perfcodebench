package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// Reuse a single histogram across iterations; reset it during checksum
	hist := make([]uint64, buckets)
	var h uint64
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	n := len(keys)
	intBuckets := int(buckets)

	for iter := 0; iter < iters; iter++ {
		// Accumulate into histogram; unroll for throughput
		hk := keys
		hv := vals
		i := 0
		for i+3 < n {
			k0 := hk[i]
			k1 := hk[i+1]
			k2 := hk[i+2]
			k3 := hk[i+3]
			hist[k0] += uint64(hv[i])
			hist[k1] += uint64(hv[i+1])
			hist[k2] += uint64(hv[i+2])
			hist[k3] += uint64(hv[i+3])
			i += 4
		}
		for ; i < n; i++ {
			hist[hk[i]] += uint64(hv[i])
		}

		// Compute checksum and reset histogram for next iteration
		h = offset
		j := 0
		for j+3 < intBuckets {
			v0 := hist[j]
			v1 := hist[j+1]
			v2 := hist[j+2]
			v3 := hist[j+3]

			h ^= v0 + 1
			h *= prime
			h ^= v1 + 1
			h *= prime
			h ^= v2 + 1
			h *= prime
			h ^= v3 + 1
			h *= prime

			hist[j] = 0
			hist[j+1] = 0
			hist[j+2] = 0
			hist[j+3] = 0
			j += 4
		}
		for ; j < intBuckets; j++ {
			v := hist[j]
			h ^= v + 1
			h *= prime
			hist[j] = 0
		}
	}
	return h
}
