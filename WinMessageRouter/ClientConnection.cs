using System;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Collections;
using System.Threading;

namespace WinMessageRouter
{
	/// <summary>
	/// 
	/// </summary>
	public class ClientConnection
	{
		public static int PACKET_HEADER_SIZE = 8;
		public static int MAX_BUF_SIZE = 2048;
		public static string VERSION = "Windows Message Router 1.2";

		private Socket ss;
		private MessageSender sender;
		private byte[] packetBuffer;
		private long packetBufferPos;
		private string tmpBuffer;
		private byte[] packetData;
		private long  packetBufferSize;
		private bool use_block_read;
		private string name;
		private Thread threadRunner;

		private static const string CMD_LIST = 
		"list - get a list of connected clients\n"+
		"register - register for online/offlien updates for all clients\n"+
		"deregister - deregister for online/offline updates\n"+
		"validateConnections - force valdiation of all connected clients\n"+
		"version - get the version of the server\n"+
		"enable eavesdropping - enable eavesdropping mode\n"+
		"disable eavesdropping - disable eavesdropping mode\n"+
		"eavesdrop - eavesdrop on a designated client (body of message must have client name)\n"+
		"uneavesdrop - uneavesdrop on designated client (body of message must have client name)\n"+
		"uneavesdropall - uneavesdrop on all clients currently being eavesdropped on\n"+
		"gloabaleavesdrop - eavesdrop on all clients\n"+
		"unglobaleavesdrop - uneavesdrop on all clients\n"+
		"enable error messages - enable error messages to be returned to clients\n"+
		"disable error messages - prevent error messages from being returned to clients\n"+
		"get stats - get current server statistics\n"+
		"reset stats - reset server statistics\n"+
		"kill sender - kill the send queue of a designated client (body of message must have client name)\n"+
		"kill connection - kill the designated client connection (body of message must have client name)\n"+
		"enable logging - enable the logging subsystem\n"+
		"disable logging - disable the logging subsystem\n"+
		"set log level <info|warn|debug|shout> - set the logging level\n"+
		"help - get this message";

		public ClientConnection(Socket sock, bool blockread)
		{
			ss = sock;
			use_block_read = blockread;
			sender = new MessageSender(sock);
			sender.start();
			packetBufferPos = 0;
			packetBufferSize = 0;
		}

		public void start() 
		{
			threadRunner = new Thread(new ThreadStart(run));
			threadRunner.Start();
		}

		public void stop() 
		{
			if (threadRunner != null) 
			{
				threadRunner.Abort();
			}
		}

		private Message getMessage() 
		{
			byte[] packet_header = new byte[PACKET_HEADER_SIZE];

			getHeaderData(ref packet_header, PACKET_HEADER_SIZE);
  
			//examine packet header to determine total message length
			byte toLength = packet_header[0];
			byte fromLength = packet_header[1];
			byte threadLength = packet_header[2];
			byte subjectLength = packet_header[3];
			long bodyLength =0;
			bodyLength |= packet_header[4];
			bodyLength <<= 8;
			bodyLength |= packet_header[5];
			bodyLength <<= 8;
			bodyLength |= packet_header[6];
			bodyLength <<= 8;
			bodyLength |= packet_header[7];
			long totalLength = toLength+fromLength+threadLength+subjectLength+bodyLength;
			packetData = new byte[totalLength]; 

			getData(ref packetData, totalLength);

			//now construct the Message object
			Message msg = new Message();
			int index = 0;
			string messagedata = Encoding.ASCII.GetString(packetData);
			if (toLength != 0) 
			{
				msg.To = messagedata.Substring(0, toLength);
				index += toLength;
			}
			if (fromLength != 0) 
			{
				msg.From = messagedata.Substring(index, fromLength);
				index += fromLength;
			}
			if (threadLength != 0) 
			{
				msg.Thread = messagedata.Substring(index, threadLength);
				index += threadLength;
			}
			if (subjectLength != 0) 
			{
				msg.Subject = messagedata.Substring(index, subjectLength);
				index += subjectLength;
			}
			if (bodyLength != 0) 
			{
				msg.Body = messagedata.Substring(index, (int)bodyLength);
			}
  
			return msg;
		}

		private void routeMessage(Message msg) 
		{
			ClientConnection targetConnection = Context.getInstance().getConnectionRegistry().findConnection(msg.To);
			if (targetConnection != null) 
			{
				targetConnection.sendMessage(msg);
			}
			else 
			{
				Message reply = new Message();
				reply.Subject = "ERROR";
				reply.To = msg.From;
				reply.Body = "Unknown client: " + msg.To;
				sendMessage(reply);
			}
		}

		private bool processMessage(Message msg) 
		{
			if ((msg.To != null) && (msg.To != ""))
			{
				Context.getInstance().getLogger().log(msg.From, msg.To, msg.Subject, msg.Body, Logger.LEVEL_INFO);
				routeMessage(msg);
				return true;
			}
			else 
			{
				return handleMessage(msg);
			}
		}

		private void registerClient(string name) 
		{
			this.name = name;
			sender.Name = name;
			Context.getInstance().getConnectionRegistry().registerConnection(this, name);
			//notify any listeners of this registration
			Message msg = new Message();
			msg.Subject = "online";
			msg.Body = name;
			Context.getInstance().getListenerRegistry().notifyListeners(msg);
		}

		private bool handleMessage(Message msg) 
		{
			string subject = msg.Subject;
			Context.getInstance().getLogger().log(msg.From, "server", msg.Subject, msg.Body, Logger.LEVEL_INFO);
			try 
			{  
				Message reply = new Message();
				reply.Thread = msg.Thread;
				if (subject == "connect") 
				{  //handle the connect request
					reply.Subject = "connected";
					reply.To = msg.Body;
					registerClient(msg.Body);
				}
				else if (subject == "disconnect") 
				{
					return false;
				}
				else if (subject == "list") 
				{  //handle the list request
					reply.To = msg.From;
					reply.Subject = "list";
					string list = Context.getInstance().getConnectionRegistry().listConnections();
					Context.getInstance().getLogger().log("Current List", list, Logger.LEVEL_INFO);
					reply.Body = list;
				}
				else if (subject == "register") 
				{
					Context.getInstance().getListenerRegistry().registerListener(this);
					reply.To = msg.From;
					reply.Subject = "registered";
				}
				else if (subject == "deregister") 
				{
					Context.getInstance().getListenerRegistry().deregisterListener(this);
					reply.To = msg.From;
					reply.Subject = "deregistered";
				}
				else if (subject == "version") 
				{
					reply.To = msg.From;
					reply.Subject = "version";
					reply.Body = VERSION;
				}
				else if (subject == "eavesdrop") 
				{
				}
				else if (subject == "globaleavesdrop") 
				{
				}
				else if (subject == "uneavesdrop") 
				{
				}
				else if(subject == "uneavesdropall") 
				{
				}
				else if (subject == "unglobaleavesdrop")
				{
				}
				else if (subject == "enable eavesdropping") 
				{
				}
				else if (subject == "disable eavesdropping")
				{
				}
				else if (subject == "enable error messages") 
				{
				}
				else if (subject == "disable error messages") 
				{
				}
				else if (subject == "get stats") 
				{
				}
				else if (subject == "reset stats") 
				{
				}
				else if (subject == "kill connection") 
				{
				}
				else if (subject == "set log level") 
				{
					if (subject.IndexOf("info") >= 0) 
					{
						Context.getInstance().getLogger().CurrentLevel = Logger.LEVEL_INFO;
						reply.Subject = "log level set to INFO";
					}
					else if (subject.IndexOf("warn") >= 0) 
					{
						Context.getInstance().getLogger().CurrentLevel = Logger.LEVEL_WARN;
						reply.Subject = "log level set to WARN";
					}
					else if (subject.IndexOf("debug") >= 0) 
					{
						Context.getInstance().getLogger().CurrentLevel = Logger.LEVEL_DEBUG;
						reply.Subject = "log level set to DEBUG";
					}
					else if (subject.IndexOf("shout") >= 0) 
					{
						Context.getInstance().getLogger().CurrentLevel = Logger.LEVEL_SHOUT;
						reply.Subject = "log level set to SHOUT";
					}
					else 
					{
						reply.Subject = "unknown log level";
					}
					 reply.To = msg.From;
				}
				else if (subject == "enable logging") 
				{
					Context.getInstance().getLogger().enable();
					reply.Subject = "logging enabled";
					reply.To = msg.From;
				}
				else if (subject == "disable logging") 
				{
					Context.getInstance().getLogger().disable();
					reply.Subject = "logging disabled";
					reply.To = msg.From;
				}
				else if (subject == "help") 
				{
					reply.To = msg.From;
					reply.Subject = "Command List";
					reply.Body = CMD_LIST;
				}
				else 
				{ //send an error reply
					reply.To = msg.From;
					reply.Subject = "ERROR";
					reply.Body = "Unknown command";
				}
				sendMessage(reply);
			}
			catch (Exception ex) 
			{
				Context.getInstance().getLogger().forceLog(ex.Message);
			}
			return true;

		}

		private void getData(ref byte[] buffer, long size) 
		{
			byte[] tmp_buffer = new byte[size];
			int index = 0;
			while (index < size) 
			{
				int recv_size = ss.Receive(tmp_buffer, 0, (int)(size-index), SocketFlags.None);
				pack(tmp_buffer, 0, recv_size, ref buffer, index);
				index += recv_size;
			}
		}

		private void getHeaderData(ref byte[] buffer, int size) 
		{
			byte[] tmp_buffer = new byte[size];
			int index = 0;
			while (index < size) 
			{
				int recv_size = ss.Receive(tmp_buffer, 0, (int)(size-index), SocketFlags.None);
				pack(tmp_buffer, 0, recv_size, ref buffer, index);
				index += recv_size;
			}
		}

		public void run() 
		{
			Message msg;
			bool keepRunning = true;
			
			try 
			{
				while (keepRunning) 
				{
					if (use_block_read) 
					{
						Queue messages = getMessages();
						while (messages.Count > 0) 
						{
							msg = (Message)messages.Dequeue();
							keepRunning = processMessage(msg);
							if (!keepRunning) 
							{
								break;
							}
						}
					}
					else 
					{
						msg = getMessage();
						if (msg != null) 
						{
							keepRunning = processMessage(msg);
						}
					}
				}
			}
			catch (Exception ex) 
			{
				Context.getInstance().getLogger().log("Socket Exception", ex.Message, Logger.LEVEL_DEBUG);
			}
			Context.getInstance().getListenerRegistry().deregisterListener(this);
			deregisterClient();
			if (sender != null) 
			{
				sender.stop();
			}

			return;
		}

		public void sendMessage(Message msg) 
		{
			sender.addMessage(msg);
		}

		public void deregisterClient() 
		{
			Context.getInstance().getConnectionRegistry().deregisterConnection(this.name);
			//notify listeners
			Message msg = new Message();
			msg.Subject = "offline";
			msg.Body = name;
			Context.getInstance().getListenerRegistry().notifyListeners(msg);  
		}

		public Queue getMessages() 
		{
			byte[] readbuffer = new byte[MAX_BUF_SIZE];
			Queue messages = new Queue();
			//we want to fill the packetBuffer with as much data as we can get
			long actualSize = ss.Receive(readbuffer,0, MAX_BUF_SIZE, SocketFlags.None); 
			long residualPacketLength = packetBufferSize-packetBufferPos;
			byte[] tmpBuf = new byte[actualSize + residualPacketLength];  //create a temporary buffer to hold the data;
			//since the tmpBuf will become the packetBuffer, set the size now
			packetBufferSize = actualSize + residualPacketLength;  
			//pack the residual data from the packet buffer into the tmp buffer
			pack(packetBuffer, (int)packetBufferPos, (int)residualPacketLength, ref tmpBuf, 0);
			//pack the newly read data into the tmp buffer 
			pack(readbuffer, 0, (int)actualSize, ref tmpBuf, (int)residualPacketLength);
			packetBuffer = tmpBuf;  //set the packet buffer to the tmp buffer

			packetBufferPos = 0;  //set the packetBufferPos to the beginning of the buffer
			//if there aren't even 8 bytes in the buffer there's not even enough for a header so
			//there's no use in parsing it yet
			if (packetBufferSize < PACKET_HEADER_SIZE) 
			{
				return messages;
			}
  
			//now parse through the packet buffer to pull out as many messages as possible
			while ((packetBufferPos+PACKET_HEADER_SIZE) < packetBufferSize) 
			{ //make sure there's enough room for the next packet header
				//process the current packet header
				byte toLength = packetBuffer[packetBufferPos]; packetBufferPos++;
				byte fromLength = packetBuffer[packetBufferPos]; packetBufferPos++;
				byte threadLength = packetBuffer[packetBufferPos]; packetBufferPos++;
				byte subjectLength = packetBuffer[packetBufferPos]; packetBufferPos++;
				long bodyLength =0;
				bodyLength |= packetBuffer[packetBufferPos]; packetBufferPos++;
				bodyLength <<= 8;
				bodyLength |= packetBuffer[packetBufferPos]; packetBufferPos++;
				bodyLength <<= 8;
				bodyLength |= packetBuffer[packetBufferPos]; packetBufferPos++;
				bodyLength <<= 8;
				bodyLength |= packetBuffer[packetBufferPos]; packetBufferPos++;
				long totalLength = toLength+fromLength+threadLength+subjectLength+bodyLength;
				//check to make sure the remainder of the packetBuffer fully contains this packet
				if ((packetBufferSize-packetBufferPos) < totalLength) 
				{
					packetBufferPos -= PACKET_HEADER_SIZE; //step back to tbe beginning of this packet
					return messages;  //return the current list of messages
				}
				//otherwise, we need to constuct the Message
				Message msg = new Message();
				byte[] data = createSubStr(packetBuffer, (int)packetBufferPos, (int)totalLength);
				string packetBufferStr = Encoding.ASCII.GetString(data);
				int pos = 0;
				if (toLength != 0) 
				{
					msg.To = packetBufferStr.Substring(pos, toLength);
					pos += toLength;
				}
				if (fromLength != 0) 
				{
					msg.From = packetBufferStr.Substring(pos, fromLength);
					pos += fromLength;
				}
				if (threadLength != 0) 
				{
					msg.Thread = packetBufferStr.Substring(pos, threadLength);
					pos += threadLength;
				}
				if (subjectLength != 0) 
				{
					msg.Subject = packetBufferStr.Substring(pos, subjectLength);
					pos += subjectLength;
				}
				if (bodyLength != 0) 
				{
					msg.Body = packetBufferStr.Substring(pos, (int)bodyLength);
					pos += (int)bodyLength;
				}
				messages.Enqueue(msg);
				packetBufferPos += totalLength;  //update the packet buffer pos
			}
			return messages;

		}

		public void pack(byte[] src, int srcStartPos, int srcLength, ref byte[] dest, int destStartPos) 
		{
			int index = destStartPos;
			for (int i=srcStartPos; i<srcLength; i++) 
			{
				dest[index] = src[i];
				index++;
			}
		}

		private byte[] createSubStr(byte[] src, int start, int length) 
		{
			byte[] ret = new byte[length];
			for (int i=0; i<length; i++) 
			{
				ret[i] = src[i+start];
			}
			return ret;
		}
	}
}
