import java.awt.event.*; // Using AWT event classes and listener interfaces
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.util.concurrent.TimeUnit;

import javax.swing.*;
import javax.swing.text.DefaultCaret;



public class TestSuite_Gui extends JFrame implements ActionListener {

	private static final long serialVersionUID = 1L;
	private JButton btnRun;
	private JButton btnStop;
	private JRadioButton radioEnum;
	private JRadioButton radioLoopback;
	private JRadioButton radioMPSSE;
	private JRadioButton radioFastSerial;
	private JRadioButton radioJTAG;
	private JRadioButton radioSpiFlash;
	private JRadioButton radioSpiEeprom;
	private JRadioButton radioI2cEeprom;
	private JTextField txtDev1;
	private JTextField txtDev2;
	private JTextField txtSize;
	private JLabel lblNewLabel;
	private JButton btnClear;
	private JTextArea textArea;
	private String path;

	private final int MAINCOMMAND_ENUMERATE = 0;
	private final int MAINCOMMAND_LOOPBACK = 1;
	private final int MAINCOMMAND_MPSSE = 2;
	private final int MAINCOMMAND_FASTSERIAL = 3;
	private final int MAINCOMMAND_JTAG = 4;
	private final int MAINCOMMAND_SPIFLASH = 5;
	private final int MAINCOMMAND_SPIEEPROM = 6;
	private final int MAINCOMMAND_I2CEEPROM = 7;
	
	private final String[] MAINCOMMAND_ARRAY = {
			"Enumerate devices", 
			"Loopback test", 
			"MPSSE test", 
			"Fast Serial test", 
			"JTAG test", 
			"SPI Flash test",
			"SPI EEPROM test",
			"I2C EEPROM test",
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
    	lblTestDevices.setBounds(37, 355, 125, 14);
    	getContentPane().add(lblTestDevices);
    	
    	txtDev1 = new JTextField();
    	txtDev1.setToolTipText("Empty means test all devices");
    	txtDev1.setBounds(37, 380, 125, 20);
    	getContentPane().add(txtDev1);
    	txtDev1.setColumns(10);
    	txtDev1.setText("\"UMFTPD2A A\"");
    	txtDev1.setEnabled(false);
    	
    	txtDev2 = new JTextField();
    	txtDev2.setToolTipText("Applicable only for Loopback test");
    	txtDev2.setBounds(37, 405, 125, 20);
    	getContentPane().add(txtDev2);
    	txtDev2.setColumns(10);
    	txtDev2.setText("\"UMFTPD2A B\"");
    	txtDev2.setEnabled(false);

    	lblNewLabel = new JLabel("Loopback Size");
    	lblNewLabel.setBounds(37, 433, 89, 14);
    	getContentPane().add(lblNewLabel);    	
    	lblNewLabel.setEnabled(false);
    	
    	txtSize = new JTextField();
    	txtSize.setBounds(126, 430, 36, 20);
    	getContentPane().add(txtSize);
    	txtSize.setColumns(10);
    	txtSize.setText("1024");
    	txtSize.setEnabled(false);

    	
		//
		// Test modes
		//
		
	    JLabel lblTestMode = new JLabel("TEST MODE");
	    lblTestMode.setBounds(37, 31, 125, 14);
	    getContentPane().add(lblTestMode);
	    
	    radioEnum = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_ENUMERATE]);
	    radioEnum.setBounds(37, 56, 157, 23);
	    radioEnum.setSelected(true);
	    radioEnum.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev1.setEnabled(false);
            	txtDev2.setEnabled(false);
            	txtSize.setEnabled(false);
            	lblNewLabel.setEnabled(false);
            }
        });
	    getContentPane().add(radioEnum);
	    
	    radioLoopback = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_LOOPBACK]);
	    radioLoopback.setBounds(37, 82, 125, 23);
	    radioLoopback.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev1.setEnabled(true);
            	txtDev2.setEnabled(true);
            	txtSize.setEnabled(true);
            	lblNewLabel.setEnabled(true);
            }
        });
	    getContentPane().add(radioLoopback);
	    
	    radioMPSSE = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_MPSSE]);
	    radioMPSSE.setBounds(37, 108, 125, 23);
	    radioMPSSE.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev1.setEnabled(true);
            	txtDev2.setEnabled(false);
            	txtSize.setEnabled(false);
            	lblNewLabel.setEnabled(false);
            }
        });
	    getContentPane().add(radioMPSSE);
	    
	    radioFastSerial = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_FASTSERIAL]);
	    radioFastSerial.setBounds(37, 134, 125, 23);
	    radioFastSerial.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev1.setEnabled(true);
            	txtDev2.setEnabled(false);
            	txtSize.setEnabled(false);
            	lblNewLabel.setEnabled(false);
            }
        });
	    getContentPane().add(radioFastSerial);

	    radioJTAG = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_JTAG]);
	    radioJTAG.setBounds(37, 160, 125, 23);
	    radioJTAG.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev1.setEnabled(true);
            	txtDev2.setEnabled(false);
            	txtSize.setEnabled(false);
            	lblNewLabel.setEnabled(false);
            }
        });
	    getContentPane().add(radioJTAG);

	    radioSpiFlash = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_SPIFLASH]);
	    radioSpiFlash.setBounds(37, 186, 125, 23);
	    radioSpiFlash.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev1.setEnabled(true);
            	txtDev2.setEnabled(false);
            	txtSize.setEnabled(false);
            	lblNewLabel.setEnabled(false);
            }
        });
	    getContentPane().add(radioSpiFlash);	    

	    radioSpiEeprom = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_SPIEEPROM]);
	    radioSpiEeprom.setBounds(37, 212, 125, 23);
	    radioSpiEeprom.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev1.setEnabled(true);
            	txtDev2.setEnabled(false);
            	txtSize.setEnabled(false);
            	lblNewLabel.setEnabled(false);
            }
        });
	    getContentPane().add(radioSpiEeprom);	    

	    radioI2cEeprom = new JRadioButton(MAINCOMMAND_ARRAY[MAINCOMMAND_I2CEEPROM]);
	    radioI2cEeprom.setBounds(37, 238, 125, 23);
	    radioI2cEeprom.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	txtDev1.setEnabled(true);
            	txtDev2.setEnabled(false);
            	txtSize.setEnabled(false);
            	lblNewLabel.setEnabled(false);
            }
        });
	    getContentPane().add(radioI2cEeprom);	    
	    
	    ButtonGroup group = new ButtonGroup();
	    group.add(radioEnum);
	    group.add(radioLoopback);
	    group.add(radioMPSSE);
	    group.add(radioFastSerial);
	    group.add(radioJTAG);
	    group.add(radioSpiFlash);
	    group.add(radioSpiEeprom);
	    group.add(radioI2cEeprom);
	    
	    
	    //
	    // Display logs
	    //

	    textArea = new JTextArea();
	    textArea.setEditable(false);
	    textArea.setBounds(200, 30, 500, 420);
	    getContentPane().add(textArea);

	    JScrollPane scrollPane = new JScrollPane(textArea);
	    scrollPane.setBounds(200, 30, 500, 420);
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
	    btnClear.setBounds(611, 460, 89, 23);
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
	    btnRun.setBounds(37, 270, 61, 23);
	    btnRun.setEnabled(true);
	    getContentPane().add(btnRun);
	    
	    btnStop = new JButton("Stop");
	    btnStop.setBounds(101, 270, 61, 23);
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
			txtDev1.setEnabled(bEnable);   
			txtDev2.setEnabled(bEnable);   
	    	txtSize.setEnabled(bEnable);
	    	lblNewLabel.setEnabled(bEnable);
		}
		else {
			boolean bLoopbackSelected = radioLoopback.isSelected();
			boolean bEnumerateSelected = radioEnum.isSelected();
			txtDev1.setEnabled(!bEnumerateSelected);   
			txtDev2.setEnabled(bLoopbackSelected);   
	    	txtSize.setEnabled(bLoopbackSelected);
	    	lblNewLabel.setEnabled(bLoopbackSelected);
		}
    	btnRun.setEnabled(bEnable);
    	btnStop.setEnabled(!bEnable);
    	radioEnum.setEnabled(bEnable);
    	radioLoopback.setEnabled(bEnable);
    	radioMPSSE.setEnabled(bEnable);
    	radioFastSerial.setEnabled(bEnable);
    	radioJTAG.setEnabled(bEnable);
    	radioSpiFlash.setEnabled(bEnable);
    	radioSpiEeprom.setEnabled(bEnable);
    	radioI2cEeprom.setEnabled(bEnable);
    	btnClear.setEnabled(bEnable);
	}	
	
	private StringBuilder SetParameters(StringBuilder dev1, StringBuilder dev2, StringBuilder size) {
		if (radioEnum.isSelected()) {
			dev1.append(txtDev1.getText());
			dev2.append("");
			size.append("0");
			return null;
		}
		else if (radioLoopback.isSelected()) {
			dev1.append(txtDev1.getText());
			dev2.append(txtDev2.getText());
			size.append(txtSize.getText());
			return new StringBuilder("loopback");
		}
		else if (radioMPSSE.isSelected()) {
			dev1.append(txtDev1.getText());
			dev2.append("");
			size.append("0");
			return new StringBuilder("mpsse");
		}
		else if (radioFastSerial.isSelected()) {
			dev1.append(txtDev1.getText());
			dev2.append("");
			size.append("0");
			return new StringBuilder("fastserial");
		}
		else if (radioJTAG.isSelected()) {
			dev1.append(txtDev1.getText());
			dev2.append("");
			size.append("0");
			return new StringBuilder("jtag");
		}
		else if (radioSpiFlash.isSelected()) {
			dev1.append(txtDev1.getText());
			dev2.append("");
			size.append("0");
			return new StringBuilder("spiflash");
		}
		else if (radioSpiEeprom.isSelected()) {
			dev1.append(txtDev1.getText());
			dev2.append("");
			size.append("0");
			return new StringBuilder("spieeprom");
		}
		else if (radioI2cEeprom.isSelected()) {
			dev1.append(txtDev1.getText());
			dev2.append("");
			size.append("0");
			return new StringBuilder("i2ceeprom");
		}		
		return null;
	}
	
	private void actionPerformedBtnRun() {
    	class RunnableEx implements Runnable {
    		
    		private String mode;
    		private String path;
    		private String dev1;
    		private String dev2;
    		private String size;
    		
    		RunnableEx(String path, String mode, String dev1, String dev2, String size) {
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
                		cmd = new String[]{this.path, "-e"};
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
                		textArea.setText("");
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
        				class RunnableEx2 implements Runnable {
        					private String line;
        					RunnableEx2(String line) {
        						this.line = line;
        					}
							@Override
							public void run() {
								System.out.printf("%s\n", line);
								if (textArea.getLineCount() > 500) {
									textArea.setText("");
								}
								textArea.append(this.line);
								textArea.append("\n");
                				textArea.setCaretPosition(textArea.getDocument().getLength());
                				textArea.update(textArea.getGraphics());
							}								
        			    };
        				SwingUtilities.invokeLater(new RunnableEx2(line));
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
		
		StringBuilder dev1 = new StringBuilder("");
		StringBuilder dev2 = new StringBuilder("");
		StringBuilder size = new StringBuilder("");
		StringBuilder mode = SetParameters(dev1, dev2, size);;
    	(new Thread(new RunnableEx(path.toString(), mode!= null ? mode.toString() : null, dev1.toString(), dev2.toString(), size.toString()))).start();
	}
	
	private void actionPerformedBtnStop() {
    	class RunnableEx implements Runnable {
    		@Override
			public void run() {
            	String[] cmd = new String[] 
            		{"taskkill.exe", "/im", "TestSuite_FT4232.exe", "/f", "/t"};
				try {
					Process p = (new ProcessBuilder(cmd)).start();
                	boolean ret = p.waitFor(1, TimeUnit.SECONDS);
                	if (ret != true) {
                    	textArea.setText("\n\nThe test is still running!\n");
                		BufferedWriter buf = new BufferedWriter(new OutputStreamWriter(p.getOutputStream()));
                		if (buf != null) {
                			for (int i=0; i<10; i++) {
                				buf.write("quit");
                            	textArea.append("\n\nForcing the test to abort...\n");
                				ret = p.waitFor(300, TimeUnit.MILLISECONDS);
                				if (ret) {
                					break;
                				}
                			}
                		}
                	}
                	textArea.append("\n\nThe test has been aborted successfully!\n");
				} catch (IOException e) {
					e.printStackTrace();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
    		}
    	};
    	(new Thread(new RunnableEx())).start();
	}
	
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == btnRun) {
			enableControls(false);
			actionPerformedBtnRun();
		}
		else if (e.getSource() == btnStop) {
			enableControls(true);
			actionPerformedBtnStop();
		}
	}
	
	private void initUI() {
	    
	    setTitle("Test Suite");
	    setSize(750, 550);
	    setLocationRelativeTo(null);
	    setDefaultCloseOperation(EXIT_ON_CLOSE);
	    setVisible(true);
	    setResizable(false);
	    getContentPane().setLayout(null);
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