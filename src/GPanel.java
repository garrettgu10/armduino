import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;

import javax.swing.JPanel;

public class GPanel extends JPanel{
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