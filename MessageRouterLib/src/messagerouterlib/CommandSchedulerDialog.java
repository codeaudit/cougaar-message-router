package messagerouterlib;

import java.awt.*;
import javax.swing.*;

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
  JList jList1 = new JList();

  public CommandSchedulerDialog(Frame frame, String title, boolean modal) {
    super(frame, title, modal);
    try {
      jbInit();
      pack();
    }
    catch(Exception ex) {
      ex.printStackTrace();
    }
  }

  public CommandSchedulerDialog() {
    this(null, "", false);
  }

  private void jbInit() throws Exception {
    panel1.setLayout(borderLayout1);
    jButtonRemove.setText("Remove");
    jButtonClose.setToolTipText("");
    jButtonClose.setText("Close");
    getContentPane().add(panel1);
    panel1.add(jPanel1, BorderLayout.SOUTH);
    jPanel1.add(jButtonRemove, null);
    jPanel1.add(jButtonClose, null);
    panel1.add(jScrollPane1, BorderLayout.CENTER);
    jScrollPane1.getViewport().add(jList1, null);
  }
}
