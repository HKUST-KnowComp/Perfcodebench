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
	pixels := width * height
	out := make([]byte, pixels*4)
	const offsetFNV = uint64(1469598103934665603)
	const primeFNV = uint64(1099511628211)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = offsetFNV
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			inv := 255 - a

			// red
			s := uint32(src[base])
			d := uint32(dst[base])
			val := (s*uint32(a) + d*uint32(inv) + 127) / 255
			out[base] = byte(val)
			hash ^= uint64(val)
			hash *= primeFNV

			// green
			s = uint32(src[base+1])
			d = uint32(dst[base+1])
			val = (s*uint32(a) + d*uint32(inv) + 127) / 255
			out[base+1] = byte(val)
			hash ^= uint64(val)
			hash *= primeFNV

			// blue
			s = uint32(src[base+2])
			d = uint32(dst[base+2])
			val = (s*uint32(a) + d*uint32(inv) + 127) / 255
			out[base+2] = byte(val)
			hash ^= uint64(val)
			hash *= primeFNV

			// alpha (always 255)
			out[base+3] = 255
			hash ^= 255
			hash *= primeFNV
		}
	}
	return hash
}
