package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < pixels; i++ {
			idx := i << 2
			a := src[idx+3]
			ia := uint32(255 - a)
			ua := uint32(a)

			r := uint32(src[idx])
			g := uint32(src[idx+1])
			b := uint32(src[idx+2])

			dr := uint32(dst[idx])
			dg := uint32(dst[idx+1])
			db := uint32(dst[idx+2])

			hash ^= uint64(uint8((r*ua + dr*ia + 127) / 255))
			hash *= 1099511628211
			hash ^= uint64(uint8((g*ua + dg*ia + 127) / 255))
			hash *= 1099511628211
			hash ^= uint64(uint8((b*ua + db*ia + 127) / 255))
			hash *= 1099511628211
			hash ^= 255
			hash *= 1099511628211
		}
	}
	return hash
}