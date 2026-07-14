package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			r := uint8((uint32(src[base]) * uint32(a) + uint32(dst[base]) * uint32(255 - a) + 127) / 255)
			g := uint8((uint32(src[base+1]) * uint32(a) + uint32(dst[base+1]) * uint32(255 - a) + 127) / 255)
			b := uint8((uint32(src[base+2]) * uint32(a) + uint32(dst[base+2]) * uint32(255 - a) + 127) / 255)
			hash ^= uint64(r)
			hash *= 1099511628211
			hash ^= uint64(g)
			hash *= 1099511628211
			hash ^= uint64(b)
			hash *= 1099511628211
			hash ^= 255
			hash *= 1099511628211
		}
	}
	return hash
}