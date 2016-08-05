

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Scanner;

public class LabviewListener implements Runnable {
	private Scanner scanner;
	private boolean closed = false;
	private LabviewSocket socket;

	public LabviewListener(InputStream inputStream, LabviewSocket socket) {
		scanner = new Scanner(new BufferedReader(new InputStreamReader(
				inputStream)));
		this.socket = socket;
	}

	public void close() {
		closed = true;
	}

	@Override
	public void run() {
		while (!closed) {
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			if (scanner.hasNext()) {
				try {
					processInput(scanner.nextLine());
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}

	/**
	 * Processes newline terminated input from labview
	 * 
	 * @param nextLine
	 * @throws IOException 
	 */
	private void processInput(String nextLine) throws IOException {
		if(nextLine.equals("good")){
			socket.sendMessage("abort");
		}
		else if(nextLine.equals("bad")){
			
		}
		else if(nextLine.equals("abort")){
			socket.close();
		}
		System.out.println(nextLine);
	}

}
