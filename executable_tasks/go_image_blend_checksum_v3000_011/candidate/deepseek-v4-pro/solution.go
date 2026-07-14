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
	srcIdx, dstIdx, outIdx := 0, 0, 0
	for i := 0; i < pixels; i++ {
		a := src[srcIdx+3]
		out[outIdx] = solutionBlendByte(src[srcIdx], dst[dstIdx], a)
		out[outIdx+1] = solutionBlendByte(src[srcIdx+1], dst[dstIdx+1], a)
		out[outIdx+2] = solutionBlendByte(src[srcIdx+2], dst[dstIdx+2], a)
		out[outIdx+3] = 255
		srcIdx += 4
		dstIdx += 4
		outIdx += 4
	}
	return solutionChecksumBytes(out)
}
