package messagerouterlib;

public class ServerTest {
  public String server;

  public ServerTest(String server) {
    this.server = server;
  }

  public void testConnection() {
    Session session = new Session();
      try {
        session.connect(server, "test");
        System.out.println("connection test passed");
        session.disconnect();
      }
      catch (ConnectionException ex) {
        System.out.println("connection test failed");
      }
  }

  public void testListCmd() {
    Session session = new Session();
    try {
      session.connect(server, "test");
      Message retMsg = session.sendMessage("", "list", "");
      if (retMsg.getBody().equals("test\n")) {
        System.out.println("list command test passed");
      }
      else {
        System.out.println("list command test failed");
      }
      session.disconnect();
    }
    catch (ConnectionException ex) {
      System.out.println("Exception in list command");
    }
  }

  public void testRegisterCmd() {
    Session session1 = new Session();
    Session session2 = new Session();
    try {
      session1.connect(server, "test1");
      Message expectedMsg = new Message();
      expectedMsg.setSubject("online");
      expectedMsg.setBody("test2");
      Message retMsg = session1.sendMessage("", "register", "");
      if (!retMsg.getSubject().equals("registered")) {
        System.out.println("register command test failed - invalid repsonse");
        session1.disconnect();
        session2.disconnect();
        return;
      }
      CheckListener listener = new CheckListener("reigster command test", expectedMsg);
      session1.addListener(listener);
      session2.connect(server, "test2");
      try {
        Thread.currentThread().sleep(1000);
      }
      catch (InterruptedException ex1) {
      }
      session1.removeListener(listener);
      session1.disconnect();
      session2.disconnect();
    }
    catch (ConnectionException ex) {
      System.out.println("Exception in tet register command");
    }
  }

  public void testEavesdropCmd() {
    Session session1 = new Session();
    Session session2 = new Session();
    Session session3 = new Session();

    try {
      session1.connect(server, "test1");
      session2.connect(server, "test2");
      session3.connect(server, "test3");
      Message retMsg = session1.sendMessage("", "enable eavesdropping", "");
      if (!retMsg.getSubject().equals("eavesdropping enabled")) {
        System.out.println(
            "test eavesdrop command failed - could not enable eavesdropping");
        session1.disconnect();
        session2.disconnect();
        session3.disconnect();
        return;
      }
      retMsg = session1.sendMessage("", "eavesdrop", "test2");
      if (!retMsg.getSubject().equals("eavesdrop enabled")) {
        System.out.println("test eavesdrop command failed - invalid response");
        session1.disconnect();
        session2.disconnect();
        session3.disconnect();
        return;
      }
      Message expectedMsg = new Message();
      expectedMsg.setSubject("from test2 to test3");
      CheckListener cl = new CheckListener("eavesdrop command test", expectedMsg);
      session1.addListener(cl);
      session2.postMessage("test3", "from test2 to test3", "");
      try {
        Thread.currentThread().sleep(1000);
      }
      catch (InterruptedException ie) {
      }
      session1.removeListener(cl);
      session1.sendMessage("", "disable eavesdropping", "");
      session1.disconnect();
      session2.disconnect();
      session3.disconnect();
    }
    catch (ConnectionException ex) {
      System.out.println("Exception in test eavesdrop command");
    }
  }

  public void testGlobalEavesdropCmd() {
    Session session1 = new Session();
    Session session2 = new Session();
    Session session3 = new Session();

    try {
      session1.connect(server, "test1");
      session2.connect(server, "test2");
      session3.connect(server, "test3");
      Message retMsg = session1.sendMessage("", "enable eavesdropping", "");
      if (!retMsg.getSubject().equals("eavesdropping enabled")) {
        System.out.println("test eavesdrop command failed - could not enable eavesdropping");
        session1.disconnect();
        session2.disconnect();
        session3.disconnect();
        return;
      }

      retMsg = session1.sendMessage("", "globaleavesdrop", "");
      if (!retMsg.getSubject().equals("globaleavesdrop enabled")) {
        System.out.println(
            "test global eavesdrop command failed - invalid response");
        session1.disconnect();
        session2.disconnect();
        session3.disconnect();

        return;
      }
      Message expectedMsg = new Message();
      expectedMsg.setSubject("from test2 to test3");
      CheckListener cl = new CheckListener("global eavesdrop command test",
                                           expectedMsg);
      session1.addListener(cl);
      session2.postMessage("test3", "from test2 to test3", "");
      try {
        Thread.currentThread().sleep(1000);
      }
      catch (InterruptedException ie) {
      }
      session1.removeListener(cl);
      session1.sendMessage("", "disable eavesdropping", "");
      session1.disconnect();
      session2.disconnect();
      session3.disconnect();
    }
    catch (ConnectionException ex) {

    }
  }

  public void testEnableErrorMsgsCmd() {
    Session session = new Session();
    try {
      session.connect(server, "test1");
      Message retMsg = session.sendMessage("", "enable error messages", "");
      if (!retMsg.getSubject().equals("error messages enabled")) {
        System.out.println("Test Enable error messages failed - invalid response");
        session.disconnect();
        return;
      }
      retMsg = session.sendMessage("", "Bogus command", "");
      if (!retMsg.getSubject().equals("ERROR") || !retMsg.getBody().equals("Unknown command")) {
        System.out.println("Test Enable error messages failed - invalid error response");
        session.sendMessage("", "disable error messages", "");
        session.disconnect();
        return;
      }
      System.out.println("Enable error messages test passed");
      session.sendMessage("", "disable error messages", "");
      session.disconnect();
    }
    catch (ConnectionException ex) {
     System.out.println("Exception in test enable error messages command");
    }
  }

  public void testLoggingCmd() {
    Session session = new Session();
    try {
      session.connect(server, "test1");
      Message retMsg = session.sendMessage("", "enable logging", "");
      if (!retMsg.getSubject().equals("logging enabled")) {
        System.out.println("Test Logging failed - invalid response to enable logging");
        session.disconnect();
        return;
      }
      retMsg = session.sendMessage("", "disable logging", "");
      if (!retMsg.getSubject().equals("logging disabled")) {
        System.out.println("Test Logging failed - invalid response to disable logging");
        session.disconnect();
        return;
      }
      System.out.println("Logging test passed");
      session.disconnect();
    }
    catch (Exception ex) {
      System.out.println("Exception in test logging command");
    }
  }

  public void testLogLevelCmd() {
    Session session = new Session();
    try {
      session.connect(server, "test1");
      Message retMsg = session.sendMessage("", "enable logging", "");
       if (!retMsg.getSubject().equals("logging enabled")) {
         System.out.println("Test Log Level failed - invalid response to enable logging");
         session.disconnect();
         return;
       }
       retMsg = session.sendMessage("", "set log level info", "");
       if (!retMsg.getSubject().equals("log level set to INFO")) {
        System.out.println("Test Log Level failed - invalid response to set log level info");
        session.disconnect();
        return;
      }
      retMsg = session.sendMessage("", "set log level warn", "");
      if (!retMsg.getSubject().equals("log level set to WARN")) {
       System.out.println("Test Log Level failed - invalid response to set log level warn");
       session.disconnect();
       return;
     }
     retMsg = session.sendMessage("", "set log level debug", "");
      if (!retMsg.getSubject().equals("log level set to DEBUG")) {
       System.out.println("Test Log Level failed - invalid response to set log level debug");
       session.disconnect();
       return;
     }
     retMsg = session.sendMessage("", "set log level shout", "");
      if (!retMsg.getSubject().equals("log level set to SHOUT")) {
       System.out.println("Test Log Level failed - invalid response to set log level shout");
       session.disconnect();
       return;
     }
     System.out.println("Log level test passed");
     session.disconnect();
    }
    catch (Exception ex) {
      System.out.println("Exception in test log level");
    }
  }

  public void runTests() {
    testConnection();
    testListCmd();
    testRegisterCmd();
    testEavesdropCmd();
    testGlobalEavesdropCmd();
    testEnableErrorMsgsCmd();
    testLoggingCmd();
    testLogLevelCmd();
  }


  public static void main(String[] args) {
    if (args.length == 0) {
      System.out.println("specify server");
      return;
    }
    ServerTest st = new ServerTest(args[0]);
    st.runTests();
    System.exit(0);
  }

  class CheckListener
      implements AsyncMessageReceiverListener {
    private Message expectedMsg;
    private String testName;

    public CheckListener(String testName, Message expectedMsg) {
      this.testName = testName;
      this.expectedMsg = expectedMsg;
    }
    /**
     * receiveMsg
     *
     * @param msg Message
     * @todo Implement this messagerouterlib.AsyncMessageReceiverListener method
     */
    public void receiveMsg(Message msg) {
      if (msg.getSubject().equals(expectedMsg.getSubject()) && msg.getBody().equals(expectedMsg.getBody())) {
        System.out.println(testName + " passed");
      }
      else {
        System.out.println(testName + " failed");
      }
    }

    /**
     * messageReceiverClosed
     *
     * @param mr MessageReceiver
     * @todo Implement this messagerouterlib.AsyncMessageReceiverListener method
     */
    public void messageReceiverClosed(MessageReceiver mr) {
    }
  }
}
