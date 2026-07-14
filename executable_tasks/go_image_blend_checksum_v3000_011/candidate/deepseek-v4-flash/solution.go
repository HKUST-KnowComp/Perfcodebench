package main

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	const offset = 1469598103934665603
	const prime = 1099511628211

	pixels := width * height
	var lastHash uint64
	// Local references for better bounds check elimination
	s := src
	d := dst

	for iter := 0; iter < iters; iter++ {
		h := offset
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := s[base+3]
			na := 255 - a
			r := (uint32(s[base])*uint32(a) + uint32(d[base])*uint32(na) + 127) / 255
			g := (uint32(s[base+1])*uint32(a) + uint32(d[base+1])*uint32(na) + 127) / 255
			b := (uint32(s[base+2])*uint32(a) + uint32(d[base+2])*uint32(na) + 127) / 255

			h ^= uint64(r)
			h *= prime
			h ^= uint64(g)
			h *= prime
			h ^= uint64(b)
			h *= prime
			h ^= uint64(255)
			h *= prime
		}
		lastHash = h
	}
	return lastHash
}
