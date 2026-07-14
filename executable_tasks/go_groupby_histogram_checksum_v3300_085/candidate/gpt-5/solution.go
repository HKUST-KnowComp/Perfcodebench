package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}

	// Reuse a single histogram across iterations and zero it during hashing.
	hist := make([]uint64, buckets)
	n := len(keys)

	for iter := 0; iter < iters; iter++ {
		// Accumulate values into histogram (unrolled by 4).
		if n > 0 {
			_ = keys[n-1]
			_ = vals[n-1]
			if buckets > 0 {
				_ = hist[buckets-1]
			}
		}
		i := 0
		for i+3 < n {
			k0 := keys[i+0]
			k1 := keys[i+1]
			k2 := keys[i+2]
			k3 := keys[i+3]
			hist[k0] += uint64(vals[i+0])
			hist[k1] += uint64(vals[i+1])
			hist[k2] += uint64(vals[i+2])
			hist[k3] += uint64(vals[i+3])
			i += 4
		}
		for i < n {
			k := keys[i]
			hist[k] += uint64(vals[i])
			i++
		}

		// Compute checksum and zero histogram for next iteration (unrolled by 4).
		h := uint64(1469598103934665603)
		m := int(buckets)
		j := 0
		for j+3 < m {
			v0 := hist[j+0]
			v1 := hist[j+1]
			v2 := hist[j+2]
			v3 := hist[j+3]
			h ^= v0 + 1
			h *= 1099511628211
			h ^= v1 + 1
			h *= 1099511628211
			h ^= v2 + 1
			h *= 1099511628211
			h ^= v3 + 1
			h *= 1099511628211
			hist[j+0] = 0
			hist[j+1] = 0
			hist[j+2] = 0
			hist[j+3] = 0
			j += 4
		}
		for j < m {
			v := hist[j]
			h ^= v + 1
			h *= 1099511628211
			hist[j] = 0
			j++
		}

		if iter == iters-1 {
			return h
		}
	}
	return 0
}
