using System;

namespace WinMessageRouter
{
	/// <summary>
	/// Summary description for LogEntry.
	/// </summary>
	public class LogEntry
	{
		public DateTime timestamp;
		public int logLevel;
		public string from = "";
		public string to = "";
		public string subject = "";
		public string msg = "";

		public LogEntry(int logLevel, string msg):this(logLevel, null, null, null, msg) 
		{
		}

		public LogEntry(int logLevel, string subject, string msg):this(logLevel, null, null, subject, msg)
		{
		}

		public LogEntry(int logLevel, string from, string to, string subject, string msg) 
		{
			timestamp = System.DateTime.Now;
			this.logLevel = logLevel;
			this.from = from;
			this.to = to;
			this.subject = subject;
			this.msg = msg;
		}
	}
}
