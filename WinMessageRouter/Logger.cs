using System;

namespace WinMessageRouter
{
	/// <summary>
	/// 
	/// </summary>
	public class Logger
	{
		private bool enabled = false;
		public Logger()
		{
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

		public void log(string msg)
		{
			if (enabled) 
			{
				System.Console.WriteLine(System.DateTime.Now.ToLongTimeString() + " - " + msg);
			}
		}

		public void log(string subject, string msg)
		{
			if (enabled) 
			{
				System.Console.WriteLine(System.DateTime.Now.ToLongTimeString() + " - " + subject + " : " + msg);
			}		
		}

		public void log(string from, string to, string subject, string msg) 
		{
			System.Console.WriteLine(System.DateTime.Now.ToLongTimeString() + " - " + "from: " + from +
				" - to: " + to + " - subject: " + subject + " : " + msg);
		}
	}
}
