using System;
using System.Threading;

namespace WinMessageRouter
{
	/// <summary>
	/// Summary description for ServerStats.
	/// </summary>
	public class ServerStats
	{
		private const int MAX_INTERVALS = 180;
		private const int SLEEP_INTERVAL = 5000;

		private static ServerStats currentInstance;
		private Thread threadRunner;
		private long incomingMsgCount;
		private long outgoingMsgCount;
		private long prevCount;
		private Mutex inlock = new Mutex();
		private Mutex outlock = new Mutex();
		private bool keepRunning = true;
		private double raw, avg1, avg2, avg3, max;
		private DateTime startTime;
		private long[] diffQueue = new long[MAX_INTERVALS];
		private long cycleCount;

		public ServerStats()
		{
			incomingMsgCount = outgoingMsgCount = prevCount = cycleCount = 0;
			raw = avg1 = avg2 = avg3 = max = 0.0;
			startTime = DateTime.Now;
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

		public void run() 
		{
			long sum = 0;
			long intervalCount = 0;

			while (keepRunning) 
			{
				Thread.Sleep(SLEEP_INTERVAL);
				diffQueue[cycleCount] = incomingMsgCount - prevCount;
				prevCount = incomingMsgCount;

				//calculate raw rate
				raw = (double)diffQueue[cycleCount]/(double)(SLEEP_INTERVAL/1000);
				max = raw>max?raw:max;
                
				//calculate 1 minute avg
				sum = 0;
				intervalCount = (long) (60000/SLEEP_INTERVAL);
				for (int i=0; i<intervalCount; i++) 
				{
					sum += diffQueue[(cycleCount + MAX_INTERVALS - i)%MAX_INTERVALS];
				}
				avg1 = (double)sum/60;  //msgs per sec over last minute

				//calculate 5 minute avg
				sum = 0;
				intervalCount = (long) (300000/SLEEP_INTERVAL);
				for (int i=0; i<intervalCount; i++) 
				{
					sum += diffQueue[(cycleCount + MAX_INTERVALS - i)%MAX_INTERVALS];
				}
				avg2 = (double)sum/300;  

				//calculate the total minute avg
				sum = 0;
				for (int i=0; i<MAX_INTERVALS; i++) { sum += diffQueue[i]; }
				avg3 = (double)sum/((SLEEP_INTERVAL*MAX_INTERVALS)/1000);

				//printf("cycle: %d  avg1: %f  avg2: %f  avg3: %f  max: %f\n", cycleCount, avg1, avg2, avg3, max);
				cycleCount = (cycleCount + 1) % MAX_INTERVALS;  //adjust the cycle count

				if (cycleCount %60 == 0) 
				{
					if (Context.getInstance().getStatLogging()) 
					{
						logStats();
					}
				}
			}
		}

		public void incrementIncomingMsgCount() 
		{
			inlock.WaitOne();
			incomingMsgCount++;
			inlock.ReleaseMutex();
		}

		public void incrementOutgoingMsgCount() 
		{
			outlock.WaitOne();
			outgoingMsgCount++;
			outlock.ReleaseMutex();
		}
			
		public long getIncomingMsgCount() 
		{
			return incomingMsgCount;
		}

		public long getOutgoingMsgCount() 
		{
			return outgoingMsgCount;
		}

		public void resetStats() 
		{
			incomingMsgCount = outgoingMsgCount = 0;
		}

		public string getStatsStr() 
		{
			TimeSpan uptime = new TimeSpan(DateTime.Now.Ticks - startTime.Ticks);

			return "\nstart time: " + startTime.ToString() +
				   "\nup time: " + uptime.ToString() + 
				   "\nincoming msg count: " + incomingMsgCount + 
				   "\noutgoing msg count: " + outgoingMsgCount +
				   "\nraw: " + raw + 
				   "\n1 min: " + avg1 +
				   "\n5 min: " + avg2 +
				   "\n15 min: " + avg3 +
				   "\nmax: " + max;
		}

		public static ServerStats getInstance() 
		{
			if (currentInstance == null) 
			{
				currentInstance = new ServerStats();
				currentInstance.start();
			}
			return currentInstance;
		}

		private void logStats() 
		{
			Context ctx = Context.getInstance();
			Logger lgr = ctx.getLogger();
			lgr.log("Server Stats", getStatsStr(), Logger.LEVEL_DEBUG);
			lgr.log("Connection Stats", ctx.getConnectionRegistry().getConnectionStatsStr(), Logger.LEVEL_DEBUG);
			lgr.log("Send Queue Stats", ctx.getConnectionRegistry().getSendQueueStats(), Logger.LEVEL_DEBUG);
			lgr.log("Connection Profiles", ctx.getConnectionRegistry().getConnectionProfiles(), Logger.LEVEL_DEBUG);
		}
	}
}
