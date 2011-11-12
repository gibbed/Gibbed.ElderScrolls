/* Copyright (c) 2011 Rick (rick 'at' gibbed 'dot' us)
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */

using System;
using System.Collections.Generic;
using System.IO;
using Gibbed.IO;

namespace Gibbed.Skyrim.FileFormats
{
    public class CompiledScriptFile
    {
        public byte MajorVersion;
        public byte MinorVersion;
        public ushort GameId;
        public ulong Unknown3;
        public string SourceFileName;
        public string Unknown5;
        public string Unknown6;
        public CompiledScript.DebugInfo DebugInfo = new CompiledScript.DebugInfo();
        public List<CompiledScript.UserFlag> UserFlags = new List<CompiledScript.UserFlag>();
        public List<CompiledScript.Object> Objects = new List<CompiledScript.Object>();

        public void Deserialize(Stream input)
        {
            var magic = input.ReadValueU32(true);
            if (magic != 0xFA57C0DE &&
                magic.Swap() != 0xFA57C0De)
            {
                throw new FormatException();
            }

            var littleEndian = magic == 0xFA57C0DE;

            this.MajorVersion = input.ReadValueU8();
            this.MinorVersion = input.ReadValueU8();

            if (this.MajorVersion != 3 ||
                (this.MinorVersion != 0 && this.MinorVersion != 1))
            {
                throw new FormatException();
            }

            this.GameId = input.ReadValueU16(littleEndian);
            if (this.GameId != 1)
            {
                throw new FormatException();
            }

            this.Unknown3 = input.ReadValueU64(littleEndian);
            this.SourceFileName = input.ReadString(input.ReadValueU16(littleEndian), true);
            this.Unknown5 = input.ReadString(input.ReadValueU16(littleEndian), true);
            this.Unknown6 = input.ReadString(input.ReadValueU16(littleEndian), true);

            var stringCount = input.ReadValueU16(littleEndian);
            var strings = new string[stringCount];
            for (int i = 0; i < stringCount; i++)
            {
                strings[i] = input.ReadString(input.ReadValueU16(littleEndian), true);
            }

            this.DebugInfo.Unknown0 = input.ReadValueU8();
            this.DebugInfo.ModificationTime = input.ReadValueU64(littleEndian);
            this.DebugInfo.Functions.Clear();
            var debugFunctionCount = input.ReadValueU16(littleEndian);
            for (int i = 0; i < debugFunctionCount; i++)
            {
                var debugFunction = new CompiledScript.DebugFunction();
                debugFunction.ObjectName = strings[input.ReadValueU16(littleEndian)];
                debugFunction.StateName = strings[input.ReadValueU16(littleEndian)];
                debugFunction.FunctionName = strings[input.ReadValueU16(littleEndian)];
                debugFunction.Type = input.ReadValueU8();
                debugFunction.LineNumbers.Clear();
                var instructionCount = input.ReadValueU16(littleEndian);
                for (int j = 0; j < instructionCount; j++)
                {
                    debugFunction.LineNumbers.Add(input.ReadValueU16(littleEndian));
                }
                this.DebugInfo.Functions.Add(debugFunction);
            }

            this.UserFlags.Clear();
            var userFlagCount = input.ReadValueU16(littleEndian);
            for (int i = 0; i < userFlagCount; i++)
            {
                var userFlag = new CompiledScript.UserFlag();
                userFlag.Name = strings[input.ReadValueU16(littleEndian)];
                userFlag.Index = input.ReadValueU8();
                this.UserFlags.Add(userFlag);
            }

            this.Objects.Clear();
            var objectCount = input.ReadValueU16(littleEndian);
            for (int i = 0; i < objectCount; i++)
            {
                var obj = new CompiledScript.Object();
                obj.Name = strings[input.ReadValueU16(littleEndian)];

                var dataSize = input.ReadValueU32(littleEndian);
                if (dataSize < 4)
                {
                    throw new FormatException();
                }

                obj.Data = new byte[dataSize - 4];
                if (input.Read(obj.Data, 0, obj.Data.Length) != obj.Data.Length)
                {
                    throw new FormatException();
                }
                this.Objects.Add(obj);
            }
        }
    }
}
