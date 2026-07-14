package main

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	// Stream the FNV-1a checksum directly over the serialized bytes
	// to avoid allocating and populating an intermediate buffer.
	const offset64 uint64 = 1469598103934665603
	const prime64 uint64 = 1099511628211

	var hash uint64
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		h := offset64
		for i := 0; i < n; i++ {
			v := ids[i]
			h ^= uint64(byte(v))
			h *= prime64
			h ^= uint64(byte(v >> 8))
			h *= prime64
			h ^= uint64(byte(v >> 16))
			h *= prime64
			h ^= uint64(byte(v >> 24))
			h *= prime64

			v2 := ts[i]
			h ^= uint64(byte(v2))
			h *= prime64
			h ^= uint64(byte(v2 >> 8))
			h *= prime64
			h ^= uint64(byte(v2 >> 16))
			h *= prime64
			h ^= uint64(byte(v2 >> 24))
			h *= prime64

			l1 := levels[i]
			h ^= uint64(byte(l1))
			h *= prime64
			h ^= uint64(byte(l1 >> 8))
			h *= prime64

			l2 := lens[i]
			h ^= uint64(byte(l2))
			h *= prime64
			h ^= uint64(byte(l2 >> 8))
			h *= prime64
		}
		hash = h
	}
	return hash
}
