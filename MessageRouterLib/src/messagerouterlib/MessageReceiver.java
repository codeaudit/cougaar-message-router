package messagerouterlib;

import java.net.Socket;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.net.SocketException;

public class MessageReceiver extends Thread {
  private static final long MESSAGE_TIMEOUT = 60000;  //60 seconds
  private static final String MSG_PING = "command[help]";
  private static final String MSG_PONG = "here";

  private boolean keepRunning = true;
  Socket s;
  String name;
  HashMap syncListenerMap = new HashMap();  //map for synchronous msg listener
  HashMap syncMessageMap = new HashMap();   //map for synchronous messages
  ArrayList asyncListeners = new ArrayList();  //array for asynchronous listeners

  public MessageReceiver(Socket s, String name) {
    this.s = s;
    this.name = name;
  }

  public void addAsyncListener(AsyncMessageReceiverListener listener) {
    asyncListeners.add(listener);
  }

  public void removeAsyncListener(AsyncMessageReceiverListener listener) {
    asyncListeners.remove(listener);
  }

  public Message waitForMessage(SyncMessageReceiverListener listener,
                                String threadId) throws MessageException {
    return waitForMessage(listener, threadId, MESSAGE_TIMEOUT);
  }

  public Message waitForMessage(SyncMessageReceiverListener listener,
                                String threadId, long timeout) throws MessageException {

    //check to see if the message is already in the message map
    Message msg = (Message)syncMessageMap.get(threadId);
    if (msg != null) {
      syncMessageMap.remove(threadId);
      return msg;
    }

    //otherwise wait for the message
    syncListenerMap.put(threadId, listener);
    synchronized (listener) {
      try {
        listener.wait(timeout);
        msg = (Message) syncMessageMap.get(threadId);
        if (msg != null) {
          syncMessageMap.remove(threadId);
          syncListenerMap.remove(threadId);
          return msg;
        }
        syncListenerMap.remove(threadId);
        throw new MessageException("Timeout occurred");
      }
      catch (InterruptedException ex) {
        syncListenerMap.remove(threadId);
        throw new MessageException(ex.getMessage());
      }
    }
  }


  private void notifyListeners(Message msg) {
    if (syncListenerMap.containsKey(msg.thread)) {
      syncMessageMap.put(msg.thread, msg); //place the message in the message map
      SyncMessageReceiverListener listener = (SyncMessageReceiverListener)syncListenerMap.get(msg.thread);
      synchronized(listener) {
        listener.notifyAll();  //notify the waiting thread that the message is ready
      }
    }
    else {
      for (Iterator i = asyncListeners.iterator(); i.hasNext(); ) {
        ((AsyncMessageReceiverListener)i.next()).receiveMsg(msg);
      }
    }
  }

  public void notifyListnersOfClosure() {
    //notify async message listeners
    for (Iterator i = asyncListeners.iterator(); i.hasNext();) {
      ((AsyncMessageReceiverListener)i.next()).messageReceiverClosed(this);
    }

    //notify sync message listeners
    for (Iterator i = syncListenerMap.values().iterator(); i.hasNext(); ) {
      ((SyncMessageReceiverListener)i.next()).messageReceiverClosed(this);
    }
  }

  public void run() {
    byte[] header = new byte[8];
    while (keepRunning) {
      try {
        if (!s.isConnected()) return;  //exit the thread if the socket isn't connected
        int count = 0;
        int index = 0;
        while (count >= 0 && index < 8) {
          count = s.getInputStream().read(header, index, 8 - index);
          index += count;
        }
        if (count < 0)  {  //exit the thread if -1 is returned
          notifyListnersOfClosure();
          return;
        }

        //System.out.println(name + "=>header count: " + index);
        int toLength = header[0] & 0xff;
        int fromLength = header[1] & 0xff;
        int threadLength = header[2] & 0xff;
        int subjectLength = header[3] & 0xff;
        int bodyLength = 0;
        bodyLength += ((header[4] & 0xff)<<24);
        bodyLength += ((header[5] & 0xff)<<16);
        bodyLength += ((header[6] & 0xff)<<8);
        bodyLength += header[7] & 0xff;

        int totalLength = toLength + fromLength + threadLength + subjectLength +
            bodyLength;
        //System.out.println(name + "=>totalSize: " + totalLength);
        byte[] data = new byte[totalLength];
        count = 0;
        index = 0;
        while (count >= 0 && index < totalLength) {
          count = s.getInputStream().read(data, index, totalLength - index);
          index += count;
        }
        if (count < 0) {  //exit thread if -1 returned
          notifyListnersOfClosure();
          return;
        }

        //construct the message object from the data stream
        Message msg = new Message();
        index = 0;
        if (toLength != 0) {
          msg.to = new String(data, index, toLength);
          index+=toLength;
        }
        if (fromLength != 0) {
          msg.from = new String(data, index, fromLength);
          index+=fromLength;
        }
        if (threadLength != 0) {
          msg.thread = new String(data, index, threadLength);
          index+=threadLength;
        }
        if (subjectLength != 0) {
          msg.subject = new String(data, index, subjectLength);
          index+=subjectLength;
        }
        if (bodyLength != 0) {
          msg.body = new String(data, index, bodyLength);
        }

        //if this is a PING message then we we to respond to it
        if (msg.body != null && msg.body.equals(MSG_PING)) {
          handlePing(msg);
        }
        else {  //otherwise just notify any registered listeners
          notifyListeners(msg);
        }

      }
      catch (SocketException se) {
        //System.out.println("Socket exception in Receiver");
        break;  // break out of the loop
      }
      catch (IOException ex) {
        ex.printStackTrace();
      }

    }
  }

  private void handlePing(Message msg) {
    System.out.println("received ping request using thread: " + msg.thread);
    if (Session.currentSession != null && Session.currentSession.isConnected()) {
      Session.currentSession.postMessage("", "", msg.thread, MSG_PONG);
    }
  }


}
