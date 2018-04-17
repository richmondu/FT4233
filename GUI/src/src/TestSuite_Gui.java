import java.awt.event.*; // Using AWT event classes and listener interfaces
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import javax.swing.*;
import javax.swing.text.DefaultCaret;



public class TestSuite_Gui extends JFrame implements ActionListener {

	private static final long serialVersionUID = 1L;
	private JButton btnRun;
	private JButton btnStop;
	private JRadioButton radioLoopback;
	private JRadioButton radioMPSSE;
	private JRadioButton radioFastSerial;
	private JRadioButton radioJTAG;
	private JRadioButton radioSpiFlash;
	private JTextField txtDev1;
	private JTextField txtDev2;
	private JTextField txtSize;
	private JLabel lblNewLabel;
	private JButton btnClear;
	private JTextArea textArea;
	private String path;

	private final int MAINCOMMAND_LOOPBACK = 0;
	private final int MAINCOMMAND_MPSSE = 1;
	private final int MAINCOMMAND_FASTSERIAL = 2;
	private final int MAINCOMMAND_JTAG = 3;
	private final int MAINCOMMAND_SPIFLASH = 4;
	
	private final String[] MAINCOMMAND_ARRAY = {
			"Loopback test", 
			"MPSSE test", 
			"Fast Serial test", 
			"JTAG test", 
			"SPI Flash test"
			};
	
	// Constructor to setup the GUI components and event handlers
	public TestSuite_Gui() {
		initUI();
		addComponents();
	}
	
	
	private void addComponents() {

	    //
	    // Devices to test
	    //
	    
    	JLabel lblTestDevices = new JLabel("DEVICES TO TEST");
    	lblTestDevices.setBounds(37, 255, 125, 14);
    	getContentPane().add(lblTestDevices);
    	
    	txtDev1 = new JTextField();
    	txtDev1.setToolTipText("Empty means test all devices");
    	txtDev1.setBounds(37, 280, 125, 20);
    	getContentPane().add(txtDev1);
    	txtDev1.setColumns(10);
    	txtDev1.setText("\"UMFTPD2A A\"");
    	
    	txtDev2 = new JTextField();
    	txtDev2.setToolTipText("Applicable only for Loopback test");
    	txtDev2.setBounds(37, 305, 125, 20);
    	getContentPane().add(txtDev2);
    	txtDev2.setColumns(10);
    	txtDev2.setText("\"UMFTPD2A B\"");

    	lblNewLabel = new JLabel("Loopback Size");
    	lblNewLabel.setBounds(37, 333, 89, 14);
    	getContentPane().add(lblNewLabel);    	
    	
    	txtSize = new JTextField();
    	txtSize.setBounds(126, 330, 36, 20);
    	getContentPane().add(txtSize);
    	txtSize.setColumns(10);
    	txtSize.setText("1024");

    	
		//
		// Test modes
		//
		
	    JLabel lblTestMode = new JLabel("TEST MODE");
	    lblTestMode.setBounds(37, 31, 125, 14);
	    getContentPane().add(lblTestMode);
		
	    radioLoopback = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_LOOPBACK]);
	    radioLoopback.setBounds(37, 56, 125, 23);
	    radioLoopback.setSelected(true);
	    radioLoopback.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev2.setEnabled(true);
            	txtSize.setEnabled(true);
            	lblNewLabel.setEnabled(true);
            }
        });
	    getContentPane().add(radioLoopback);
	    
	    radioMPSSE = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_MPSSE]);
	    radioMPSSE.setBounds(37, 82, 125, 23);
	    radioMPSSE.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev2.setEnabled(false);
            	txtSize.setEnabled(false);
            	lblNewLabel.setEnabled(false);
            }
        });
	    getContentPane().add(radioMPSSE);
	    
	    radioFastSerial = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_FASTSERIAL]);
	    radioFastSerial.setBounds(37, 108, 125, 23);
	    radioFastSerial.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev2.setEnabled(false);
            	txtSize.setEnabled(false);
            	lblNewLabel.setEnabled(false);
            }
        });
	    getContentPane().add(radioFastSerial);

	    radioJTAG = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_JTAG]);
	    radioJTAG.setBounds(37, 134, 125, 23);
	    radioJTAG.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev2.setEnabled(false);
            	txtSize.setEnabled(false);
            	lblNewLabel.setEnabled(false);
            }
        });
	    getContentPane().add(radioJTAG);

	    radioSpiFlash = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_SPIFLASH]);
	    radioSpiFlash.setBounds(37, 160, 125, 23);
	    radioSpiFlash.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev2.setEnabled(false);
            	txtSize.setEnabled(false);
            	lblNewLabel.setEnabled(false);
            }
        });
	    getContentPane().add(radioSpiFlash);	    
	    
	    ButtonGroup group = new ButtonGroup();
	    group.add(radioLoopback);
	    group.add(radioMPSSE);
	    group.add(radioFastSerial);
	    group.add(radioJTAG);
	    group.add(radioSpiFlash);
	    
	    
	    //
	    // Display logs
	    //

	    textArea = new JTextArea();
	    textArea.setEditable(false);
	    textArea.setBounds(200, 31, 500, 320);
	    getContentPane().add(textArea);

	    JScrollPane scrollPane = new JScrollPane(textArea);
	    scrollPane.setBounds(200, 30, 500, 320);
	    scrollPane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
	    scrollPane.setViewportView(textArea);
	    scrollPane.setEnabled(true);
	    scrollPane.getVerticalScrollBar().setUnitIncrement(10);
	    scrollPane.setAutoscrolls(true);
	    DefaultCaret caret = (DefaultCaret)textArea.getCaret();
	    caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);
	    getContentPane().add(scrollPane);
	    
	    JScrollBar scrollBar = scrollPane.getVerticalScrollBar();
	    scrollBar.setEnabled(true);
	    
	    btnClear = new JButton("Clear");
	    btnClear.setBounds(611, 360, 89, 23);
	    btnClear.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	textArea.setText("");
            }
        });
	    getContentPane().add(btnClear);
    	
	    
	    //
	    // Action buttons
	    //
	    
	    btnRun = new JButton("Run");
	    btnRun.setBounds(37, 190, 61, 23);
	    btnRun.setEnabled(true);
	    getContentPane().add(btnRun);
	    
	    btnStop = new JButton("Stop");
	    btnStop.setBounds(101, 190, 61, 23);
	    btnStop.setEnabled(false);
	    getContentPane().add(btnStop);
	    
	    String directory = TestSuite_Gui.class.getProtectionDomain().getCodeSource().getLocation().getPath();
	    try {
	    	directory = URLDecoder.decode(directory, "UTF-8");
	    	if (directory.charAt(0)=='/') {
	    		directory = directory.substring(1);
	    	}
	    	if (directory.endsWith(".jar")) {
	    		directory = directory.substring(0, directory.lastIndexOf('/')+1);
	    	}
		} catch (UnsupportedEncodingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}	    
	    
	    final String application = "TestSuite_FT4232.exe";//"F:\\Users\\richmond\\workspace\\TestSuite_Gui\\bin\\TestSuite_FT4232.exe";
    	directory = directory + application;
    	path = directory.replace("/", "\\");
    	textArea.setText(path + "\n");
    	
	    btnRun.addActionListener(this); 
	    btnStop.addActionListener(this);	    
	}

	private void enableControls(boolean bEnable) {
		if (!bEnable) {
			txtDev2.setEnabled(bEnable);   
	    	txtSize.setEnabled(bEnable);
	    	lblNewLabel.setEnabled(bEnable);
		}
		else {
			boolean bLoopbackSelected = radioLoopback.isSelected();
			txtDev2.setEnabled(bLoopbackSelected);   
	    	txtSize.setEnabled(bLoopbackSelected);
	    	lblNewLabel.setEnabled(bLoopbackSelected);
		}
    	btnRun.setEnabled(bEnable);
    	btnStop.setEnabled(!bEnable);
    	radioLoopback.setEnabled(bEnable);
    	radioMPSSE.setEnabled(bEnable);
    	radioFastSerial.setEnabled(bEnable);
    	radioJTAG.setEnabled(bEnable);
    	radioSpiFlash.setEnabled(bEnable);
    	txtDev1.setEnabled(bEnable);
    	btnClear.setEnabled(bEnable);
	}	
	
	private void initUI() {
	    
	    setTitle("Test Suite");
	    setSize(750, 450);
	    setLocationRelativeTo(null);
	    setDefaultCloseOperation(EXIT_ON_CLOSE);
	    setVisible(true);
	    setResizable(false);
	    getContentPane().setLayout(null);
	}
 
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == btnRun) {
			
			enableControls(false);
	      	
        	class RunnableEx2 implements Runnable {
        		
        		private String mode;
        		private String path;
        		private String dev1;
        		private String dev2;
        		private String size;
        		
        		RunnableEx2(String path, String mode, String dev1, String dev2, String size) {
        			if (mode == null) {
        				this.mode = mode;
        			}
        			else {
        				this.mode = new String(mode);
        			}
        			this.path = new String(path);
        			this.dev1 = new String(dev1);
        			this.dev2 = new String(dev2);
        			this.size = new String(size);
        		}
        		
    			@Override
    			public void run() {
            		try {
                    	String[] cmd = null;
                    	if (this.mode == null) {
                    		cmd = new String[]{this.path};
                    	}
                    	else {
                    		if (this.dev1.length() != 0 && this.dev2.length() != 0) {
                    			cmd = new String[]{this.path, "-m", this.mode, "-d", this.dev1, "-d", this.dev2, "-b", this.size};
                    		}
                    		else if (this.dev1.length() != 0) {
                    			cmd = new String[]{this.path, "-m", this.mode, "-d", this.dev1};
                    		}
                    		else {
                    			cmd = new String[]{this.path, "-m", this.mode};
                    		}
                    	}

                    	Process p;
                    	try {
                			//p = Runtime.getRuntime().exec(cmd);
                    		p = (new ProcessBuilder(cmd)).start();
                    	}
                    	catch (Exception x) {
                    		textArea.append("\nThe executable file could not be found!\n" );
                    		p = null;
                    		enableControls(true);
                    		return;
                    	}
                    	
            			String line;
            			BufferedReader reader = new BufferedReader(new InputStreamReader(p.getInputStream()));
            			
            			while ((line = reader.readLine()) != null) {
            				class RunnableEx implements Runnable {
            					private String line;
            					RunnableEx(String line) {
            						this.line = line;
            					}
								@Override
								public void run() {
									System.out.printf("%s\n", line);
									textArea.append(this.line);
									textArea.append("\n");
	                				textArea.setCaretPosition(textArea.getDocument().getLength());
	                				textArea.update(textArea.getGraphics());
								}								
            			    };
            				SwingUtilities.invokeLater(new RunnableEx(line));
            			}
            			p.waitFor();
            			p = null;
            			
            			enableControls(true);
            		} 
            		catch (Exception x) {
            			x.printStackTrace();
            		}
    			}
    		};
    		
    		String mode = null;
    		String dev1 = null;
    		String dev2 = null;
    		String size = null;
    		if (radioLoopback.isSelected()) {
    			mode = new String("loopback");
    			dev1 = new String(txtDev1.getText());
    			dev2 = new String(txtDev2.getText());
    			size = new String(txtSize.getText());
    		}
    		else if (radioMPSSE.isSelected()) {
    			mode = new String("mpsse");
    			dev1 = new String(txtDev1.getText());
    			dev2 = new String("");
    			size = new String("0");
    		}
    		else if (radioFastSerial.isSelected()) {
    			mode = new String("fastserial");
    			dev1 = new String(txtDev1.getText());
    			dev2 = new String("");
    			size = new String("0");
    		}
    		else if (radioJTAG.isSelected()) {
    			mode = new String("jtag");
    			dev1 = new String(txtDev1.getText());
    			dev2 = new String("");
    			size = new String("0");
    		}
    		else if (radioSpiFlash.isSelected()) {
    			mode = new String("spiflash");
    			dev1 = new String(txtDev1.getText());
    			dev2 = new String("");
    			size = new String("0");
    		}
        	(new Thread(new RunnableEx2(path, mode, dev1, dev2, size))).start();
		}
		else if (e.getSource() == btnStop) {
			enableControls(true);
        	class RunnableEx3 implements Runnable {
        		@Override
    			public void run() {
                	String[] cmd = new String[] 
                		{"taskkill.exe", "/im", "TestSuite_FT4232.exe", "/f", "/t"};
					try {
						Process p = (new ProcessBuilder(cmd)).start();
                    	p.waitFor();
                    	textArea.append("\n\nThe test has been aborted successfully!\n");
					} catch (IOException e) {
						e.printStackTrace();
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
        		}
        	};
        	(new Thread(new RunnableEx3())).start();
		}
	}
	
	public static void main(String[] args) {
		SwingUtilities.invokeLater(new Runnable() {
			@Override
			public void run() {
				new TestSuite_Gui();
			}
		});
	}
}