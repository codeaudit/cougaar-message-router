using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Collections;

namespace WinMessageRouter
{
	/// <summary>
	/// 
	/// </summary>
	public class MessageSender
	{
		private Thread threadRunner;
		private Socket ss;
		private Queue stack = new Queue();
		private bool keepRunning = true;
		private string name;
		private Mutex sendLock = new Mutex();
		private int maxSendQueueSize = Context.getInstance().getMaxSendQueueSize();

		public MessageSender(Socket sock)
		{
			this.ss = sock;
		}
		
		public void start() 
		{
			threadRunner = new Thread(new ThreadStart(run));
			threadRunner.Start();
		}

		public void stop() 
		{	
			keepRunning = false;
			if (threadRunner != null) 
			{
				threadRunner.Abort();
			}
		}

		public void addMessage(Message msg) 
		{
			if (keepRunning) 
			{
				if (stack.Count >= maxSendQueueSize) 
				{
					Context.getInstance().getLogger().log("Max Send Queue Size exceeded", name, Logger.LEVEL_DEBUG);
				}
				else 
				{
					stack.Enqueue(msg);
				}
			}
		}

		public void run() 
		{
			try 
			{
				while (keepRunning) 
				{
					while (this.stack.Count > 0) 
					{
						sendMessage((Message)stack.Dequeue());
					}
					Thread.Sleep(TimeSpan.FromMilliseconds(500));
				}
			}
			catch (Exception ex) 
			{
				Context.getInstance().getLogger().forceLog("MessageSender: " + ex.Message);
			}
		}

		public string getStats() 
		{
			string ret = "queue length: " + stack.Count;
			return ret;
		}

		public void sendMessage(Message msg) 
		{
			sendLock.WaitOne();
			try 
			{
				byte[] sendBuffer = new byte[ClientConnection.PACKET_HEADER_SIZE+msg.MessageData.Length];
				byte[] header = (byte[])msg.MessageHeader;
				for (int i=0; i<ClientConnection.PACKET_HEADER_SIZE; i++) 
				{
					sendBuffer[i] = header[i];
				}
				int index = ClientConnection.PACKET_HEADER_SIZE;
				char[] data = msg.MessageData.ToCharArray();
				for (int i=0; i<data.Length; i++) 
				{
					sendBuffer[index++] = (byte)data[i];
				}
				ss.Send(sendBuffer);
			}
			catch (Exception ex) 
			{
				Context.getInstance().getLogger().forceLog("Message Sender; Error in sendMessage(): " + ex.Message);
			}
			sendLock.ReleaseMutex();
		}

		public string Name
		{
			get
			{
				return name;
			}
			set
			{
				name = value;
			}
		}
	}
}
