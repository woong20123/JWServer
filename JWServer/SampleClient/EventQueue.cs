using System;
using System.Collections.Generic;
using System.Linq;
using System.Printing;
using System.Text;
using System.Threading.Tasks;

namespace SampleClient
{
    public interface AsyncEventBase
    {
        public void Invoke();
        public string GetName();
    }

    class asyncEvent : AsyncEventBase
    {
        private Action _callBack;
        private readonly string _name;

        public asyncEvent(Action callBack, string name)
        {
            this._callBack = callBack;
            this._name = name;
        }

        public void Invoke()
        {
            try
            {
                _callBack.Invoke();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        public string GetName() { return _name; }
    }

    class asyncEvent<T1> : AsyncEventBase
    {
        private Action<T1> _callBack;
        private readonly string _name;
        private T1 _arg1;

        public asyncEvent(Action<T1> callBack, string name, T1 arg1)
        {
            this._callBack = callBack;
            this._name = name;
            _arg1 = arg1;
        }

        public void Invoke()
        {
            try
            {
                _callBack.Invoke(_arg1);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        public string GetName() { return _name; }
    }

    class asyncEvent<T1, T2> : AsyncEventBase
    {
        private Action<T1, T2> _callBack;
        private readonly string _name;
        private T1 _arg1;
        private T2 _arg2;

        public asyncEvent(Action<T1, T2> callBack, string name, T1 arg1, T2 arg2)
        {
            this._callBack = callBack;
            this._name = name;
            _arg1 = arg1;
            _arg2 = arg2;
        }

        public void Invoke()
        {
            try
            {
                _callBack.Invoke(_arg1, _arg2);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        public string GetName() { return _name; }
    }

    class asyncEvent<T1, T2, T3> : AsyncEventBase
    {
        private Action<T1, T2, T3> _callBack;
        private readonly string _name;
        private T1 _arg1;
        private T2 _arg2;
        private T3 _arg3;

        public asyncEvent(Action<T1, T2, T3> callBack, string name, T1 arg1, T2 arg2, T3 arg3)
        {
            this._callBack = callBack;
            this._name = name;
            _arg1 = arg1;
            _arg2 = arg2;
            _arg3 = arg3;
        }

        public void Invoke()
        {
            try
            {
                _callBack.Invoke(_arg1, _arg2, _arg3);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        public string GetName() { return _name; }
    }

    class asyncEvent<T1, T2, T3, T4> : AsyncEventBase
    {
        private Action<T1, T2, T3, T4> _callBack;
        private readonly string _name;
        private T1 _arg1;
        private T2 _arg2;
        private T3 _arg3;
        private T4 _arg4;

        public asyncEvent(Action<T1, T2, T3, T4> callBack, string name, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
        {
            this._callBack = callBack;
            this._name = name;
            _arg1 = arg1;
            _arg2 = arg2;
            _arg3 = arg3;
            _arg4 = arg4;
        }

        public void Invoke()
        {
            try
            {
                _callBack.Invoke(_arg1, _arg2, _arg3, _arg4);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        public string GetName() { return _name; }
    }

    class asyncEvent<T1, T2, T3, T4, T5> : AsyncEventBase
    {
        private Action<T1, T2, T3, T4, T5> _callBack;
        private readonly string _name;
        private T1 _arg1;
        private T2 _arg2;
        private T3 _arg3;
        private T4 _arg4;
        private T5 _arg5;

        public asyncEvent(Action<T1, T2, T3, T4, T5> callBack, string name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
        {
            this._callBack = callBack;
            this._name = name;
            _arg1 = arg1;
            _arg2 = arg2;
            _arg3 = arg3;
            _arg4 = arg4;
            _arg5 = arg5;
        }

        public void Invoke()
        {
            try
            {
                _callBack.Invoke(_arg1, _arg2, _arg3, _arg4, _arg5);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        public string GetName() { return _name; }
    }


    public class AsyncEventFactory
    {
        public static AsyncEventBase Create(Action callBack, string name) { return new asyncEvent(callBack, name); }
        public static AsyncEventBase Create<T1>(Action<T1> callBack, string name, T1 arg1) { return new asyncEvent<T1>(callBack, name, arg1); }
        public static AsyncEventBase Create<T1, T2>(Action<T1, T2> callBack, string name, T1 arg1, T2 arg2) { return new asyncEvent<T1, T2>(callBack, name, arg1, arg2); }
        public static AsyncEventBase Create<T1, T2, T3>(Action<T1, T2, T3> callBack, string name, T1 arg1, T2 arg2, T3 arg3) { return new asyncEvent<T1, T2, T3>(callBack, name, arg1, arg2, arg3); }
        public static AsyncEventBase Create<T1, T2, T3, T4>(Action<T1, T2, T3, T4> callBack, string name, T1 arg1, T2 arg2, T3 arg3, T4 arg4) { return new asyncEvent<T1, T2, T3, T4>(callBack, name, arg1, arg2, arg3, arg4); }
        public static AsyncEventBase Create<T1, T2, T3, T4, T5>(Action<T1, T2, T3, T4, T5> callBack, string name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5) { return new asyncEvent<T1, T2, T3, T4, T5>(callBack, name, arg1, arg2, arg3, arg4, arg5); }
    }


    class EventQueue
    {
        public EventQueue(string name)
        {
            this._name = name;
        }

        private string _name = string.Empty;
        private object _lock = new object();
        private ManualResetEvent _stopEvent = new ManualResetEvent(false);
        private ManualResetEvent _notifyEvent = new ManualResetEvent(false);
        private List<AsyncEventBase> _queueList = new List<AsyncEventBase>();

        public void Process()
        {
            WaitHandle[] handles = { _stopEvent, _notifyEvent };
            List<AsyncEventBase> queueList = new List<AsyncEventBase>();

            while (!_stopEvent.WaitOne(0))
            {
                int rc = WaitHandle.WaitAny(handles, 1000);
                if (WaitHandle.WaitTimeout == rc)
                {
                    lock (_lock)
                    {
                        queueList.TrimExcess();
                        _queueList.TrimExcess();
                    }

                    continue;
                }

                lock (_lock)
                {
                    queueList.AddRange(_queueList);
                    _queueList.Clear();
                    _notifyEvent.Reset();
                }

                foreach (AsyncEventBase e in queueList)
                {
                    e.Invoke();
                }

                queueList.Clear();
            }
        }

        public void Stop()
        {
            _stopEvent.Set();
        }

        public void Add(AsyncEventBase args)
        {
            lock (_lock)
            {
                _queueList.Add(args);
                _notifyEvent.Set();
            }
        }
    }
}
