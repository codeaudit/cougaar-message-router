package messagerouterlib;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import java.awt.event.*;
/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class CommandSchedulerDialog extends JDialog {
  JPanel panel1 = new JPanel();
  BorderLayout borderLayout1 = new BorderLayout();
  JPanel jPanel1 = new JPanel();
  JButton jButtonRemove = new JButton();
  JButton jButtonClose = new JButton();
  JScrollPane jScrollPane1 = new JScrollPane();
  DefaultListModel lm = new DefaultListModel();
  JList jList1 = new JList(lm);

  public CommandSchedulerDialog(Frame frame, String title, boolean modal) {
    super(frame, title, modal);
    try {
      jbInit();
      init();
      pack();
    }
    catch(Exception ex) {
      ex.printStackTrace();
    }
  }

  public CommandSchedulerDialog() {
    this(null, "", false);
  }

  private void init() {
    ArrayList list = Client.getCommandSchedulers();
    for (int i=0; i<list.size(); i++) {
      CommandScheduler cs = (CommandScheduler)list.get(i);
      lm.addElement(cs);
    }
    jList1.repaint();

  }

  private void jbInit() throws Exception {
    panel1.setLayout(borderLayout1);
    jButtonRemove.setText("Remove");
    jButtonRemove.addActionListener(new CommandSchedulerDialog_jButtonRemove_actionAdapter(this));
    jButtonClose.setToolTipText("");
    jButtonClose.setText("Close");
    jButtonClose.addActionListener(new CommandSchedulerDialog_jButtonClose_actionAdapter(this));
    getContentPane().add(panel1);
    panel1.add(jPanel1, BorderLayout.SOUTH);
    jPanel1.add(jButtonRemove, null);
    jPanel1.add(jButtonClose, null);
    panel1.add(jScrollPane1, BorderLayout.CENTER);
    jScrollPane1.getViewport().add(jList1, null);
  }

  void jButtonRemove_actionPerformed(ActionEvent e) {

  }

  void jButtonClose_actionPerformed(ActionEvent e) {
    this.dispose();
  }

}

class CommandSchedulerDialog_jButtonRemove_actionAdapter implements java.awt.event.ActionListener {
  CommandSchedulerDialog adaptee;

  CommandSchedulerDialog_jButtonRemove_actionAdapter(CommandSchedulerDialog adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jButtonRemove_actionPerformed(e);
  }
}

class CommandSchedulerDialog_jButtonClose_actionAdapter implements java.awt.event.ActionListener {
  CommandSchedulerDialog adaptee;

  CommandSchedulerDialog_jButtonClose_actionAdapter(CommandSchedulerDialog adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jButtonClose_actionPerformed(e);
  }
}
