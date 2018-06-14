using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatak.DNP3.Interface
{
    /// <summary>
    /// Transform
    /// </summary>
    public class TaskCompletionProxy : ITaskCallback
    {
        readonly TaskCompletionSource<TaskCompletion> completionSource = new TaskCompletionSource<TaskCompletion>();

        public TaskCompletionProxy(ITaskCallback proxy)
        {
            this.proxy = proxy;
        }

        public Task<TaskCompletion> CompletionTask
        {
            get 
            {
                return completionSource.Task;
            }
        }

        void ITaskCallback.OnStart()
        {
            if (proxy != null)
            {
                proxy.OnStart();
            }
        }

        void ITaskCallback.OnComplete(TaskCompletion result)
        {
            completionSource.TrySetResult(result);

            if (proxy != null)
            {
                proxy.OnComplete(result);
            }            
        }

        void ITaskCallback.OnDestroyed()
        {
            if (proxy != null)
            {
                proxy.OnDestroyed();
            }
        }

        readonly ITaskCallback proxy;
    }
}
