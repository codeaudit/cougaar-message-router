package messagerouterlib;

public interface AsyncMessageReceiverListener {
  public void receiveMsg(Message msg);
  public void messageReceiverClosed(MessageReceiver mr);
}
