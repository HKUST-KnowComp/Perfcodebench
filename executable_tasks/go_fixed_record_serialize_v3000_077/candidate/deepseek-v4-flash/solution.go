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
	if iters == 0 {
		return 0
	}
	const offset64 uint64 = 1469598103934665603
	const prime64 uint64 = 1099511628211
	var hash uint64 = offset64
	for i := range ids {
		// ids[i] little-endian (4 bytes)
		hash ^= uint64(ids[i] & 0xff)
		hash *= prime64
		hash ^= uint64((ids[i] >> 8) & 0xff)
		hash *= prime64
		hash ^= uint64((ids[i] >> 16) & 0xff)
		hash *= prime64
		hash ^= uint64((ids[i] >> 24) & 0xff)
		hash *= prime64

		// ts[i] little-endian (4 bytes)
		hash ^= uint64(ts[i] & 0xff)
		hash *= prime64
		hash ^= uint64((ts[i] >> 8) & 0xff)
		hash *= prime64
		hash ^= uint64((ts[i] >> 16) & 0xff)
		hash *= prime64
		hash ^= uint64((ts[i] >> 24) & 0xff)
		hash *= prime64

		// levels[i] little-endian (2 bytes)
		hash ^= uint64(levels[i] & 0xff)
		hash *= prime64
		hash ^= uint64((levels[i] >> 8) & 0xff)
		hash *= prime64

		// lens[i] little-endian (2 bytes)
		hash ^= uint64(lens[i] & 0xff)
		hash *= prime64
		hash ^= uint64((lens[i] >> 8) & 0xff)
		hash *= prime64
	}
	return hash
}
