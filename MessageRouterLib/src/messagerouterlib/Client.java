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
  SortedListModel onlineUsers = new SortedListModel();
  //DefaultListModel onlineUsers = new DefaultListModel();
  History msgHistory = new History();
  History subjectHistory = new History();

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
  BorderLayout borderLayout2 = new BorderLayout();
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

  public Client() {
    try {
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
    jLabelServer.setRequestFocusEnabled(true);
    jLabelServer.setText("Server");
    jLabelUserId.setText("User Id");
    jTextFieldServer.setPreferredSize(new Dimension(75, 20));
    jTextFieldServer.setText("");
    jTextFieldUser.setPreferredSize(new Dimension(75, 20));
    jTextFieldUser.setText("");
    this.setDefaultCloseOperation(EXIT_ON_CLOSE);
    this.setTitle("Message Clent");
    jLabeltargetUser.setText("Target");
    jTextFieldTargetUser.setPreferredSize(new Dimension(75, 20));
    jTextFieldTargetUser.setText("");
    jPanelSendMessages.setBorder(border2);
    jPanelSendMessages.setPreferredSize(new Dimension(96, 107));
    jPanelSendMessages.setLayout(borderLayout2);
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
    jTextFieldSendMessages.setBorder(titledBorder7);
    jTextFieldSendMessages.setToolTipText("");
    jTextFieldSendMessages.addKeyListener(new Client_jTextFieldSendMessages_keyAdapter(this));
    jPanelSendMessages.add(jTextFieldSendSubject,  BorderLayout.NORTH);
    jPanelSendMessages.add(jTextFieldSendMessages, BorderLayout.SOUTH);
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
    jSplitPane1.setDividerLocation(200);
    jSplitPane2.setDividerLocation(600);
  }

  public static void main(String[] args) {
    Client client = new Client();
    client.setSize(800,400);
    center(client);
    client.show();
  }

  public static void center (Window w) {
      Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
      Dimension windowSize = w.getSize();
      w.setLocation((screenSize.width - windowSize.width)/2, (screenSize.height - windowSize.height)/2);
  }



  public void receiveMsg(Message msg) {
    //System.out.println(msg.getSubject() + " : " + msg.getBody());
    String subject = msg.getSubject();
    if (subject != null && subject.equals("online")) {
      onlineUsers.addElement(msg.getBody());
      displayMessage(msg.getBody()+ " : Online", onlineClientAttrSet);
    }
    else if (subject != null && subject.equals("offline")) {
      onlineUsers.removeElement(msg.getBody());
      displayMessage(msg.getBody()+" : Offline", offlineClientAttrSet);
    }
    else {
      subject = subject == null?"":subject;
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


  void jToggleButtonRegister_actionPerformed(ActionEvent e) {
    if (jToggleButtonRegister.isSelected()) {
      register();
      jToggleButtonRegister.setText("Deregister");
    }
    else {
      deregister();
      jToggleButtonRegister.setText("Register");
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
        onlineUsers.addElement(st.nextToken());
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
    if (jButtonMultiConnect.getText().equals("Multi-Connect")) {
      String res = JOptionPane.showInputDialog(this,
                                               "Enter number of connections",
                                               "Multi-Connect");
      if (res != null) {
        jButtonMultiConnect.setText("Multi-Disconnect");
        int count = Integer.parseInt(res);
        MultiSessions = new Session[count];
        for (int i = 0; i < count; i++) {
          MultiSessions[i] = new Session();
          MultiSessions[i].connect(this.jTextFieldServer.getText(),
                                   this.jTextFieldUser.getText() +
                                   String.valueOf(i));
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
    }
  }

  void jListOnlineUsers_mouseClicked(MouseEvent e) {
    if (e.getClickCount() == 2) {
      jTextFieldTargetUser.setText((String)jListOnlineUsers.getSelectedValue());
      Flasher.instance().flashBackground(jTextFieldTargetUser, Color.yellow, 2);
      jTextFieldSendMessages.requestFocus();
    }
  }

  void jToggleButtonConnect_actionPerformed(ActionEvent e) {
    if (jToggleButtonConnect.isSelected())
      try {
        jTextPaneDisplayMessages.setText("");
        session = new Session();
        if (session.connect(jTextFieldServer.getText(), jTextFieldUser.getText())) {
          displayMessage("Connected\n", incomingMsgAttrSet);
          session.addListener(this);
          jToggleButtonConnect.setText("Disconnect");
        }
        else {
          displayMessage("Connection attempt failed\n", incomingMsgAttrSet);
        }
      }
      catch (Exception ex) {
        ex.printStackTrace();
      }
    else {
      disconnect();
    }

  }

  private void disconnect() {
    if (session != null && session.isConnected()) {
      session.disconnect();
    }
    jToggleButtonConnect.setText("Connect");
    jToggleButtonRegister.setSelected(false);
    onlineUsers.clear();
    jToggleButtonRegister.setText("Register");
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
          displayMessage(subject,
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

