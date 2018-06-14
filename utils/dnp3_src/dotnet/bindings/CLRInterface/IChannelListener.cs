using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Callbacks for a channel
    /// </summary>
    public interface IChannelListener
    {
        /// <summary>
        /// Invoked when the ChannelState changes
        /// </summary>
        /// <param name="state">the new state of the channel</param>
        void OnStateChange(ChannelState state);
    }

    public static class ChannelListener
    {
        private class PrintingChannelListener : IChannelListener
        {
            void IChannelListener.OnStateChange(ChannelState state)
            {
                Console.WriteLine("Channel state: " + state);
            }
        }

        private class NullChannelListener : IChannelListener
        {
            void IChannelListener.OnStateChange(ChannelState state) { }            
        }

        public static IChannelListener Print()
        {
            return new PrintingChannelListener();
        }

        public static IChannelListener None()
        {
            return new NullChannelListener();
        }
    }
}
