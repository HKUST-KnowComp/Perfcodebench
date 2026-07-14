package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const offset = 1469598103934665603
	const prime = 1099511628211
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := offset
		for i := 0; i < len(ids); i++ {
			// id (4 bytes, little-endian)
			b := byte(ids[i])
			h ^= uint64(b)
			h *= prime
			b = byte(ids[i] >> 8)
			h ^= uint64(b)
			h *= prime
			b = byte(ids[i] >> 16)
			h ^= uint64(b)
			h *= prime
			b = byte(ids[i] >> 24)
			h ^= uint64(b)
			h *= prime

			// ts (4 bytes, little-endian)
			b = byte(ts[i])
			h ^= uint64(b)
			h *= prime
			b = byte(ts[i] >> 8)
			h ^= uint64(b)
			h *= prime
			b = byte(ts[i] >> 16)
			h ^= uint64(b)
			h *= prime
			b = byte(ts[i] >> 24)
			h ^= uint64(b)
			h *= prime

			// level (2 bytes, little-endian)
			b = byte(levels[i])
			h ^= uint64(b)
			h *= prime
			b = byte(levels[i] >> 8)
			h ^= uint64(b)
			h *= prime

			// len (2 bytes, little-endian)
			b = byte(lens[i])
			h ^= uint64(b)
			h *= prime
			b = byte(lens[i] >> 8)
			h ^= uint64(b)
			h *= prime
		}
		hash = h
	}
	return hash
}
