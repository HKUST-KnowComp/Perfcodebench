package main

func run(src, dst []byte, width, height, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	hash := uint64(1469598103934665603)
	pixels := width * height
	for i := 0; i < pixels; i++ {
		base := i << 2
		a := uint32(src[base+3])
		da := 255 - a

		r := (uint32(src[base])*a + uint32(dst[base])*da + 127) / 255
		hash ^= uint64(r)
		hash *= 1099511628211

		g := (uint32(src[base+1])*a + uint32(dst[base+1])*da + 127) / 255
		hash ^= uint64(g)
		hash *= 1099511628211

		b := (uint32(src[base+2])*a + uint32(dst[base+2])*da + 127) / 255
		hash ^= uint64(b)
		hash *= 1099511628211

		hash ^= uint64(255)
		hash *= 1099511628211
	}
	return hash
}
