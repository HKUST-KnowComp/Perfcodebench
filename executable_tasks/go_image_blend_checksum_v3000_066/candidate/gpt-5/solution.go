package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	n := pixels * 4
	var final uint64
	const (
		fnvOffset uint64 = 1469598103934665603
		fnvPrime  uint64 = 1099511628211
	)
	// Bounds-check elimination hints
	if n > 0 {
		_ = src[n-1]
		_ = dst[n-1]
	}
	for iter := 0; iter < iters; iter++ {
		h := fnvOffset
		for base := 0; base < n; base += 4 {
			// Read alpha once and reuse for RGB channels
			a := uint32(src[base+3])
			inv := uint32(255) - a

			// R
			s := uint32(src[base+0])
			d := uint32(dst[base+0])
			v := (s*a + d*inv + 127) / 255
			h ^= uint64(uint8(v))
			h *= fnvPrime

			// G
			s = uint32(src[base+1])
			d = uint32(dst[base+1])
			v = (s*a + d*inv + 127) / 255
			h ^= uint64(uint8(v))
			h *= fnvPrime

			// B
			s = uint32(src[base+2])
			d = uint32(dst[base+2])
			v = (s*a + d*inv + 127) / 255
			h ^= uint64(uint8(v))
			h *= fnvPrime

			// A = 255
			h ^= 255
			h *= fnvPrime
		}
		final = h
	}
	return final
}
