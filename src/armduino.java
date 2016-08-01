import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Scanner;

import javax.swing.*;

import jssc.SerialPortException;

public class armduino {
	
	Arduino a;
	JFrame keyFrame;
	GPanel keyPanel;
	JTextField[] textFields;
	boolean vacuumOn = false;
	
	public void send(KeyEvent e) throws SerialPortException{
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
		case KeyEvent.VK_V: if(vacuumOn){
			a.vacoff();
		}else{
			a.vacon();
		}
		vacuumOn = !vacuumOn;
		break;
		default: changeColor = false;
		}
		if(changeColor){
			keyPanel.changeColor();
			keyFrame.repaint();
		}
	}
	
	public float parseFloat(String s, List<String> variables){
		//this function is basically a parseFloat function that also takes into account the variables in the list.
		try{
			if(s.substring(0,3).equals("VAR")){
				try{
					return Float.parseFloat(variables.get(Integer.parseInt(s.substring(3))-1));
				}catch(IndexOutOfBoundsException e){
					System.out.println("Variable not found, replacing with 0");
					return 0;
				}
			}
		}catch (StringIndexOutOfBoundsException e){
			return Float.parseFloat(s);
		}
		return Float.parseFloat(s);
	}
	
	enum commandResult{PASS,ERROR,EXIT,CRITICAL,COMMENT};
	
	private commandResult execCommand(String[] props, String tabs, List<String> vars) throws SerialPortException, FileNotFoundException, InterruptedException{
		if(props[0].equals("REQUIRE")){
			int required = (int)parseFloat(props[1],vars); //required number of variables in assertion
			if(required > vars.size()){
				System.out.println(tabs+"Error: Inclusion requires "+(int)parseFloat(props[1],vars)+" variables.");
				return commandResult.CRITICAL;
			}
		}else if(props[0].equals("GOTO")){
			a.absmove(parseFloat(props[1],vars), parseFloat(props[2],vars), parseFloat(props[3],vars));
		}
		else if(props[0].equals("MOVE")){
			a.move(parseFloat(props[1],vars), parseFloat(props[2],vars), parseFloat(props[3],vars));
		}
		else if(props[0].equals("CALIBRATE")){
			a.calibrate();
		}
		else if(props[0].equals("WAIT")){
			Thread.sleep((long)parseFloat(props[1],vars));
		}else if(props[0].equals("RIGHT")){
			a.move(parseFloat(props[1],vars), 0, 0);
		}
		else if(props[0].equals("LEFT")){
			a.move(-parseFloat(props[1],vars), 0, 0);
		}
		else if(props[0].equals("UP")){
			a.move(0, 0, parseFloat(props[1],vars));
		}
		else if(props[0].equals("DOWN")){
			a.move(0, 0, -parseFloat(props[1],vars));
		}
		else if(props[0].equals("FORWARD")){
			a.move(0, parseFloat(props[1],vars), 0);
		}
		else if(props[0].equals("BACK")){
			a.move(0, -parseFloat(props[1],vars), 0);
		}
		else if(props[0].equals("VACON")){
			a.vacon();
		}
		else if(props[0].equals("VACOFF")){
			a.vacoff();
		}
		else if(props[0].equals("INCLUDE")){
			//passedVariables is a list of the variables that can appear after the file name
			ArrayList<String> passedVariables = new ArrayList<String> (Arrays.asList(props));
			passedVariables.remove(0);
			passedVariables.remove(0);
			execFile(new FileInputStream(props[1]),tabs.length()+1,passedVariables);
		}
		else if(props[0].equals("EXIT")){
			return commandResult.EXIT;
		}
		else if(props[0].charAt(0) == '%'){ //if the first character is %, the line is a comment
			return commandResult.COMMENT;
		}else{
			return commandResult.ERROR;
		}
		return commandResult.PASS;
	}
	
	PrintWriter pw = null; //this is what we use to write console commands to file
	Thread shutDown = new Thread(){ //the code in this thread is executed on exit
		public void run(){
			if(pw!=null){
				pw.close();
			}
		}
	};
	
	public void execFile(InputStream in, int nestedLevel, List<String> vars) throws FileNotFoundException{ 
		//nestedLevel indicates how many tabs to place in front of each printed line
		Scanner s = new Scanner(in);
		int linenum = 1;
		boolean isConsoleInput = in.equals(System.in);
		
		String tabs = new String(new char[nestedLevel]).replace("\0", "\t");
		System.out.print(tabs+linenum+": ");
		
		boolean writeToFile = false;
		
		if(in.equals(System.in)){ //from command input
			writeToFile = true;
			int i; //Each new file must be written to a new location, i is the smallest number where a file commands(i).arm doesn't exist.
			for(i = 0; new File("console/commands"+i+".arm").exists(); i++){}
			pw = new PrintWriter("console/commands"+i+".arm");
		}
		while(s.hasNextLine()){
			linenum++;
			String ln = s.nextLine();
			String[] props = ln.split(" ");
			if(pw!=null && writeToFile){
				pw.write(ln+"\n");
			}
			
			if(!isConsoleInput){
				System.out.print(ln);
				System.out.println();
			}
			
			try{
				if(ln.isEmpty()){
					return;
				}
				else{
					commandResult result = execCommand(props, tabs, vars);
					if(result==commandResult.ERROR){
						System.out.println(tabs+"Command invalid on line "+linenum+":");
						System.out.println(tabs+"\t"+ln);
					}else if(result==commandResult.CRITICAL || result == commandResult.EXIT){
						break;
					}
				}
			}catch(Exception e){
				System.out.println(tabs+"ERROR: LINE "+linenum);
				System.out.println(tabs+ln);
				e.printStackTrace();
			}
			while(a.busy){
				try {
					Thread.sleep(10);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			if(isConsoleInput || s.hasNextLine()){
				System.out.print(tabs+linenum+": ");
			}
		}
		s.close();
		if(pw!=null && writeToFile){
			pw.close();
		}
	}
	
	public void openConsole(){
		System.out.println("Enter commands. EXIT to finish and save.");
		try {
			execFile(System.in,0, new ArrayList<String>());
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
	
	public void start(){
		a = new Arduino("/dev/cu.usbmodem1421"); //Change this string when changing Arduinos
		
		Runtime.getRuntime().addShutdownHook(shutDown);
		
		keyPanel = new GPanel();
		
		keyFrame = new JFrame("Window");
		keyFrame.setContentPane(keyPanel);
		keyFrame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
		keyFrame.setSize(500,200);
		keyFrame.setVisible(true);
		keyFrame.addKeyListener(new myListener());
		
		try {
			Thread.sleep(200);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}
		
		openConsole();
	}
	
	public static void main(String[] args) {
		armduino arm = new armduino();
		arm.start();
	}
	class myListener implements KeyListener{

		@Override
		public void keyTyped(KeyEvent e) {}

		@Override
		public void keyPressed(KeyEvent e) {}

		@Override
		public void keyReleased(KeyEvent e) {
			try {
				send(e);
			} catch (SerialPortException e1) {
				e1.printStackTrace();
			}
		}
	}
}