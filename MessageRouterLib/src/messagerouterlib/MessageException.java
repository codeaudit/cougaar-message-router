package messagerouterlib;

public class MessageException extends Exception {
  private String description;

  public MessageException() {
  }
  public MessageException(String msg) {
    this.description = msg;
  }

  public String getDescription() {
    return description;
  }
}
