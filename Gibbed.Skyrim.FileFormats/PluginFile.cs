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
using System.Linq;
using System.Text;
using Gibbed.IO;

namespace Gibbed.Skyrim.FileFormats
{
    public class PluginFile
    {
        public void Serialize(Stream output)
        {
            throw new NotImplementedException();
        }

        public void Serialize(Stream output)
        {
            throw new NotImplementedException();
        }

        public void Deserialize(Stream input)
        {
            input.Seek(0, SeekOrigin.Begin);

            var magic = input.ReadValueU32(true);
            if (magic != 0x34534554 && // '4SET'
                magic.Swap() != 0x34534554) // 'TES4'
            {
                throw new FormatException();
            }

            var littleEndian = magic == 0x34534554;

            input.Seek(0, SeekOrigin.Begin);

            while (input.Position < input.Length)
            {
                if (input.Position + 24 > input.Length)
                {
                    throw new FormatException();
                }

                var header = new Plugin.RecordHeader();
                header.Deserialize(input, littleEndian);

                var info = new Plugin.RecordInfo();
                info.Header = header;
                info.DataOffset = input.Position;

                input.Seek(info.Header.DataSize, SeekOrigin.Current);
            }

            throw new NotImplementedException();
        }
    }
}
