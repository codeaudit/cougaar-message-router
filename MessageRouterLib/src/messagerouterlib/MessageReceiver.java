package messagerouterlib;

import java.net.Socket;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.net.SocketException;

public class MessageReceiver extends Thread {
  private static final long MESSAGE_TIMEOUT = 60000;  //60 seconds
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
      syncMessageMap.remove(msg);
      return msg;
    }

    //otherwise wait for the message
    syncListenerMap.put(threadId, listener);
    synchronized (listener) {
      try {
        listener.wait(timeout);
        msg = (Message) syncMessageMap.get(threadId);
        if (msg != null) {
          syncMessageMap.remove(msg);
          return msg;
        }
        throw new MessageException("Timeout occurred");
      }
      catch (InterruptedException ex) {
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

  public void run() {
    byte[] header = new byte[8];
    while (keepRunning) {
      try {
        int count = 0;
        int index = 0;
        while (index < 8) {
          count = s.getInputStream().read(header, index, 8 - index);
          index += count;
        }
        //System.out.println(name + "=>header count: " + index);
        int toLength = header[0] & 0xff;
        int fromLength = header[1] & 0xff;
        int threadLength = header[2] & 0xff;
        int subjectLength = header[3] & 0xff;
        int bodyLength = 0;
        bodyLength = header[4] & 0xff;
        bodyLength <<= 8;
        bodyLength = header[5] & 0xff;
        bodyLength <<= 8;
        bodyLength = header[6] & 0xff;
        bodyLength <<= 8;
        bodyLength = header[7] & 0xff;

        int totalLength = toLength + fromLength + threadLength + subjectLength +
            bodyLength;
        //System.out.println(name + "=>totalSize: " + totalLength);
        byte[] data = new byte[totalLength];
        count = 0;
        index = 0;
        while (index < totalLength) {
          count = s.getInputStream().read(data, index, totalLength - index);
          index += count;
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

        notifyListeners(msg);

      }
      catch (SocketException se) {
        System.out.println("Socket exception in Receiver");
        break;  // break out of the loop
      }
      catch (IOException ex) {
        ex.printStackTrace();
      }

    }
  }

}
