package messagerouterlib;

import java.io.IOException;
import java.net.Socket;
import java.net.*;

public class Session implements MessageReceiverListener {
  private static final int PORT = 6667;

  Socket connection;
  String serverName;
  String userName;
  private int msgCount = 0;
  MessageReceiver receiver ;

  public Session() {
  }

  /**
   * connect
   *
   * @param serverName String
   */
  public boolean connect(String serverName, String userName) {
    this.userName = userName;
    this.serverName = serverName;

    try {
      connection = new Socket(serverName, PORT);
      receiver = new MessageReceiver(connection, userName);
      Message reply = this.sendMessage("", "connect", userName);
      if ((reply != null) && (reply.body.equals("connected"))) {
        return true;
      }
    }
    catch (UnknownHostException ex) {
    }
    catch (IOException ex) {
    }
    return false;
  }

  /**
   * synchronous mechanism for sending mechanims
   *
   * @param toUser String
   * @param subject String
   * @param body String
   */
  public Message sendMessage(String toUser, String subject, String body) {
    String thread = String.valueOf(msgCount++);
    send(userName, toUser, subject, thread, body);
    try {
      Message msg = receiver.waitForMessage(this, thread);
      return msg;
    }
    catch (MessageException ex) {
      System.out.println("Error waiting for message: " + ex.getDescription());
    }
    return null;
  }

  /**
   * postMessage
   *
   * @param toUser String
   * @param subject String
   * @param body String
   */
  public void postMessage(String toUser, String subject, String thread, String body) {
    send(userName, toUser, subject, body, thread);
  }

  private void send(String from, String to, String subject, String thread, String body) {
    byte[] header = new byte[8];
    header[0] = (byte) (to.length() & 0xff);
    header[1] = (byte) (from.length() & 0xff);
    header[2] = (byte) (thread.length() & 0xff);
    header[3] = (byte) (subject.length() & 0xff);
    int bodyLength = body.length();
    header[4] = (byte) (bodyLength & 0xff000000);
    header[5] = (byte) (bodyLength & 0x00ff0000);
    header[6] = (byte) (bodyLength & 0x0000ff00);
    header[7] = (byte) (bodyLength & 0x000000ff);

    byte[] msg = new byte[8 + to.length() + from.length() + subject.length() +
        body.length()];
    int index = 0;
    System.arraycopy(header, 0, msg, 0, 8);
    index = 8;
    if (to.length() > 0) {
      System.arraycopy(to.getBytes(), 0, msg, index, to.length());
      index += to.length();
    }
    if (from.length() > 0) {
      System.arraycopy(from.getBytes(), 0, msg, index, from.length());
      index += from.length();
    }
    if (thread.length() > 0) {
      System.arraycopy(thread.getBytes(), 0, msg, index, thread.length());
      index += thread.length();
    }
    if (subject.length() > 0) {
      System.arraycopy(subject.getBytes(), 0, msg, index, subject.length());
      index += subject.length();
    }
    if (body.length() > 0) {
      System.arraycopy(body.getBytes(), 0, msg, index, body.length());
    }

    try {
      connection.getOutputStream().write(msg);
    }
    catch (IOException ex) {
    }
  }
}
