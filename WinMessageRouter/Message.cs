using System;

namespace WinMessageRouter
{
	/// <summary>
	/// Summary description for Message.
	/// </summary>
	public class Message
	{
		private string from="";
		private string to="";
		private string thread="";
		private string body="";
		private string subject="";
		private byte[] msgHeader;
		private string msgData;

		public Message()
		{
		}

		public Message(Message m) 
		{
			this.from = m.from;
			this.to = m.to;
			this.thread = m.thread;
			this.subject = m.subject;
			this.body = m.body;
		}

		public Message(string from, string to, string subject, string thread, string body) 
		{
			this.from = from;
			this.to = to;
			this.subject = subject;
			this.thread = thread;
			this.body = body;
		}


		public string From
		{
			get
			{
				return from;
			}
			set
			{
				from = value;
			}
		}

		public string To
		{
			get
			{
				return to;
			}
			set
			{
				to = value;
			}
		}

		public string Thread
		{
			get
			{
				return thread;
			}
			set
			{
				thread = value;
			}
		}

		public string Body
		{
			get
			{
				return body;
			}
			set
			{
				body = value;
			}
		}

		public string Subject
		{
			get
			{
				return subject;
			}
			set
			{
				subject = value;
			}
		}

		public string MessageData
		{
			get
			{
				if (msgData == null) 
				{
					msgData = to+from+thread+subject+body;
				}
				return msgData;
			}
			set
			{
				msgData = value;
			}
		}

		public object MessageHeader
		{
			get
			{
				msgHeader = new byte[8];
				msgHeader[0] = (byte)to.Length;
				msgHeader[1] = (byte)from.Length;
				msgHeader[2] = (byte)thread.Length;
				msgHeader[3] = (byte)subject.Length;
				long bodyLength = body.Length;
				msgHeader[4] = (byte)((bodyLength & 0xff000000) >> 24);
				msgHeader[5] = (byte)((bodyLength & 0x00ff0000) >> 16);
				msgHeader[6] = (byte)((bodyLength & 0x0000ff00) >> 8);
				msgHeader[7] = (byte)(bodyLength & 0x000000ff);
				return msgHeader;
			}
			set
			{
			}
		}
	}
}
