import java.util.*; import com.google.gson.Gson; import com.google.gson.JsonElement; import com.google.gson.JsonObject;

public class Solution {
  private static final Gson GSON = new Gson();

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      JsonElement jsonElement = GSON.fromJson(row, JsonElement.class);
      JsonObject jsonObject = jsonElement.getAsJsonObject();
      int u = jsonObject.get("u").getAsInt();
      int s = jsonObject.get("s").getAsInt();
      boolean f = jsonObject.get("f").getAsBoolean();
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}