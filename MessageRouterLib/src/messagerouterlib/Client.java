package messagerouterlib;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.border.*;
import com.borland.jbcl.layout.*;
import java.util.StringTokenizer;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.AttributeSet;
import javax.swing.text.*;
import java.util.Vector;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.List;


public class Client extends JFrame
    implements AsyncMessageReceiverListener {
  //SortedListModel onlineUsers = new SortedListModel();
  DefaultListModel onlineUsers = new DefaultListModel();
  History msgHistory = new History();
  History subjectHistory = new History();
  static Client currentInstance;

  JSplitPane jSplitPane1 = new JSplitPane();
  BorderLayout borderLayout1 = new BorderLayout();
  JPanel jPanel1 = new JPanel();
  JLabel jLabelServer = new JLabel();
  JTextField jTextFieldServer = new JTextField();
  JLabel jLabelUserId = new JLabel();
  JTextField jTextFieldUser = new JTextField();

  Session session;
  Session[] MultiSessions;
  Border border1;
  JLabel jLabeltargetUser = new JLabel();
  JTextField jTextFieldTargetUser = new JTextField();
  JPanel jPanelSendMessages = new JPanel();
  Border border2;
  Border border3;
  TitledBorder titledBorder1;
  Border border4;
  TitledBorder titledBorder2;
  JTextField jTextFieldSendSubject = new JTextField();
  Border border5;
  TitledBorder titledBorder3;
  Border border6;
  TitledBorder titledBorder4;
  JSplitPane jSplitPane2 = new JSplitPane();
  JScrollPane jScrollPane1 = new JScrollPane();
  JScrollPane jScrollPane2 = new JScrollPane();
  JList jListOnlineUsers = new JList(onlineUsers);
  Border border7;
  TitledBorder titledBorder5;
  Border border8;
  TitledBorder titledBorder6;
  JToggleButton jToggleButtonRegister = new JToggleButton();
  JButton jButtonMultiConnect = new JButton();
  JToggleButton jToggleButtonConnect = new JToggleButton();
  JTextPane jTextPaneDisplayMessages = new JTextPane();
  SimpleAttributeSet incomingMsgAttrSet;
  SimpleAttributeSet outgoingMsgAttrSet;
  SimpleAttributeSet onlineClientAttrSet;
  SimpleAttributeSet offlineClientAttrSet;
  JTextField jTextFieldSendMessages = new JTextField();
  Border border9;
  TitledBorder titledBorder7;
  VerticalFlowLayout verticalFlowLayout1 = new VerticalFlowLayout();
  JButton jButtonMessageStresser = new JButton();
  JPopupMenu jPopupMenuMessageArea = new JPopupMenu();
  JMenuItem jMenuItemClearMessages = new JMenuItem();
  JMenuBar jMenuBarMain = new JMenuBar();
  JMenu jMenuServer = new JMenu();
  JMenuItem jMenuItemServerConnect = new JMenuItem();
  JMenuItem jMenuItemServerDisconnect = new JMenuItem();
  JMenu jMenuOptions = new JMenu();
  JMenu jMenuTest = new JMenu();
  JMenuItem jMenuItemOptionsRegister = new JMenuItem();
  JMenuItem jMenuItemOptionsDeregister = new JMenuItem();
  JMenuItem jMenuItemOptionsEavesdrop = new JMenuItem();
  JMenuItem jMenuItemOptionsUneavesdrop = new JMenuItem();
  JMenuItem jMenuItemTestMulticonnect = new JMenuItem();
  JMenuItem jMenuItemTestMessageStresser = new JMenuItem();
  JMenuItem jMenuItemTestMultidisconnect = new JMenuItem();
  JMenuItem jMenuItemOptionsGlobalEavesdrop = new JMenuItem();
  JMenuItem jMenuItemOptionsGlobalUneavesdrop = new JMenuItem();

  class HostItem implements Comparable {
    String name;
    boolean eavesDroppingEnabled = false;

    public HostItem(String name) {
      this.name = name;
    }

    /**
     * compareTo
     *
     * @param o Object
     * @return int
     */
    public int compareTo(Object o) {
      return name.compareTo(((HostItem)o).name);
    }

    public String toString() {
      return name;
    }

    public boolean equals(Object o) {
      return name.equals(o.toString());
    }

    /**
     * Returns a hash code value for the object.
     *
     * @return a hash code value for this object.
     * @todo Implement this java.lang.Object method
     */
    public int hashCode() {
      return name.hashCode();
    }

  }

  class OnlineListCellRenderer
      extends DefaultListCellRenderer {
    /**
     * Return a component that has been configured to display the specified value.
     *
     * @param list The JList we're painting.
     * @param value The value returned by list.getModel().getElementAt(index).
     * @param index The cells index.
     * @param isSelected True if the specified cell was selected.
     * @param cellHasFocus True if the specified cell has the focus.
     * @return A component whose paint() method will render the specified value.
     * @todo Implement this javax.swing.ListCellRenderer method
     */
    public Component getListCellRendererComponent(JList list, Object value,
                                                  int index, boolean isSelected,
                                                  boolean cellHasFocus) {
      HostItem hi = (HostItem)value;
      Component c = super.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);
      if (isSelected) {
        c.setBackground(Color.blue);
      }
      else {
        c.setBackground(Color.white);
      }
      if (hi.eavesDroppingEnabled) {
        c.setBackground(Color.red);
      }
      else {
        c.setBackground(Color.white);
      }

      return c;
    }

  }

  public Client() {
    try {
      currentInstance = this;
      jbInit();
      Flasher.instance();  //initialize the flasher instance so that it's currently running
      initializeAttributeSets();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }

  private void initializeAttributeSets() {
    incomingMsgAttrSet = new SimpleAttributeSet();
    outgoingMsgAttrSet = new SimpleAttributeSet();
    onlineClientAttrSet = new SimpleAttributeSet();
    offlineClientAttrSet = new SimpleAttributeSet();
    StyleConstants.setForeground(incomingMsgAttrSet, Color.BLACK);
    StyleConstants.setForeground(outgoingMsgAttrSet, Color.BLUE);
    StyleConstants.setForeground(onlineClientAttrSet, Color.GREEN);
    StyleConstants.setForeground(offlineClientAttrSet, Color.RED);
    StyleConstants.setBold(incomingMsgAttrSet, true);
    StyleConstants.setBold(onlineClientAttrSet, true);
    StyleConstants.setBold(offlineClientAttrSet, true);
    StyleConstants.setItalic(outgoingMsgAttrSet, true);
  }
  private void jbInit() throws Exception {
    border1 = new EtchedBorder(EtchedBorder.RAISED,Color.white,new Color(165, 163, 151));
    border2 = new EtchedBorder(EtchedBorder.RAISED,Color.white,new Color(165, 163, 151));
    border3 = new EtchedBorder(EtchedBorder.RAISED,Color.white,new Color(178, 178, 178));
    titledBorder1 = new TitledBorder(border3,"Subject");
    border4 = new EtchedBorder(EtchedBorder.RAISED,Color.white,new Color(178, 178, 178));
    titledBorder2 = new TitledBorder(border4,"Message");
    border5 = new EtchedBorder(EtchedBorder.RAISED,Color.white,new Color(178, 178, 178));
    titledBorder3 = new TitledBorder(border5,"Subject");
    border6 = new EtchedBorder(EtchedBorder.RAISED,Color.white,new Color(165, 163, 151));
    titledBorder4 = new TitledBorder(border6,"Message");
    border7 = new EtchedBorder(EtchedBorder.RAISED,Color.white,new Color(165, 163, 151));
    titledBorder5 = new TitledBorder(border7,"Messages");
    border8 = new EtchedBorder(EtchedBorder.RAISED,Color.white,new Color(165, 163, 151));
    titledBorder6 = new TitledBorder(border8,"Online Users");
    border9 = new EtchedBorder(EtchedBorder.RAISED,Color.white,new Color(178, 178, 178));
    titledBorder7 = new TitledBorder(border9,"Send Message");
    this.getContentPane().setLayout(borderLayout1);
    jSplitPane1.setOrientation(JSplitPane.VERTICAL_SPLIT);
    jSplitPane1.setBorder(border1);
    jSplitPane1.setLastDividerLocation(250);
    jSplitPane1.setResizeWeight(1.0);
    jLabelServer.setRequestFocusEnabled(true);
    jLabelServer.setText("Server");
    jLabelUserId.setText("User Id");
    jTextFieldServer.setPreferredSize(new Dimension(75, 20));
    jTextFieldServer.setText("");
    jTextFieldUser.setPreferredSize(new Dimension(75, 20));
    jTextFieldUser.setText("");
    this.setDefaultCloseOperation(EXIT_ON_CLOSE);
    this.setJMenuBar(jMenuBarMain);
    this.setTitle("Message Client");
    jLabeltargetUser.setText("Target");
    jTextFieldTargetUser.setPreferredSize(new Dimension(75, 20));
    jTextFieldTargetUser.setText("");
    jPanelSendMessages.setBorder(border2);
    jPanelSendMessages.setMaximumSize(new Dimension(32767, 32767));
    jPanelSendMessages.setOpaque(false);
    jPanelSendMessages.setPreferredSize(new Dimension(96, 107));
    jPanelSendMessages.setRequestFocusEnabled(true);
    jPanelSendMessages.setToolTipText("");
    jPanelSendMessages.setLayout(verticalFlowLayout1);
    jTextFieldSendSubject.setBorder(titledBorder3);
    jTextFieldSendSubject.setText("");
    jTextFieldSendSubject.addKeyListener(new Client_jTextFieldSendSubject_keyAdapter(this));
    jSplitPane2.setOrientation(JSplitPane.HORIZONTAL_SPLIT);
    jScrollPane1.setBorder(titledBorder5);
    jScrollPane2.setBorder(titledBorder6);
    jToggleButtonRegister.setText("Register");
    jToggleButtonRegister.addActionListener(new Client_jToggleButtonRegister_actionAdapter(this));
    jButtonMultiConnect.setText("Multi-Connect");
    jButtonMultiConnect.addActionListener(new Client_jButtonMultiConnect_actionAdapter(this));
    jListOnlineUsers.addMouseListener(new Client_jListOnlineUsers_mouseAdapter(this));
    jToggleButtonConnect.setText("Connect");
    jToggleButtonConnect.addActionListener(new Client_jToggleButtonConnect_actionAdapter(this));
    jTextPaneDisplayMessages.setEditable(false);
    jTextPaneDisplayMessages.addMouseListener(new Client_jTextPaneDisplayMessages_mouseAdapter(this));
    jTextFieldSendMessages.setBorder(titledBorder7);
    jTextFieldSendMessages.setToolTipText("");
    jTextFieldSendMessages.addKeyListener(new Client_jTextFieldSendMessages_keyAdapter(this));
    jButtonMessageStresser.setText("Msg Stresser");
    jButtonMessageStresser.addActionListener(new Client_jToggleButtonMessageStresser_actionAdapter(this));
    jMenuItemClearMessages.setText("Clear Messages");
    jMenuItemClearMessages.addActionListener(new Client_jMenuItemClearMessages_actionAdapter(this));
    jMenuServer.setText("Server");
    jMenuItemServerConnect.setText("Connect");
    jMenuItemServerConnect.addActionListener(new Client_jMenuItemServerConnect_actionAdapter(this));
    jMenuItemServerDisconnect.setText("Disconnect");
    jMenuItemServerDisconnect.addActionListener(new Client_jMenuItemServerDisconnect_actionAdapter(this));
    jMenuOptions.setText("Options");
    jMenuTest.setText("Test");
    jMenuItemOptionsRegister.setText("Register");
    jMenuItemOptionsRegister.addActionListener(new Client_jMenuItemOptionsRegister_actionAdapter(this));
    jMenuItemOptionsDeregister.setText("Deregister");
    jMenuItemOptionsDeregister.addActionListener(new Client_jMenuItemOptionsDeregister_actionAdapter(this));
    jMenuItemOptionsEavesdrop.setText("Eavesdrop");
    jMenuItemOptionsEavesdrop.addActionListener(new Client_jMenuItemOptionsEavesdrop_actionAdapter(this));
    jMenuItemOptionsUneavesdrop.setText("Uneavesdrop");
    jMenuItemOptionsUneavesdrop.addActionListener(new Client_jMenuItemOptionsUneavesdrop_actionAdapter(this));
    jMenuItemTestMulticonnect.setText("Multiconnect");
    jMenuItemTestMulticonnect.addActionListener(new Client_jMenuItemTestMulticonnect_actionAdapter(this));
    jMenuItemTestMessageStresser.setText("Message Stresser");
    jMenuItemTestMessageStresser.addActionListener(new Client_jMenuItemTestMessageStresser_actionAdapter(this));
    jMenuItemTestMultidisconnect.setText("Multidisconnect");
    jMenuItemTestMultidisconnect.addActionListener(new Client_jMenuItemTestMultidisconnect_actionAdapter(this));
    jMenuItemOptionsGlobalEavesdrop.setText("Global Eavesdrop");
    jMenuItemOptionsGlobalUneavesdrop.setEnabled(false);
    jMenuItemOptionsGlobalUneavesdrop.setText("Global Uneavesdrop");
    jPanelSendMessages.add(jTextFieldSendSubject, null);
    jPanelSendMessages.add(jTextFieldSendMessages, null);
    jSplitPane1.add(jSplitPane2, JSplitPane.TOP);
    jSplitPane2.add(jScrollPane1, JSplitPane.TOP);
    jScrollPane1.getViewport().add(jTextPaneDisplayMessages, null);
    jSplitPane2.add(jScrollPane2, JSplitPane.BOTTOM);
    jScrollPane2.getViewport().add(jListOnlineUsers, null);
    this.getContentPane().add(jSplitPane1,  BorderLayout.CENTER);
    jSplitPane1.add(jPanelSendMessages, JSplitPane.BOTTOM);
    this.getContentPane().add(jPanel1, BorderLayout.NORTH);
    jPanel1.add(jLabelServer, null);
    jPanel1.add(jTextFieldServer, null);
    jPanel1.add(jLabelUserId, null);
    jPanel1.add(jTextFieldUser, null);
    jPanel1.add(jLabeltargetUser, null);
    jPanel1.add(jTextFieldTargetUser, null);
    jPanel1.add(jToggleButtonConnect, null);
    jPanel1.add(jToggleButtonRegister, null);
    jPanel1.add(jButtonMultiConnect, null);
    jPanel1.add(jButtonMessageStresser, null);
    jPopupMenuMessageArea.add(jMenuItemClearMessages);
    jMenuBarMain.add(jMenuServer);
    jMenuBarMain.add(jMenuOptions);
    jMenuBarMain.add(jMenuTest);
    jMenuServer.add(jMenuItemServerConnect);
    jMenuServer.add(jMenuItemServerDisconnect);
    jMenuOptions.add(jMenuItemOptionsRegister);
    jMenuOptions.add(jMenuItemOptionsDeregister);
    jMenuOptions.add(jMenuItemOptionsEavesdrop);
    jMenuOptions.add(jMenuItemOptionsUneavesdrop);
    jMenuOptions.add(jMenuItemOptionsGlobalEavesdrop);
    jMenuOptions.add(jMenuItemOptionsGlobalUneavesdrop);
    jMenuTest.add(jMenuItemTestMulticonnect);
    jMenuTest.add(jMenuItemTestMultidisconnect);
    jMenuTest.add(jMenuItemTestMessageStresser);
    jSplitPane1.setDividerLocation(200);
    jSplitPane2.setDividerLocation(600);
    jListOnlineUsers.setCellRenderer(new OnlineListCellRenderer());
    jListOnlineUsers.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);

    jMenuItemTestMulticonnect.setEnabled(true);
    jMenuItemTestMultidisconnect.setEnabled(false);
    jMenuItemServerConnect.setEnabled(true);
    jMenuItemServerDisconnect.setEnabled(false);
    jMenuItemOptionsRegister.setEnabled(true);
    jMenuItemOptionsDeregister.setEnabled(false);
  }

  public static void main(String[] args) {
    if (args.length > 0) {
      try {
        if (args[0].equals("-stress")) {
          String userName = args[1];
          String server = args[2];
          String targetUser = args[3];
          int rate = Integer.parseInt(args[4]);
          Session session = new Session();
          if (session.connect(server, userName)) {
            long msgCount = 0;
            while (true) {
              for (int i=0; i<rate; i++) {
                msgCount++;
                session.postMessage(targetUser, ("subject " + String.valueOf(msgCount)),
                                    Util.randomString(1000));
              }
              Thread.currentThread().sleep(2000);
            }
          }
        }
        else if (args[0].equals("-stress2")) {
          String userName = args[1];
          String server = args[2];
          String targetUser = args[3];
          int rate = Integer.parseInt(args[4]);
          Session session = new Session();
          long msgCount = 0;
          while (true) {
            if (session.connect(server, userName)) {
              for (int i = 0; i < rate; i++) {
                msgCount++;
                session.postMessage(targetUser,
                                    ("subject " + String.valueOf(msgCount)),
                                    Util.randomString(1000));
              }
              session.disconnect();
            }
            Thread.currentThread().sleep(2000);
          }

        }
        else if (args[0].equals("-stress3")) {
          String userName = args[1];
          String server = args[2];
          String targetUser = args[3];
          int rate = Integer.parseInt(args[4]);
          Session session = new Session();
          long msgCount = 0;
          while (true) {
            if (session.connect(server, userName)) {
              for (int i = 0; i < rate; i++) {
                msgCount++;
                session.postMessage(targetUser,
                                    ("subject " + String.valueOf(msgCount)),
                                    Util.randomString(1000));
              }
              session.dropConnection();
            }
            Thread.currentThread().sleep(2000);
          }

        }

      }

      catch (Exception e) {
        System.out.println("Unable to start stress test.  Format: -stress <user name> <server> <target user> <msg rate>");
      }
    }
    else {
      Client client = new Client();
      client.setSize(800, 400);
      center(client);
      client.show();
    }
  }

  public static void center (Window w) {
      Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
      Dimension windowSize = w.getSize();
      w.setLocation((screenSize.width - windowSize.width)/2, (screenSize.height - windowSize.height)/2);
  }

  public void messageReceiverClosed(MessageReceiver mr) {
    disconnect();
    displayMessage("Server connection failure", offlineClientAttrSet);
  }

  public void receiveMsg(Message msg) {
    //System.out.println(msg.getSubject() + " : " + msg.getBody());
    String subject = msg.getSubject();
    if (subject != null) {
      if (subject.equals("online")) {
        onlineUsers.addElement(new HostItem(msg.getBody()));
        displayMessage(msg.getBody() + " : Online", onlineClientAttrSet);
      }
      else if (subject.equals("offline")) {
        onlineUsers.removeElement(new HostItem(msg.getBody()));
        displayMessage(msg.getBody() + " : Offline", offlineClientAttrSet);
      }
      else if (subject.equals("eavesdrop enabled")) {
        Object[] values = jListOnlineUsers.getSelectedValues();
        if (values == null) return;
        for (int i=0; i<values.length; i++) {
          ((HostItem)values[i]).eavesDroppingEnabled = true;
        }
      }
      else if (subject.equals("eavesdrop disabled")) {
        Object[] values = jListOnlineUsers.getSelectedValues();
        if (values == null) return;
        for (int i=0; i<values.length; i++) {
          ((HostItem)values[i]).eavesDroppingEnabled = false;
        }
      }
      else if (subject.equals("globaleavesdrop enabled")) {

      }
      else if (subject.equals("globaleavesdrop disabled")) {

      }
      else {
        String from = msg.getFrom() == null?"server":msg.getFrom();
        String body = msg.getBody()!=null?msg.getBody():"";
        displayMessage(subject, from, body, incomingMsgAttrSet);
      }
    }
    else {
      subject = "";
      String from = msg.getFrom() == null?"server":msg.getFrom();
      String body = msg.getBody()!=null?msg.getBody():"";
      displayMessage(subject, from, body, incomingMsgAttrSet);
    }
  }

  private void displayMessage(String text, AttributeSet as) {
    try {
      if (!text.endsWith("\n")) {
        text += "\n";
      }
      jTextPaneDisplayMessages.getDocument().insertString(
          jTextPaneDisplayMessages.getDocument().getLength(),
          text,
          as);

      jTextPaneDisplayMessages.setCaretPosition(jTextPaneDisplayMessages.getDocument().getLength());
    }
    catch (BadLocationException ex) {
    }

  }

  private void displayMessage(String subject, String from, String body, AttributeSet as) {
      String text = "FROM: " + from + " - " + subject + " : " + body;
      displayMessage(text, as);
  }

  private void displaySendMessage(String subject, String to, String body, AttributeSet as) {
    String text = "TO: " + to + " - " + subject + " : " + body;
    displayMessage(text, as);
  }


  void jToggleButtonRegister_actionPerformed(ActionEvent e) {
    handleRegister();
  }

  private void handleRegister() {
    if (session == null || !session.isConnected()) {
      jToggleButtonRegister.setSelected(false);
      jMenuItemOptionsDeregister.setEnabled(false);
      jMenuItemOptionsRegister.setEnabled(true);
      return;
    }
    if (jToggleButtonRegister.isSelected() || jMenuItemOptionsRegister.isEnabled()) {
      register();
      jToggleButtonRegister.setText("Deregister");
      jMenuItemOptionsDeregister.setEnabled(true);
      jMenuItemOptionsRegister.setEnabled(false);
    }
    else {
      deregister();
      jToggleButtonRegister.setText("Register");
      jMenuItemOptionsDeregister.setEnabled(false);
      jMenuItemOptionsRegister.setEnabled(true);
    }
  }

  private void register() {
    if (session != null && session.isConnected()) {
      onlineUsers.clear();
      //get the current list
      Message msg = session.sendMessage("", "list", "");
      System.out.println("LIST: " + msg.getBody());
      StringTokenizer st = new StringTokenizer(msg.getBody(), "\n");
      while (st.hasMoreTokens()) {
        onlineUsers.addElement(new HostItem(st.nextToken()));
      }
      session.postMessage("", "register", "");
    }
  }

  private void deregister() {
    onlineUsers.clear();
    if (session != null && session.isConnected()) {
     session.postMessage("", "deregister", "");
   }
  }

  void jButtonMultiConnect_actionPerformed(ActionEvent e) {
    multiConnect();
  }

  private void multiConnect() {
    if (jButtonMultiConnect.getText().equals("Multi-Connect")) {
      String res = JOptionPane.showInputDialog(this,
                                               "Enter number of connections",
                                               "Multi-Connect");
      if (res != null) {
        jButtonMultiConnect.setText("Multi-Disconnect");
        jMenuItemTestMulticonnect.setEnabled(false);
        jMenuItemTestMultidisconnect.setEnabled(true);
        int count = Integer.parseInt(res);
        MultiSessions = new Session[count];
        for (int i = 0; i < count; i++) {
          MultiSessions[i] = new Session();
          try {
            MultiSessions[i].connect(this.jTextFieldServer.getText(),
                                     this.jTextFieldUser.getText() +
                                     String.valueOf(i));
          }
          catch (ConnectionException ex1) {
          }
        }
      }
    }
    else {
      for (int i=0; i<MultiSessions.length; i++) {
        try {
          ( (Session) MultiSessions[i]).disconnect();
        }
        catch (Exception ex) {
          System.out.println("Error disconnectiong from session");
        }
      }
      jButtonMultiConnect.setText("Multi-Connect");
      jMenuItemTestMulticonnect.setEnabled(true);
      jMenuItemTestMultidisconnect.setEnabled(false);
    }
  }

  void jListOnlineUsers_mouseClicked(MouseEvent e) {
    if (e.getClickCount() == 2) {
      jTextFieldTargetUser.setText(((HostItem)jListOnlineUsers.getSelectedValue()).name);
      Flasher.instance().flashBackground(jTextFieldTargetUser, Color.yellow, 2);
      jTextFieldSendMessages.requestFocus();
    }
  }

  void jToggleButtonConnect_actionPerformed(ActionEvent e) {
    handleConnect();

  }

  private void handleConnect() {
    if (jToggleButtonConnect.isSelected() || jMenuItemServerConnect.isEnabled())
      try {
        jTextPaneDisplayMessages.setText("");
        session = new Session();
        try {
          if (session.connect(jTextFieldServer.getText(),
                              jTextFieldUser.getText())) {
            displayMessage("Connected\n", incomingMsgAttrSet);
            session.addListener(this);
            jToggleButtonConnect.setText("Disconnect");
            jMenuItemServerConnect.setEnabled(false);
            jMenuItemServerDisconnect.setEnabled(true);
          }
          else {
            jToggleButtonConnect.setSelected(false);
            jMenuItemServerConnect.setEnabled(true);
            jMenuItemServerDisconnect.setEnabled(false);
            displayMessage("Connection attempt failed\n", offlineClientAttrSet);
          }
        }
        catch (ConnectionException ex1) {
          jToggleButtonConnect.setSelected(false);
          jMenuItemServerConnect.setEnabled(true);
          jMenuItemServerDisconnect.setEnabled(false);
          displayMessage("Connection attempt failed: " + ex1.getMessage()+"\n", offlineClientAttrSet);
        }
      }
      catch (Exception ex) {
        ex.printStackTrace();
      }
    else {
      disconnect();
      jMenuItemServerConnect.setEnabled(true);
      jMenuItemServerDisconnect.setEnabled(false);
      displayMessage("Disconnected\n", offlineClientAttrSet);
    }
  }

  private void disconnect() {
    if (session != null && session.isConnected()) {
      session.disconnect();
    }
    jToggleButtonConnect.setSelected(false);
    jToggleButtonConnect.setText("Connect");
    jToggleButtonRegister.setSelected(false);
    onlineUsers.clear();
    jToggleButtonRegister.setText("Register");
  }

  private void eavesdrop() {
    if (session == null || !session.isConnected()) return;

    //get the list of selected hosts from the online list
    Object[] values = jListOnlineUsers.getSelectedValues();
    if (values == null) return;

    //iterate through the list of hosts
    for (int i =0; i<values.length; i++) {
      //if a host is already being eavesdropped on, skip it
      HostItem hi = (HostItem)values[i];
      if (!hi.eavesDroppingEnabled) {
        session.postMessage("", "eavesdrop", hi.name);
      }
    }
  }

  private void uneavesdrop() {
    if (session == null || !session.isConnected()) return;
    //get the list of selected hosts from the online list
    Object[] values = jListOnlineUsers.getSelectedValues();
    if (values == null) return;

    //iterate through the list of hosts
    for (int i =0; i<values.length; i++) {
      //if a host is not being eavesdropped on, skip it
      HostItem hi = (HostItem) values[i];
      if (hi.eavesDroppingEnabled) {
        session.postMessage("", "uneavesdrop", hi.name);
      }
    }
  }

  private void globalEavesDrop() {
    if (session == null || !session.isConnected()) return;
    jMenuItemOptionsGlobalEavesdrop.setEnabled(false);
    jMenuItemOptionsGlobalUneavesdrop.setEnabled(true);
    session.postMessage("", "globaleavesdrop", "");
  }

  private void globalUneavesdrop() {
    if (session == null || !session.isConnected()) return;
    jMenuItemOptionsGlobalEavesdrop.setEnabled(true);
    jMenuItemOptionsGlobalUneavesdrop.setEnabled(false);
    session.postMessage("", "unglobaleavesdrop", "");
  }

  void jTextFieldSendMessages_keyPressed(KeyEvent e) {
    if(e.getKeyCode() == e.VK_DOWN) {
      String s = msgHistory.forward();
      if (s != null) {
        jTextFieldSendMessages.setText(s);
      }
    }
    else if (e.getKeyCode() == e.VK_UP) {
      String s = msgHistory.back();
      if (s != null) {
        jTextFieldSendMessages.setText(s);
      }
    }
    else if (e.getKeyChar() == '\n') {
      String body = jTextFieldSendMessages.getText();
      if (body.length() > 0) {
        if (body.charAt(body.length() - 1) == '\n') body = body.substring(0,
            body.length() - 1);
        String subject = jTextFieldSendSubject.getText();
        if (subject.length() > 0) {
          if (subject.charAt(subject.length() - 1) == '\n') subject = subject.
              substring(0, subject.length() - 1);
        }
        if (session != null && session.isConnected()) {
          session.postMessage(jTextFieldTargetUser.getText(),
                              subject,
                              body);
          displaySendMessage(subject,
                         jTextFieldTargetUser.getText(), body,
                         outgoingMsgAttrSet);
        }
        msgHistory.add(body);
        subjectHistory.add(subject);
        subjectHistory.reset();
        msgHistory.reset();
        jTextFieldSendMessages.setText("");
        jTextFieldSendSubject.setText("");
      }
    }

  }

  void jTextFieldSendSubject_keyPressed(KeyEvent e) {
    if(e.getKeyCode() == e.VK_DOWN) {
      String s = subjectHistory.forward();
      if (s != null) {
        jTextFieldSendSubject.setText(s);
      }
    }
    else if (e.getKeyCode() == e.VK_UP) {
      String s = subjectHistory.back();
      if (s != null) {
        jTextFieldSendSubject.setText(s);
      }
    }
    else if (e.getKeyChar() == '\n') {
      jTextFieldSendMessages.requestFocus();
    }

  }

  void jButtonMessageStresser_actionPerformed(ActionEvent e) {
    messageStresser();
  }

  private void messageStresser() throws HeadlessException {
    String res = JOptionPane.showInputDialog(this, "Number of messages to send");
    try {
      int count = Integer.parseInt(res);
      Thread t = new Thread(new Stresser(count));
      t.start();
    }
    catch (NumberFormatException nfe) {}
  }

  void jMenuItemClearMessages_actionPerformed(ActionEvent e) {
    jTextPaneDisplayMessages.setText("");
  }

  void jTextPaneDisplayMessages_mouseClicked(MouseEvent e) {
    if ((e.getModifiers() & Event.META_MASK) == Event.META_MASK ) {
      jPopupMenuMessageArea.show(jTextPaneDisplayMessages, e.getX(), e.getY());
    }
  }

  void jMenuItemServerConnect_actionPerformed(ActionEvent e) {
    handleConnect();
  }

  void jMenuItemServerDisconnect_actionPerformed(ActionEvent e) {
    handleConnect();
  }

  void jMenuItemOptionsRegister_actionPerformed(ActionEvent e) {
    handleRegister();
  }

  void jMenuItemOptionsDeregister_actionPerformed(ActionEvent e) {
    handleRegister();
  }

  void jMenuItemOptionsEavesdrop_actionPerformed(ActionEvent e) {
    eavesdrop();
  }

  void jMenuItemOptionsUneavesdrop_actionPerformed(ActionEvent e) {
    uneavesdrop();
  }

  void jMenuItemTestMulticonnect_actionPerformed(ActionEvent e) {
    multiConnect();
  }

  void jMenuItemTestMessageStresser_actionPerformed(ActionEvent e) {
    messageStresser();
  }

  void jMenuItemTestMultidisconnect_actionPerformed(ActionEvent e) {
    multiConnect();
  }
}

class Flasher extends Thread {
  static private Flasher me;
  Object semaphore = new Object();
  Component c;
  Color flashColor;
  int flashCount;

  private Flasher(){
    start();
  }
  public static Flasher instance() {
    if (me == null) me = new Flasher();
    return me;
  }

  public static void flashBackground(Component c, Color flashColor, int flashCount) {
    me.c = c;
    me.flashColor = flashColor;
    me.flashCount = flashCount;
    synchronized(me.semaphore) {
        me.semaphore.notifyAll();
    }
  }

  public void run() {
    while (true) {
      synchronized (semaphore) {
        try {
          semaphore.wait();
        }
        catch (InterruptedException ex) {
        }
      }
      Color origColor = c.getBackground();
      for (int i=0; i<flashCount; i++) {
        c.setBackground(flashColor);
        try {
          Thread.currentThread().sleep(100);
        }
        catch (Exception ex) {}
        c.setBackground(origColor);
      }
    }
  }
}

class Client_jTextFieldSendSubject_keyAdapter extends java.awt.event.KeyAdapter {
  Client adaptee;

  Client_jTextFieldSendSubject_keyAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void keyPressed(KeyEvent e) {
    adaptee.jTextFieldSendSubject_keyPressed(e);
  }
}

class Client_jToggleButtonRegister_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jToggleButtonRegister_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jToggleButtonRegister_actionPerformed(e);
  }
}

class Client_jButtonMultiConnect_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jButtonMultiConnect_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jButtonMultiConnect_actionPerformed(e);
  }
}

class Client_jListOnlineUsers_mouseAdapter extends java.awt.event.MouseAdapter {
  Client adaptee;

  Client_jListOnlineUsers_mouseAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void mouseClicked(MouseEvent e) {
    adaptee.jListOnlineUsers_mouseClicked(e);
  }
}

class Client_jToggleButtonConnect_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jToggleButtonConnect_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jToggleButtonConnect_actionPerformed(e);
  }
}



class History {
   Vector list;
   int pos = 0;


   public History() {
     list = new Vector();
   }

   public String forward() {
     if (!list.isEmpty() && pos > 0) {
       return (String)list.get(--pos);
     }
     return null;
   }

   public String back() {
     if (!list.isEmpty()) {
       String ret = (String)list.elementAt(pos);
       if (pos < (list.size()-1)) pos++;
       return ret;
     }
     return null;

   }

   public void reset() {
     pos = 0;
   }

   public void add(String s) {
     if (s == null || s.equals("")) return;

     if (list.contains(s)) {
       list.remove(s);
     }
     list.insertElementAt(s, 0);
   }
 }

 class SortedListModel extends DefaultListModel {
  public SortedListModel() {
  }

  public SortedListModel(Vector v) {
      setElements(v);
  }

  public void setElements(Vector v) {
      clear();
      Object arr[]=v.toArray();
      Arrays.sort(arr);
      for (int i=0; i<arr.length; i++)
          super.addElement(arr[i]);
  }

  public void add(int idx, Object o) {
      addElement(o);
  }

  public void setElementAt(Object o, int idx) {
      removeElementAt(idx);
      addElement(o);
  }

  public void addElement(Object o) {
      Comparable a=(Comparable)o;
      int i=0;
      for (i=0; i<getSize(); i++) {
          if (a.compareTo(getElementAt(i))<0) {
              break;
          }
      }
      super.add(i,o);
  }
}

class Client_jTextFieldSendMessages_keyAdapter extends java.awt.event.KeyAdapter {
  Client adaptee;

  Client_jTextFieldSendMessages_keyAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void keyPressed(KeyEvent e) {
    adaptee.jTextFieldSendMessages_keyPressed(e);
  }
}

class Client_jToggleButtonMessageStresser_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jToggleButtonMessageStresser_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jButtonMessageStresser_actionPerformed(e);
  }
}

class Stresser implements Runnable {
  int msgCount;
  public Stresser(int msgCount) {
    this.msgCount = msgCount;
  }

  public void run() {
    for (int i=0; i<msgCount; i++) {
      if (Client.currentInstance.session != null && Client.currentInstance.session.isConnected()) {
        Client.currentInstance.session.postMessage(Client.currentInstance.jTextFieldTargetUser.getText(), "Test Message " + i, "Test Message");
      }
    }
  }
}

class Client_jMenuItemClearMessages_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jMenuItemClearMessages_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItemClearMessages_actionPerformed(e);
  }
}

class Client_jTextPaneDisplayMessages_mouseAdapter extends java.awt.event.MouseAdapter {
  Client adaptee;

  Client_jTextPaneDisplayMessages_mouseAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void mouseClicked(MouseEvent e) {
    adaptee.jTextPaneDisplayMessages_mouseClicked(e);
  }
}

class Client_jMenuItemServerConnect_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jMenuItemServerConnect_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItemServerConnect_actionPerformed(e);
  }
}

class Client_jMenuItemServerDisconnect_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jMenuItemServerDisconnect_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItemServerDisconnect_actionPerformed(e);
  }
}

class Client_jMenuItemOptionsRegister_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jMenuItemOptionsRegister_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItemOptionsRegister_actionPerformed(e);
  }
}

class Client_jMenuItemOptionsDeregister_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jMenuItemOptionsDeregister_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItemOptionsDeregister_actionPerformed(e);
  }
}

class Client_jMenuItemOptionsEavesdrop_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jMenuItemOptionsEavesdrop_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItemOptionsEavesdrop_actionPerformed(e);
  }
}

class Client_jMenuItemOptionsUneavesdrop_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jMenuItemOptionsUneavesdrop_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItemOptionsUneavesdrop_actionPerformed(e);
  }
}

class Client_jMenuItemTestMulticonnect_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jMenuItemTestMulticonnect_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItemTestMulticonnect_actionPerformed(e);
  }
}

class Client_jMenuItemTestMessageStresser_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jMenuItemTestMessageStresser_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItemTestMessageStresser_actionPerformed(e);
  }
}

class Client_jMenuItemTestMultidisconnect_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jMenuItemTestMultidisconnect_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItemTestMultidisconnect_actionPerformed(e);
  }
}




