using System;

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
			System.Console.WriteLine(System.DateTime.Now.ToLongTimeString() + " - " + msg);
		}

		public void log(string msg, int level)
		{
			if (enabled) 
			{
				System.Console.WriteLine(System.DateTime.Now.ToLongTimeString() + " - " + getLevelStr(level) +": " + msg);
			}
		}

		public void log(string subject, string msg, int level)
		{
			if (enabled) 
			{
				System.Console.WriteLine(System.DateTime.Now.ToLongTimeString() + " - " + getLevelStr(level) + ": " + subject + " : " + msg);
			}		
		}

		public void log(string from, string to, string subject, string msg, int level) 
		{
			if (enabled) 
			{
				System.Console.WriteLine(System.DateTime.Now.ToLongTimeString() + " - " + getLevelStr(level) + ": " + "from: " + from +
					" - to: " + to + " - subject: " + subject + " : " + msg);
			}
		}
	}
}
