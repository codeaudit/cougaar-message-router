using System;
using System.IO;
using System.Threading;
using System.Collections;

namespace WinMessageRouter
{
	/// <summary>
	/// 
	/// </summary>
	public class Logger
	{
		public const int LEVEL_INFO = 1;
		public const int LEVEL_WARN = 2;
		public const int LEVEL_DEBUG = 3;
		public const int LEVEL_SHOUT = 4;
		public string[] LEVEL_STRS = {"INFO", "WARN", "DEBUG", "SHOUT"};

		private bool enabled = false;
		private int currentLevel = LEVEL_INFO;
		private Thread threadRunner;
		private Stack stack = new Stack();
		private bool keepRunning = true;
		private string logFilePath = "messagerouter.log";

		public int CurrentLevel
		{
			get
			{
				return currentLevel;
			}
			set
			{
				currentLevel = value;
			}
		}

		public Logger()
		{
		}

		public void setLogFilePath(string path) 
		{
			logFilePath = path;
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

		public void run() 
		{
			while (keepRunning) 
			{
				Thread.Sleep(TimeSpan.FromMilliseconds(5000));
				while (keepRunning && (stack.Count > 0)) 
				{
					LogEntry entry = (LogEntry)stack.Pop();
					if (entry.logLevel >= currentLevel) 
					{
						writeLogEntry(entry);
					}
				}
			}
		}

		private void writeLogEntry(LogEntry entry) 
		{
			StreamWriter w = File.AppendText(logFilePath);

			if (entry.subject == "") //msg only format
			{ 
				w.WriteLine("{0} - {1}: {2}", entry.timestamp, getLevelStr(entry.logLevel), entry.msg);
			}
			else if (entry.to == "") // subject and msg format
			{
				w.WriteLine("{0} - {1}: {2}: {3}", entry.timestamp, getLevelStr(entry.logLevel), entry.subject, entry.msg);
			}
			else 
			{
				w.WriteLine("{0} - {1}: from: {2} - to: {3} - {4} : {5}", entry.timestamp, getLevelStr(entry.logLevel), entry.from, 
					entry.to, entry.subject, entry.msg);
			}
			w.Close();
		}

		public void addLogEntry(LogEntry entry) 
		{
			stack.Push(entry);
		}

		string getLevelStr(int level) 
		{
			return LEVEL_STRS[level];
		}

		public void enable() 
		{
			enabled = true;
		}

		public void disable() 
		{
			enabled = false;
		}

		public bool isenabled() 
		{
			return enabled;
		}
		
		public void forceLog(string msg) 
		{
			addLogEntry(new LogEntry(LEVEL_DEBUG, msg));
		}

		public void log(string msg, int level)
		{
			if (enabled) 
			{
				addLogEntry(new LogEntry(level, msg));
			}
		}

		public void log(string subject, string msg, int level)
		{
			if (enabled) 
			{
				addLogEntry(new LogEntry(level, subject, msg));				
			}		
		}

		public void log(string from, string to, string subject, string msg, int level) 
		{
			if (enabled) 
			{
				addLogEntry(new LogEntry(level, from, to, subject, msg));
			}
		}
	}
}
