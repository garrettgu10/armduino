import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.Scanner;

import javax.swing.*;

import jssc.SerialPortException;

class GPanel extends JPanel{
	private static final long serialVersionUID = 5992612117451928397L;
	Color bgcolor = new Color((int)(Math.random()*255),(int)(Math.random()*255),(int)(Math.random()*255));
	
	public void paint(Graphics g){
		//JPanel paint function
		Graphics2D g2 = (Graphics2D)g;
		g2.setColor(bgcolor);
		g2.fillRect(0, 0, this.getWidth(), this.getHeight());
		g2.setColor(Color.WHITE);
		g2.drawString("U R NOW CONTROLLING 3D STAGE",10,100);
	}
	
	public void changeColor(){ //changes background color
		bgcolor = new Color((int)(Math.random()*255),(int)(Math.random()*255),(int)(Math.random()*255));
	}
}

public class armduino {
	
	static Arduino a;
	static JFrame keyFrame;
	static GPanel keyPanel;
	
	public static void send(KeyEvent e) throws SerialPortException{
		boolean changeColor = true;
		switch(e.getKeyCode()){
		case KeyEvent.VK_UP: a.move(0, 0.5f, 0);break;
		case KeyEvent.VK_DOWN: a.move(0, -0.5f, 0);break;
		case KeyEvent.VK_LEFT: a.move(-0.5f, 0, 0);break;
		case KeyEvent.VK_RIGHT: a.move(0.5f, 0, 0);break;
		case KeyEvent.VK_ENTER: a.move(0, 0, 0.5f); break;
		case KeyEvent.VK_BACK_SPACE: a.move(0, 0, -0.5f);break;
		case KeyEvent.VK_G: a.move(5, 5, 5); break;
		case KeyEvent.VK_C: a.calibrate(); break;
		case KeyEvent.VK_V: a.addQueue("vacon"); 
		break;
		default: changeColor = false;
		}
		if(changeColor){
			keyPanel.changeColor();
			keyFrame.repaint();
		}
	}

	public static void main(String[] args) {
		a = new Arduino("/dev/cu.usbmodem1421"); //Change this string when changing Arduinos
		
		keyPanel = new GPanel();
		
		keyFrame = new JFrame("Window");
		keyFrame.setContentPane(keyPanel);
		keyFrame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
		keyFrame.setSize(500,200);
		keyFrame.setVisible(true);
		keyFrame.addKeyListener(new myListener());
		
		
		while(true){
			Scanner s = new Scanner(System.in);
			System.out.print("Enter x coord: ");
			float x = s.nextFloat();
			System.out.print("Enter y coord: ");
			float y = s.nextFloat();
			System.out.print("Enter z coord: ");
			float z = s.nextFloat();
			try {
				a.absmove(x, y, z);
			} catch (SerialPortException e) {
				e.printStackTrace();
			}
		}
	}
}

class myListener implements KeyListener{

	@Override
	public void keyTyped(KeyEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void keyPressed(KeyEvent e) {
		// TODO Auto-generated method stub
	}

	@Override
	public void keyReleased(KeyEvent e) {
		// TODO Auto-generated method stub
		try {
			armduino.send(e);
		} catch (SerialPortException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
	}
}