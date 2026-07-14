import java.util.PriorityQueue;

public class Solution {
  static class Pair implements Comparable<Pair> {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
    @Override public int compareTo(Pair other) {
      if (score != other.score) return Integer.compare(other.score, score);
      return Integer.compare(index, other.index);
    }
  }

  public static long run(int[] scores, int k, int iters) {
    PriorityQueue<Pair> pq = new PriorityQueue<>();
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      pq.clear();
      for (int i = 0; i < scores.length; i++) {
        if (pq.size() < k) {
          pq.add(new Pair(scores[i], i));
        } else if (pq.peek().score < scores[i]) {
          pq.poll();
          pq.add(new Pair(scores[i], i));
        }
      }
      Object[] array = pq.toArray();
      java.util.Arrays.sort(array, (a, b) -> ((Pair) a).score == ((Pair) b).score ? Integer.compare(((Pair) a).index, ((Pair) b).index) : Integer.compare(((Pair) b).score, ((Pair) a).score));
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) ((Pair)array[i]).score) << 32) ^ (((Pair)array[i]).index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}