/*
    nray source code is Copyright(c) 2020
                        Nicolas Delbecq

    This file is part of nray.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */
#include "nray.h"

#include "geometry.h"


int main() {
    Vec3 v(1.0, 1.2, 3.2);
    Vec3 v2(1.0, 1.2, 3.2);
    Vec3 v3 = v * v2 * 2;
    v3 *= 5;
    v3 *= v2;
    Vec3 v4 = v / v2 / 1.0;
    v4 /= 3;
    v4 /= v3;
    Vec3 v5 = Normalize(v4);
    bool eq = v == v2;
    bool neq = v != v2;
    Float d = Distance(v, v2);
    d = DistanceSquared(v, v2);
    Float dot = Dot(v, v2);
    Vec3 v6 = Cross(v, v2);
    std::cout << "Hello World!" << "\n";
    std::cout << (Normalize(v) * 10).Length() << std::endl;
    std::cout << v*v/Normalize(v) << std::endl;
    return 0;
}