package messagerouterlib;

public class Message {
  protected String from, to, thread, subject, body;

  public Message() {
  }

  public String getFrom() {
    return from;
  }

  public String getTo() {
    return to;
  }

  public String getThread() {
    return thread;
  }

  public String getSubject() {
    return subject;
  }

  public String getBody() {
    return body;
  }

  public void setFrom(String s) {
    from = s;
  }

  public void setTo(String s) {
    to = s;
  }

  public void setThread(String s) {
    thread = s;
  }

  public void setSubject(String s) {
    subject = s;
  }

  public void setBody(String s) {
    body = s;
  }

  public String toString() {
    return from + ":" + to + ":" + thread + ":"+subject + ":" + body;
  }
}



