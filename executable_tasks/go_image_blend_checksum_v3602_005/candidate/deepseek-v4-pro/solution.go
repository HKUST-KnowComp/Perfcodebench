package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	var hash uint64 = 1469598103934665603
	for i := 0; i < pixels; i++ {
		base := i * 4
		a := src[base+3]
		// blend red
		r := uint16(src[base])*uint16(a) + uint16(dst[base])*uint16(255-a) + 127
		hash ^= uint64(r / 255)
		hash *= 1099511628211
		// blend green
		g := uint16(src[base+1])*uint16(a) + uint16(dst[base+1])*uint16(255-a) + 127
		hash ^= uint64(g / 255)
		hash *= 1099511628211
		// blend blue
		b := uint16(src[base+2])*uint16(a) + uint16(dst[base+2])*uint16(255-a) + 127
		hash ^= uint64(b / 255)
		hash *= 1099511628211
		// alpha channel
		hash ^= 255
		hash *= 1099511628211
	}
	return hash
}
