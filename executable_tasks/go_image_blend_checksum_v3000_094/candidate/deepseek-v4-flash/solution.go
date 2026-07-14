package main

func run(src, dst []byte, width, height, iters int) uint64 {
	if iters == 0 {
		return 0
	}
	const offset64 = 1469598103934665603
	const prime64 = 1099511628211
	hash := offset64
	pixels := width * height
	for i := 0; i < pixels; i++ {
		base := i * 4
		a := uint32(src[base+3])
		// red
		s := uint32(src[base])
		d := uint32(dst[base])
		r := uint8((s*a + d*(255-a) + 127) / 255)
		hash ^= uint64(r)
		hash *= prime64
		// green
		s = uint32(src[base+1])
		d = uint32(dst[base+1])
		g := uint8((s*a + d*(255-a) + 127) / 255)
		hash ^= uint64(g)
		hash *= prime64
		// blue
		s = uint32(src[base+2])
		d = uint32(dst[base+2])
		b := uint8((s*a + d*(255-a) + 127) / 255)
		hash ^= uint64(b)
		hash *= prime64
		// alpha always 255
		hash ^= uint64(255)
		hash *= prime64
	}
	return hash
}
