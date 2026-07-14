package main

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	n := len(data)
	for i := 0; i < n; i += 4 {
		hash ^= uint64(data[i])
		hash *= 1099511628211
		hash ^= uint64(data[i+1])
		hash *= 1099511628211
		hash ^= uint64(data[i+2])
		hash *= 1099511628211
		hash ^= uint64(data[i+3])
		hash *= 1099511628211
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	totalBytes := pixels * 4
	out := make([]byte, totalBytes)

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i << 2
			a := src[base+3]
			invA := 255 - a

			// R
			sR := src[base]
			dR := dst[base]
			out[base] = uint8((uint32(sR)*uint32(a) + uint32(dR)*uint32(invA) + 127) / 255)

			// G
			sG := src[base+1]
			dG := dst[base+1]
			out[base+1] = uint8((uint32(sG)*uint32(a) + uint32(dG)*uint32(invA) + 127) / 255)

			// B
			sB := src[base+2]
			dB := dst[base+2]
			out[base+2] = uint8((uint32(sB)*uint32(a) + uint32(dB)*uint32(invA) + 127) / 255)

			// Alpha
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}