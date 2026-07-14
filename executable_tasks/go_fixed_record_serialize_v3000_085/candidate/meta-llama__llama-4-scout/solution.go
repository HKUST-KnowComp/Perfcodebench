package main
import (
    "bytes"
)

func solutionChecksumBytes(data []byte) uint64 {
    var hash uint64 = 1469598103934665603
    for _, b := range data {
        hash ^= uint64(b)
        hash *= 1099511628211
    }
    return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
    var buf bytes.Buffer
    var hash uint64
    for iter := 0; iter < iters; iter++ {
        buf.Reset()
        for i := range ids {
            buf.WriteByte(byte(ids[i]));
            buf.WriteByte(byte(ids[i] >> 8));
            buf.WriteByte(byte(ids[i] >> 16));
            buf.WriteByte(byte(ids[i] >> 24));
            buf.WriteByte(byte(ts[i]));
            buf.WriteByte(byte(ts[i] >> 8));
            buf.WriteByte(byte(ts[i] >> 16));
            buf.WriteByte(byte(ts[i] >> 24));
            buf.WriteByte(byte(levels[i]));
            buf.WriteByte(byte(levels[i] >> 8));
            buf.WriteByte(byte(lens[i]));
            buf.WriteByte(byte(lens[i] >> 8));
        }
        hash = solutionChecksumBytes(buf.Bytes())
    }
    return hash
}