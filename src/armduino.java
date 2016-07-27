import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.WindowConstants;

import jssc.SerialPortException;

class GPanel extends JPanel{
	private static final long serialVersionUID = 5992612117451928397L;
	Color bgcolor = new Color((int)(Math.random()*255),(int)(Math.random()*255),(int)(Math.random()*255));

	public void paint(Graphics g){
		Graphics2D g2 = (Graphics2D)g;
		g2.setColor(bgcolor);
		g2.fillRect(0, 0, this.getWidth(), this.getHeight());
		g2.setColor(Color.WHITE);
		g2.drawString("U R NOW CONTROLLING 3D STAGE",10,100);
	}
	
	public void changeColor(){
		bgcolor = new Color((int)(Math.random()*255),(int)(Math.random()*255),(int)(Math.random()*255));
	}
}

public class armduino {
	
	static Arduino a;
	static JFrame frame;
	static GPanel panel = new GPanel();
	
	public static void send(KeyEvent e) throws SerialPortException{
		boolean changeColor = true;
		switch(e.getKeyCode()){
		case KeyEvent.VK_UP: a.move(0, 1, 0);break;
		case KeyEvent.VK_DOWN: a.move(0, -1, 0);break;
		case KeyEvent.VK_LEFT: a.move(-1, 0, 0);break;
		case KeyEvent.VK_RIGHT: a.move(1, 0, 0);break;
		case KeyEvent.VK_ENTER: a.move(0, 0, 5); break;
		case KeyEvent.VK_BACK_SPACE: a.move(0, 0, -5);break;
		case KeyEvent.VK_G: a.move(5, 5, 5); break;
		case KeyEvent.VK_C: a.calibrate(); break;
		default: changeColor = false;
		}
		if(changeColor){
			panel.changeColor();
			frame.repaint();
		}
	}

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		a = new Arduino("/dev/cu.usbmodem1421");
		
		frame = new JFrame("Window");
		frame.setContentPane(panel);
		frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
		frame.setSize(500, 200);
		frame.setVisible(true);
		frame.addKeyListener(new myListener());
		
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