using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ObjectEditor
{
    public class Vector4<T>
    {
        public T R, G, B, A;

        public Vector4(T r, T g, T b, T a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }

        public override string ToString()
        {
            return (R + " " + G + " " + B + " " + A);
        }
    }
}
