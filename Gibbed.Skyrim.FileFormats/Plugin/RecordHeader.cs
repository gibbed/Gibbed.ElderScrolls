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

using System.IO;
using Gibbed.IO;

namespace Gibbed.Skyrim.FileFormats.Plugin
{
    public class RecordHeader
    {
        public RecordType Type;
        public uint DataSize;
        public uint Flags;
        public uint FormId;
        public uint VersionControl;
        public ushort Unknown14;
        public ushort Unknown16;

        public void Serialize(Stream output, bool littleEndian)
        {
            output.WriteValueEnum<RecordType>(this.Type);
            output.WriteValueU32(this.DataSize);
            output.WriteValueU32(this.Flags);
            output.WriteValueU32(this.FormId);
            output.WriteValueU32(this.VersionControl);
            output.WriteValueU16(this.Unknown14);
            output.WriteValueU16(this.Unknown16);
        }

        public void Deserialize(Stream input, bool littleEndian)
        {
            this.Type = input.ReadValueEnum<RecordType>(littleEndian);
            this.DataSize = input.ReadValueU32(littleEndian);
            this.Flags = input.ReadValueU32(littleEndian);
            this.FormId = input.ReadValueU32(littleEndian);
            this.VersionControl = input.ReadValueU32(littleEndian);
            this.Unknown14 = input.ReadValueU16(littleEndian);
            this.Unknown16 = input.ReadValueU16(littleEndian);
        }
    }
}
