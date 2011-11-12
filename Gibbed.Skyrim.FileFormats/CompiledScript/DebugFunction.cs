using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Gibbed.Skyrim.FileFormats.CompiledScript
{
    public class DebugFunction
    {
        public string ObjectName;
        public string StateName;
        public string FunctionName;
        public byte Type;
        public List<ushort> LineNumbers = new List<ushort>();
    }
}
