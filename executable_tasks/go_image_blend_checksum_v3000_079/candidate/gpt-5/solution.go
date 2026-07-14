package main

func run(src, dst []byte, width, height, iters int) uint64 {
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	pixels := width * height
	n := pixels << 2
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := offset
		for base := 0; base < n; base += 4 {
			a := uint32(src[base+3])
			inv := 255 - a

			s0 := uint32(src[base])
			d0 := uint32(dst[base])
			v := uint8((s0*a + d0*inv + 127) / 255)
			h ^= uint64(v)
			h *= prime

			s1 := uint32(src[base+1])
			d1 := uint32(dst[base+1])
			v = uint8((s1*a + d1*inv + 127) / 255)
			h ^= uint64(v)
			h *= prime

			s2 := uint32(src[base+2])
			d2 := uint32(dst[base+2])
			v = uint8((s2*a + d2*inv + 127) / 255)
			h ^= uint64(v)
			h *= prime

			// Alpha channel is opaque (255)
			h ^= 255
			h *= prime
		}
		hash = h
	}
	return hash
}
