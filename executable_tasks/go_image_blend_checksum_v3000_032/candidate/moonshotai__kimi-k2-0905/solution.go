package main

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

func solutionChecksumBytes(data []byte) uint64 {
	var h uint64 = 1469598103934665603
	for _, b := range data {
		h ^= uint64(b)
		h *= 1099511628211
	}
	return h
}

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		j := 0
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			o1 := solutionBlendByte(src[base], dst[base], a)
			o2 := solutionBlendByte(src[base+1], dst[base+1], a)
			o3 := solutionBlendByte(src[base+2], dst[base+2], a)
			out[j] = o1
			out[j+1] = o2
			out[j+2] = o3
			out[j+3] = 255
			j += 4
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}