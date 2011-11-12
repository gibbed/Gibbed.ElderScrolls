using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Gibbed.Skyrim.FileFormats.CompiledScript
{
    public class DebugInfo
    {
        public byte Unknown0;
        public ulong ModificationTime;
        public List<DebugFunction> Functions = new List<DebugFunction>();
    }
}
