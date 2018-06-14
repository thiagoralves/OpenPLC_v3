using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Set of useful extension methods
    /// </summary>
    public static class Extensions
    {
        public static void Each<T>(this IEnumerable<T> ie, Action<T> action)
        {            
            foreach (var e in ie) action(e);
        }

        public static void EachIndex<T>(this IEnumerable<T> ie, Action<T, int> action)
        {
            var i = 0;
            foreach (var e in ie) action(e, i++);
        }       
    }
}
