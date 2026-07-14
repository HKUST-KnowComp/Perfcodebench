package main

const fnv64Offset = 1469598103934665603
const fnv64Prime = 1099511628211

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = fnv64Offset
	for _, b := range data {
		hash ^= uint64(b)
		hash *= fnv64Prime
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	var hash uint64
	n := len(ids)
	if n > 0 {
		// Help the compiler eliminate bounds checks inside the loop
		_ = ids[n-1]
		_ = ts[n-1]
		_ = levels[n-1]
		_ = lens[n-1]
	}
	for iter := 0; iter < iters; iter++ {
		h := uint64(fnv64Offset)
		for i := 0; i < n; i++ {
			v := ids[i]
			h ^= uint64(byte(v))
			h *= fnv64Prime
			h ^= uint64(byte(v >> 8))
			h *= fnv64Prime
			h ^= uint64(byte(v >> 16))
			h *= fnv64Prime
			h ^= uint64(byte(v >> 24))
			h *= fnv64Prime

			w := ts[i]
			h ^= uint64(byte(w))
			h *= fnv64Prime
			h ^= uint64(byte(w >> 8))
			h *= fnv64Prime
			h ^= uint64(byte(w >> 16))
			h *= fnv64Prime
			h ^= uint64(byte(w >> 24))
			h *= fnv64Prime

			lv := levels[i]
			h ^= uint64(byte(lv))
			h *= fnv64Prime
			h ^= uint64(byte(lv >> 8))
			h *= fnv64Prime

			ln := lens[i]
			h ^= uint64(byte(ln))
			h *= fnv64Prime
			h ^= uint64(byte(ln >> 8))
			h *= fnv64Prime
		}
		hash = h
	}
	return hash
}
