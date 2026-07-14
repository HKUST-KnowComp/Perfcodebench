package main

const (
	fnvOffset64 uint64 = 1469598103934665603
	fnvPrime64  uint64 = 1099511628211
)

func solutionChecksumBytes(data []byte) uint64 {
	h := fnvOffset64
	for i := 0; i < len(data); i++ {
		h ^= uint64(data[i])
		h *= fnvPrime64
	}
	return h
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	var hash uint64
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		h := fnvOffset64
		for i := 0; i < n; i++ {
			// ids[i] (uint32) little-endian
			x := uint64(ids[i])
			h ^= x & 0xff
			h *= fnvPrime64
			h ^= (x >> 8) & 0xff
			h *= fnvPrime64
			h ^= (x >> 16) & 0xff
			h *= fnvPrime64
			h ^= (x >> 24) & 0xff
			h *= fnvPrime64

			// ts[i] (uint32) little-endian
			x = uint64(ts[i])
			h ^= x & 0xff
			h *= fnvPrime64
			h ^= (x >> 8) & 0xff
			h *= fnvPrime64
			h ^= (x >> 16) & 0xff
			h *= fnvPrime64
			h ^= (x >> 24) & 0xff
			h *= fnvPrime64

			// levels[i] (uint16) little-endian
			y := uint64(levels[i])
			h ^= y & 0xff
			h *= fnvPrime64
			h ^= (y >> 8) & 0xff
			h *= fnvPrime64

			// lens[i] (uint16) little-endian
			y = uint64(lens[i])
			h ^= y & 0xff
			h *= fnvPrime64
			h ^= (y >> 8) & 0xff
			h *= fnvPrime64
		}
		hash = h
	}
	return hash
}
