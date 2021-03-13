var instr0=function() {
    r[r.length - 2] = r[r.length - 2] | r.pop()
};

var instr3=function() {
    for (var o = e[f++], u = [], n = e[f++], t = e[f++], p = [], h = 0; h < n; h++) {
        u[e[f++]] = r[e[f++]];
    }
    for (h = 0; h < t; h++) {
        p[h] = e[f++];
    }
    r.push(function i() {
        var n = u.slice(0);
        n[0] = [this],
        n[1] = [arguments],
        n[2] = [i];
        for (var t = 0; t < p.length && t < arguments.length; t++) {
            if (0 < p[t]) {
                n[p[t]] = [arguments[t]];
            }
        }
        return __TENCENT_CHAOS_VM(o, e, c, n, l, g, s, a)
    })
};

var instr5=function() {
    r.push(undefined);
};

var instr7=function() {
    return !0
};

var instr8=function() {
    var n = r[r.length - 2];
    n[0][n[1]] = r[r.length - 1]
};

var instr9=function() {
    r.push(!r.pop())
};

var instr12=function() {
    r.push([e[f++]])
};

var instr13=function() {
    r[r.length - 1] = e[f++]
};

var instr15=function() {
    r[r.length - 2] = r[r.length - 2] % r.pop()
};

var instr16=function() {
    r.push("")
};

var instr17=function() {
    f = e[f++]
};

var instr24=function() {
    var n = r.pop();
    r.push([r[r.pop()][0], n])
};

var instr25=function() {
    var n = e[f++];
    r[n] = r[n] === undefined ? [] : r[n]
};

var instr31=function() {
    var n = e[f++],
        t = r[r.length - 2 - n];
    r[r.length - 2 - n] = r.pop(), r.push(t)
};

var instr32=function() {
    var n = e[f++];
    r[r.length - 1] && (f = n)
};

var instr33=function() {
    r.length = e[f++]
};

var instr34=function() {
    r[r.length - 2] = r[r.length - 2] >= r.pop()
};

var instr37=function() {
    r.pop()
};

var instr41=function() {
    r[r[r.length - 2][0]][0] = r[r.length - 1]
};

var instr44=function() {
    r[r.length - 2] = r[r.length - 2] * r.pop()
};

var instr45=function() {
    r.push([c, r.pop()])
};

var instr48=function() {
    r.push(r[r.pop()[0]][0])
};

var instr52=function() {
    var n = e[f++],
        t = n ? r.slice(-n) : [];
    r.length -= n;
    var o = r.pop();
    r.push(o[0][o[1]].apply(o[0], t))
};
var instr53=function() {
    r.push(r[e[f++]][0])
};

var instr55=function() {
    r.push(e[f++])
};

var instr57=function() {
    var n = r.pop();
    r.push(n[0][n[1]])
};

var instr58=function() {
    r.push(r[r.length - 1])
};

var instr63=function() {
    r.length -= e[f++]
};

var instr65=function() {
    var n = r.pop(),
        t = r.pop();
    r.push([t[0][t[1]], n])
};

var instr66=function() {
    r[r.length - 2] = r[r.length - 2] + r.pop()
};

var instr68=function() {
    r[r.length - 1] += String.fromCharCode(e[f++])
};

u = [
    instr0,  // 0
    ,
    ,
    instr3, // 3
    ,
    instr5, // 5
    ,
    instr7, // 7
    instr8, // 8
    instr9, // 9
    ,
    ,
    instr12, // 12,
    instr13, // 13
    ,
    instr15, // 15
    instr16, // 16
    instr17, // 17
    ,
    ,
    ,
    ,
    ,
    ,
    instr24, // 24
    instr25, // 25
    ,
    ,
    ,
    ,
    ,
    instr31, // 31
    instr32, // 32
    instr33, // 33
    instr34, // 34
    ,
    ,
    instr37, // 37
    ,
    ,
    ,
    instr41, // 41
    ,
    ,
    instr44, // 44
    instr45, // 45
    ,
    ,
    instr48, // 48
    ,
    ,
    ,
    instr52, // 52
    instr53, // 53
    ,
    instr55, // 55
    ,
    instr57, // 57
    instr58, // 58
    ,
    ,
    ,
    ,
    instr63, // 63
    ,
    instr65, // 65
    instr66, // 66
    ,
    instr68, // 68
]