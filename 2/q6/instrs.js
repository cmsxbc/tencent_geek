let instrs = [];
instrs[0] = function() {
    var C = x.pop();
    x.push(C[0][C[1]])
};
instrs[1] = function() {
    x[x[x.length - 2][0]][0] = x[x.length - 1]
};
instrs[2] = function() {
    x[x.length - 2] = x[x.length - 2] << x.pop()
};
instrs[3] = function() {
    x.push(typeof x.pop())
};
instrs[4] = function() {
    x.push(null)
};
instrs[5] = function() {
    x[x.length - 2] = x[x.length - 2] == x.pop()
};
instrs[6] = function() {
    var C = x.pop(),
        I = x[x.length - 1];
    x[I[0]][0] = C[0][C[1]]
};
instrs[7] = function() {
    x.push([x.pop(), x.pop()].reverse())
};
instrs[8] = function() {
    x[x.length - 1] = G[f++]
};
instrs[9] = function() {
    I.push([G[f++], x.length, G[f++]])
};
instrs[10] = function() {
    x.length -= G[f++]
};
instrs[11] = function() {
    x[x.length - 2] = x[x.length - 2] in x.pop()
};
instrs[12] = function() {
    var C = x.pop();
    x.push([x[x.pop()][0], C])
};
instrs[13] = function() {};
instrs[14] = function() {
    var C = G[f++],
        I = x[x.length - 2 - C];
    x[x.length - 2 - C] = x.pop(), x.push(I)
};
instrs[15] = function() {
    x.push(x[x.length - 1])
};
instrs[16] = function() {
    x.push([x.pop()])
};
instrs[17] = function() {};
instrs[18] = function() {
    var C = G[f++],
        I = C ? x.slice(-C) : [];
    x.length -= C, I.unshift(null), x.push(new(Function.prototype.bind.apply(x.pop(), I)))
};
instrs[19] = function() {};
instrs[20] = function() {};
instrs[21] = function() {
    x.push(~x.pop())
};
instrs[22] = function() {
    x[x.length - 2] = x[x.length - 2] & x.pop()
};
instrs[23] = function() {
    f = G[f++]
};
instrs[24] = function() {
    x.pop()
};
instrs[25] = function() {
    var C = G[f++],
        I = C ? x.slice(-C) : [];
    x.length -= C, x.push(x.pop().apply(w, I))
};
instrs[26] = function() {
    x.push([w, x.pop()])
};
instrs[27] = function() {
    x[x.length - 2] = x[x.length - 2] + x.pop()
};
instrs[28] = function() {
    var C = x.pop();
    x.push(delete C[0][C[1]])
};
instrs[29] = function() {
    x[x.length - 2] = x[x.length - 2] - x.pop()
};
instrs[30] = function() {
    var C = x[x.length - 2];
    C[0][C[1]] = x[x.length - 1]
};
instrs[31] = function() {
    x.push(G[f++])
};
instrs[32] = function() {
    x[x.length - 2] = x[x.length - 2] ^ x.pop()
};
instrs[33] = function() {
    x[x.length - 2] = x[x.length - 2] | x.pop()
};
instrs[34] = function() {
    x[x.length - 2] = x[x.length - 2] / x.pop()
};
instrs[35] = function() {
    x[x.length - 2] = x[x.length - 2] >= x.pop()
};
instrs[36] = function() {
    var C = G[f++],
        I = C ? x.slice(-C) : [];
    x.length -= C, I.unshift(null);
    var g = x.pop();
    x.push(new(Function.prototype.bind.apply(g[0][g[1]], I)))
};
instrs[37] = function() {
    x.push(!x.pop())
};
instrs[38] = function() {};
instrs[39] = function() {
    x[x.length - 2] = x[x.length - 2] >>> x.pop()
};
instrs[40] = function() {
    x.push(undefined)
};
instrs[41] = function() {
    x.push(!0)
};
instrs[42] = function() {
    x.push(!1)
};
instrs[43] = function() {
    x[x.length - 2] = x[x.length - 2] >> x.pop()
};
instrs[44] = function() {
    for (var g = G[f++], Q = [], C = G[f++], I = G[f++], H = [], A = 0; A < C; A++) Q[G[f++]] = x[G[f++]];
    for (A = 0; A < I; A++) H[A] = G[f++];
    x.push(function B() {
        var C = Q.slice(0);
        C[0] = [this], C[1] = [arguments], C[2] = [B];
        for (var I = 0; I < H.length && I < arguments.length; I++) 0 < H[I] && (C[H[I]] = [arguments[I]]);
        return __TENCENT_CHAOS_VM(g, G, w, C, h, b, t)
    })
};
instrs[45] = function() {
    x.push("")
};
instrs[46] = function() {
    x[x.length - 2] = x[x.length - 2] * x.pop()
};
instrs[47] = function() {
    g = null
};
instrs[48] = function() {};
instrs[49] = function() {
    I.pop()
};
instrs[50] = function() {
    x.push(x[x.pop()[0]][0])
};
instrs[51] = function() {
    var C = x.pop(),
        I = x[x.length - 1];
    I[0][I[1]] = x[C[0]][0]
};
instrs[52] = function() {
    var C = G[f++];
    x[x.length - 1] && (f = C)
};
instrs[53] = function() {
    throw x[x.length - 1]
};
instrs[54] = function() {
    return !0
};
instrs[55] = function() {
    x[x.length - 2] = x[x.length - 2] % x.pop()
};
instrs[56] = function() {
    x.length = G[f++]
};
instrs[57] = function() {};
instrs[58] = function() {
    x[x.length - 2] = x[x.length - 2] === x.pop()
};
instrs[59] = function() {};
instrs[60] = function() {
    var C = x.pop(),
        I = x[x.length - 1];
    I[0][I[1]] = C[0][C[1]]
};
instrs[61] = function() {
    x[x.length - 2] = x[x.length - 2] > x.pop()
};
instrs[62] = function() {
    x[x[x.length - 1][0]] = x[x[x.length - 1][0]] === undefined ? [] : x[x[x.length - 1][0]]
};
instrs[63] = function() {
    var C = x.pop(),
        I = x.pop();
    x.push([I[0][I[1]], C])
};
instrs[64] = function() {
    var C = G[f++],
        I = C ? x.slice(-C) : [];
    x.length -= C;
    var g = x.pop();
    x.push(g[0][g[1]].apply(g[0], I))
};
instrs[65] = function() {};
instrs[66] = function() {
    x[x.length - 2] += String.fromCharCode(G[f++] + x[x.length - 1])
};
instrs[67] = function() {
    return !!g
};