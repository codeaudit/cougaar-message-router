package messagerouterlib;

import java.util.*;

public class Util {
  public Util() {
  }

  public static String randomString(int max) {
    String retStr = "";
    long value = Math.round((Math.random()* max));
    for (long l = 0; l<value; l++) {
      retStr += "a";
    }
    return retStr;
  }

  public static String fixedString(int size) {
    char[] tmp = new char[size];
    for (int i=0; i<size; i++) {
      tmp[i] += 'f';
    }
    return String.valueOf(tmp);
  }
}
