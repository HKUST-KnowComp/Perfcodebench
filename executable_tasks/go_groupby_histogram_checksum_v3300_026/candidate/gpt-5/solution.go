package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// Preallocate histogram once and reuse across iterations
	hb := int(buckets)
	hist := make([]uint64, hb)

	// Use the minimum length to keep bounds checks minimal and safe
	n := len(keys)
	if len(vals) < n {
		n = len(vals)
	}
	ks := keys[:n]
	vs := vals[:n]

	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Accumulate values into histogram
		i := 0
		for i+4 <= n {
			k0, v0 := ks[i], vs[i]
			k1, v1 := ks[i+1], vs[i+1]
			k2, v2 := ks[i+2], vs[i+2]
			k3, v3 := ks[i+3], vs[i+3]
			hist[k0] += uint64(v0)
			hist[k1] += uint64(v1)
			hist[k2] += uint64(v2)
			hist[k3] += uint64(v3)
			i += 4
		}
		for i < n {
			hist[ks[i]] += uint64(vs[i])
			i++
		}

		// Hash the histogram and zero it in the same pass for next iteration
		h = offset
		j := 0
		end4 := hb - (hb % 4)
		for j < end4 {
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
		for j < hb {
			v := hist[j]
			h ^= v + 1
			h *= prime
			hist[j] = 0
			j++
		}
	}
	return h
}
