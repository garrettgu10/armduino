import java.util.LinkedList;
import java.util.Queue;

import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;

public class Arduino {
	private SerialPort serialPort; // the serial port communicator to the arduino
	boolean busy = false; // whether the arm is running or not
	private Queue<String> queue; // the queue for multiple commands, will finish execution before starting next function
	boolean initializing = true;

	// constructor for an Arduino object, takes one parameter of the serial port ID
	public Arduino(String portID) {
		busy = false;
		queue = new LinkedList<String>();
		serialPort = new SerialPort(portID);
		try {
			serialPort.openPort();// Open serial port
			serialPort.setParams(SerialPort.BAUDRATE_9600, SerialPort.DATABITS_8, 
					SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);// Set params. Also you can set params by the string:
																		//serialPort.setParams(9600, 8, 1, 0);
			serialPort.addEventListener(new SerialPortReader());
		} catch (SerialPortException ex) {
			System.out.println(ex);
		}
	}

	// writes a string to the arduino, private
	private void write(String string) throws SerialPortException {
		busy = true;
		serialPort.writeString(string);
	}
	
	public void absmove(float x, float y, float z) throws SerialPortException {
		String string = x + "," + y + "," + z + ",abs";
		addQueue(string);
	}

	// moves the arm to (x,y,z)
	public void move(float x, float y, float z) throws SerialPortException {
		String string = x + "," + y + "," + z;
		addQueue(string);
	}

	// moves the arm up, then to (x,y), then back down and turns on the vacuum pen
	public void pickup(float x, float y) throws SerialPortException {
		String string = x + "," + y + ",pickup";
		addQueue(string);
	}

	// moves the arm up, then to (x,y), then back down and turns off the vacuum pen
	public void dropoff(float x, float y) throws SerialPortException {
		String string = x + "," + y + ",dropoff";
		addQueue(string);
	}
	
	public void calibrate() throws SerialPortException {
		addQueue("cal");
	}
	
	public void vacon() throws SerialPortException {
		addQueue("vacon");
	}
	
	public void vacoff() throws SerialPortException {
		addQueue("vacoff");
	}

	// adds a command to the queue, private
	private void addQueue(String string) throws SerialPortException {
		if (busy && !initializing)
			queue.add(string);
		else
			write(string);
	}

	// runs the next item in the queue, private
	private void runQueue() throws SerialPortException {
		if (queue.peek() != null){
			write(queue.remove());
		}else
			busy = false;
	}

	// listener for arduino input, if a finish signal is communicated back, run the next item in the queue
	class SerialPortReader implements SerialPortEventListener {
		public void serialEvent(SerialPortEvent event) {
			if (event.isRXCHAR()) {// If data is available
				try {
					if (serialPort.readString(event.getEventValue()).equals("\n")) {
						initializing = false;
						runQueue();
					}
				} catch (SerialPortException e) {
					e.printStackTrace();
				}
			} else if (event.isCTS()) {// If CTS line has changed state
				if (event.getEventValue() == 1) {// If line is ON
					System.out.println("CTS - ON");
				} else {
					System.out.println("CTS - OFF");
				}
			} else if (event.isDSR()) {/// If DSR line has changed state
				if (event.getEventValue() == 1) {// If line is ON
					System.out.println("DSR - ON");
				} else {
					System.out.println("DSR - OFF");
				}
			}
		}
	}
}
