

import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class LabviewSocket {
	private PrintWriter writer;
	LabviewListener listener;
	ServerSocket serverSocket;
	
	public static void main(String[] args) throws IOException {
		LabviewSocket socket = new LabviewSocket(2287);
	}
	
	public LabviewSocket(int port) throws IOException {
		serverSocket = new ServerSocket(port);
		Socket clientSocket = serverSocket.accept();
		System.out.println("success");
		writer = new PrintWriter(clientSocket.getOutputStream());
		listener = new LabviewListener(clientSocket.getInputStream(), this);
		listener.run();
	}

	public void sendMessage(String message) {
		writer.write(message);
		writer.append('\n');
		writer.flush();
	}

	public void close() throws IOException {
		listener.close();
		serverSocket.close();
	}
}
