package messagerouterlib;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.border.*;


public class Client extends JFrame
    implements AsyncMessageReceiverListener {
  JSplitPane jSplitPane1 = new JSplitPane();
  BorderLayout borderLayout1 = new BorderLayout();
  JTextArea jTextAreaDisplayMessages = new JTextArea();
  JTextArea jTextAreaSendMessages = new JTextArea();
  JPanel jPanel1 = new JPanel();
  JLabel jLabelServer = new JLabel();
  JTextField jTextFieldServer = new JTextField();
  JLabel jLabelUserId = new JLabel();
  JTextField jTextFieldUser = new JTextField();
  JButton jButtonConnect = new JButton();

  Session session;
  Border border1;
  JButton jButtonDisconnect = new JButton();
  JLabel jLabeltargetUser = new JLabel();
  JTextField jTextFieldTargetUser = new JTextField();

  public Client() {
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    border1 = new EtchedBorder(EtchedBorder.RAISED,Color.white,new Color(165, 163, 151));
    this.getContentPane().setLayout(borderLayout1);
    jSplitPane1.setOrientation(JSplitPane.VERTICAL_SPLIT);
    jSplitPane1.setBorder(border1);
    jSplitPane1.setLastDividerLocation(250);
    jTextAreaDisplayMessages.setEnabled(false);
    jTextAreaDisplayMessages.setPreferredSize(new Dimension(75, 17));
    jTextAreaDisplayMessages.setText("");
    jTextAreaSendMessages.setToolTipText("");
    jTextAreaSendMessages.addKeyListener(new Client_jTextAreaSendMessages_keyAdapter(this));
    jLabelServer.setRequestFocusEnabled(true);
    jLabelServer.setText("Server");
    jLabelUserId.setText("User Id");
    jTextFieldServer.setPreferredSize(new Dimension(75, 20));
    jTextFieldServer.setText("");
    jTextFieldUser.setPreferredSize(new Dimension(75, 20));
    jTextFieldUser.setText("");
    jButtonConnect.setText("Connect");
    jButtonConnect.addActionListener(new Client_jButtonConnect_actionAdapter(this));
    this.setDefaultCloseOperation(EXIT_ON_CLOSE);
    this.setTitle("Message Clent");
    jButtonDisconnect.setText("Disconnect");
    jButtonDisconnect.addActionListener(new Client_jButtonDisconnect_actionAdapter(this));
    jLabeltargetUser.setText("Target");
    jTextFieldTargetUser.setPreferredSize(new Dimension(75, 20));
    jTextFieldTargetUser.setText("");
    this.getContentPane().add(jSplitPane1,  BorderLayout.CENTER);
    jSplitPane1.add(jTextAreaDisplayMessages, JSplitPane.TOP);
    jSplitPane1.add(jTextAreaSendMessages, JSplitPane.BOTTOM);
    this.getContentPane().add(jPanel1, BorderLayout.NORTH);
    jPanel1.add(jLabelServer, null);
    jPanel1.add(jTextFieldServer, null);
    jPanel1.add(jLabelUserId, null);
    jPanel1.add(jTextFieldUser, null);
    jPanel1.add(jLabeltargetUser, null);
    jPanel1.add(jTextFieldTargetUser, null);
    jPanel1.add(jButtonConnect, null);
    jPanel1.add(jButtonDisconnect, null);
    jSplitPane1.setDividerLocation(250);
  }

  public static void main(String[] args) {
    Client client = new Client();
    client.setSize(600,400);
    center(client);
    client.show();
  }

  public static void center (Window w) {
      Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
      Dimension windowSize = w.getSize();
      w.setLocation((screenSize.width - windowSize.width)/2, (screenSize.height - windowSize.height)/2);
  }


  void jButtonConnect_actionPerformed(ActionEvent e) {
    try {
      jTextAreaDisplayMessages.setText("");
      session = new Session();
      if (session.connect(jTextFieldServer.getText(), jTextFieldUser.getText()) ) {
        jTextAreaDisplayMessages.append("Connected\n");
        session.addListener(this);
      }
      else {
        jTextAreaDisplayMessages.append("Connection attempt failed\n");
      }
    }
    catch (Exception ex) {
      ex.printStackTrace();
    }
  }

  void jButtonDisconnect_actionPerformed(ActionEvent e) {
    if (session != null && session.isConnected()) {
      session.disconnect();
    }
  }

  void jTextAreaSendMessages_keyTyped(KeyEvent e) {
    if (e.getKeyChar() == '\n') {
      session.postMessage(jTextFieldTargetUser.getText(),"", jTextAreaSendMessages.getText());
      jTextAreaSendMessages.setText("");
    }
  }

  public void receiveMsg(Message msg) {
    jTextAreaDisplayMessages.append(msg.getBody());
  }

}



class Client_jButtonConnect_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jButtonConnect_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jButtonConnect_actionPerformed(e);
  }
}

class Client_jButtonDisconnect_actionAdapter implements java.awt.event.ActionListener {
  Client adaptee;

  Client_jButtonDisconnect_actionAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jButtonDisconnect_actionPerformed(e);
  }
}

class Client_jTextAreaSendMessages_keyAdapter extends java.awt.event.KeyAdapter {
  Client adaptee;

  Client_jTextAreaSendMessages_keyAdapter(Client adaptee) {
    this.adaptee = adaptee;
  }
  public void keyTyped(KeyEvent e) {
    adaptee.jTextAreaSendMessages_keyTyped(e);
  }
}
